# Author: Zili Wang
# Created: 02/19/2024
# Verifies that WEST and MAXSAT are equivalent
# Uses Gokul's MLTL -> Propositional logic translation (fast)
# Please clone Gokul's repo: https://github.com/gokulhari/MLTLMaxSAT-FORMATS
# cd into it and run ./installer.sh

import subprocess
import time
import os
import sys
import re
from verify_r2u2 import compare_files, iterate_traces, get_n
import z3

def run_west(formula):
    west_exec = "./west"
    subprocess.run(f"cd .. && {west_exec} \"{formula}\" cd ./verification", 
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, shell=True)
    
def run_translate(formula):
    translate = "./MLTLMaxSAT-FORMATS/build/main"
    spec_mltl = "./maxsat_output/spec.mltl"
    out = "./maxsat_output/out.smt"
    with open(spec_mltl, "w") as f:
        f.write(formula+"\n")
    cmd = f"{translate} -f {spec_mltl} -t boolFast -o {out}"
    subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, shell=True)

def verify(formula):
    pass

if __name__ == '__main__':
    formula = "G[0,3] (p0 | p1)"
    


