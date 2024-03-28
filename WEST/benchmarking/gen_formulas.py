# This script generates 3 datasets varying across different parameters:
# 1. Formulas of varying nesting depth d, with n=4, p=0.5, delta=3, m=5
# 2. Formulas of varying number of variables n, with d=2, p=0.5, delta=10, m=10
# 3. Formulas of varying max interval bound m, with n=4, d=2, p=0.5, delta=m
# For each dataset, we vary the parameter of interest and 
# create NUM_SAMPLES formulas for each value of the parameter.
# Creates a directory for each dataset, and the value of the parameter is the
# name of the file containing the formulas for a given value of the parameter.

from random_mltl_formulas import generate_random_formula
import random
import os

if __name__ == '__main__':
    random.seed(2024)
    NUM_SAMPLES = 250

    # dataset 1: varying nesting depth d
    dir1 = "d"
    os.makedirs(dir1, exist_ok=True)
    for d in range(7):
        formulas = []
        for _ in range(NUM_SAMPLES):
            formula = generate_random_formula(4, d, 0.5, 3, 3)
            formulas.append(formula)
        with open(f"{dir1}/{d}.txt", "w") as f:
            for formula in formulas:
                f.write(formula + "\n")

    # dataset 2: varying number of variables n
    dir2 = "n"
    os.makedirs(dir2, exist_ok=True)
    for n in range(2, 9):
        formulas = []
        for _ in range(NUM_SAMPLES):
            formula = generate_random_formula(n, 2, 0.5, 8, 8)
            formulas.append(formula)
        with open(f"{dir2}/{n}.txt", "w") as f:
            for formula in formulas:
                f.write(formula + "\n")

    # dataset 3: varying max interval bound m
    dir3 = "m"
    os.makedirs(dir3, exist_ok=True)
    for m in [3, 6, 9, 12, 15]:
        formulas = []
        for _ in range(NUM_SAMPLES):
            formula = generate_random_formula(4, 2, 0.5, m, m)
            formulas.append(formula)
        with open(f"{dir3}/{m}.txt", "w") as f:
            for formula in formulas:
                f.write(formula + "\n")
    print("Formulas generated")
