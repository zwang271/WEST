# Author: Zili Wang
# Date: 02/13/2024
# Translate formula in old WEST syntax to updated MLTL syntax

import sys
sys.path.append("../string_src/")
from parser import from_west
import re

def minimize_vars(formula: str):
    # iterate through all variables in the formula of the form p0, p1, p2, ...
    indices = set()
    for match in re.finditer(r"p\d+", formula):
        var = match.group()
        num = int(var[1:])
        indices.add(num)
    indices = sorted(list(indices))
    for i, index in enumerate(indices):
        formula = formula.replace(f"p{index}", f"a{i}")
    for i in range(len(indices)):
        formula = formula.replace(f"a{i}", f"p{i}")
    return formula     

if __name__ == "__main__":
    out = open("./formulas.txt", "w")
    for file in ["./formulas_d0.txt", "./formulas_d1.txt", "./formulas_d2.txt"]:
        with open(file, "r") as f:
            for line in f:
                formula = line.strip()
                formula = from_west(formula)
                formula = formula.replace("~", "!")
                formula = minimize_vars(formula)
                out.write(formula + "\n")
    out.close()