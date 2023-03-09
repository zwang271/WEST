import os
from PyQt5.QtWidgets import * 
from PyQt5.QtGui import * 
from random import choice


CWD = os.getcwd()
FONT = QFont('Times', 15)


def gen_reg(w_reg):
    output = ""
    for c in w_reg:
        if c == "s":
            output += choice(["0", "1"])
        else:
            output += c
    return output


def rewrite(formula):
    # define rewrite mapping
    REWRITE = {
        ":": ", ",
        ">": " -> ",
        "v": " v ",
        "&": " & ",
        "=": " = "
    }
    f = ""
    for i, char in enumerate(formula):
        if i in [0, len(formula)-1]:
            # pass
            f += char
        elif char in REWRITE.keys():
            f += REWRITE[char]
        else:
            f += char
    return f; 


def process_file():
    # read in formula from file
    formula = []
    with open("./west_output/formula.txt") as file:
        for line in file:
            line = line.rstrip()
            formula.append(rewrite(line))
    
    # construct regular expression from file
    regexp = []
    west_regexp = []
    with open("./west_output/regexp.txt") as file:
        temp_regexp = []
        temp_west_regexp = []
        for line in file:
            if line == "\n":
                regexp.append(temp_regexp)
                west_regexp.append(temp_west_regexp)
                temp_regexp = []
                temp_west_regexp = []
            else:
                temp = line.rstrip()
                temp_west_regexp.append(temp)
                temp = temp.replace("s", "[01]")
                temp_regexp.append(temp) 
    
    return formula, regexp, west_regexp


def run(func, argList):
    execute = ".\gui\west_lib.exe " + func + " "
    for arg in argList:
        execute += '"' + arg + '" '
    os.system(execute)

    # print(execute)

    try:
        return open(f'.\gui\{func}.txt', 'r')
    except FileNotFoundError:
        return []


if __name__ == '__main__':
    outfile1 = run("Wff_check", ["~G[0:2](p0 v p1)"])
    outfile2 = run("Wff_to_Nnf", ["~G[0:2](p0 v p1)"])
    outfile3 = run("get_n", ["F[0:2](~p0v~p1)"])
    outfile4 = run("reg", ["F[0:2](~p0v~p1)", "y", "n"])

    files = [outfile1, outfile2, outfile3, outfile4]
    for outfile in files:
        for line in outfile: 
            print(line)