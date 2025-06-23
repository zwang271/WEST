# Author: Zili Wang
# Created: 02/19/2024
# Verifies that WEST and AllSAT are equivalent
# Uses Gokul's MLTL -> Propositional logic translation (fast)
# Please clone Gokul's repo: https://github.com/gokulhari/MLTLMaxSAT-FORMATS
# cd into it and run ./installer.sh

import subprocess
import time
import os
import sys
import re
from verify_r2u2 import compare_files, get_mn
import z3
from pycosat import solve, itersolve
from dd import autoref as _bdd

def run_west(formula):
    west_exec = "./west"
    subprocess.run(f"cd ../../src && {west_exec} \"{formula}\"", 
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, shell=True)
    
def add_unary_parenthesis(formula):
    end_chars = ["&", "|", "U", "R", ")"]
    # find index of the first G or F
    start = min([formula.find(op) for op in ["G", "F", "!"] if formula.find(op) != -1], default=-1)
    if start == -1:
        return formula
    paren_count = 0
    for i in range(start+1, len(formula)):
        if paren_count == 0 and formula[i] in end_chars:
            end = i
            # print(start, end, formula)
            break
        if formula[i] == "(":
            paren_count += 1
        elif formula[i] == ")":
            paren_count -= 1
    else:
        return formula
    prefix = formula[:start] + "(" + formula[start]
    recurse1 = add_unary_parenthesis(formula[start+1:end]) + ")"
    recurse2 = add_unary_parenthesis(formula[end:])
    return prefix + recurse1 + recurse2

def preprocess_syntax(formula):
    formula = formula.replace(" ", "")
    temp = formula
    formula = add_unary_parenthesis(formula)
    if formula == temp:
        formula = formula[:1] + add_unary_parenthesis(formula[1:])
    formula = formula.replace("|", " | ").replace("&", " & ")
    # replace "U[num, num]" with spaces around it
    formula = formula.replace("R", " R").replace("U", " U")
    formula = formula.replace("G", " G").replace("F", " F")
    formula = formula.replace("]", "] ") 
    formula = formula.strip()
    return formula

def run_translater(formula):
    # define file paths
    translater = "./MLTLMaxSAT-FORMATS/build/main"
    spec_mltl = "./maxsat_output/spec.mltl"
    out = "./maxsat_output/boolean_func.smt"

    if formula == "p0":
        with open("./maxsat_output/boolean_func.smt", "w") as f:
            f.write("(declare-fun p0 (Int) Bool)\n(assert (p0 0))\n(check-sat)\n")
        to_boolean_pure()
        return

    with open(spec_mltl, "w") as f:
        f.write(formula+"\n")
    cmd = f"{translater} -f {spec_mltl} -t boolFast -o {out}"
    subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, shell=True)
    to_boolean_pure()

def to_boolean_pure():
    with open("./maxsat_output/boolean_func.smt", "r") as f:
        smt_str = f.read()
    # find all term such as "(p1 0)" and convert to the variable "p1_0"
    smt_str = re.sub(r"\((\w+) (\d+)\)", r"\1_\2", smt_str)
    variables = list(set(re.findall(r"\w+_\d+", smt_str)))
    variables.sort()
    assertions = []
    for line in smt_str.split("\n"):
        if line.startswith("(assert"):
            assertions.append(line)
    declarations = [f"(declare-const {v} Bool)" for v in variables]
    boolean_smt = "\n".join(declarations + assertions)
    with open("./maxsat_output/boolean_pure.smt", "w") as f:
        f.write(boolean_smt)

#############################################################################
# Z3 approach, too slow to be feasible
#############################################################################
def allsat_z3(m):
    # intialize z3 solver with the smt file
    s = z3.Solver()
    s.from_file("./maxsat_output/out.smt")
    # get all models
    models = []
    while s.check() == z3.sat:
        model = s.model()
        models.append(model)
        block = []
        for d in model:
            for i in range(m):
                d_val = model.evaluate(d(i))
                block.append(d(i) != d_val)
        s.add(z3.Or(block))
        print(f"Found {len(models)} models", end="\r")
    return models

#############################################################################
# pycosat approach, requires dimacs cnf format
# Currently using z3's tseitin-cnf tactic to convert to CNF
# But it does not preserve equivalence, only satisfiability
# Still need to find a way to convert to CNF, maybe just use naive distributivity
#############################################################################
def allsat_pycosat(m):
    with open("./maxsat_output/boolean_pure.smt", "r") as f:
        boolean_smt = f.read()
    # convert to CNF with z3, using tseitin-cnf tactic with distributivity
    g = z3.Goal()
    g.add(z3.parse_smt2_string(boolean_smt))
    t = z3.Tactic('tseitin-cnf')
    cnf = t(g)

#############################################################################
# dd approach, using dd package
#############################################################################
def to_infix(tokens):
    if len(tokens) == 1:
        return tokens[0]
    assert tokens[0] == "(" and tokens[-1] == ")"
    tokens = tokens[1:-1]
    op = tokens.pop(0)
    args = []
    paren_count = 0
    temp_arg = []
    while tokens:
        token = tokens.pop(0)
        if token == "(":
            paren_count += 1
        elif token == ")":
            paren_count -= 1
        temp_arg.append(token)
        if paren_count == 0:
            args.append(temp_arg)
            temp_arg = []
    args = [to_infix(arg) for arg in args]
    if op in ["&", "|", "<=>"]:
        op = f" {op} "
        infix = f"({op.join(args)})"
    elif op == "~":
        infix = f"~({args[0]})"
    else:
        raise ValueError(f"Unknown operator {op}")
    return infix

def prefix_to_infix(prefix: str):
    prefix = prefix.replace("false", "False").replace("true", "True")
    prefix = prefix.replace("and", "&").replace("or", "|").replace("=", "<=>").replace("not", "~")
    prefix = prefix.replace("(", "( ").replace(")", " )")
    tokens = prefix.split()    
    infix = to_infix(tokens)
    return infix

def dd_models_to_traces(models, variables, m, n):
    traces = []
    variables = [var for var in variables if var.startswith("p")]
    # print(variables)
    for model in models:
        # print(model)
        trace = ["s" * n for _ in range(m)]
        model = {var: model[var] for var in variables if var in model}
        for var, val in model.items():
            var_idx, time = re.match(r"p(\d+)_(\d+)", var).groups()
            var_idx = int(var_idx)
            time = int(time)
            val = "1" if val else "0"	
            # print(var, val, var_idx, time)
            trace[time] = trace[time][:var_idx] + val + trace[time][var_idx+1:]
        trace = ",".join(trace)
        traces.append(trace)	
        # print(trace)
        # quit()
    return traces

def allsat_dd(m, n, verbose=False):
    with open("./maxsat_output/boolean_pure.smt", "r") as f:
        boolean_smt = f.read()
    variables = list(set(re.findall(r"\w+_\d+", boolean_smt)))
    variables.sort()
    assertions = []
    for line in boolean_smt.split("\n"):
        if line.startswith("(assert "):
            assertions.append(line.removeprefix("(assert ").removesuffix(")"))
    start = time.perf_counter()
    # create a dd manager
    bdd = _bdd.BDD()
    [bdd.declare(v) for v in variables]
    clause = None
    for a in assertions:
        a = prefix_to_infix(a)
        if verbose:
            print(a)
        if clause is None:
            clause = bdd.add_expr(a)
        else:
            clause = bdd.apply("and", clause, bdd.add_expr(a))
    # get all models
    models = list(bdd.pick_iter(clause))
    end = time.perf_counter()
    print(f"Ran bdd in {end-start:.5f} seconds")
    return dd_models_to_traces(models, variables, m, n)

#############################################################################
# Verify
#############################################################################
def verify(formula, verbose=False):
    start = time.perf_counter()
    run_west(formula)
    end = time.perf_counter()
    m, n = get_mn(formula)
    print(f"Verifying formula \"{formula}\" with {m} time steps and {n} variables")
    print(f"Ran WEST in {end-start:.5f} seconds")
    run_translater(formula)
    # models = allsat_z3(m)
    # models = allsat_pycosat(m)
    traces = allsat_dd(m, n, verbose)
    with open("./maxsat_output/out.txt", "w") as f:
        f.write(formula + "\n")
        f.write("\n".join(traces))
    with open("../../src/output/output.txt", "r") as f1:
        with open("./maxsat_output/out.txt", "r") as f2:
            if compare_files(f1, f2):
                total = time.perf_counter() - start
                print(f"Output files are identical on formula \"{formula}\", verified in {total:.5f} seconds\n")
                return True
            else:
                print(f"Output files are different on formula \"{formula}\"\n")
                return False

if __name__ == '__main__':
    if len(sys.argv) > 1:
        formula = sys.argv[1].strip()
        formula = preprocess_syntax(formula)
        print(formula)
        verify(formula, verbose=True)
        exit(0)

    disagreeing_formulas = []
    checkpoint = None
    # verify all formulas in verify_formulas, skip up to checkpoint if checkpoint is given
    skip = False if checkpoint is None else True
    with open(f"./verify_formulas/formulas.txt", "r") as f:
        for line in f:
            formula = line.strip()
            if skip:
                print(f"Skipping verification for formula \"{formula}\", before checkpoint\n")
                if formula == checkpoint:
                    skip = False
                continue
            formula = preprocess_syntax(formula)
            print(formula)
            if not verify(formula):
                exit(1)
    
    # if we get here, all formulas were verified
    print("CONGRATULATIONS! ALL FORMULAS WERE VERIFIED!")


