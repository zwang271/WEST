//! WEST - Metric Linear Temporal Logic (MLTL) Parser and Compiler
//!
//! This library provides:
//! - A robust parser for flexible MLTL formula syntax
//! - NNF (Negation Normal Form) conversion
//! - Compilation to bit-vector regex expressions
//!
//! ```

pub mod mltl;      // MLTL datatype + NNF conversion
pub mod parse;     // Parser for MLTL syntax
pub mod regex;     // BitVecRegex operations
pub mod west;      // MLTL → WestRegex compilation

pub use mltl::MLTL;
pub use regex::{TraceRegex, WestRegex};
pub use west::{compile, compile_with_context, reg};

