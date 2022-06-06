from Custom_notation import *

"""
This file implements the following Context-Free Grammar
for a well-formed formula for LTL:

Digit  ->  ‘0’ | ‘1’ | … |’9’
Num  ->  Digit Num |  Digit
Prop_var  ->  ‘p’ Num

Prop_cons  ->  ‘T’ | ‘F’
Unary_Prop_conn  ->  ‘~’
Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’

Temp_cons  ->  ‘S’
Unary_Temp_conn  ->  ‘N’ | ‘E’ | ‘A’
Binary_Temp_conn  ->  ‘U’ | ‘W’


Wff  ->  Prop_var | Prop_cons | Temp_cons
                   | Unary_Prop_conn Wff | Unary_Temp_conn Wff
                   | ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn Wff ‘)’


The Prop constants: True, False are represented by: ‘T’, ‘F’.
The Unary Prop connective: Negation is represented by: ‘~’.
The Binary Prop connectives: Or, And, Iff, Implies are represented by: ‘v’, ‘&’, ‘=’, ‘>’.

The Temp constant: Start is represented by: ‘S’.
The Unary Temp connectives: Next, Eventually, Always are represented by: ‘N’, ‘E’, ‘A’.
The Binary Temp connectives: Until, Weak until are represented by: ‘U’, ‘W’.

"""


# Digit  ->  ‘0’ | ‘1’ | … |’9’
def Digit_check(s):
    return s == '0' or s == '1' or s == '2' or s == '3' or s == '4' or s == '5' or s == '6' or s == '7' or s == '8' or s == '9'


# Num  ->  Digit Num |  Digit
def Num_check(s):
    len_s = len(s)
    if (len_s == 1):
        return Digit_check(s)

    char = Slice_char(s, 0)
    alpha = Slice(s, 1, len_s-1)
    return Digit_check(char) and Num_check(alpha)


# Prop_var  ->  ‘p’ Num
def Prop_var_check(s):
    len_s = len(s)

    char = Slice_char(s, 0)
    alpha = Slice(s, 1, len_s-1)
    return char == 'p' and Num_check(alpha)


# Prop_cons  ->  ‘T’ | ‘F’
def Prop_cons_check(s):
    return s == 'T' or s == 'F'


# Unary_Prop_conn  ->  ‘~’
def Unary_Prop_conn_check(s):
    return s == '~'


# Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’
def Binary_Prop_conn_check(s):
    return s == 'v' or s == '&' or s == '=' or s == '>'


# Temp_cons  ->  ‘S’
def Temp_cons_check(s):
    return s == 'S'

# Unary_Temp_conn  ->  ‘N’ | ‘E’ | ‘A’
def Unary_Temp_conn_check(s):
    return s == 'N' or s == 'E' or s == 'A'


# Binary_Temp_conn  ->  ‘U’ | ‘W’
def Binary_Temp_conn_check(s):
    return s == 'U' or s == 'W'



#Wff  ->  Prop_var | Prop_cons | Temp_cons
#                   | Unary_Prop_conn Wff | Unary_Temp_conn Wff
#                   | ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn Wff ‘)’

def Wff_check(s):
    len_s = len(s)

    # Prop_var | Prop_cons | Temp_cons
    if (Prop_var_check(s) or Prop_cons_check(s) or Temp_cons_check(s)):
        return True

    # Unary_Prop_conn Wff | Unary_Temp_conn Wff
    if (Unary_Prop_conn_check(Slice_char(s, 0)) or Unary_Temp_conn_check(Slice_char(s, 0))):
        alpha = Slice(s, 1, len_s-1)
        return  Wff_check(alpha)

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn Wff ‘)
    if (Slice_char(s, 0) == '(' and Slice_char(s, len_s-1) == ')'):

        # Number of '(' in s
        left_count = 0
        # Number of ')' in s
        right_count = 0


        #    s = '(' + alpha + s[binary_conn] + beta + ')'
        #    where s[binary_conn] is a binary connecitve.

        #    When left_count == right_count and s[binary_conn] is a binary connective,
        #    we are done parsing alpha and have found binary_conn.

        for binary_conn in Range(1, len_s-1, 1):
            char = Slice_char(s, binary_conn)

            if(char == '('):
                left_count = left_count + 1

            if(char == ')'):
                right_count = right_count + 1

            # Done parsing alpha.
            if(left_count == right_count and (Binary_Prop_conn_check(char) or Binary_Temp_conn_check(char))):
                break

        alpha = Slice(s, 1, binary_conn-1)
        beta = Slice(s, binary_conn+1, len_s-2)
        return Wff_check(alpha) and Wff_check(beta)

    return False


# Given a well-formed PTL formula wff,
# return the index of the primary binary connective.
# If this does not occur, return -1.
def primary_binary_conn(wff):
    len_wff = len(wff)

    if (Slice_char(wff, 0) == '(' and Slice_char(wff, len_wff-1) == ')'):
        left_count = 0
        right_count = 0

        for binary_conn in Range(1, len_wff-1, 1):
            char = Slice_char(wff, binary_conn)

            if(char == '('):
                left_count = left_count + 1

            if(char == ')'):
                right_count = right_count + 1

            if(left_count == right_count and (Binary_Prop_conn_check(char) or Binary_Temp_conn_check(char))):
                break

        return binary_conn

    return -1
