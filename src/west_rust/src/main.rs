// use bitvec;

mod mltl;
mod parse;

pub use mltl::MLTL_WEST;


fn main() {
    println!("WEST MLTL Parser and Analyzer\n");

    // Example formulas to parse and display
    let formulas = vec![
        "G[0,5](x)",
        "x & y | z",
        "G[0,5](x & y) | F[1,3](z)",
        "NOT(x AND y)",
    ];

    for formula_str in formulas {
        println!("Parsing: {}", formula_str);
        match MLTL_WEST::parse(formula_str) {
            Ok(formula) => {
                formula.print_parsetree();
                println!("Well-defined: {}\n", formula.welldef());
            }
            Err(e) => {
                println!("Error: {}\n", e);
            }
        }
    }
}