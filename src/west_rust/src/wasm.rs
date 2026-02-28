//! WASM bindings for WEST
//!
//! Exposes `validate_formula` to JavaScript via wasm-bindgen.
//! Input: MLTL formula string
//! Output: JSON string with NNF, variable info, computations, etc.

use wasm_bindgen::prelude::*;
use serde::Serialize;

use crate::{MLTL, compile, compile_with_context};

/// Result structure returned to JavaScript as JSON.
#[derive(Serialize)]
struct WestResult {
    success: bool,
    nnf: String,
    variables: usize,
    variable_mapping: Vec<VariableMapping>,
    complen: usize,
    bits_needed: usize,
    computations: Vec<String>,
    count: usize,
    subformulas: Vec<SubformulaResult>,
}

#[derive(Serialize)]
struct VariableMapping {
    name: String,
    index: usize,
}

#[derive(Serialize)]
struct SubformulaResult {
    formula: String,
    computations: Vec<String>,
    count: usize,
}

/// Format an MLTL formula to a human-readable string.
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
        MLTL::Until(lb, ub, l, r) => {
            format!("(({}) U[{},{}] ({}))", format_mltl(l), lb, ub, format_mltl(r))
        }
        MLTL::Release(lb, ub, l, r) => {
            format!("(({}) R[{},{}] ({}))", format_mltl(l), lb, ub, format_mltl(r))
        }
    }
}

/// Collect all unique subformulas from an MLTL formula.
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

/// Validate and compile an MLTL formula, returning results as JSON.
///
/// Called from JavaScript. Returns a JSON string with all compilation results.
#[wasm_bindgen]
pub fn validate_formula(formula: &str) -> Result<String, JsValue> {
    let formula = formula.trim();

    // Parse
    let parsed = MLTL::parse(formula)
        .map_err(|e| JsValue::from_str(&format!("Parse error: {}", e)))?;

    // Convert to NNF
    let nnf = parsed.clone().to_nnf();
    let nnf_str = format_mltl(&nnf);

    // Collect variable info
    let var_map = nnf.collect_vars();
    let n = var_map.len().max(1);
    let cl = nnf.complen();
    let bits_needed = 2 * n * cl;

    // Sort variables by index
    let mut sorted_vars: Vec<_> = var_map.iter().collect();
    sorted_vars.sort_by_key(|&(_, idx)| *idx);
    let variable_mapping: Vec<VariableMapping> = sorted_vars
        .iter()
        .map(|(name, idx)| VariableMapping {
            name: name.to_string(),
            index: **idx,
        })
        .collect();

    // Compile main formula
    let regex = compile(parsed);
    let computations: Vec<String> = regex
        .traces()
        .iter()
        .map(|t| t.to_trace_string())
        .collect();

    // Compile each subformula
    let subformula_strs = collect_subformulas(&nnf);
    let mut subformulas = Vec::new();
    for subf_str in &subformula_strs {
        if let Ok(subf) = MLTL::parse(subf_str) {
            let sub_regex = compile_with_context(subf, &var_map, n, cl);
            let sub_computations: Vec<String> = sub_regex
                .traces()
                .iter()
                .map(|t| t.to_trace_string())
                .collect();
            subformulas.push(SubformulaResult {
                formula: subf_str.clone(),
                count: sub_computations.len(),
                computations: sub_computations,
            });
        }
    }

    let result = WestResult {
        success: true,
        nnf: nnf_str,
        variables: n,
        variable_mapping,
        complen: cl,
        bits_needed,
        count: computations.len(),
        computations,
        subformulas,
    };

    serde_json::to_string(&result)
        .map_err(|e| JsValue::from_str(&format!("Serialization error: {}", e)))
}
