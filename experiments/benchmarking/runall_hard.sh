#!/bin/bash
# Run HARD benchmarks comparing C++ vs Rust WEST implementations
# These benchmarks use larger parameters to stress-test both implementations

echo "=== Generating Hard Benchmark Formulas ==="
python3 gen_formulas_hard.py

echo ""
echo "=== Benchmarking C++ vs Rust WEST (Hard Formulas) ==="

# Run benchmarks for each dataset
python3 benchmark_compare.py -dir ./hard/d
python3 benchmark_compare.py -dir ./hard/m
python3 benchmark_compare.py -dir ./hard/n

echo ""
echo "=== Generating Cactus Plots ==="
python3 plot_cactus.py -dir ./hard

echo ""
echo "Done! Check cactus_hard_d.png, cactus_hard_m.png, cactus_hard_n.png, and cactus_hard_combined.png"
