from lark import Lark, Transformer, v_args, exceptions
import sys
import re

# Define a grammar using EBNF notation
grammar = r'''
start: wff

digit: "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
num: digit num | digit
interval: "[" num "," num "]"
prop_var: "p" num

prop_cons: "true" | "false"
neg: "~"
implies: "->" 
and: "&"
or: "|"
equiv: "="

unary_temp_conn: "F" | "G"
binary_temp_conn: "U" | "R"

assoc_or: "(" wff or assoc_or_tail ")"
assoc_or_tail: wff or assoc_or_tail | wff

assoc_and: "(" wff and assoc_and_tail ")"
assoc_and_tail: wff and assoc_and_tail | wff

assoc_equiv: "(" wff equiv assoc_equiv_tail ")"
assoc_equiv_tail: wff equiv assoc_equiv_tail | wff

assoc_prop_conn: and | or | equiv
pre_assoc_expr: assoc_prop_conn "([" wff "," pre_assoc_tail "])"
pre_assoc_tail: wff "," pre_assoc_tail | wff

wff: prop_var | prop_cons
   | neg wff
   | unary_temp_conn interval wff
   | "(" wff binary_temp_conn interval wff ")"
   | "(" wff implies wff ")"
   | assoc_or 
   | assoc_and
   | assoc_equiv
   | pre_assoc_expr

%import common.WS
%ignore WS
'''
parser = Lark(grammar, parser='lalr', start='wff')


def check_wff(input_string, verbose=True):
    if len(input_string) == 0:
        return False, None
    try:
        # Attempt to parse the input string
        tree = parser.parse(input_string)
        return True, tree, None
    except exceptions.LarkError as e:
        return False, None, e


def translate_syntax(formula):
    formula = formula.replace("or", "v").replace("|", "v") # or, | ---> v
    formula = formula.replace("and", "&") # and ---> &
    formula = formula.replace("->", ">") # "->" ---> ">"
    formula = formula.replace("true", "T") # true ---> T
    formula = formula.replace("false", "!") # false ---> !
    # [num, num] ---> [num: num]
    for interval in re.findall("\s*[0-9]+\s*,\s*[0-9]+\s*", formula):
        formula = formula.replace(interval, interval.replace(",", ":"))
    return formula

def translate_inorder(formula:str):
    assoc_ops = ["&", "|", "=", "->"]
    start, end = formula.find("("), formula.rfind(")")
    if start < 0 or end < 0:
        return formula

    in_order = formula[start:end+1]
    in_order = in_order[1:-1].replace(" ", "") # remove opening and closing paren, and spaces

    idx = 0
    parse_op = False # flag to determine whether to parse for op or wff
    arg_list, op_list = [], [] # lists of arguments and operations parsed from in order
    for i, c in enumerate(in_order):
        token = in_order[idx:i+1]
        # print(idx, i, token)
        if not parse_op:
            is_wff, tree, e = check_wff(token, verbose=False)
            if not is_wff:
                continue
            arg_list.append(token)
            idx, parse_op = i+1, not parse_op
        else: # parsing for op
            if token not in assoc_ops:
                continue
            op_list.append(token)
            idx, parse_op = i+1, not parse_op

    # print(arg_list)
    # print(op_list, "\n")   

    # recursive call to rewrite all the wff inside arg_list
    arg_list = [translate_inorder(arg) for arg in arg_list] 

    op = op_list[0]
    if len(op_list) == 1:
        return formula[:start] + "(" + f" {op} ".join(arg_list) + ")" + formula[end+1:]

    if len(set(op_list)) > 1: # should never reach here
        print("something went wrong...")

    arg_list = ", ".join(arg_list)
    rewrite = f"{op} [{arg_list}]"
    return formula[:start] + "(" + rewrite + ")" + formula[end+1:]


def to_west(wff : str, tree):
    wff = translate_inorder(wff)
    wff = translate_syntax(wff)
    return wff


if __name__ == '__main__':
    if len(sys.argv) != 2:
        quit()

    wff = sys.argv[1]
    is_wff, tree, e = check_wff(wff)
    if is_wff:
        wff = to_west(wff, tree)
        print(wff)
        # write west style wff to file for commandline tool
    else:
        print(e)