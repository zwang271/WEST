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
wff: wff_base | wff_neg | wff_bintemp | wff_untemp | wff_binprop | "(" wff ")"   

// Whitespace handling
%import common.WS
%ignore WS
%import common.NUMBER
'''

ops = {
    "true": "True",
    "false": "False",
    "neg": "!",
    "implies": "->",
    "and": "&",
    "or": "|",
    "equiv": "<=>",
    "future": "F",
    "global": "G",
    "until": "U",
    "release": "R"
}

parser = Lark(grammar, parser='lalr', start='wff')

def parse_mltl(formula):
    try:
        AST = parser.parse(formula)
    except exceptions.UnexpectedCharacters as e:
        sys.exit(f"Error: Unexpected character {e.char} at position {e.pos_in_stream}")
    return AST

def convert_nnf(node):
    while node.data == "wff":
        node = node.children[0]
        wff_type = node.data
    if wff_type == "wff_base":
        child = node.children[0]
        assert(child.data in ["prop_cons", "prop_var"])
        if child.data == "prop_cons":
            return(ops[child.children[0].data])
        elif child.data == "prop_var":
            variable = child.children[0].children[0].value
            return(f"p{variable}")
        
    if len(node.children) == 3:
        child1, child2, child3 = node.children
        if wff_type == "wff_untemp":
            temp_conn = ops[child1.children[0].data]
            lb = child2.children[0].children[0].value
            ub = child2.children[1].children[0].value
            wff = convert_nnf(child3)
            return (f"({temp_conn}[{lb},{ub}]{wff})")
        if wff_type == "wff_binprop":
            prop_conn = ops[child2.children[0].data]
            wff1 = convert_nnf(child1)
            wff2 = convert_nnf(child3)
            return (f"({wff1} {prop_conn} {wff2})")
        
    if wff_type == "wff_bintemp":
        child1, child2, child3, child4 = node.children
        assert(child1.data == "wff" and child2.data == "binary_temp_conn" and
               child3.data == "interval" and child4.data == "wff")
        temp_conn = ops[child2.children[0].data]
        wff1 = convert_nnf(child1)
        lb = child3.children[0].children[0].value
        ub = child3.children[1].children[0].value
        wff2 = convert_nnf(child4)
        return (f"({wff1} {temp_conn}[{lb}, {ub}] {wff2})")

    # convert negation using duals      
    if wff_type == "wff_neg":
        child1, child2 = node.children
        assert(child1.data == "neg" and child2.data == "wff")
        node = child2
        while node.data == "wff":
            node = node.children[0]
            wff_type = node.data
        if wff_type == "wff_base":
            child = node.children[0]
            assert(child.data in ["prop_cons", "prop_var"])
            if child.data == "prop_cons":
                prop_cons = ops[child.children[0].data]
                if prop_cons == "True":
                    return "False"
                elif prop_cons == "False":
                    return "True"
            elif child.data == "prop_var":
                variable = child.children[0].children[0].value
                return(f"(!p{variable})")
            
        if len(node.children) == 3:
            child1, child2, child3 = node.children
            if wff_type == "wff_untemp":
                temp_conn = ops[child1.children[0].data]
                lb = child2.children[0].children[0].value
                ub = child2.children[1].children[0].value
                wff = convert_nnf(child3)
                return (f"({temp_conn}[{lb},{ub}]{wff})")
            if wff_type == "wff_binprop":
                prop_conn = ops[child2.children[0].data]
                wff1 = convert_nnf(child1)
                wff2 = convert_nnf(child3)
                return (f"({wff1} {prop_conn} {wff2})")
            
        if wff_type == "wff_bintemp":
            child1, child2, child3, child4 = node.children
            assert(child1.data == "wff" and child2.data == "binary_temp_conn" and
                child3.data == "interval" and child4.data == "wff")
            temp_conn = ops[child2.children[0].data]
            wff1 = convert_nnf(child1)
            lb = child3.children[0].children[0].value
            ub = child3.children[1].children[0].value
            wff2 = convert_nnf(child4)
            return (f"({wff1} {temp_conn}[{lb}, {ub}] {wff2})")


if __name__ == '__main__':
    formula = sys.argv[1]
    AST = parse_mltl(formula)
    # print(AST.pretty())
    nnf_formula = convert_nnf(AST)
    print(nnf_formula)