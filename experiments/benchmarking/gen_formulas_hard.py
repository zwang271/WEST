# This script generates HARDER benchmark datasets for comparing C++ vs Rust WEST
# Parameters are increased to create formulas that take longer to compute
# 
# 1. Formulas of varying nesting depth d, with n=5, p=0.6, delta=8, m=10
# 2. Formulas of varying number of variables n, with d=3, p=0.6, delta=15, m=15
# 3. Formulas of varying max interval bound m, with n=5, d=3, p=0.6, delta=m
#
# Usage: python3 gen_formulas_hard.py

from random_mltl_formulas import generate_random_formula
import random
import os

if __name__ == '__main__':
    random.seed(2026)
    NUM_SAMPLES = 100  # fewer samples since each takes longer

    # Create separate directories for hard benchmarks
    base_dir = "hard"
    os.makedirs(base_dir, exist_ok=True)

    # dataset 1: varying nesting depth d (harder: deeper nesting, larger bounds)
    dir1 = f"{base_dir}/d"
    os.makedirs(dir1, exist_ok=True)
    for d in range(2, 7):  # d=2 to 6
        formulas = []
        for _ in range(NUM_SAMPLES):
            formula = generate_random_formula(n=5, d=d, p=0.6, delta=8, m=10)
            formulas.append(formula)
        with open(f"{dir1}/{d}.txt", "w") as f:
            for formula in formulas:
                f.write(formula + "\n")
        print(f"Generated {NUM_SAMPLES} formulas for d={d}")

    # dataset 2: varying number of variables n (harder: deeper, larger bounds)
    dir2 = f"{base_dir}/n"
    os.makedirs(dir2, exist_ok=True)
    for n in range(3, 8):  # n=3 to 7
        formulas = []
        for _ in range(NUM_SAMPLES):
            formula = generate_random_formula(n=n, d=3, p=0.6, delta=15, m=15)
            formulas.append(formula)
        with open(f"{dir2}/{n}.txt", "w") as f:
            for formula in formulas:
                f.write(formula + "\n")
        print(f"Generated {NUM_SAMPLES} formulas for n={n}")

    # dataset 3: varying max interval bound m (harder: more variables, deeper)
    dir3 = f"{base_dir}/m"
    os.makedirs(dir3, exist_ok=True)
    for m in [10, 20, 30, 40, 50]:  # much larger bounds
        formulas = []
        for _ in range(NUM_SAMPLES):
            formula = generate_random_formula(n=5, d=3, p=0.6, delta=m, m=m)
            formulas.append(formula)
        with open(f"{dir3}/{m}.txt", "w") as f:
            for formula in formulas:
                f.write(formula + "\n")
        print(f"Generated {NUM_SAMPLES} formulas for m={m}")

    print("\nHard formulas generated in ./hard/")
    print("Run: python3 benchmark_compare.py -dir ./hard/d")
    print("     python3 benchmark_compare.py -dir ./hard/m")
    print("     python3 benchmark_compare.py -dir ./hard/n")
