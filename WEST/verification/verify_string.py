# Author: Zili Wang
# Last updated: 02/08/2024
# Verify that WEST and string_west produce the same output
# Usage: python verify.py [formula] [-skip]
# If no formula is given, verify all formulas in ./verify_formulas

import subprocess
import time
import os
import sys
from string_src.parser import *
import pathlib

# Replaces all s in the string with all permutations of 0 and 1
def expand_string(string):
    def expand_string_helper(string):
        if 's' not in string:
            return {string}
        else:
            return expand_string_helper(string.replace('s', '0', 1))\
                .union(expand_string_helper(string.replace('s', '1', 1)))
    return expand_string_helper(string)

# w is a string longer than every string in L
# pads all strings in L to be the same length as w
def pad_uniform(L, w):
    n = len(w.split(",")[0])
    delta_m = len(w.split(",")) - len(L[0].split(","))
    padding = ("," + "s"*n) * delta_m
    return [line + padding for line in L]

def compare_files(f1, f2):
    # skip the first line
    f1.readline()
    f2.readline()

    # turn each file into a list 
    f1_name = f1.name
    f2_name = f2.name
    f1 = [line.strip() for line in list(f1)]
    f2 = [line.strip() for line in list(f2)]

    # degenerate cases involving empty files
    if len(f1) == 0 and len(f2) == 0:
        return True
    elif len(f1) == 0 and len(f2) != 0:
        return False
    elif len(f1) != 0 and len(f2) == 0:
        return False

    if len(f1[0]) > len(f2[0]):
        f2 = pad_uniform(f2, f1[0])
    elif len(f2[0]) > len(f1[0]):
        f1 = pad_uniform(f1, f2[0])
    
    expanded1 = set()
    expanded2 = set()
    # check that the expanded strings are the same
    for line in f1:
        for w in expand_string(line):
            expanded1.add(w)
    for line in f2:
        for w in expand_string(line):
            expanded2.add(w)

    # turn each sets into a list sorted by lex order
    expanded1 = sorted(list(expanded1))
    expanded2 = sorted(list(expanded2))
    if expanded1 == expanded2:
        return True
    
    # print out the differences 
    for i, line in enumerate(f1):
        if line not in f2:
            print(f"Line {i+2} of {f1_name} is not in {f2_name}")
    for i, line in enumerate(f2):
        if line not in f1:
            print(f"Line {i+2} of {f2_name} is not in {f1_name}")
    return False

def verify(formula, skip=False):
    start = time.time()
    formula1 = formula.replace("~", "!")
    west_exec = ".\\west.exe" if sys.platform == "win32" else "./west"
    string_west_exec = ".\\west_string.exe" if sys.platform == "win32" else "./west_string"
    subprocess.run(f"cd .. && {west_exec} \"{formula1}\" cd ./verification", 
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, shell=True)
    end = time.time()
    west_time = end - start

    formula2 = formula.replace("!", "~")
    start = time.time()
    subprocess.run(f"{string_west_exec} \"{formula2}\"", 
                   stdout=subprocess.DEVNULL, 
                   stderr=subprocess.DEVNULL, 
                   shell=True)
    end = time.time()
    string_west_time = end - start

    if skip:
        print(f"Skipped verification for formula \"{formula}\"")
        print(f"Time elapsed to run west: {west_time:.5f}")
        print(f"Time elapsed to run string_west: {string_west_time:.5f}")
        print(f"Speedup: {string_west_time / west_time:.5f}\n")
        return True

    # verify that the two programs produce the same output, but ignore the first line
    # west outputs to ../output/output.txt
    # string_west outputs to ./string_output/string_output.txt
    start = time.time()
    with open("../output/output.txt", "r") as f1, open("./string_output/string_output.txt", "r") as f2:
        if not compare_files(f1, f2):
            print(f"Output files are different on formula \"{formula}\"\n")
            return False
    end = time.time()
    compare_time = end - start
    # print all floats to 5 decimal places
    print(f"Output files are identical on formula \"{formula}\", verified in {compare_time} seconds")
    print(f"Time elapsed to run west: {west_time:.5f}")
    print(f"Time elapsed to run string_west: {string_west_time:.5f}")
    print(f"Speedup: {string_west_time / west_time:.5f}\n")
    return True

if __name__ == '__main__':
    # if given a formula as an argument, verify that formula instead
    if len(sys.argv) > 1:
        formula = sys.argv[1]
        skip = False
        if len(sys.argv) > 2:
            skip = sys.argv[2] == "-skip"
        verify(formula, skip)
        exit(0)

    with open(pathlib.Path("./verify_formulas") / "formulas.txt", "r") as f:
        for line in f:
            formula = line.strip()
            formula = from_west(formula)
            print(formula)
            if not verify(formula):
                exit(1)
    
    # if we get here, all formulas were verified
    print("CONGRATULATIONS! ALL FORMULAS WERE VERIFIED!")

