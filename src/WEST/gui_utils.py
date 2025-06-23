# Author: Zili Wang
# Last updated: 01/19/2024

import re
from random import choice
import os
from lark import Lark, Transformer, v_args, exceptions

# Read grammar from ./src/ebnf_grammar.txt
grammar = r'''
// Start rule
start: wff

// Basic elements
num: NUMBER
interval: "[" num "," num "]"
prop_var: "p" num | "a" num

// Propositional constants and connectors
prop_cons: "true" | "false"
neg: "!"
binary_prop_conn: "->" | "&" | "|" | "="

// Temporal connectors
unary_temp_conn: "F" | "G"
binary_temp_conn: "U" | "R"

// Well-formed formula
wff: prop_var | prop_cons | neg wff | "(" wff ")"
    | unary_temp_conn interval wff
    | "(" wff binary_prop_conn wff ")"
    | wff binary_prop_conn wff
    | "(" wff binary_temp_conn interval wff ")"
    | wff binary_temp_conn interval wff

// Whitespace handling
%import common.WS
%ignore WS
%import common.NUMBER
'''
parser = Lark(grammar, parser='lalr', start='wff')

def check_wff(input_string, verbose=True):
    if len(input_string) == 0:
        return False, "Empty input string."
    try:
        # Attempt to parse the input string
        tree = parser.parse(input_string)
        return True, None
    except exceptions.LarkError as e:
        return False, str(e)

def pretty_display(formula):
    formula = formula.replace("&", " & ").replace("|", " | ").replace("->", " -> ").replace("=", " = ")
    # match all instances of (R or U)[num, num] and add spaces before and after
    formula = re.sub(r"([RU])\[(\d+),(\d+)\]", r" \1[\2,\3] ", formula)
    formula = re.sub(r"([FG])\[(\d+),(\d+)\]", r"\1[\2,\3] ", formula)
    return formula 

def gen_reg(w_reg):
    if w_reg is None:
        return ""
    if w_reg is None:
        return ""
    output = ""
    for c in w_reg:
        if c == "s":
            output += choice(["0", "1"])
        else:
            output += c
    return output

def check_valid_trace(trace, n, t):
    if len(trace) != n * t + t - 1:
        return False
    trace = trace + ","
    while len(trace) > 0:
        vars = trace[:n]
        comma = trace[n]
        if not all([c in "01" for c in vars]) or comma != ",":
            return False
        trace = trace[n+1:]
    return True 

def read_trace(file):
    contents = file.read().strip()
    contents = contents.replace(",", "")
    trace = contents.split("\n")
    trace = ",".join(trace)
    return trace

# read from ./src/grammar.html
grammar_html = open(os.path.join(os.path.dirname(__file__), "grammar.html"), "r").read()



