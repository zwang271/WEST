//! Bit-vector regex representations for MLTL satisfying traces.
//!
//! Two-bit encoding per proposition per timestep:
//!   11 = don't-care (s), 10 = true (1), 01 = false (0), 00 = null (contradiction)
//!
//! - [`TraceRegex`]: One trace pattern (a single bitvector).
//! - [`WestRegex`]: A set of trace patterns (implicit disjunction).

use bitvec::prelude::*;
use std::fmt;
use std::ops;

// ─── TraceRegex ──────────────────────────────────────────────────────────────

/// A single trace pattern encoded as a bitvector with 2-bit-per-prop encoding.
///
/// Total bit length = `2 * num_vars * trace_len`.
/// Bit at index `2*num_vars*t + 2*k` is the high bit for prop `k` at timestep `t`;
/// the next bit (`+1`) is the low bit.
#[derive(Clone, Eq, PartialEq, Hash)]
pub struct TraceRegex {
    pub num_vars: usize,
    pub trace_len: usize,
    bits: BitVec<usize, Msb0>,
}

impl fmt::Debug for TraceRegex {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "TraceRegex({})", self.to_trace_string())
    }
}

impl TraceRegex {
    // ── Constructors ─────────────────────────────────────────────────────

    /// All don't-cares (`11` everywhere). Represents "any trace satisfies".
    pub fn all_stars(num_vars: usize, trace_len: usize) -> Self {
        let len = 2 * num_vars * trace_len;
        let mut bits = BitVec::with_capacity(len);
        bits.resize(len, true);
        Self { num_vars, trace_len, bits }
    }

    /// Prop `k` forced true at timestep 0 (bit `2k+1` cleared), rest don't-care.
    pub fn prop_true(k: usize, num_vars: usize, trace_len: usize) -> Self {
        let mut t = Self::all_stars(num_vars, trace_len);
        t.bits.set(2 * k + 1, false); // 10 = true
        t
    }

    /// Prop `k` forced false at timestep 0 (bit `2k` cleared), rest don't-care.
    pub fn prop_false(k: usize, num_vars: usize, trace_len: usize) -> Self {
        let mut t = Self::all_stars(num_vars, trace_len);
        t.bits.set(2 * k, false); // 01 = false
        t
    }

    /// Build from a raw bit string like `"11100110"`.
    /// Panics if `s.len() != 2 * num_vars * trace_len`.
    pub fn from_bitstring(s: &str, num_vars: usize, trace_len: usize) -> Self {
        let expected = 2 * num_vars * trace_len;
        assert_eq!(s.len(), expected, "bitstring length {} != expected {}", s.len(), expected);
        let mut bits = BitVec::<usize, Msb0>::with_capacity(expected);
        for ch in s.chars() {
            bits.push(ch == '1');
        }
        Self { num_vars, trace_len, bits }
    }

    // ── Queries ──────────────────────────────────────────────────────────

    /// Total number of raw bits.
    #[inline]
    pub fn bit_len(&self) -> usize {
        self.bits.len()
    }

    /// True if any 2-bit pair is `00` (contradiction).
    ///
    /// Optimized: works at the word level with early exit.
    #[inline]
    pub fn is_null(&self) -> bool {
        let raw = self.bits.as_raw_slice();
        let total_bits = self.bits.len();
        let bits_per_word = std::mem::size_of::<usize>() * 8;
        
        // Process full words
        let full_words = total_bits / bits_per_word;
        for &word in &raw[..full_words] {
            // Check if any aligned 2-bit pair is 00.
            // For each pair (bit i, bit i+1) where i is even in bitvec terms:
            // In Msb0, bitvec bit 0 = MSB of word, so pairs are at word bit positions
            // (63,62), (61,60), etc. - i.e., checking odd bit positions after OR-shift.
            // combined[odd_pos] = 0 means that pair was 00.
            let combined = word | (word >> 1);
            // MASK has 1s at odd bit positions (1,3,5,...,63) in standard LSB-0 numbering
            const MASK: usize = 0xAAAA_AAAA_AAAA_AAAA_u64 as usize;
            if (combined & MASK) != MASK {
                return true;
            }
        }
        
        // Handle remaining bits in the last partial word (if any)
        let remaining_bits = total_bits % bits_per_word;
        if remaining_bits > 0 {
            let last_word = raw[full_words];
            // Only check the top `remaining_bits` bits (Msb0 ordering)
            // Shift to align used bits to MSB, then apply same check
            let num_pairs = remaining_bits / 2;
            for p in 0..num_pairs {
                let bit_idx = p * 2;
                // In Msb0, bit 0 is at position (bits_per_word - 1) in the word
                let word_pos = bits_per_word - 1 - bit_idx;
                let hi = (last_word >> word_pos) & 1;
                let lo = (last_word >> (word_pos - 1)) & 1;
                if hi == 0 && lo == 0 {
                    return true;
                }
            }
        }
        false
    }

    /// Can `self` and `other` be merged via bitwise OR?
    ///
    /// True when their XOR has either:
    /// - fewer than 2 set bits (identical or single-bit difference), or
    /// - exactly 2 set bits forming an aligned pair (positions `2k, 2k+1`).
    ///
    /// Optimized: computes XOR word-by-word without allocation.
    #[inline]
    pub fn can_simplify_with(&self, other: &TraceRegex) -> bool {
        debug_assert_eq!(self.bit_len(), other.bit_len());
        
        let self_raw = self.bits.as_raw_slice();
        let other_raw = other.bits.as_raw_slice();
        
        let mut total_diff = 0usize;
        let mut first_diff_word_idx = None;
        let mut first_diff_word_xor = 0usize;
        
        for (i, (&a, &b)) in self_raw.iter().zip(other_raw.iter()).enumerate() {
            let xor = a ^ b;
            let count = xor.count_ones() as usize;
            if count > 0 {
                total_diff += count;
                if total_diff > 2 {
                    return false; // Early exit
                }
                if first_diff_word_idx.is_none() {
                    first_diff_word_idx = Some(i);
                    first_diff_word_xor = xor;
                }
            }
        }
        
        if total_diff < 2 {
            return true;
        }
        
        // Exactly 2 differing bits - check if they're an aligned pair
        if total_diff == 2 {
            // Both bits must be in the same word for aligned pair
            if first_diff_word_xor.count_ones() == 2 {
                // Find the two bit positions within the word
                let first_bit = first_diff_word_xor.trailing_zeros() as usize;
                let remaining = first_diff_word_xor & !(1usize << first_bit);
                let second_bit = remaining.trailing_zeros() as usize;
                
                // Convert to global bit position and check alignment
                let bits_per_word = std::mem::size_of::<usize>() * 8;
                let word_idx = first_diff_word_idx.unwrap();
                // In Msb0 ordering, bit 0 of bitvec is at the MSB of word 0
                let global_first = word_idx * bits_per_word + (bits_per_word - 1 - second_bit);
                let global_second = word_idx * bits_per_word + (bits_per_word - 1 - first_bit);
                
                // Check if they form an aligned 2-bit pair (positions 2k, 2k+1)
                return global_first % 2 == 0 && global_second == global_first + 1;
            }
        }
        false
    }

    // ── Display ──────────────────────────────────────────────────────────

    /// Convert to the `s`/`1`/`0` trace string with commas between timesteps.
    pub fn to_trace_string(&self) -> String {
        let n = self.num_vars;
        let mut out = String::with_capacity(self.trace_len * (n + 1));
        for t in 0..self.trace_len {
            if t > 0 {
                out.push(',');
            }
            for k in 0..n {
                let hi = self.bits[2 * n * t + 2 * k];
                let lo = self.bits[2 * n * t + 2 * k + 1];
                out.push(match (hi, lo) {
                    (true, true) => 's',
                    (true, false) => '1',
                    (false, true) => '0',
                    (false, false) => '_', // null marker
                });
            }
        }
        out
    }

    /// Raw bits as a `0`/`1` string (for debugging / tests).
    pub fn to_bitstring(&self) -> String {
        self.bits.iter().map(|b| if *b { '1' } else { '0' }).collect()
    }

    // ── Shift ────────────────────────────────────────────────────────────

    /// Shift constraint to a later timestep by `m` bit positions.
    ///
    /// The original constraint (at timestep 0) is moved to appear `m` bits later,
    /// and the vacated positions (at the front) are filled with `1`s (don't-care).
    ///
    /// Uses the flip → shift_right → flip trick: shift_right fills vacated
    /// positions with 0, so we flip before and after to get 1s instead.
    pub fn shift_left_ones(&self, m: usize) -> TraceRegex {
        if m == 0 {
            return self.clone();
        }
        let len = self.bits.len();
        // flip, shift right (moves bits toward higher indices), flip back
        let mut bits = !self.bits.clone();
        bits.shift_right(m);
        bits = !bits;
        // Ensure length stays the same
        bits.resize(len, true);
        TraceRegex {
            num_vars: self.num_vars,
            trace_len: self.trace_len,
            bits,
        }
    }

    /// Compute bitwise AND with another trace, returning None if the result is null.
    ///
    /// This is more efficient than `a & b` followed by `is_null()` because it:
    /// 1. Checks for null during computation (early exit)
    /// 2. Avoids allocation entirely if result is null
    #[inline]
    pub fn and_if_not_null(&self, other: &TraceRegex) -> Option<TraceRegex> {
        debug_assert_eq!(self.bit_len(), other.bit_len());
        
        let self_raw = self.bits.as_raw_slice();
        let other_raw = other.bits.as_raw_slice();
        let bits_per_word = std::mem::size_of::<usize>() * 8;
        const MASK: usize = 0xAAAA_AAAA_AAAA_AAAA_u64 as usize;
        
        // First pass: check if result would be null (without allocating)
        let total_bits = self.bits.len();
        let full_words = total_bits / bits_per_word;
        
        for i in 0..full_words {
            let and_word = self_raw[i] & other_raw[i];
            let combined = and_word | (and_word >> 1);
            if (combined & MASK) != MASK {
                return None; // Would produce null
            }
        }
        
        // Handle remaining bits
        let remaining_bits = total_bits % bits_per_word;
        if remaining_bits > 0 && full_words < self_raw.len() {
            let and_word = self_raw[full_words] & other_raw[full_words];
            let num_pairs = remaining_bits / 2;
            for p in 0..num_pairs {
                let bit_idx = p * 2;
                let word_pos = bits_per_word - 1 - bit_idx;
                let hi = (and_word >> word_pos) & 1;
                let lo = (and_word >> (word_pos - 1)) & 1;
                if hi == 0 && lo == 0 {
                    return None;
                }
            }
        }
        
        // Not null - now allocate and compute
        Some(TraceRegex {
            num_vars: self.num_vars,
            trace_len: self.trace_len,
            bits: self.bits.clone() & other.bits.clone(),
        })
    }
}

// Implement `&` operator for TraceRegex (bitwise AND).
impl ops::BitAnd for &TraceRegex {
    type Output = TraceRegex;

    #[inline]
    fn bitand(self, rhs: &TraceRegex) -> TraceRegex {
        debug_assert_eq!(self.bit_len(), rhs.bit_len());
        TraceRegex {
            num_vars: self.num_vars,
            trace_len: self.trace_len,
            bits: self.bits.clone() & rhs.bits.clone(),
        }
    }
}

// Implement `|` operator for TraceRegex (bitwise OR / merge).
impl ops::BitOr for &TraceRegex {
    type Output = TraceRegex;

    #[inline]
    fn bitor(self, rhs: &TraceRegex) -> TraceRegex {
        debug_assert_eq!(self.bit_len(), rhs.bit_len());
        TraceRegex {
            num_vars: self.num_vars,
            trace_len: self.trace_len,
            bits: self.bits.clone() | rhs.bits.clone(),
        }
    }
}

// ─── WestRegex ───────────────────────────────────────────────────────────────

/// A set of [`TraceRegex`] patterns, representing their implicit disjunction.
///
/// Invariants maintained by all operations:
/// 1. Every trace has the same `num_vars` and `trace_len`.
/// 2. No trace is null (contains a `00` pair).
/// 3. The set is always simplified (no two traces pass `can_simplify_with`).
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct WestRegex {
    pub num_vars: usize,
    pub trace_len: usize,
    traces: Vec<TraceRegex>,
}

impl WestRegex {
    /// Empty regex: no satisfying traces (equivalent to `false`).
    pub fn empty(num_vars: usize, trace_len: usize) -> Self {
        Self { num_vars, trace_len, traces: Vec::new() }
    }

    /// Single all-stars trace (equivalent to `true`).
    pub fn all(num_vars: usize, trace_len: usize) -> Self {
        Self {
            num_vars,
            trace_len,
            traces: vec![TraceRegex::all_stars(num_vars, trace_len)],
        }
    }

    /// Build from a pre-existing vec of traces (applies simplify).
    pub fn from_traces(num_vars: usize, trace_len: usize, traces: Vec<TraceRegex>) -> Self {
        let mut w = Self { num_vars, trace_len, traces };
        w.simplify();
        w
    }

    /// Number of trace patterns in the set.
    #[inline]
    pub fn len(&self) -> usize {
        self.traces.len()
    }

    /// True if there are no traces (unsatisfiable).
    #[inline]
    pub fn is_empty(&self) -> bool {
        self.traces.is_empty()
    }

    /// Immutable access to the trace patterns.
    pub fn traces(&self) -> &[TraceRegex] {
        &self.traces
    }

    // ── SIMP ─────────────────────────────────────────────────────────────

    /// Simplify in place: repeatedly merge any pair of traces that differ
    /// in at most one aligned 2-bit position.
    ///
    /// Uses an iterative fixed-point loop instead of recursion.
    pub fn simplify(&mut self) {
        'restart: loop {
            let n = self.traces.len();
            for i in 0..n {
                for j in (i + 1)..n {
                    if self.traces[i].can_simplify_with(&self.traces[j]) {
                        // Merge j into i via bitwise OR, then remove j.
                        let merged = &self.traces[i] | &self.traces[j];
                        self.traces[i] = merged;
                        self.traces.swap_remove(j);
                        continue 'restart;
                    }
                }
            }
            break; // No more merges found — fixed point reached.
        }
    }

    // ── AND ──────────────────────────────────────────────────────────────

    /// AND (conjunction): Cartesian product of traces with bitwise AND.
    /// Null results are discarded. The result is simplified.
    pub fn and(&self, other: &WestRegex) -> WestRegex {
        debug_assert_eq!(self.num_vars, other.num_vars);
        debug_assert_eq!(self.trace_len, other.trace_len);

        let mut result_traces = Vec::with_capacity(self.traces.len() * other.traces.len());
        for a in &self.traces {
            for b in &other.traces {
                // Fused AND + null check: avoids allocation for null results
                if let Some(c) = a.and_if_not_null(b) {
                    result_traces.push(c);
                }
            }
        }
        let mut result = WestRegex {
            num_vars: self.num_vars,
            trace_len: self.trace_len,
            traces: result_traces,
        };
        result.simplify();
        result
    }

    // ── OR ───────────────────────────────────────────────────────────────

    /// OR (disjunction): set union followed by simplification.
    pub fn or(&self, other: &WestRegex) -> WestRegex {
        debug_assert_eq!(self.num_vars, other.num_vars);
        debug_assert_eq!(self.trace_len, other.trace_len);

        let mut combined = self.traces.clone();
        combined.extend_from_slice(&other.traces);
        let mut result = WestRegex {
            num_vars: self.num_vars,
            trace_len: self.trace_len,
            traces: combined,
        };
        result.simplify();
        result
    }

    // ── SHIFT ────────────────────────────────────────────────────────────

    /// Shift all traces left by `m` bits, padding vacated positions with `1`s.
    ///
    /// Used to place a subformula's trace at a specific timestep offset.
    pub fn shift(&self, m: usize) -> WestRegex {
        let shifted_traces = self.traces.iter().map(|t| t.shift_left_ones(m)).collect();
        WestRegex {
            num_vars: self.num_vars,
            trace_len: self.trace_len,
            traces: shifted_traces,
        }
    }
}

impl fmt::Display for WestRegex {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if self.traces.is_empty() {
            return write!(f, "(empty)");
        }
        for (i, tr) in self.traces.iter().enumerate() {
            if i > 0 {
                writeln!(f)?;
            }
            write!(f, "{}", tr.to_trace_string())?;
        }
        Ok(())
    }
}

// ─── Tests ───────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    // Shorthand: build a TraceRegex from a bitstring with given (n, cl).
    fn tr(s: &str, n: usize, cl: usize) -> TraceRegex {
        TraceRegex::from_bitstring(s, n, cl)
    }

    // ── TraceRegex basics ────────────────────────────────────────────────

    #[test]
    fn test_all_stars() {
        let t = TraceRegex::all_stars(2, 3);
        assert_eq!(t.bit_len(), 12);
        assert_eq!(t.to_bitstring(), "111111111111");
        assert_eq!(t.to_trace_string(), "ss,ss,ss");
    }

    #[test]
    fn test_prop0_true() {
        // n=2, cl=1. Prop 0 true → bits: 10 11 (p0=true, p1=don't care)
        let t = TraceRegex::prop_true(0, 2, 1);
        assert_eq!(t.to_bitstring(), "1011");
        assert_eq!(t.to_trace_string(), "1s");
    }

    #[test]
    fn test_prop0_true_len3() {
        // n=2, cl=3. Prop 0 true → bits: 10 11 10 11 10 11 (p0=true, p1=don't care)
        let t = TraceRegex::prop_true(0, 2, 3);
        assert_eq!(t.to_bitstring(), "101111111111");
        assert_eq!(t.to_trace_string(), "1s,ss,ss");
    }

    #[test]
    fn test_prop1true() {
        // n=2, cl=1. Prop 1 true → bits: 11 10 (p0=don't care, p1=true)
        let t = TraceRegex::prop_true(1, 2, 1);
        assert_eq!(t.to_bitstring(), "1110");
        assert_eq!(t.to_trace_string(), "s1");
    }

    #[test]
    fn test_prop1_true_len3() {
        // n=2, cl=3. Prop 1 true → bits: 11 10 11 10 11 10 (p0=don't care, p1=true)
        let t = TraceRegex::prop_true(1, 2, 3);
        assert_eq!(t.to_bitstring(), "111011111111");
        assert_eq!(t.to_trace_string(), "s1,ss,ss");
    }

    #[test]
    fn test_prop0_false() {
        // n=2, cl=1. Prop 0 false → bits: 01 11 (p0=false, p1=don't care)
        let t = TraceRegex::prop_false(0, 2, 1);
        assert_eq!(t.to_bitstring(), "0111");
        assert_eq!(t.to_trace_string(), "0s");
    }

    #[test]
    fn test_prop0_false_len3() {
        // n=2, cl=3. Prop 0 false → bits: 01 11 01 11 01 11 (p0=false, p1=don't care)
        let t = TraceRegex::prop_false(0, 2, 3);
        assert_eq!(t.to_bitstring(), "011111111111");
        assert_eq!(t.to_trace_string(), "0s,ss,ss");
    }

    #[test]
    fn test_prop1_false() {
        // n=2, cl=1. Prop 1 false → bits: 11 01 (p0=don't care, p1=false)
        let t = TraceRegex::prop_false(1, 2, 1);
        assert_eq!(t.to_bitstring(), "1101");
        assert_eq!(t.to_trace_string(), "s0");
    }

    #[test]
    fn test_prop1_false_len3() {
        // n=2, cl=3. Prop 1 false → bits: 11 01 11 01 11 01 (p0=don't care, p1=false)
        let t = TraceRegex::prop_false(1, 2, 3);
        assert_eq!(t.to_bitstring(), "110111111111");
        assert_eq!(t.to_trace_string(), "s0,ss,ss");
    }

    #[test]
    fn test_trace_display() {
        // For "s1,0s,11" with n=2, cl=3:
        //   s=11, 1=10, 0=01, s=11, 1=10, 1=10
        //   → 111001111010  (12 bits)
        let t = tr("111001111010", 2, 3);
        assert_eq!(t.to_trace_string(), "s1,0s,11");
    }

    // ── is_null ──────────────────────────────────────────────────────────

    #[test]
    fn test_is_null_false() {
        let t = TraceRegex::all_stars(2, 2);
        assert!(!t.is_null());
    }

    #[test]
    fn test_is_null_true() {
        // 00 in first pair → null
        let t = tr("0011", 1, 2);
        assert!(t.is_null());
    }

    #[test]
    fn test_is_null_second_pair() {
        // First pair ok (11), second pair null (00)
        let t = tr("1100", 1, 2);
        assert!(t.is_null());
    }

    #[test]
    fn test_not_is_null() {
        // Offset null: first pair is 10 (true), second pair is 01 (false) → not null
        let t = tr("1001", 1, 2);
        assert!(!t.is_null());
    }

    #[test]
    fn test_is_null_all_valid_types() {
        // 11 10 01 = s, 1, 0 — no nulls
        let t = tr("111001", 3, 1);
        assert!(!t.is_null());
    }

    // ── Bitwise AND (TraceRegex & TraceRegex) ────────────────────────────

    #[test]
    fn test_trace_and_basic() {
        // 1110 & 1101 = 1100 → null (p1 is 00)
        let a = tr("1110", 1, 2); // p0: s, 1
        let b = tr("1101", 1, 2); // p0: s, 0
        let c = &a & &b;
        // 1110 & 1101 = 1100: pair0 = 11(s), pair1 = 00(null)
        assert_eq!(c.to_bitstring(), "1100");
        assert!(c.is_null());
    }

    #[test]
    fn test_trace_and_compatible() {
        // 1011 & 1110 = 1010 → p0=10(1), p1=10(1)
        let a = tr("1011", 2, 1); // p0=1, p1=s
        let b = tr("1110", 2, 1); // p0=s, p1=1
        let c = &a & &b;
        assert_eq!(c.to_bitstring(), "1010");
        assert!(!c.is_null());
        assert_eq!(c.to_trace_string(), "11");
    }

    #[test]
    fn test_trace_and_identity() {
        let a = tr("10110110", 2, 2);
        let stars = TraceRegex::all_stars(2, 2);
        assert_eq!(&a & &stars, a);
    }

    // ── Bitwise OR (TraceRegex | TraceRegex) ─────────────────────────────

    #[test]
    fn test_trace_or_merge() {
        // 10 | 01 = 11 = s
        let a = tr("10", 1, 1); // "1"
        let b = tr("01", 1, 1); // "0"
        let c = &a | &b;
        assert_eq!(c.to_trace_string(), "s");
    }

    // ── can_simplify_with ────────────────────────────────────────────────

    #[test]
    fn test_can_simplify_identical() {
        let a = tr("1011", 2, 1);
        assert!(a.can_simplify_with(&a));
    }

    #[test]
    fn test_can_simplify_one_bit_diff() {
        // Differ in a single bit → yes (xor count = 1 < 2)
        let a = tr("1011", 2, 1);
        let b = tr("1111", 2, 1);
        assert!(a.can_simplify_with(&b));
    }

    #[test]
    fn test_can_simplify_aligned_pair() {
        // Differ exactly in aligned pair (bits 2,3) → yes
        let a = tr("1110", 2, 1); // p0=s, p1=1
        let b = tr("1101", 2, 1); // p0=s, p1=0
        assert!(a.can_simplify_with(&b));
    }

    #[test]
    fn test_cannot_simplify_unaligned() {
        // Differ in 2 bits but NOT aligned pair
        let a = tr("1100", 2, 1); // p0=s, p1=null... (doesn't matter for the check)
        let b = tr("1001", 2, 1);
        // xor = 0101, positions 1 and 3 → not an aligned pair (1 is odd)
        assert!(!a.can_simplify_with(&b));
    }

    #[test]
    fn test_cannot_simplify_three_diffs() {
        let a = tr("111111", 3, 1);
        let b = tr("100100", 3, 1);
        // xor has 4 bits set
        assert!(!a.can_simplify_with(&b));
    }

    // ── WestRegex SIMP ───────────────────────────────────────────────────

    #[test]
    fn test_simplify_two_traces_merge() {
        // n=1, cl=1. Traces: "1" (10) and "0" (01). Should merge to "s" (11).
        let a = tr("10", 1, 1);
        let b = tr("01", 1, 1);
        let w = WestRegex::from_traces(1, 1, vec![a, b]);
        assert_eq!(w.len(), 1);
        assert_eq!(w.traces()[0].to_trace_string(), "s");
    }

    #[test]
    fn test_simplify_no_merge() {
        // n=2, cl=1. Traces: "10" (1001) and "01" (0110) — differ in 4 bits, can't merge.
        let a = tr("1001", 2, 1);
        let b = tr("0110", 2, 1);
        let w = WestRegex::from_traces(2, 1, vec![a, b]);
        assert_eq!(w.len(), 2);
    }

    #[test]
    fn test_simplify_chain() {
        // n=1, cl=2. Four traces that should chain-merge:
        // "1,1" = 10 10
        // "1,0" = 10 01
        // "0,1" = 01 10
        // "0,0" = 01 01
        // Step 1: "1,1" + "1,0" → "1,s" (10 11), "0,1" + "0,0" → "0,s" (01 11)
        // Step 2: "1,s" + "0,s" → "s,s" (11 11)
        let traces = vec![
            tr("1010", 1, 2),
            tr("1001", 1, 2),
            tr("0110", 1, 2),
            tr("0101", 1, 2),
        ];
        let w = WestRegex::from_traces(1, 2, traces);
        assert_eq!(w.len(), 1);
        assert_eq!(w.traces()[0].to_trace_string(), "s,s");
    }

    // ── WestRegex AND ────────────────────────────────────────────────────

    #[test]
    fn test_west_and_basic() {
        // n=2, cl=1.
        // W1 = {"1s"} (prop 0 true)          = {1011}
        // W2 = {"s0"} (prop 1 false)          = {1101}
        // W1 AND W2 = {1011 & 1101} = {1001}  = {"10"} (p0=true, p1=false)
        let w1 = WestRegex::from_traces(2, 1, vec![TraceRegex::prop_true(0, 2, 1)]);
        let w2 = WestRegex::from_traces(2, 1, vec![TraceRegex::prop_false(1, 2, 1)]);
        let result = w1.and(&w2);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "10");
    }

    #[test]
    fn test_west_and_null_discarded() {

        // n=1, cl=1.
        // W1 = {"1"} = {10}
        // W2 = {"0"} = {01}
        // AND: 10 & 01 = 00 → null → discarded → empty result
        let w1 = WestRegex::from_traces(1, 1, vec![tr("10", 1, 1)]);
        let w2 = WestRegex::from_traces(1, 1, vec![tr("01", 1, 1)]);
        let result = w1.and(&w2);
        assert!(result.is_empty());
    }

    #[test]
    fn test_west_and_cross_product() {
        // n=1, cl=2.
        // W1 = {"1,s", "0,s"} = {1011, 0111}  (essentially "s,s" if simplified,
        //    but let's force them unsimplifiable by using n=2)
        //
        // Actually, let's do a cleaner example:
        // n=2, cl=1.
        // W1 = {"1s", "0s"} = {1011, 0111}       ← these CAN simplify to "ss"
        // Let me pick ones that can't simplify.
        //
        // W1 = {"10"} = {1001}   (p0=1, p1=0)
        // W2 = {"s1", "s0"} = {1110, 1101}
        // Cross product:
        //   1001 & 1110 = 1000 → null (p1=00) → discard
        //   1001 & 1101 = 1001 → "10" → keep
        // Result: {"10"} = {1001}
        let w1 = WestRegex::from_traces(2, 1, vec![tr("1001", 2, 1)]);
        let w2 = WestRegex::from_traces(2, 1, vec![tr("1110", 2, 1), tr("1101", 2, 1)]);
        let result = w1.and(&w2);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "10");
    }

    #[test]
    fn test_west_and_with_simplify() {
        // n=1, cl=2.
        // W1 = {"s,s"} = {1111}
        // W2 = {"1,s", "0,s"} = {1011, 0111}
        // Cross:
        //   1111 & 1011 = 1011 → "1,s"
        //   1111 & 0111 = 0111 → "0,s"
        // These can simplify: xor = 1100, two bits at aligned pair (0,1) → merge
        // Merged: 1011 | 0111 = 1111 → "s,s"
        let w1 = WestRegex::all(1, 2);
        let w2 = WestRegex::from_traces(1, 2, vec![tr("1011", 1, 2), tr("0111", 1, 2)]);
        let result = w1.and(&w2);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s,s");
    }

    // ── WestRegex OR ─────────────────────────────────────────────────────

    #[test]
    fn test_west_or_basic() {
        // n=1, cl=1.
        // {"1"} OR {"0"} → simplify → {"s"}
        let w1 = WestRegex::from_traces(1, 1, vec![tr("10", 1, 1)]);
        let w2 = WestRegex::from_traces(1, 1, vec![tr("01", 1, 1)]);
        let result = w1.or(&w2);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "s");
    }

    #[test]
    fn test_west_or_no_merge() {
        // n=2, cl=1.
        // {"10"} (1001) OR {"01"} (0110) — differ in 4 bits → no merge
        let w1 = WestRegex::from_traces(2, 1, vec![tr("1001", 2, 1)]);
        let w2 = WestRegex::from_traces(2, 1, vec![tr("0110", 2, 1)]);
        let result = w1.or(&w2);
        assert_eq!(result.len(), 2);
    }

    #[test]
    fn test_west_and_empty() {
        // AND with empty (false) gives empty
        let w1 = WestRegex::from_traces(1, 1, vec![tr("10", 1, 1)]);
        let w2 = WestRegex::empty(1, 1);
        assert!(w1.and(&w2).is_empty());
    }

    #[test]
    fn test_west_or_with_empty() {
        // OR with empty is identity
        let w1 = WestRegex::from_traces(1, 1, vec![tr("10", 1, 1)]);
        let w2 = WestRegex::empty(1, 1);
        let result = w1.or(&w2);
        assert_eq!(result.len(), 1);
        assert_eq!(result.traces()[0].to_trace_string(), "1");
    }

    // ── Larger integration-style tests ───────────────────────────────────

    #[test]
    fn test_and_then_or() {
        // n=2, cl=1.
        // (p0=1 AND p1=0) OR (p0=0 AND p1=1)
        // = {"10"} OR {"01"}
        let p0t = WestRegex::from_traces(2, 1, vec![TraceRegex::prop_true(0, 2, 1)]);
        let p1f = WestRegex::from_traces(2, 1, vec![TraceRegex::prop_false(1, 2, 1)]);
        let p0f = WestRegex::from_traces(2, 1, vec![TraceRegex::prop_false(0, 2, 1)]);
        let p1t = WestRegex::from_traces(2, 1, vec![TraceRegex::prop_true(1, 2, 1)]);

        let left = p0t.and(&p1f);   // {"10"}
        let right = p0f.and(&p1t);  // {"01"}
        let result = left.or(&right);

        // "10" = 1001, "01" = 0110 → differ in 4 bits → no merge
        assert_eq!(result.len(), 2);
    }

    // ── TraceRegex shift tests ───────────────────────────────────────────

    #[test]
    fn test_trace_shift_zero() {
        let t = tr("1011", 2, 1);
        let shifted = t.shift_left_ones(0);
        assert_eq!(shifted.to_bitstring(), "1011");
    }

    #[test]
    fn test_trace_shift_basic() {
        // "10 11" (1,s) → shift by 2 → "11 10" (s,1)
        let t = tr("1011", 1, 2);
        let shifted = t.shift_left_ones(2);
        assert_eq!(shifted.to_bitstring(), "1110");
        assert_eq!(shifted.to_trace_string(), "s,1");
    }

    #[test]
    fn test_trace_shift_full_timestep() {
        // "10 11 11" (1,s,s) → shift by 4 → "11 11 10" (s,s,1)
        let t = tr("101111", 1, 3);
        let shifted = t.shift_left_ones(4);
        assert_eq!(shifted.to_bitstring(), "111110");
        assert_eq!(shifted.to_trace_string(), "s,s,1");
    }

    #[test]
    fn test_trace_shift_multivar() {
        // n=2: "10 11" (1s at t0) → shift by 4 → "11 11 10 11" wouldn't fit
        // With n=2, cl=2: bits = 8
        // "10 11 11 11" (1s,ss) → shift by 4 → "11 11 10 11" (ss,1s)
        let t = tr("10111111", 2, 2);
        let shifted = t.shift_left_ones(4);
        assert_eq!(shifted.to_bitstring(), "11111011");
        assert_eq!(shifted.to_trace_string(), "ss,1s");
    }

    // ── WestRegex shift tests ────────────────────────────────────────────

    #[test]
    fn test_west_shift_all_traces() {
        // Create WestRegex with 2 traces that can't simplify (differ in >2 bits or non-aligned)
        // With n=2, cl=1: "10" (1001) and "01" (0110) differ in 4 bits
        let t1 = tr("1001", 2, 1);  // "10"
        let t2 = tr("0110", 2, 1);  // "01"
        let w = WestRegex::from_traces(2, 1, vec![t1.clone(), t2.clone()]);
        assert_eq!(w.len(), 2);  // Can't simplify
        
        // Now with n=2, cl=2 for shifting
        let t1_long = tr("10011111", 2, 2);  // "10,ss"
        let t2_long = tr("01101111", 2, 2);  // "01,ss"
        let w2 = WestRegex::from_traces(2, 2, vec![t1_long, t2_long]);
        assert_eq!(w2.len(), 2);  // Still can't simplify
        
        let shifted = w2.shift(4);  // Shift by 1 timestep (4 bits)
        // Both should be shifted: "ss,10" and "ss,01"
        assert_eq!(shifted.len(), 2);
    }

    #[test]
    fn test_west_shift_preserves_simplification() {
        // Shifting shouldn't add simplifiable pairs that weren't there before
        let t1 = tr("1011", 1, 2);  // "1,s"
        let t2 = tr("0111", 1, 2);  // "0,s"
        // These can simplify to "s,s" before shifting
        let w = WestRegex::from_traces(1, 2, vec![t1, t2]);
        assert_eq!(w.len(), 1);  // Pre-simplified to "s,s"
        
        let shifted = w.shift(2);
        // After shift, "s,s" stays "s,s" (all 1s shifted → still all 1s at the relevant portion)
        assert_eq!(shifted.len(), 1);
    }
}
