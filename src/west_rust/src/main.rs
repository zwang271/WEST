//! WEST Command-Line Interface
//!
//! Usage:
//!   west_core <input_file>       # Read formula from .mltl or .txt file
//!   west_core "<formula_string>" # Parse formula directly from string
//!
//! Output:
//!   - Prints NNF, propositional variables, computation length, and bits needed
//!   - Prints first 10 computations
//!   - Writes <project_root>/output/output_rust.txt (NNF + all computations)
//!   - Writes <project_root>/output/subformulas_rust.txt (each subformula + its regexes)

use std::env;
use std::fs;
use std::io::Write;
use std::path::PathBuf;
use std::time::Instant;

use west_rust::{MLTL, compile, compile_with_context};

/// Get the project root directory based on the executable location.
/// The executable is expected to be in `<project_root>/bin/`.
fn get_project_root() -> PathBuf {
    if let Ok(exe_path) = env::current_exe() {
        // Go up from bin/ to project root
        if let Some(bin_dir) = exe_path.parent() {
            if let Some(project_root) = bin_dir.parent() {
                return project_root.to_path_buf();
            }
        }
    }
    // Fallback to current directory if we can't determine exe path
    PathBuf::from(".")
}

/// Format an MLTL formula to a string representation that can be parsed back.
fn format_mltl<T: std::fmt::Display>(formula: &MLTL<T>) -> String {
    match formula {
        MLTL::True => "true".to_string(),
        MLTL::False => "false".to_string(),
        MLTL::Prop(name) => format!("{}", name),
        MLTL::Not(sub) => format!("!{}", format_mltl(sub)),
        MLTL::And(l, r) => format!("({} & {})", format_mltl(l), format_mltl(r)),
        MLTL::Or(l, r) => format!("({} | {})", format_mltl(l), format_mltl(r)),
        MLTL::Global(lb, ub, sub) => format!("G[{},{}]({})", lb, ub, format_mltl(sub)),
        MLTL::Future(lb, ub, sub) => format!("F[{},{}]({})", lb, ub, format_mltl(sub)),
        MLTL::Until(lb, ub, l, r) => format!("(({}) U[{},{}] ({}))", format_mltl(l), lb, ub, format_mltl(r)),
        MLTL::Release(lb, ub, l, r) => format!("(({}) R[{},{}] ({}))", format_mltl(l), lb, ub, format_mltl(r)),
    }
}

/// Collect all unique subformulas from an MLTL formula.
/// 
/// Returns a Vec of (formatted_string, MLTL) pairs.
fn collect_subformulas(formula: &MLTL<String>) -> Vec<String> {
    let mut subformulas = Vec::new();
    collect_subformulas_inner(formula, &mut subformulas);
    
    // Remove duplicates while preserving order
    let mut seen = std::collections::HashSet::new();
    subformulas.retain(|f| seen.insert(f.clone()));
    subformulas
}

fn collect_subformulas_inner(formula: &MLTL<String>, acc: &mut Vec<String>) {
    acc.push(format_mltl(formula));
    
    match formula {
        MLTL::True | MLTL::False | MLTL::Prop(_) => {}
        MLTL::Not(sub) => {
            collect_subformulas_inner(sub, acc);
        }
        MLTL::And(l, r) | MLTL::Or(l, r) => {
            collect_subformulas_inner(l, acc);
            collect_subformulas_inner(r, acc);
        }
        MLTL::Global(_, _, sub) | MLTL::Future(_, _, sub) => {
            collect_subformulas_inner(sub, acc);
        }
        MLTL::Until(_, _, l, r) | MLTL::Release(_, _, l, r) => {
            collect_subformulas_inner(l, acc);
            collect_subformulas_inner(r, acc);
        }
    }
}

/// Trim common suffixes from a list of strings if all end with 's' or ','.
fn trim_suffixes(mut strings: Vec<String>) -> Vec<String> {
    if strings.len() <= 1 {
        return strings;
    }
    
    loop {
        let all_end_s = strings.iter().all(|s| s.ends_with('s'));
        let all_end_comma = strings.iter().all(|s| s.ends_with(','));
        
        if all_end_s || all_end_comma {
            for s in &mut strings {
                s.pop();
            }
        } else {
            break;
        }
    }
    strings
}

fn main() {
    let args: Vec<String> = env::args().collect();
    
    if args.len() < 2 {
        eprintln!("Usage:");
        eprintln!("\t{} <input_file>", args[0]);
        eprintln!("\t{} \"<formula_string>\"", args[0]);
        std::process::exit(1);
    }
    
    // Read formula from file or argument
    let wff = if args[1].ends_with(".mltl") || args[1].ends_with(".txt") {
        match fs::read_to_string(&args[1]) {
            Ok(content) => content.lines().last().unwrap_or("").trim().to_string(),
            Err(e) => {
                eprintln!("Error: could not open input file: {}", e);
                std::process::exit(1);
            }
        }
    } else {
        args[1].clone()
    };
    
    // Only trim leading/trailing whitespace - internal whitespace is needed for parsing
    let wff = wff.trim().to_string();
    
    // Parse formula
    let formula = match MLTL::parse(&wff) {
        Ok(f) => f,
        Err(e) => {
            eprintln!("Error parsing formula: {}", e);
            std::process::exit(1);
        }
    };
    
    // Convert to NNF
    let nnf = formula.clone().to_nnf();
    let nnf_str = format_mltl(&nnf);
    
    // Count variables and compute metrics
    let var_map = nnf.collect_vars();
    let n = var_map.len().max(1); // at least 1 for formulas without props
    let cl = nnf.complen();
    let bits_needed = 2 * n * cl;
    
    // Sort variables by index for display
    let mut sorted_vars: Vec<_> = var_map.iter().collect();
    sorted_vars.sort_by_key(|&(_, idx)| *idx);
    
    println!("\tnnf: {}", nnf_str);
    println!("\tpropositional variables: {}", n);
    if !sorted_vars.is_empty() {
        println!("\tvariable mapping:");
        for (name, idx) in &sorted_vars {
            println!("\t  {} -> index {}", name, idx);
        }
    }
    println!("\tcomputation length: {}", cl);
    println!("\tBits needed: 2 * {} * {} = {}", n, cl, bits_needed);
    
    // Compile formula
    let start = Instant::now();
    let regex = compile(formula.clone());
    let duration = start.elapsed();
    
    // Get trace strings
    let computations: Vec<String> = regex.traces().iter()
        .map(|t| t.to_trace_string())
        .collect();
    
    println!("=======================================================");
    for (i, comp) in computations.iter().enumerate().take(10) {
        println!("\t{}", comp);
        if i == 9 && computations.len() > 10 {
            break;
        }
    }
    if computations.len() > 10 {
        println!("\t...");
    }
    println!("=======================================================");
    println!("\tTime taken: {} milliseconds", duration.as_millis());
    println!("\tNumber of computations: {}", computations.len());
    
    // Create output directory (relative to project root, like C++ version)
    let project_root = get_project_root();
    let output_dir = project_root.join("output");
    if let Err(e) = fs::create_dir_all(&output_dir) {
        eprintln!("Warning: could not create output directory: {}", e);
    }
    
    // Write output_rust.txt
    let output_path = output_dir.join("output_rust.txt");
    if let Ok(mut file) = fs::File::create(&output_path) {
        writeln!(file, "NNF: {}", nnf_str).ok();
        writeln!(file, "Variables: {}", n).ok();
        if !sorted_vars.is_empty() {
            writeln!(file, "Variable mapping:").ok();
            for (name, idx) in &sorted_vars {
                writeln!(file, "  {} -> index {}", name, idx).ok();
            }
        }
        writeln!(file, "Computation length: {}", cl).ok();
        writeln!(file, "Computations:").ok();
        for comp in &computations {
            writeln!(file, "{}", comp).ok();
        }
        println!("Output written to {}", output_path.display());
    } else {
        eprintln!("Warning: could not write output file");
    }
    
    // Write subformulas_rust.txt
    let subformulas_path = output_dir.join("subformulas_rust.txt");
    if let Ok(mut file) = fs::File::create(&subformulas_path) {
        let subformulas = collect_subformulas(&nnf);
        
        for subf_str in &subformulas {
            writeln!(file, "{}", subf_str).ok();
            
            // Parse and compile each subformula using parent's variable mapping
            match MLTL::parse(subf_str) {
                Ok(subf) => {
                    let sub_regex = compile_with_context(subf, &var_map, n, cl);
                    let sub_computations: Vec<String> = sub_regex.traces().iter()
                        .map(|t| t.to_trace_string())
                        .collect();
                    
                    for comp in &sub_computations {
                        writeln!(file, "{}", comp).ok();
                    }
                }
                Err(e) => {
                    writeln!(file, "Error: {}", e).ok();
                }
            }
            writeln!(file).ok();
        }
        println!("Subformulas written to {}", subformulas_path.display());
    } else {
        eprintln!("Warning: could not write subformulas file");
    }
    
    println!();
}