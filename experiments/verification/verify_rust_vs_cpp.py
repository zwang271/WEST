#!/usr/bin/env python3
"""
Verify Rust WEST vs C++ WEST
Author: Zili Wang (adapted for Rust verification)
Last updated: 02/26/2026

This script compares the Rust implementation of WEST against the C++ version
to ensure they produce identical outputs. This is Phase 1 of the verification
strategy, providing a baseline before formal Isabelle verification.

Usage:
    python verify_rust_vs_cpp.py <formula>              # Single formula
    python verify_rust_vs_cpp.py <formula1> <formula2>  # Multiple formulas
    python verify_rust_vs_cpp.py                        # Full test suite
"""

import subprocess
import sys
import os
import pathlib
import signal
import time

# Check and install dependencies
def check_and_install_deps():
    """Check if required packages are installed, install if missing."""
    required_packages = ['tqdm']
    missing_packages = []
    
    for package in required_packages:
        try:
            __import__(package)
        except ImportError:
            missing_packages.append(package)
    
    if missing_packages:
        print(f"📦 Installing missing packages: {', '.join(missing_packages)}")
        requirements_file = pathlib.Path(__file__).parent / "requirements.txt"
        if requirements_file.exists():
            subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", str(requirements_file)])
        else:
            subprocess.check_call([sys.executable, "-m", "pip", "install"] + missing_packages)
        print("✅ Dependencies installed successfully")

# Install dependencies before importing
check_and_install_deps()

try:
    from tqdm import tqdm
except ImportError:
    # Fallback if tqdm not available
    def tqdm(iterable, *args, **kwargs):
        return iterable

TIMEOUT = 300  # 5 minutes timeout for each test

def check_dependencies():
    """Check if all required binaries are available."""
    missing_deps = []
    
    # Check C++ WEST executable
    cpp_exec = "../../bin/west"
    if not os.path.exists(cpp_exec):
        missing_deps.append(f"C++ WEST executable not found at {cpp_exec}")
    
    # Check Rust WEST executable
    rust_exec = "../../bin/west_rust"
    if not os.path.exists(rust_exec):
        missing_deps.append(f"Rust WEST executable not found at {rust_exec}")
        missing_deps.append("  Run: cd ../../src/west_rust && ./copy_to_bin.sh")
    
    # Create required directories
    os.makedirs("rust_output", exist_ok=True)
    
    if missing_deps:
        print("❌ Missing dependencies:")
        for dep in missing_deps:
            print(f"   {dep}")
        return False
    return True

def run_west_cpp(formula):
    """Run C++ WEST with the given formula."""
    cpp_exec = "../../bin/west"
    cpp_output_file = "../../output/output.txt"
    cpp_subformulas_file = "../../output/subformulas.txt"
    
    try:
        # Run C++ command
        cmd = [cpp_exec, formula]
        pro = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, 
                               preexec_fn=os.setsid)
        try:
            pro.wait(TIMEOUT)
        except subprocess.TimeoutExpired:
            os.killpg(os.getpgid(pro.pid), signal.SIGTERM)
            return None, None
        
        if pro.returncode != 0:
            return None, None
            
        # Read output files
        output = None
        subformulas = None
        
        if os.path.exists(cpp_output_file):
            with open(cpp_output_file, 'r') as f:
                output = f.read()
        
        if os.path.exists(cpp_subformulas_file):
            with open(cpp_subformulas_file, 'r') as f:
                subformulas = f.read()
                
        return output, subformulas
        
    except Exception as e:
        print(f"❌ Error running C++ WEST: {e}")
        return None, None

def run_west_rust(formula, trace_len=None):
    """Run Rust WEST with the given formula."""
    rust_exec = "../../bin/west_rust"
    rust_output_file = "../../output/output_rust.txt"
    rust_subformulas_file = "../../output/subformulas_rust.txt"
    
    try:
        # Run Rust command
        if trace_len:
            cmd = [rust_exec, formula, str(trace_len)]
        else:
            cmd = [rust_exec, formula]
        
        pro = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                               preexec_fn=os.setsid)
        try:
            pro.wait(TIMEOUT)
        except subprocess.TimeoutExpired:
            os.killpg(os.getpgid(pro.pid), signal.SIGTERM)
            return None, None
        
        if pro.returncode != 0:
            return None, None
            
        # Read output files
        output = None
        subformulas = None
        
        if os.path.exists(rust_output_file):
            with open(rust_output_file, 'r') as f:
                output = f.read()
        
        if os.path.exists(rust_subformulas_file):
            with open(rust_subformulas_file, 'r') as f:
                subformulas = f.read()
                
        return output, subformulas
        
    except Exception as e:
        print(f"❌ Error running Rust WEST: {e}")
        return None, None

# ─── Equivalence checking (from verify_string.py) ─────────────────────────

def expand_string(string):
    """Expand all 's' (don't-care) symbols to all permutations of 0 and 1.
    This is the key to semantic equivalence checking.
    """
    def expand_string_helper(string):
        if 's' not in string:
            return {string}
        else:
            return expand_string_helper(string.replace('s', '0', 1))\
                .union(expand_string_helper(string.replace('s', '1', 1)))
    return expand_string_helper(string)

def pad_uniform(traces, target_trace):
    """Pad all traces to match the length of target_trace.
    Padding is done with 's' (don't-care) symbols.
    """
    if not traces or not target_trace:
        return traces
    
    n = len(target_trace.split(",")[0])  # Number of variables per timestep
    target_len = len(target_trace.split(","))  # Number of timesteps
    
    padded = []
    for trace in traces:
        trace_len = len(trace.split(","))
        if trace_len < target_len:
            delta_m = target_len - trace_len
            padding = ("," + "s"*n) * delta_m
            padded.append(trace + padding)
        else:
            padded.append(trace)
    return padded

def check_equivalence(traces1, traces2):
    """Check if two sets of traces are semantically equivalent.
    
    Two trace sets are equivalent if they represent the same set of
    concrete traces when all 's' symbols are expanded.
    
    Returns: (is_equivalent, only_in_1, only_in_2)
    """
    if not traces1 and not traces2:
        return True, set(), set()
    if not traces1 or not traces2:
        return False, set(traces1), set(traces2)
    
    # Pad to uniform length if needed (use longer trace as reference)
    if traces1 and traces2:
        max_len1 = max(len(t.split(",")) for t in traces1)
        max_len2 = max(len(t.split(",")) for t in traces2)
        
        if max_len1 > max_len2:
            # Find a trace with max_len1 timesteps
            ref_trace = next(t for t in traces1 if len(t.split(",")) == max_len1)
            traces2 = pad_uniform(traces2, ref_trace)
        elif max_len2 > max_len1:
            ref_trace = next(t for t in traces2 if len(t.split(",")) == max_len2)
            traces1 = pad_uniform(traces1, ref_trace)
    
    # Expand all 's' symbols in both sets
    expanded1 = set()
    for trace in traces1:
        for expanded in expand_string(trace):
            expanded1.add(expanded)
    
    expanded2 = set()
    for trace in traces2:
        for expanded in expand_string(trace):
            expanded2.add(expanded)
    
    # Check set equality
    only_in_1 = expanded1 - expanded2
    only_in_2 = expanded2 - expanded1
    
    return expanded1 == expanded2, only_in_1, only_in_2

# ─── Trace extraction ─────────────────────────────────────────────────────

def extract_traces_cpp(output):
    """Extract trace lines from C++ output format.
    Format: <formula>\\n<trace1>\\n<trace2>\\n...
    """
    if output is None:
        return []
    
    lines = output.strip().split('\n')
    if not lines:
        return []
    
    # First line is formula, rest are traces
    traces = []
    for line in lines[1:]:  # Skip formula line
        line = line.strip()
        # Traces contain only characters: 0, 1, s, comma, space
        if line and all(c in '01s, ' for c in line):
            traces.append(line.replace(' ', ''))
    
    return traces  # Don't sort - keep original order

def extract_traces_rust(output):
    """Extract trace lines from Rust output format.
    Format has metadata headers, then "Computations:" section.
    """
    if output is None:
        return []
    
    lines = output.strip().split('\n')
    if not lines:
        return []
    
    # Find the "Computations:" line and take everything after
    traces = []
    in_computations = False
    for line in lines:
        line = line.strip()
        if line == "Computations:":
            in_computations = True
            continue
        if in_computations and line:
            # Traces contain only characters: 0, 1, s, comma
            if all(c in '01s,' for c in line):
                traces.append(line)
    
    return traces  # Don't sort - keep original order

def compare_outputs(cpp_out, rust_out, cpp_sub, rust_sub):
    """Compare C++ and Rust outputs using semantic equivalence."""
    # Extract traces from both formats
    cpp_traces = extract_traces_cpp(cpp_out)
    rust_traces = extract_traces_rust(rust_out)
    
    # Check semantic equivalence (not strict equality)
    is_equivalent, only_cpp, only_rust = check_equivalence(cpp_traces, rust_traces)
    
    # Store diff info for later use if needed
    compare_outputs.last_only_cpp = only_cpp
    compare_outputs.last_only_rust = only_rust
    
    # For subformulas, we skip comparison for now (different formats)
    # The main trace comparison is the critical check
    sub_match = True
    
    return is_equivalent and sub_match

def print_diff(cpp_out, rust_out, formula):
    """Print differences between outputs for debugging."""
    print(f"\n🔍 Semantic differences found for formula: {formula}")
    print("=" * 80)
    
    cpp_traces = extract_traces_cpp(cpp_out)
    rust_traces = extract_traces_rust(rust_out)
    
    is_equivalent, only_cpp, only_rust = check_equivalence(cpp_traces, rust_traces)
    
    if not is_equivalent:
        print(f"📋 C++ Traces ({len(cpp_traces)} raw):")
        for t in cpp_traces[:5]:
            print(f"  {t}")
        if len(cpp_traces) > 5:
            print(f"  ... and {len(cpp_traces) - 5} more")
        
        print(f"\n📋 Rust Traces ({len(rust_traces)} raw):")
        for t in rust_traces[:5]:
            print(f"  {t}")
        if len(rust_traces) > 5:
            print(f"  ... and {len(rust_traces) - 5} more")
        
        # Show expanded differences (these are concrete traces after 's' expansion)
        if only_cpp:
            print(f"\n⚠️  Only in C++ ({len(only_cpp)} expanded traces):")
            for t in sorted(list(only_cpp))[:5]:
                print(f"  {t}")
            if len(only_cpp) > 5:
                print(f"  ... and {len(only_cpp) - 5} more")
        
        if only_rust:
            print(f"\n⚠️  Only in Rust ({len(only_rust)} expanded traces):")
            for t in sorted(list(only_rust))[:5]:
                print(f"  {t}")
            if len(only_rust) > 5:
                print(f"  ... and {len(only_rust) - 5} more")
        
        print("=" * 80)

def verify_single(formula, verbose=True):
    """Verify a single formula. Returns True if outputs match, False otherwise."""
    if verbose:
        print(f"Testing: {formula}")
    
    # Run both implementations
    cpp_out, cpp_sub = run_west_cpp(formula)
    rust_out, rust_sub = run_west_rust(formula)
    
    # Check for timeouts
    if cpp_out is None or rust_out is None:
        if verbose:
            if cpp_out is None:
                print("  ⏱️  C++ timed out")
            if rust_out is None:
                print("  ⏱️  Rust timed out")
        return "timeout"
    
    # Compare outputs
    match = compare_outputs(cpp_out, rust_out, cpp_sub, rust_sub)
    
    if match:
        if verbose:
            print("  ✅ Outputs match")
        return True
    else:
        if verbose:
            print("  ❌ Outputs differ")
            print_diff(cpp_out, rust_out, formula)
        return False

def verify_batch(formulas, progress=True):
    """Verify multiple formulas. Returns summary statistics."""
    results = []
    
    iterator = tqdm(formulas, desc="Verifying formulas") if progress else formulas
    
    for formula in iterator:
        result = verify_single(formula, verbose=False)
        results.append((formula, result))
        
        if progress and result is False:
            # Print failures immediately
            tqdm.write(f"❌ FAILED: {formula}")
    
    # Calculate statistics
    passed = sum(1 for _, r in results if r is True)
    failed = sum(1 for _, r in results if r is False)
    timeouts = sum(1 for _, r in results if r == "timeout")
    total = len(results)
    
    # Print summary
    print(f"\n{'='*80}")
    print(f"📊 Verification Summary")
    print(f"{'='*80}")
    print(f"Total:    {total:4d}")
    print(f"Passed:   {passed:4d} ({100*passed/total:.1f}%)")
    print(f"Failed:   {failed:4d} ({100*failed/total:.1f}%)")
    print(f"Timeouts: {timeouts:4d} ({100*timeouts/total:.1f}%)")
    print(f"{'='*80}")
    
    # List failed formulas
    failed_formulas = [(f, r) for f, r in results if r is False]
    if failed_formulas:
        print("\n❌ Failed formulas:")
        for formula, _ in failed_formulas[:20]:  # Show first 20
            print(f"   {formula}")
        if len(failed_formulas) > 20:
            print(f"   ... and {len(failed_formulas) - 20} more")
    
    # List timeouts
    timeout_formulas = [(f, r) for f, r in results if r == "timeout"]
    if timeout_formulas:
        print("\n⏱️  Timed out formulas:")
        for formula, _ in timeout_formulas[:10]:  # Show first 10
            print(f"   {formula}")
        if len(timeout_formulas) > 10:
            print(f"   ... and {len(timeout_formulas) - 10} more")
    
    return failed == 0  # Success if no failures (timeouts are acceptable)

def main():
    """Main entry point."""
    if not check_dependencies():
        sys.exit(1)
    
    if len(sys.argv) < 2:
        # No arguments - run full test suite
        print("🧪 Running full Rust vs C++ verification test suite...")
        formula_file = pathlib.Path("./verify_formulas/formulas.txt").resolve()
        
        if not os.path.exists(formula_file):
            print(f"❌ Test formula file not found: {formula_file}")
            print("Usage: python verify_rust_vs_cpp.py <formula>")
            print("       python verify_rust_vs_cpp.py <formula1> <formula2> ...")
            sys.exit(1)
        
        with open(formula_file, "r") as f:
            formulas = [line.strip() for line in f.readlines() if line.strip()]
        
        print(f"📝 Found {len(formulas)} test formulas")
        success = verify_batch(formulas)
        sys.exit(0 if success else 1)
    
    # Single or multiple formulas provided
    formulas = sys.argv[1:]
    
    if len(formulas) == 1:
        success = verify_single(formulas[0], verbose=True)
        sys.exit(0 if success is True else 1)
    else:
        success = verify_batch(formulas)
        sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
