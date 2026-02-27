# This script compares performance of bin/west (C++ bitoptimized) vs bin/west_rust (Rust)
# Runs both implementations on the same formulas and records timing for each
# Usage: python3 benchmark_compare.py -dir <directory>

import sys
import subprocess
import signal
import time
from tqdm import tqdm
import os
import argparse

# Paths to the binaries (relative to this script's location)
WEST_CPP = "../../bin/west"
WEST_RUST = "../../bin/west_rust"

# Output files used by each implementation
OUTPUT_CPP = "../../output/output.txt"
OUTPUT_RUST = "../../output/output_rust.txt"

def run_west(formula: str, binary: str, output_file: str, timelimit: int = 120):
    """Run a WEST implementation on a formula and return the time taken."""
    start = time.perf_counter()
    formula = formula.replace("a", "p")
    
    # Get absolute path to the binary
    abs_binary = os.path.abspath(binary)
    bin_dir = os.path.dirname(abs_binary)
    
    cmd = f'{abs_binary} "{formula}"'
    pro = subprocess.Popen(cmd, 
                           shell=True, 
                           stdout=subprocess.DEVNULL,
                           stderr=subprocess.DEVNULL,
                           preexec_fn=os.setsid,
                           cwd=bin_dir)
    try:
        pro.wait(timelimit)
    except subprocess.TimeoutExpired:
        os.killpg(os.getpgid(pro.pid), signal.SIGTERM)
        return timelimit, -1
    
    elapsed = time.perf_counter() - start
    
    # Count output lines
    output_path = os.path.join(os.path.dirname(__file__), output_file)
    try:
        with open(output_path, "r") as f:
            output_length = len(f.readlines()) - 1  # skip first line (formula)
    except FileNotFoundError:
        output_length = -1
    
    return elapsed, output_length

def runall_compare(formulas: list[str], timelimit: int = 120):
    """Run both C++ and Rust implementations on all formulas."""
    cpp_times = []
    rust_times = []
    cpp_outputs = []
    rust_outputs = []
    
    for formula in tqdm(formulas, desc="Benchmarking"):
        # Run C++ version
        cpp_time, cpp_out = run_west(formula, WEST_CPP, OUTPUT_CPP, timelimit)
        cpp_times.append(cpp_time)
        cpp_outputs.append(cpp_out)
        
        # Run Rust version
        rust_time, rust_out = run_west(formula, WEST_RUST, OUTPUT_RUST, timelimit)
        rust_times.append(rust_time)
        rust_outputs.append(rust_out)
    
    return cpp_times, rust_times, cpp_outputs, rust_outputs

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compare C++ vs Rust WEST performance")
    parser.add_argument("-dir", required=True, help="Directory containing .txt files with formulas")
    parser.add_argument("-timelimit", default=120, type=int, help="Time limit for each formula (seconds)")
    args = parser.parse_args()
    
    dir_path = args.dir
    timelimit = args.timelimit
    
    # Update paths to be relative to script location
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    
    for file in sorted(os.listdir(dir_path)):
        if file.endswith(".txt"):
            print(f"\nRunning comparison on {file}")
            with open(os.path.join(dir_path, file), "r") as f:
                formulas = [line.strip() for line in f.readlines() if line.strip()]
            
            value = file.replace(".txt", "")
            cpp_times, rust_times, cpp_outputs, rust_outputs = runall_compare(formulas, timelimit)
            
            # Write results for C++ 
            with open(os.path.join(dir_path, f"{value}.cpp.result"), "w") as f:
                for i in range(len(formulas)):
                    f.write(f"{formulas[i]} : {cpp_times[i]} : {cpp_outputs[i]}\n")
            
            # Write results for Rust
            with open(os.path.join(dir_path, f"{value}.rust.result"), "w") as f:
                for i in range(len(formulas)):
                    f.write(f"{formulas[i]} : {rust_times[i]} : {rust_outputs[i]}\n")
            
            # Print summary
            import numpy as np
            cpp_avg = np.mean(cpp_times)
            rust_avg = np.mean(rust_times)
            speedup = cpp_avg / rust_avg if rust_avg > 0 else float('inf')
            print(f"  C++ avg: {cpp_avg:.4f}s, Rust avg: {rust_avg:.4f}s, Speedup (C++/Rust): {speedup:.2f}x")
