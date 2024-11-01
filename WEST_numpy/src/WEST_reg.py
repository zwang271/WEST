# Author: Zili Wang
# Last updated: Nov 1st 2024
import sys
from MLTL_parser import parse_mltl

def WEST_reg(node):
    while node.data == "wff":
        node = node.children[0]
        wff_type = node.data
    if wff_type == "wff_base":
        child = node.children[0]
        assert(child.data in ["prop_cons", "prop_var", "wff"])
        if child.data == "prop_cons":
            print(child.children[0].data)
        elif child.data == "prop_var":
            variable = child.children[0].children[0].value
            print(variable)
        elif child.data == "wff":
            print(WEST_reg(child))
            
    if wff_type == "wff_neg":
        child1, child2 = node.children
        assert(child1.data == "neg" and child2.data == "wff")
        if child1.data == "neg":
            print(f"(!{WEST_reg(child2)})")
        
    if len(node.children) == 3:
        child1, child2, child3 = node.children
        if wff_type == "wff_untemp":
            temp_conn = child1.children[0].data
            lb = child2.children[0].children[0].value
            ub = child2.children[1].children[0].value
            wff = WEST_reg(child3)
            print(f"({temp_conn} {lb} {ub} {wff})")
        if wff_type == "wff_binprop":
            prop_conn = child2.children[0].data
            wff1 = WEST_reg(child1)
            wff2 = WEST_reg(child3)
            print(f"({wff1} {prop_conn} {wff2})")
        
    if wff_type == "wff_bintemp":
        child1, child2, child3, child4 = node.children
        assert(child1.data == "wff" and child2.data == "binary_temp_conn" and
               child3.data == "interval" and child4.data == "wff")
        temp_conn = child2.children[0].data
        wff1 = WEST_reg(child1)
        lb = child3.children[0].children[0].value
        ub = child3.children[1].children[0].value
        wff2 = WEST_reg(child4)
        print(f"({temp_conn} {lb} {ub} {wff1} {wff2})")


if __name__ == '__main__':
    formula = sys.argv[1]
    AST = parse_mltl(formula)
    WEST_reg(AST)
