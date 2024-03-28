# Author: Zili Wang
# Generates random MTL formulas
# Arguments: 
#      -samples : number of formulas to generate
#      -n : number of variables named p0, p1, ... (default: 5)
#      -d : maximum depth of the formula (default: 2)
#      -p : probability of choosing temporal operators (default: 0.5)
#      -delta : maximum delta between i and j in [i, j] (default: 10)
#      -m : maximum bound value - max of all i and j in intervals [i, j] (default: 10)
#      -seed: seed for random number generator (default: None)
#      (--help will produce a usage message)
# 
# Output: writes to ./n[n]_d[d]_p[p]_delta[delta]_m[m]_seed[seed]/formulas.txt

import argparse
import random
import os

def generate_random_formula(n:int, d:int, p:float, delta:int, m:int):
    '''
    Generates a random MTL formula
    Arguments:
        n : number of variables named p0, p1, ...
        d : maximum depth of the formula
        p : probability of choosing temporal operators
        delta : maximum delta between i and j in [i, j]
        m : maximum bound value - max of all i and j in intervals [i, j]
    Returns:
        a random MTL formula
    '''
    def random_formula(depth):
        if depth == 0:
            return f"p{random.randint(0, n-1)}"
        if random.random() < p: # temporal operator
            i = random.randint(0, m)
            j = random.randint(i, min(i+delta, m))
            op = random.choice(["F", "G", "U", "R"])
            if op in ["U", "R"]:
                f1 = random_formula(depth-1)
                f2 = random_formula(depth-1)
                return f"({f1} {op}[{i},{j}] {f2})"
            else: # op in ["F", "G"]
                f = random_formula(depth-1)
                return f"{op}[{i},{j}] {f}"
        else: # boolean operator
            op = random.choice(["&", "|", "!"])
            if op == "!":
                f = random_formula(depth-1)
                return f"!{f}"
            else: # op in ["&", "|"]
                f1 = random_formula(depth-1)
                f2 = random_formula(depth-1)
                return f"({f1} {op} {f2})"

    return random_formula(d)

def main():
    parser = argparse.ArgumentParser(description="Generates random MTL formulas")
    parser.add_argument("-samples", type=int, default=100,
                        help="number of formulas to generate (default: 100)")
    parser.add_argument("-n", type=int, default=5,
                        help="number of variables (named p0, p1, ...) (default: 5)")
    parser.add_argument("-d", type=int, default=2,
                        help="maximum depth of the formula (default: 2)")
    parser.add_argument("-p", type=float, default=0.5,
                        help="probability of choosing temporal operators (default: 0.5)")
    parser.add_argument("-delta", type=int, default=10,
                        help="maximum delta between i and j in [i, j] (default: 10)")
    parser.add_argument("-m", type=int, default=10,
                        help="maximum bound value (i.e., max of all i and j in intervals [i, j]) (default: 10)")
    parser.add_argument("-seed", type=int, default=None,
                        help="seed for random number generator (default: None)")
    args = parser.parse_args()
    if args.seed is not None:
        random.seed(args.seed)

    formulas = []
    for _ in range(args.samples):
        formula = generate_random_formula(args.n, args.d, args.p, args.delta, args.m)
        formulas.append(formula)
    
    outdir = f"n{args.n}_d{args.d}_p{args.p}_delta{args.delta}_m{args.m}_seed{args.seed}"
    os.makedirs(outdir, exist_ok=True)
    with open(os.path.join(outdir, "formulas.txt"), "w") as f:
        for formula in formulas:
            f.write(formula + "\n")
    print(f"Formulas written to {outdir}")
    return outdir

if __name__ == "__main__":
    main()
