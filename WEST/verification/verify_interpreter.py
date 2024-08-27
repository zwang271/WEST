# Author: Zili Wang
# Date Created: 02/12/2024
# Verify that WEST and the MLTL interpreter produce the same output

import subprocess
import time
import os
import sys
from string_src.parser import from_west
import re
from verify_r2u2 import compare_files, iterate_traces, get_n

def run_west(formula):
    west_exec = "./west"
    subprocess.run(f"cd .. && {west_exec} \"{formula}\" cd ./verification", 
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, shell=True)

def run_interpreter(formula, traces, verbose=False):
    interpreter = "../../MLTL_interpreter/interpret_batch"
    spec_mltl = "./interpreter_output/spec.mltl"
    traces_dir = "./interpreter_output/traces/" 
    verdicts = "./interpreter_output/verdicts.txt"
    with open(spec_mltl, "w") as f:
        f.write(formula+"\n")
    # empty the traces directory
    if not os.path.exists(traces_dir):
        os.makedirs(traces_dir)
    for file in os.listdir(traces_dir):
        os.remove(traces_dir + file)
    # write each trace to a file in the traces directory
    for i, trace in enumerate(traces):
        if i % 100_000 == 0 and verbose:
            print(f"Writing trace {i} to file")
        with open(f"{traces_dir}trace{i}.csv", "w") as f:
            for t in trace:
                f.write(",".join(list(t)) + "\n")
    # run the interpreter
    cmd = f"{interpreter} {spec_mltl} {traces_dir} {verdicts}"
    print(cmd)
    subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, shell=True)

def get_mn():
    with open("../output/output.txt", "r") as f:
        regex = f.readlines()
        regex = regex[1] if len(regex) > 1 else None
        if regex is not None:
            regex = regex.split(",") if "," in regex else [regex.strip()]
            m = len(regex)
            n = len(regex[0]) if m > 0 else 0
        else:
            # in formula, find all instances of p0, p1, p2, etc. and find the max
            n = get_n(formula)
            if n > 0:
                m = 1
            elif n == 0:
                m, n = 1, 5
    return m, n

def verify(formula):
    start = time.perf_counter()
    run_west(formula)
    m, n = get_mn()
    print(f"Verifying formula \"{formula}\" with {m} time steps and {n} variables")
    traces = list(iterate_traces(m, n))
    run_interpreter(formula, traces)
    true_indices = []
    with open("./interpreter_output/verdicts.txt", "r") as f:
        # each line is of the form "...trace{i}.csv : verdict"
        # store all the indices of verdicts that are true, ie "1"
        for line in f:
            if line.strip().endswith("1"):
                true_indices.append(int(re.findall(r"\d+", line)[0]))
    # write the true traces to a file
    with open("./interpreter_output/output.txt", "w") as f:
        f.write(formula + "\n")
        for i in true_indices:
            f.write(",".join(traces[i]) + "\n")
    # compare the files
    with open("../output/output.txt", "r") as f1:
        with open("./interpreter_output/output.txt", "r") as f2:
            if compare_files(f1, f2):
                total = time.perf_counter() - start
                print(f"Output files are identical on formula \"{formula}\", verified in {total} seconds\n")
                return True
            else:
                print(f"Output files are different on formula \"{formula}\"\n")
                return False

if __name__ == "__main__":
    # if given a formula as an argument, use that as checkpoint
    checkpoint = None
    if len(sys.argv) > 1:
        formula = sys.argv[1].strip()
        checkpoint = formula

    # verify all formulas in verify_formulas, skip up to checkpoint if checkpoint is given
    # checkpoint = "F[0:2]G[0:2]p3"
    skip = False if checkpoint is None else True
    with open(f"./verify_formulas/formulas.txt", "r") as f:
        for line in f:
            formula = line.strip()
            if skip:
                print(f"Skipping verification for formula \"{formula}\", before checkpoint\n")
                if formula == checkpoint:
                    skip = False
                continue
            print(formula)
            if not verify(formula):
                exit(1)
    
    # if we get here, all formulas were verified
    print("CONGRATULATIONS! ALL FORMULAS WERE VERIFIED!")