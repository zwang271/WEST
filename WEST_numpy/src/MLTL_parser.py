# Author: Zili Wang
# Last updated: Nov 1st 2024
from lark import Lark, Transformer, v_args, exceptions
import sys

grammar = r'''
// Start rule
start: wff

// Basic elements
num: NUMBER
interval: "[" num "," num "]"
prop_var: "p" num | "a" num

// Propositional constants and connectors
true: "true" | "True"
false: "false" | "False"
neg: "!"
implies: "->"
and: "&" | "&&"
or: "|" | "||"
equiv: "=" | "<->"
prop_cons: true | false
binary_prop_conn: implies | and | or | equiv

// Temporal connectors
future: "F"
global: "G"
until: "U"
release: "R"
unary_temp_conn: future | global
binary_temp_conn: until | release

// Well-formed formula
wff_base: prop_var | prop_cons
wff_neg.1: neg wff
wff_bintemp.2: "(" wff binary_prop_conn wff ")" | wff binary_temp_conn interval wff
wff_binprop.3: "(" wff binary_prop_conn wff ")" | wff binary_prop_conn wff
wff_untemp.4: unary_temp_conn interval wff
wff: prop_var | prop_cons | wff_neg | wff_bintemp | wff_untemp | wff_binprop | "(" wff ")"   

// Whitespace handling
%import common.WS
%ignore WS
%import common.NUMBER
'''

parser = Lark(grammar, parser='lalr', start='wff')

def parse_mltl(formula):
    try:
        AST = parser.parse(formula)
    except exceptions.UnexpectedCharacters as e:
        return f"Error: {e}"
    return AST