use std::cmp::max;
use std::collections::HashMap;

#[allow(non_camel_case_types)]
#[derive(Debug, PartialEq, Clone)]
pub enum MLTL<T> {
    True,
    False,
    Prop(T),
    Not(Box<MLTL<T>>),
    And(Box<MLTL<T>>, Box<MLTL<T>>),
    Or(Box<MLTL<T>>, Box<MLTL<T>>),
    Global(usize, usize, Box<MLTL<T>>),
    Future(usize, usize, Box<MLTL<T>>),
    Until(usize, usize, Box<MLTL<T>>, Box<MLTL<T>>),
    Release(usize, usize, Box<MLTL<T>>, Box<MLTL<T>>),
}

impl<T: Ord + std::fmt::Display + std::hash::Hash> MLTL<T> {
    pub fn welldef(&self) -> bool {
        match self {
            MLTL::True | MLTL::False | MLTL::Prop(_) => true,
            MLTL::Not(sub) => sub.welldef(),
            MLTL::And(left, right) 
            | MLTL::Or(left, right) => {
                left.welldef() && right.welldef()
            }
            MLTL::Global(_lb, ub, sub) 
            | MLTL::Future(_lb, ub, sub) => {
                _lb <= ub && sub.welldef()
            }
            MLTL::Until(_lb, ub, left, right)
            | MLTL::Release(_lb, ub, left, right) => {
                _lb <= ub && left.welldef() && right.welldef()
            }
        }
    }

    pub fn complen(&self) -> usize {
        match self {
            MLTL::True | MLTL::False | MLTL::Prop(_) => 1,
            MLTL::Not(sub) => sub.complen(),
            MLTL::And(left, right) 
            | MLTL::Or(left, right) => {
                max(left.complen(), right.complen())
            }
            MLTL::Global(_lb, ub, sub) 
            | MLTL::Future(_lb, ub, sub) => {
                ub + sub.complen()
            }
            MLTL::Until(_lb, ub, left, right)
            | MLTL::Release(_lb, ub, left, right) => {
                ub + max(left.complen()-1, right.complen())
            }
        }
    }

    /// Convert formula to Negation Normal Form (NNF)
    ///
    /// In NNF, negations are applied only to propositions.
    /// This eliminates double negations and pushes negations inward using De Morgan's laws
    /// and temporal operator duality:
    /// - ¬(φ ∧ ψ) = ¬φ ∨ ¬ψ
    /// - ¬(φ ∨ ψ) = ¬φ ∧ ¬ψ
    /// - ¬G[a,b] φ = F[a,b] ¬φ
    /// - ¬F[a,b] φ = G[a,b] ¬φ
    /// - ¬(φ U[a,b] ψ) = ¬φ R[a,b] ¬ψ
    /// - ¬(φ R[a,b] ψ) = ¬φ U[a,b] ¬ψ
    pub fn to_nnf(self) -> Self {
        match self {
            MLTL::True => MLTL::True,
            MLTL::False => MLTL::False,
            MLTL::Prop(var) => MLTL::Prop(var),
            MLTL::And(left, right) => {
                MLTL::And(Box::new(left.to_nnf()), Box::new(right.to_nnf()))
            }
            MLTL::Or(left, right) => {
                MLTL::Or(Box::new(left.to_nnf()), Box::new(right.to_nnf()))
            }
            MLTL::Future(lb, ub, sub) => {
                MLTL::Future(lb, ub, Box::new(sub.to_nnf()))
            }
            MLTL::Global(lb, ub, sub) => {
                MLTL::Global(lb, ub, Box::new(sub.to_nnf()))
            }
            MLTL::Until(lb, ub, left, right) => {
                MLTL::Until(lb, ub, Box::new(left.to_nnf()), Box::new(right.to_nnf()))
            }
            MLTL::Release(lb, ub, left, right) => {
                MLTL::Release(lb, ub, Box::new(left.to_nnf()), Box::new(right.to_nnf()))
            }
            MLTL::Not(fml) => {
                match *fml {
                    MLTL::True => MLTL::False,
                    MLTL::False => MLTL::True,
                    MLTL::Prop(var) => MLTL::Not(Box::new(MLTL::Prop(var))),
                    MLTL::Not(sub) => sub.to_nnf(),
                    // De Morgan's Laws: ¬(φ ∧ ψ) = (¬φ ∨ ¬ψ)
                    MLTL::And(left, right) => {
                        MLTL::Or(
                            Box::new(MLTL::Not(left).to_nnf()),
                            Box::new(MLTL::Not(right).to_nnf())
                        )
                    }
                    // De Morgan's Laws: ¬(φ ∨ ψ) = (¬φ ∧ ¬ψ)
                    MLTL::Or(left, right) => {
                        MLTL::And(
                            Box::new(MLTL::Not(left).to_nnf()),
                            Box::new(MLTL::Not(right).to_nnf())
                        )
                    }
                    // Temporal duality: ¬F[a,b]φ = G[a,b]¬φ
                    MLTL::Future(lb, ub, sub) => {
                        MLTL::Global(lb, ub, Box::new(MLTL::Not(sub).to_nnf()))
                    }
                    // Temporal duality: ¬G[a,b]φ = F[a,b]¬φ
                    MLTL::Global(lb, ub, sub) => {
                        MLTL::Future(lb, ub, Box::new(MLTL::Not(sub).to_nnf()))
                    }
                    // Temporal duality: ¬(φ U[a,b] ψ) = ¬φ R[a,b] ¬ψ
                    MLTL::Until(lb, ub, left, right) => {
                        MLTL::Release(
                            lb, ub,
                            Box::new(MLTL::Not(left).to_nnf()),
                            Box::new(MLTL::Not(right).to_nnf())
                        )
                    }
                    // Temporal duality: ¬(φ R[a,b] ψ) = ¬φ U[a,b] ¬ψ
                    MLTL::Release(lb, ub, left, right) => {
                        MLTL::Until(
                            lb, ub,
                            Box::new(MLTL::Not(left).to_nnf()),
                            Box::new(MLTL::Not(right).to_nnf())
                        )
                    }
                }
            }
        }
    }

    /// Replace named propositions with integer indices using the provided mapping.
    ///
    /// Consumes `self` and produces an `MLTL<usize>` where every `Prop(name)` is
    /// replaced by `Prop(index)` via `var_map`. Panics if a proposition is not found
    /// in the map.
    pub fn to_indexed(self, var_map: &HashMap<T, usize>) -> MLTL<usize> {
        match self {
            MLTL::True => MLTL::True,
            MLTL::False => MLTL::False,
            MLTL::Prop(name) => {
                let idx = var_map[&name];
                MLTL::Prop(idx)
            }
            MLTL::Not(sub) => MLTL::Not(Box::new(sub.to_indexed(var_map))),
            MLTL::And(l, r) => {
                MLTL::And(Box::new(l.to_indexed(var_map)), Box::new(r.to_indexed(var_map)))
            }
            MLTL::Or(l, r) => {
                MLTL::Or(Box::new(l.to_indexed(var_map)), Box::new(r.to_indexed(var_map)))
            }
            MLTL::Global(lb, ub, sub) => {
                MLTL::Global(lb, ub, Box::new(sub.to_indexed(var_map)))
            }
            MLTL::Future(lb, ub, sub) => {
                MLTL::Future(lb, ub, Box::new(sub.to_indexed(var_map)))
            }
            MLTL::Until(lb, ub, l, r) => {
                MLTL::Until(lb, ub, Box::new(l.to_indexed(var_map)), Box::new(r.to_indexed(var_map)))
            }
            MLTL::Release(lb, ub, l, r) => {
                MLTL::Release(lb, ub, Box::new(l.to_indexed(var_map)), Box::new(r.to_indexed(var_map)))
            }
        }
    }

    /// Collect all unique proposition names in the formula, returning a map
    /// from name to a zero-based index (ordered by first occurrence via sorted order).
    pub fn collect_vars(&self) -> HashMap<T, usize>
    where
        T: Clone,
    {
        let mut vars = Vec::new();
        self.collect_vars_inner(&mut vars);
        vars.sort();
        vars.dedup();
        vars.into_iter().enumerate().map(|(i, v)| (v, i)).collect()
    }

    fn collect_vars_inner(&self, vars: &mut Vec<T>)
    where
        T: Clone,
    {
        match self {
            MLTL::True | MLTL::False => {}
            MLTL::Prop(name) => vars.push(name.clone()),
            MLTL::Not(sub) => sub.collect_vars_inner(vars),
            MLTL::And(l, r) | MLTL::Or(l, r) => {
                l.collect_vars_inner(vars);
                r.collect_vars_inner(vars);
            }
            MLTL::Global(_, _, sub) | MLTL::Future(_, _, sub) => {
                sub.collect_vars_inner(vars);
            }
            MLTL::Until(_, _, l, r) | MLTL::Release(_, _, l, r) => {
                l.collect_vars_inner(vars);
                r.collect_vars_inner(vars);
            }
        }
    }

    /// Print the parse tree in a nicely formatted hierarchical structure.
    pub fn print_parsetree(&self) {
        self.print_parsetree_indent(0);
    }

    fn print_parsetree_indent(&self, indent: usize) {
        let prefix = "  ".repeat(indent);
        match self {
            MLTL::True => println!("{}├─ True", prefix),
            MLTL::False => println!("{}├─ False", prefix),
            MLTL::Prop(p) => println!("{}├─ Prop({})", prefix, p),
            MLTL::Not(sub) => {
                println!("{}├─ Not", prefix);
                sub.print_parsetree_indent(indent + 1);
            }
            MLTL::And(left, right) => {
                println!("{}├─ And", prefix);
                println!("{}│  Left:", prefix);
                left.print_parsetree_indent(indent + 2);
                println!("{}│  Right:", prefix);
                right.print_parsetree_indent(indent + 2);
            }
            MLTL::Or(left, right) => {
                println!("{}├─ Or", prefix);
                println!("{}│  Left:", prefix);
                left.print_parsetree_indent(indent + 2);
                println!("{}│  Right:", prefix);
                right.print_parsetree_indent(indent + 2);
            }
            MLTL::Global(lb, ub, sub) => {
                println!("{}├─ Global[{}, {}]", prefix, lb, ub);
                sub.print_parsetree_indent(indent + 1);
            }
            MLTL::Future(lb, ub, sub) => {
                println!("{}├─ Future[{}, {}]", prefix, lb, ub);
                sub.print_parsetree_indent(indent + 1);
            }
            MLTL::Until(lb, ub, left, right) => {
                println!("{}├─ Until[{}, {}]", prefix, lb, ub);
                println!("{}│  Left:", prefix);
                left.print_parsetree_indent(indent + 2);
                println!("{}│  Right:", prefix);
                right.print_parsetree_indent(indent + 2);
            }
            MLTL::Release(lb, ub, left, right) => {
                println!("{}├─ Release[{}, {}]", prefix, lb, ub);
                println!("{}│  Left:", prefix);
                left.print_parsetree_indent(indent + 2);
                println!("{}│  Right:", prefix);
                right.print_parsetree_indent(indent + 2);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_nnf_literals() {
        let formula: MLTL<String> = MLTL::True;
        assert_eq!(formula.to_nnf(), MLTL::True);
        
        let formula: MLTL<String> = MLTL::False;
        assert_eq!(formula.to_nnf(), MLTL::False);
        
        let formula = MLTL::Prop("x".to_string());
        assert_eq!(formula.to_nnf(), MLTL::Prop("x".to_string()));
    }

    #[test]
    fn test_nnf_double_negation_elimination() {
        let formula = MLTL::Not(Box::new(
            MLTL::Not(Box::new(MLTL::Prop("x".to_string())))
        ));
        assert_eq!(formula.to_nnf(), MLTL::Prop("x".to_string()));
    }

    #[test]
    fn test_nnf_demorgan_and() {
        let formula = MLTL::Not(Box::new(
            MLTL::And(
                Box::new(MLTL::Prop("x".to_string())),
                Box::new(MLTL::Prop("y".to_string())),
            )
        ));
        let nnf = formula.to_nnf();
        assert_eq!(nnf, MLTL::Or(
            Box::new(MLTL::Not(Box::new(MLTL::Prop("x".to_string())))),
            Box::new(MLTL::Not(Box::new(MLTL::Prop("y".to_string())))),
        ));
    }

    #[test]
    fn test_nnf_demorgan_or() {
        let formula = MLTL::Not(Box::new(
            MLTL::Or(
                Box::new(MLTL::Prop("x".to_string())),
                Box::new(MLTL::Prop("y".to_string())),
            )
        ));
        let nnf = formula.to_nnf();
        assert_eq!(nnf, MLTL::And(
            Box::new(MLTL::Not(Box::new(MLTL::Prop("x".to_string())))),
            Box::new(MLTL::Not(Box::new(MLTL::Prop("y".to_string())))),
        ));
    }

    #[test]
    fn test_nnf_global_duality() {
        let formula = MLTL::Not(Box::new(
            MLTL::Global(0, 5, Box::new(MLTL::Prop("x".to_string())))
        ));
        let nnf = formula.to_nnf();
        assert_eq!(nnf, MLTL::Future(
            0, 5,
            Box::new(MLTL::Not(Box::new(MLTL::Prop("x".to_string()))))
        ));
    }

    #[test]
    fn test_nnf_future_duality() {
        let formula = MLTL::Not(Box::new(
            MLTL::Future(0, 5, Box::new(MLTL::Prop("x".to_string())))
        ));
        let nnf = formula.to_nnf();
        assert_eq!(nnf, MLTL::Global(
            0, 5,
            Box::new(MLTL::Not(Box::new(MLTL::Prop("x".to_string()))))
        ));
    }

    #[test]
    fn test_nnf_until_duality() {
        let formula = MLTL::Not(Box::new(
            MLTL::Until(
                0, 5,
                Box::new(MLTL::Prop("x".to_string())),
                Box::new(MLTL::Prop("y".to_string())),
            )
        ));
        let nnf = formula.to_nnf();
        assert_eq!(nnf, MLTL::Release(
            0, 5,
            Box::new(MLTL::Not(Box::new(MLTL::Prop("x".to_string())))),
            Box::new(MLTL::Not(Box::new(MLTL::Prop("y".to_string())))),
        ));
    }

    #[test]
    fn test_nnf_release_duality() {
        let formula = MLTL::Not(Box::new(
            MLTL::Release(
                0, 5,
                Box::new(MLTL::Prop("x".to_string())),
                Box::new(MLTL::Prop("y".to_string())),
            )
        ));
        let nnf = formula.to_nnf();
        assert_eq!(nnf, MLTL::Until(
            0, 5,
            Box::new(MLTL::Not(Box::new(MLTL::Prop("x".to_string())))),
            Box::new(MLTL::Not(Box::new(MLTL::Prop("y".to_string())))),
        ));
    }

    #[test]
    fn test_nnf_complex_formula() {
        let formula = MLTL::Not(Box::new(
            MLTL::And(
                Box::new(MLTL::Global(0, 5, Box::new(MLTL::Prop("x".to_string())))),
                Box::new(MLTL::Future(1, 3, Box::new(MLTL::Prop("y".to_string())))),
            )
        ));
        let nnf = formula.to_nnf();
        assert_eq!(nnf, MLTL::Or(
            Box::new(MLTL::Future(0, 5, Box::new(MLTL::Not(Box::new(MLTL::Prop("x".to_string())))))),
            Box::new(MLTL::Global(1, 3, Box::new(MLTL::Not(Box::new(MLTL::Prop("y".to_string())))))),
        ));
    }

    // ── collect_vars ─────────────────────────────────────────────────────

    #[test]
    fn test_collect_vars_single() {
        let f = MLTL::Prop("x".to_string());
        let vars = f.collect_vars();
        assert_eq!(vars.len(), 1);
        assert_eq!(vars["x"], 0);
    }

    #[test]
    fn test_collect_vars_multiple_sorted() {
        // Sorted order: x=0, y=1, z=2
        let f = MLTL::And(
            Box::new(MLTL::Prop("z".to_string())),
            Box::new(MLTL::Or(
                Box::new(MLTL::Prop("x".to_string())),
                Box::new(MLTL::Prop("y".to_string())),
            )),
        );
        let vars = f.collect_vars();
        assert_eq!(vars.len(), 3);
        assert_eq!(vars["x"], 0);
        assert_eq!(vars["y"], 1);
        assert_eq!(vars["z"], 2);
    }

    #[test]
    fn test_collect_vars_dedup() {
        let f = MLTL::And(
            Box::new(MLTL::Prop("x".to_string())),
            Box::new(MLTL::Prop("x".to_string())),
        );
        let vars = f.collect_vars();
        assert_eq!(vars.len(), 1);
    }

    #[test]
    fn test_collect_vars_no_props() {
        let f: MLTL<String> = MLTL::And(
            Box::new(MLTL::True),
            Box::new(MLTL::False),
        );
        assert!(f.collect_vars().is_empty());
    }

    // ── to_indexed ───────────────────────────────────────────────────────

    #[test]
    fn test_to_indexed_simple() {
        let f = MLTL::Prop("x".to_string());
        let vars = f.collect_vars();
        let f2 = MLTL::Prop("x".to_string());
        let indexed = f2.to_indexed(&vars);
        assert_eq!(indexed, MLTL::Prop(0));
    }

    #[test]
    fn test_to_indexed_complex() {
        let f = MLTL::And(
            Box::new(MLTL::Global(0, 5, Box::new(MLTL::Prop("y".to_string())))),
            Box::new(MLTL::Not(Box::new(MLTL::Prop("x".to_string())))),
        );
        let vars = f.collect_vars(); // x=0, y=1
        let indexed = f.to_indexed(&vars);
        assert_eq!(indexed, MLTL::And(
            Box::new(MLTL::Global(0, 5, Box::new(MLTL::Prop(1)))),
            Box::new(MLTL::Not(Box::new(MLTL::Prop(0)))),
        ));
    }

    #[test]
    fn test_to_indexed_preserves_structure() {
        let f = MLTL::Until(
            1, 3,
            Box::new(MLTL::Prop("a".to_string())),
            Box::new(MLTL::Prop("b".to_string())),
        );
        let vars = f.collect_vars(); // a=0, b=1
        let indexed = f.to_indexed(&vars);
        assert_eq!(indexed, MLTL::Until(
            1, 3,
            Box::new(MLTL::Prop(0)),
            Box::new(MLTL::Prop(1)),
        ));
    }
}
