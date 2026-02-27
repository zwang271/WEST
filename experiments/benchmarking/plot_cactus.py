# Cactus Plot: Compares C++ (bitoptimized) vs Rust WEST performance
# A cactus plot shows how many instances each solver can solve within a given time.
# X-axis: number of instances solved (sorted by time)
# Y-axis: time taken (seconds)
# 
# Usage: python3 plot_cactus.py
# Expects .cpp.result and .rust.result files from benchmark_compare.py

import matplotlib.pyplot as plt
import numpy as np
import os

FONTSIZE = 14
TITLE_FONTSIZE = 16

def load_times(result_file):
    """Load times from a .result file."""
    times = []
    with open(result_file, "r") as f:
        for line in f:
            parts = line.strip().split(" : ")
            if len(parts) >= 2:
                times.append(float(parts[1]))
    return times

def create_cactus_plot(cpp_times, rust_times, title, output_file):
    """Create a cactus plot comparing C++ and Rust times."""
    # Sort times independently for each implementation
    cpp_sorted = sorted(cpp_times)
    rust_sorted = sorted(rust_times)
    
    # X-axis: instance number (1 to N)
    x_cpp = list(range(1, len(cpp_sorted) + 1))
    x_rust = list(range(1, len(rust_sorted) + 1))
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    ax.plot(x_cpp, cpp_sorted, 'b-', linewidth=2, label='C++ (bitoptimized)')
    ax.plot(x_rust, rust_sorted, 'r-', linewidth=2, label='Rust')
    
    ax.set_xlabel('Number of Instances Solved', fontsize=FONTSIZE)
    ax.set_ylabel('Time (seconds)', fontsize=FONTSIZE)
    ax.set_title(title, fontsize=TITLE_FONTSIZE)
    ax.legend(fontsize=FONTSIZE-2)
    ax.grid(True, alpha=0.3)
    ax.tick_params(axis='both', which='major', labelsize=FONTSIZE-2)
    
    # Set y-axis to log scale if there's a large range
    if max(cpp_sorted + rust_sorted) / min(t for t in cpp_sorted + rust_sorted if t > 0) > 100:
        ax.set_yscale('log')
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=150)
    plt.close()
    print(f"Saved: {output_file}")

def create_combined_cactus_plot(all_cpp_times, all_rust_times, output_file):
    """Create a combined cactus plot with all formulas from all datasets."""
    cpp_sorted = sorted(all_cpp_times)
    rust_sorted = sorted(all_rust_times)
    
    x_cpp = list(range(1, len(cpp_sorted) + 1))
    x_rust = list(range(1, len(rust_sorted) + 1))
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    ax.plot(x_cpp, cpp_sorted, 'b-', linewidth=2, label='C++ (bitoptimized)')
    ax.plot(x_rust, rust_sorted, 'r-', linewidth=2, label='Rust')
    
    ax.set_xlabel('Number of Instances Solved', fontsize=FONTSIZE)
    ax.set_ylabel('Time (seconds)', fontsize=FONTSIZE)
    ax.set_title('WEST Performance Comparison: C++ vs Rust (All Formulas)', fontsize=TITLE_FONTSIZE)
    ax.legend(fontsize=FONTSIZE-2)
    ax.grid(True, alpha=0.3)
    ax.tick_params(axis='both', which='major', labelsize=FONTSIZE-2)
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=150)
    plt.close()
    print(f"Saved: {output_file}")

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Generate cactus plots")
    parser.add_argument("-dir", default=".", help="Base directory containing d/, m/, n/ subdirs")
    args = parser.parse_args()
    base_dir = args.dir
    
    description = {
        "m": "Max Interval Bound (m)", 
        "n": "Number of Atomic Propositions (n)", 
        "d": "Max Nesting Depth (d)"
    }
    
    all_cpp_times = []
    all_rust_times = []
    
    for param in ["d", "m", "n"]:
        dir_path = os.path.join(base_dir, param)
        if not os.path.isdir(dir_path):
            print(f"Skipping {dir_path} - directory not found")
            continue
            
        cpp_times = []
        rust_times = []
        
        # Collect all times for this parameter
        for file in sorted(os.listdir(dir_path)):
            if file.endswith(".cpp.result"):
                cpp_times.extend(load_times(os.path.join(dir_path, file)))
            elif file.endswith(".rust.result"):
                rust_times.extend(load_times(os.path.join(dir_path, file)))
        
        if cpp_times and rust_times:
            title = f"WEST Performance: C++ vs Rust\nVarying {description[param]}"
            output_prefix = base_dir.replace("/", "_").replace(".", "") or "cactus"
            create_cactus_plot(cpp_times, rust_times, title, f"cactus_{output_prefix}_{param}.png")
            
            all_cpp_times.extend(cpp_times)
            all_rust_times.extend(rust_times)
            
            # Print summary stats
            print(f"\n{param} dataset:")
            print(f"  C++:  mean={np.mean(cpp_times):.4f}s, median={np.median(cpp_times):.4f}s")
            print(f"  Rust: mean={np.mean(rust_times):.4f}s, median={np.median(rust_times):.4f}s")
            speedup = np.mean(cpp_times) / np.mean(rust_times) if np.mean(rust_times) > 0 else float('inf')
            print(f"  Speedup (C++/Rust): {speedup:.2f}x")
        else:
            print(f"Skipping {dir_path} - no .cpp.result or .rust.result files found")
            print(f"  Run: python3 benchmark_compare.py -dir {dir_path}")
    
    # Combined plot
    if all_cpp_times and all_rust_times:
        output_prefix = base_dir.replace("/", "_").replace(".", "") or "cactus"
        create_combined_cactus_plot(all_cpp_times, all_rust_times, f"cactus_{output_prefix}_combined.png")
        print(f"\nCombined ({len(all_cpp_times)} formulas):")
        print(f"  C++:  mean={np.mean(all_cpp_times):.4f}s, median={np.median(all_cpp_times):.4f}s")
        print(f"  Rust: mean={np.mean(all_rust_times):.4f}s, median={np.median(all_rust_times):.4f}s")
        speedup = np.mean(all_cpp_times) / np.mean(all_rust_times) if np.mean(all_rust_times) > 0 else float('inf')
        print(f"  Overall speedup (C++/Rust): {speedup:.2f}x")
