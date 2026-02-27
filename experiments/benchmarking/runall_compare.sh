#!/bin/bash
# Run benchmarks comparing C++ vs Rust WEST implementations
# Then generate cactus plots

echo "=== Benchmarking C++ vs Rust WEST ==="

# Run benchmarks for each dataset
python3 benchmark_compare.py -dir ./d
python3 benchmark_compare.py -dir ./m
python3 benchmark_compare.py -dir ./n

echo ""
echo "=== Generating Cactus Plots ==="
python3 plot_cactus.py

echo ""
echo "Done! Check cactus_d.png, cactus_m.png, cactus_n.png, and cactus_combined.png"
