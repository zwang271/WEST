//! Compiler from MLTL formulas to bit-vector regex expressions
//!
//! This module orchestrates the compilation pipeline:
//! MLTL formula → NNF → indexed → WestRegex representation

use std::collections::HashMap;
use crate::MLTL;
use crate::regex::{TraceRegex, WestRegex};

/// Compile an MLTL formula to a WestRegex representation.
///
/// Pipeline:
/// 1. Convert formula to Negation Normal Form (NNF)
/// 2. Collect propositional variables and assign indices
/// 3. Convert to indexed formula (MLTL<usize>)
/// 4. Compile indexed NNF to WestRegex via `reg()`
pub fn compile(formula: MLTL<String>) -> WestRegex {
    // Step 1: Convert to NNF
    let nnf = formula.to_nnf();
    
    // Step 2: Collect variables
    let var_map = nnf.collect_vars();
    let num_vars = var_map.len();
    
    // Step 3: Compute trace length and convert to indexed
    let trace_len = nnf.complen();
    let indexed = nnf.to_indexed(&var_map);
    
    // Step 4: Compile to WestRegex
    if num_vars == 0 {
        // Formula has no propositions (just true/false)
        return reg(&indexed, 1, trace_len);
    }
    reg(&indexed, num_vars, trace_len)
}

/// Compile an MLTL formula using a pre-defined variable mapping and trace length.
///
/// This is useful for compiling subformulas while maintaining consistency
/// with the parent formula's variable indices.
///
/// # Arguments
/// * `formula` - The formula to compile
/// * `var_map` - Variable name to index mapping (from parent formula)
/// * `num_vars` - Total number of variables (from parent formula)
/// * `trace_len` - Computation length (from parent formula)
pub fn compile_with_context(
    formula: MLTL<String>,
    var_map: &HashMap<String, usize>,
    num_vars: usize,
    trace_len: usize,
) -> WestRegex {
    let nnf = formula.to_nnf();
    let indexed = nnf.to_indexed(var_map);
    
    if num_vars == 0 {
        return reg(&indexed, 1, trace_len);
    }
    reg(&indexed, num_vars, trace_len)
}

/// Recursively compile an MLTL<usize> formula (in NNF) to a WestRegex.
///
/// # Arguments
/// * `formula` - The NNF formula with integer-indexed propositions
/// * `num_vars` - Number of propositional variables (n)
/// * `trace_len` - Total computation length (cl = complen of top-level formula)
///
/// # Returns
/// A WestRegex representing all satisfying computation traces.
pub fn reg(formula: &MLTL<usize>, num_vars: usize, trace_len: usize) -> WestRegex {
    match formula {
        // ── Base cases ───────────────────────────────────────────────────
        MLTL::True => WestRegex::all(num_vars, trace_len),
        
        MLTL::False => WestRegex::empty(num_vars, trace_len),
        
        MLTL::Prop(k) => {
            // Positive prop: force prop k to be true at timestep 0
            WestRegex::from_traces(
                num_vars,
                trace_len,
                vec![TraceRegex::prop_true(*k, num_vars, trace_len)],
            )
        }
        
        // In NNF, Not only wraps Prop
        MLTL::Not(sub) => {
            match sub.as_ref() {
                MLTL::Prop(k) => {
                    // Negative prop: force prop k to be false at timestep 0
                    WestRegex::from_traces(
                        num_vars,
                        trace_len,
                        vec![TraceRegex::prop_false(*k, num_vars, trace_len)],
                    )
                }
                _ => panic!("In NNF, Not should only wrap Prop"),
            }
        }
        
        // ── Boolean connectives ──────────────────────────────────────────
        MLTL::Or(left, right) => {
            let reg_l = reg(left, num_vars, trace_len);
            let reg_r = reg(right, num_vars, trace_len);
            reg_l.or(&reg_r)
        }
        
        MLTL::And(left, right) => {
            let reg_l = reg(left, num_vars, trace_len);
            let reg_r = reg(right, num_vars, trace_len);
            reg_l.and(&reg_r)
        }
        
        // ── Unary temporal operators ─────────────────────────────────────
        
        // F[a,b] φ: φ holds at some timestep in [a,b]
        // = OR over shift(reg(φ), i * 2 * n) for i in a..=b
        MLTL::Future(lb, ub, sub) => {
            let reg_sub = reg(sub, num_vars, trace_len);
            let bits_per_step = 2 * num_vars;
            
            (*lb..=*ub).map(|i| reg_sub.shift(i * bits_per_step))
                        .reduce(|acc, x| acc.or(&x))
                        .unwrap_or_else(|| WestRegex::empty(num_vars, trace_len))
        }
        
        // G[a,b] φ: φ holds at every timestep in [a,b]
        // = AND over shift(reg(φ), i * 2 * n) for i in a..=b
        MLTL::Global(lb, ub, sub) => {
            let reg_sub = reg(sub, num_vars, trace_len);
            let bits_per_step = 2 * num_vars;
            
            (*lb..=*ub).map(|i| reg_sub.shift(i * bits_per_step))
                        .reduce(|acc, x| acc.and(&x))
                        .unwrap_or_else(|| WestRegex::all(num_vars, trace_len))
        }
        
        // ── Binary temporal operators ────────────────────────────────────
        
        // φ U[a,b] ψ: ψ holds at some i in [a,b], and φ holds at all j in [a,i)
        // Base case (i=a): shift(reg(ψ), a * 2n)
        // For i in a+1..=b: G[a,i-1]φ AND G[i,i]ψ
        MLTL::Until(lb, ub, left, right) => {
            let bits_per_step = 2 * num_vars;
            let reg_left = reg(left, num_vars, trace_len);
            let reg_right = reg(right, num_vars, trace_len);
            
            // Base case: ψ at timestep lb
            let mut result = reg_right.shift(*lb * bits_per_step);
            
            // For each i in lb+1..=ub: φ at [lb, i-1] AND ψ at i
            for i in (*lb + 1)..=*ub {
                // G[lb, i-1] φ: AND of shift(reg_left, j*2n) for j in lb..=i-1
                let g_left = (*lb..i)
                    .map(|j| reg_left.shift(j * bits_per_step))
                    .reduce(|acc, x| acc.and(&x))
                    .unwrap_or_else(|| WestRegex::all(num_vars, trace_len));
                
                // G[i, i] ψ = shift(reg_right, i*2n)
                let g_right = reg_right.shift(i * bits_per_step);
                
                result = result.or(&g_left.and(&g_right));
            }
            result
        }
        
        // φ R[a,b] ψ: ψ holds at [a,b], OR (φ at some i in [a,b-1] AND ψ at [a,i])
        // Base case: G[a,b] ψ
        // For i in a..=b-1: G[i,i]φ AND G[a,i]ψ
        MLTL::Release(lb, ub, left, right) => {
            let bits_per_step = 2 * num_vars;
            let reg_left = reg(left, num_vars, trace_len);
            let reg_right = reg(right, num_vars, trace_len);
            
            // Base case: G[a,b] ψ = AND of shift(reg_right, i*2n) for i in lb..=ub
            let mut result = (*lb..=*ub)
                .map(|i| reg_right.shift(i * bits_per_step))
                .reduce(|acc, x| acc.and(&x))
                .unwrap_or_else(|| WestRegex::all(num_vars, trace_len));
            
            // For each i in lb..ub (i.e., lb..=ub-1): φ at i AND ψ at [lb, i]
            // When ub == 0 or ub <= lb, this range is empty and we skip.
            if *ub > 0 {
                for i in *lb..(*ub) {
                    // G[i, i] φ = shift(reg_left, i*2n)
                    let g_left = reg_left.shift(i * bits_per_step);
                    
                    // G[a, i] ψ: AND of shift(reg_right, j*2n) for j in lb..=i
                    let g_right = (*lb..=i)
                        .map(|j| reg_right.shift(j * bits_per_step))
                        .reduce(|acc, x| acc.and(&x))
                        .unwrap_or_else(|| WestRegex::all(num_vars, trace_len));
                    
                    result = result.or(&g_left.and(&g_right));
                }
            }
            result
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::MLTL;

    // ── Basic propositional tests ────────────────────────────────────────

    #[test]
    fn test_reg_true() {
        let formula = MLTL::<usize>::True;
        let result = reg(&formula, 1, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s");
    }

    #[test]
    fn test_reg_false() {
        let formula = MLTL::<usize>::False;
        let result = reg(&formula, 1, 1);
        assert!(result.is_empty());
    }

    #[test]
    fn test_reg_prop_true() {
        // p0 at t=0 should be true
        let formula = MLTL::Prop(0);
        let result = reg(&formula, 1, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1");
    }

    #[test]
    fn test_reg_prop_false() {
        // !p0 at t=0 should be false
        let formula = MLTL::Not(Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "0");
    }

    #[test]
    fn test_reg_two_props() {
        // p0 & p1 at t=0: both true
        let formula = MLTL::And(
            Box::new(MLTL::Prop(0)),
            Box::new(MLTL::Prop(1)),
        );
        let result = reg(&formula, 2, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "11");
    }

    #[test]
    fn test_reg_or_props() {
        // p0 | !p0: should simplify to "s" (true)
        let formula = MLTL::Or(
            Box::new(MLTL::Prop(0)),
            Box::new(MLTL::Not(Box::new(MLTL::Prop(0)))),
        );
        let result = reg(&formula, 1, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s");
    }

    #[test]
    fn test_reg_and_contradiction() {
        // p0 & !p0: should be empty (false)
        let formula = MLTL::And(
            Box::new(MLTL::Prop(0)),
            Box::new(MLTL::Not(Box::new(MLTL::Prop(0)))),
        );
        let result = reg(&formula, 1, 1);
        assert!(result.is_empty());
    }

    // ── Global tests ─────────────────────────────────────────────────────

    #[test]
    fn test_reg_global_single_step() {
        // G[0,0] p0 = p0 at t=0
        let formula = MLTL::Global(0, 0, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1");
    }

    #[test]
    fn test_reg_global_two_steps() {
        // G[0,1] p0 = p0 at t=0 AND p0 at t=1
        let formula = MLTL::Global(0, 1, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 2);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1,1");
    }

    #[test]
    fn test_reg_global_three_steps() {
        // G[0,2] p0 = p0 at t=0,1,2
        let formula = MLTL::Global(0, 2, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 3);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1,1,1");
    }

    #[test]
    fn test_reg_global_offset() {
        // G[1,2] p0 = p0 at t=1,2 (t=0 don't care)
        let formula = MLTL::Global(1, 2, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 3);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s,1,1");
    }

    // ── Future tests ─────────────────────────────────────────────────────

    #[test]
    fn test_reg_future_single_step() {
        // F[0,0] p0 = p0 at t=0
        let formula = MLTL::Future(0, 0, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1");
    }

    #[test]
    fn test_reg_future_two_steps() {
        // F[0,1] p0 = p0 at t=0 OR p0 at t=1
        // Traces: "1,s" (1011) and "s,1" (1110)
        // XOR = 0101, positions 1,3 → NOT aligned (need 0,1 or 2,3)
        // So these do NOT simplify - we get 2 traces
        let formula = MLTL::Future(0, 1, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 2);
        assert_eq!(result.len(), 2);
    }

    #[test]
    fn test_reg_future_offset() {
        // F[1,2] p0 = p0 at t=1 OR p0 at t=2
        // Traces: "s,1,s" (11 10 11) and "s,s,1" (11 11 10)
        // XOR = 00 01 01, positions 3,5 → NOT aligned
        // Result: 2 traces
        let formula = MLTL::Future(1, 2, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 3);
        assert_eq!(result.len(), 2);
    }

    // ── Until tests ──────────────────────────────────────────────────────

    #[test]
    fn test_reg_until_base_case() {
        // p0 U[0,0] p1 = p1 at t=0 (no waiting needed)
        let formula = MLTL::Until(0, 0, 
            Box::new(MLTL::Prop(0)), 
            Box::new(MLTL::Prop(1)),
        );
        let result = reg(&formula, 2, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s1");
    }

    #[test]
    fn test_reg_until_two_steps() {
        // p0 U[0,1] p1 = p1@0 | (p0@0 & p1@1)
        // = "s1" | "1s,s1" → with n=2: "s1" at cl=1 would need extending
        // Actually with cl=2: "s1,ss" | "1s,s1"
        let formula = MLTL::Until(0, 1, 
            Box::new(MLTL::Prop(0)), 
            Box::new(MLTL::Prop(1)),
        );
        let result = reg(&formula, 2, 2);
        // "s1,ss" = 0110_1111 and "1s,s1" = 1011_0110 differ in multiple bits
        assert_eq!(result.len(), 2);
    }

    // ── Release tests ────────────────────────────────────────────────────

    #[test]
    fn test_reg_release_base_case() {
        // p0 R[0,0] p1 = G[0,0] p1 = p1 at t=0
        let formula = MLTL::Release(0, 0, 
            Box::new(MLTL::Prop(0)), 
            Box::new(MLTL::Prop(1)),
        );
        let result = reg(&formula, 2, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s1");
    }

    #[test]
    fn test_reg_release_two_steps() {
        // p0 R[0,1] p1 = G[0,1]p1 | (p0@0 & p1@0)
        // = "s1,s1" | "11,ss"
        let formula = MLTL::Release(0, 1, 
            Box::new(MLTL::Prop(0)), 
            Box::new(MLTL::Prop(1)),
        );
        let result = reg(&formula, 2, 2);
        // "s1,s1" and "11,ss" differ in multiple non-aligned positions
        assert_eq!(result.len(), 2);
    }

    // ── End-to-end compile tests ─────────────────────────────────────────

    #[test]
    fn test_compile_simple_prop() {
        let formula = MLTL::parse("p0").unwrap();
        let result = compile(formula);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1");
    }

    #[test]
    fn test_compile_global() {
        let formula = MLTL::parse("G[0,2]p0").unwrap();
        let result = compile(formula);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1,1,1");
    }

    #[test]
    fn test_compile_future() {
        let formula = MLTL::parse("F[0,2]p0").unwrap();
        let result = compile(formula);
        // F[0,2]p0 produces 3 traces: "1,s,s", "s,1,s", "s,s,1"
        // None of these can simplify (XOR bits not aligned pairs)
        assert_eq!(result.len(), 3);
    }

    #[test]
    fn test_compile_negation_to_nnf() {
        // !G[0,1]p0 = F[0,1]!p0
        let formula = MLTL::parse("!G[0,1]p0").unwrap();
        let result = compile(formula);
        // F[0,1]!p0 = !p0@0 | !p0@1 = "0,s" | "s,0"
        // XOR = 10 10, positions 0,2 → NOT aligned
        assert_eq!(result.len(), 2);
    }

    #[test]
    fn test_compile_multiple_vars() {
        // p0 & p1
        let formula = MLTL::parse("(p0 & p1)").unwrap();
        let result = compile(formula);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "11");
    }

    #[test]
    fn test_compile_true() {
        let formula = MLTL::parse("true").unwrap();
        let result = compile(formula);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s");
    }

    #[test]
    fn test_compile_false() {
        let formula = MLTL::parse("false").unwrap();
        let result = compile(formula);
        assert!(result.is_empty());
    }

    // ── Shift operation tests ────────────────────────────────────────────

    #[test]
    fn test_shift_zero() {
        // Shifting by 0 should be identity
        let formula = MLTL::Prop(0);
        let result = reg(&formula, 1, 2);
        let shifted = result.shift(0);
        assert_eq!(result.traces()[0].to_trace_string(), shifted.traces()[0].to_trace_string());
    }

    #[test]
    fn test_shift_one_timestep() {
        // Shift prop to timestep 1 in a 2-timestep trace
        // p0 at t=0: "1,s" → shift by 2 bits → "s,1"
        let formula = MLTL::Prop(0);
        let result = reg(&formula, 1, 2);
        let shifted = result.shift(2);  // 2*1 = 2 bits per timestep
        assert_eq!(shifted.traces()[0].to_trace_string(), "s,1");
    }

    #[test]
    fn test_shift_two_timesteps() {
        // Shift prop to timestep 2 in a 3-timestep trace
        let formula = MLTL::Prop(0);
        let result = reg(&formula, 1, 3);
        let shifted = result.shift(4);  // 2 timesteps * 2 bits
        assert_eq!(shifted.traces()[0].to_trace_string(), "s,s,1");
    }

    #[test]
    fn test_shift_multiple_vars() {
        // With 2 vars, shift by 1 timestep (4 bits)
        // p0 at t=0 with n=2: "1s,ss" → shift → "ss,1s"
        let formula = MLTL::Prop(0);
        let result = reg(&formula, 2, 2);
        let shifted = result.shift(4);  // 2*2 = 4 bits per timestep
        assert_eq!(shifted.traces()[0].to_trace_string(), "ss,1s");
    }

    // ── Complex formula tests ────────────────────────────────────────────

    #[test]
    fn test_global_and_or() {
        // G[0,1](p0 | p1)
        let formula = MLTL::Global(0, 1, Box::new(
            MLTL::Or(Box::new(MLTL::Prop(0)), Box::new(MLTL::Prop(1)))
        ));
        let result = reg(&formula, 2, 2);
        // At each timestep, at least one of p0,p1 must be true
        // This expands to several traces since p0|p1 at each position has multiple options
        assert!(!result.is_empty());
    }

    #[test]
    fn test_future_and() {
        // F[0,1](p0 & p1)
        let formula = MLTL::Future(0, 1, Box::new(
            MLTL::And(Box::new(MLTL::Prop(0)), Box::new(MLTL::Prop(1)))
        ));
        let result = reg(&formula, 2, 2);
        // Either "11,ss" or "ss,11"
        assert_eq!(result.len(), 2);
    }

    #[test]
    fn test_nested_global() {
        // G[0,1]G[0,0]p0 = G[0,1]p0
        let inner = MLTL::Global(0, 0, Box::new(MLTL::Prop(0)));
        let formula = MLTL::Global(0, 1, Box::new(inner));
        let result = reg(&formula, 1, 2);
        // G[0,0]p0 = p0@0. G[0,1](p0@0) = (p0@0)@0 AND (p0@0)@1 = p0@0 AND p0@1
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1,1");
    }

    #[test]
    fn test_until_longer_interval() {
        // p0 U[0,2] p1: p1 eventually at 0,1, or 2; p0 holds until then
        let formula = MLTL::Until(0, 2, 
            Box::new(MLTL::Prop(0)), 
            Box::new(MLTL::Prop(1))
        );
        let result = reg(&formula, 2, 3);
        // Multiple traces for different scenarios:
        // - p1@0: "s1,ss,ss"
        // - p0@0 & p1@1: "1s,s1,ss"
        // - p0@0 & p0@1 & p1@2: "1s,1s,s1"
        assert_eq!(result.len(), 3);
    }

    #[test]
    fn test_release_longer_interval() {
        // p0 R[0,2] p1: p1 holds throughout [0,2], OR p0 releases before that
        let formula = MLTL::Release(0, 2, 
            Box::new(MLTL::Prop(0)), 
            Box::new(MLTL::Prop(1))
        );
        let result = reg(&formula, 2, 3);
        // Multiple traces:
        // - G[0,2]p1: "s1,s1,s1"
        // - p0@0 & p1@0: "11,ss,ss"
        // - p0@1 & G[0,1]p1: "s1,11,ss"
        assert_eq!(result.len(), 3);
    }

    // ── End-to-end compile tests with complex formulas ───────────────────

    #[test]
    fn test_compile_until() {
        let formula = MLTL::parse("(p0 U[0,1] p1)").unwrap();
        let result = compile(formula);
        assert_eq!(result.len(), 2);
    }

    #[test]
    fn test_compile_release() {
        let formula = MLTL::parse("(p0 R[0,1] p1)").unwrap();
        let result = compile(formula);
        assert_eq!(result.len(), 2);
    }

    #[test]
    fn test_compile_demorgan() {
        // !(p0 & p1) = !p0 | !p1
        let formula = MLTL::parse("!(p0 & p1)").unwrap();
        let result = compile(formula);
        // !p0 | !p1: "0s" | "s0"
        // XOR = 01 01, positions 1,3 - not aligned with n=2 (would need 0,1 or 2,3)
        // Wait, with n=2, positions are: 0,1 for p0, 2,3 for p1
        // "0s" = 01 11, "s0" = 11 01
        // XOR = 10 10, positions 0,2 - not aligned
        assert_eq!(result.len(), 2);
    }

    #[test]
    fn test_compile_nested_temporal() {
        // G[0,1]F[0,0]p0 = G[0,1]p0
        let formula = MLTL::parse("G[0,1]F[0,0]p0").unwrap();
        let result = compile(formula);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1,1");
    }

    #[test]
    fn test_compile_complex_nested() {
        // (p0 -> G[0,1]p1) is the same as (!p0 | G[0,1]p1)
        // In terms of MLTL parser, we'd write this as (!p0 | G[0,1]p1)
        let formula = MLTL::parse("(!p0 | G[0,1]p1)").unwrap();
        let result = compile(formula);
        // Traces: !p0 gives "0s,ss" and G[0,1]p1 gives "s1,s1"
        // These don't simplify
        assert!(!result.is_empty());
    }

    // ── Edge cases ───────────────────────────────────────────────────────

    #[test]
    fn test_single_timestep_global() {
        // G[2,2]p0 with trace_len=3
        let formula = MLTL::Global(2, 2, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 3);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s,s,1");
    }

    #[test]
    fn test_single_timestep_future() {
        // F[2,2]p0 with trace_len=3
        let formula = MLTL::Future(2, 2, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 3);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s,s,1");
    }

    #[test]
    fn test_empty_and() {
        // p0 & false = false
        let formula = MLTL::And(Box::new(MLTL::Prop(0)), Box::new(MLTL::False));
        let result = reg(&formula, 1, 1);
        assert!(result.is_empty());
    }

    #[test]
    fn test_identity_or() {
        // p0 | true = true
        let formula = MLTL::Or(Box::new(MLTL::Prop(0)), Box::new(MLTL::True));
        let result = reg(&formula, 1, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s");
    }

    #[test]
    fn test_many_variables() {
        // Test with 4 variables: p0 & p1 & p2 & p3
        let formula = MLTL::And(
            Box::new(MLTL::And(Box::new(MLTL::Prop(0)), Box::new(MLTL::Prop(1)))),
            Box::new(MLTL::And(Box::new(MLTL::Prop(2)), Box::new(MLTL::Prop(3)))),
        );
        let result = reg(&formula, 4, 1);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1111");
    }

    #[test]
    fn test_larger_bounds() {
        // G[0,4]p0 with 5 timesteps
        let formula = MLTL::Global(0, 4, Box::new(MLTL::Prop(0)));
        let result = reg(&formula, 1, 5);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1,1,1,1,1");
    }
}