from Custom_notation import *

"""
This file implements the following Context-Free Grammar
for a well-formed formula for MLTL:

Alphabet = { ‘0’, ‘1’, …, ‘9’, ‘p’, ‘(‘, ‘)’, ‘[’, ‘]’, ‘,’ ,
                       ‘T’, ‘F’,
                       ‘~’, ‘F’, ‘G’,
                       ‘v’, ‘&’, ‘=’, ‘>’, ‘U’, ‘R’ }

Digit  ->  ‘0’ | ‘1’ | … |’9’
Num  ->  Digit Num |  Digit
Interval  ->  ‘[’  Num ‘,’ Num ‘]’
Prop_var  ->  ‘p’ Num

Prop_cons  ->  ‘T’ | ‘F’
Unary_Prop_conn  ->  ‘~’
Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’

Assoc_Prop_conn -> ‘v’ | ‘&’ | ‘=’
Array_entry -> Wff ‘,’ Array_entry  |  Wff

Unary_Temp_conn  ->  ‘F’ | ‘G’
Binary_Temp_conn  ->  ‘U’ | ‘R’


Wff ->  Prop_var | Prop_cons
                   | Unary_Prop_conn Wff
	                | Unary_Temp_conn  Interval  Wff
	             |  Assoc_Prop_conn ‘[‘  Array_entry  ‘]’

                 | ‘(‘ Wff Binary_Prop_conn Wff ‘)’
               | ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)


The Prop constants: True, False are represented by: ‘T’, ‘F’.
The Unary Prop connective: Negation is represented by: ‘~’.
The Binary Prop connectives: Or, And, Iff, Implies are represented by: ‘v’, ‘&’, ‘=’, ‘>’.

The Unary Temp connectives: Eventually, Always are represented by: ‘F’, ‘G’.
The Binary Temp connectives: Until, Weak until are represented by: ‘U’, ‘R’.

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


# Interval  ->  ‘[’  Num ‘,’ Num ‘]’
def Interval_check(s):
    len_s = len(s)

    left_bracket = Slice_char(s, 0)
    right_bracket = Slice_char(s, len_s-1)

    # Parse for comma index
    comma_index = 1
    while(Num_check(Slice(s, 1, comma_index)) and comma_index <= len_s-1):
        comma_index = comma_index + 1

    num_1 = Slice(s, 1, comma_index-1)
    comma = Slice_char(s, comma_index)
    num_2 = Slice(s, comma_index+1, len_s-2)
    return left_bracket == '[' and Num_check(num_1) and comma == ',' and Num_check(num_2) and right_bracket == ']'


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


# Assoc_Prop_conn -> ‘v’ | ‘&’ | ‘=’
def Assoc_Prop_conn_check(s):
    return s == 'v' or s == '&' or s == '='


# Array_entry -> Wff ‘,’ Array_entry  |  Wff
def Array_entry_check(s):
    len_s = len(s)

    # Number of '(' in s
    left_count = 0
    # Number of ')' in s
    right_count = 0


    #    Parse for comma_index in s
    #    When left_count == right_count, we are done parsing and have found comma_index.

    for comma_index in Range(1, len_s-1, 1):
        char = Slice_char(s, comma_index)

        if(char == '('):
            left_count = left_count + 1

        if(char == ')'):
            right_count = right_count + 1

        # Done parsing for comma_index.
        if(left_count == right_count and char == ','):
            break

    return (Wff_check(Slice(s, 0, comma_index-1)) and Slice_char(s, comma_index) == ',' and Array_entry_check(Slice(s, comma_index+1, len_s-1))) or Wff_check(s)


# Unary_Temp_conn  ->  ‘F’ | ‘G’
def Unary_Temp_conn_check(s):
    return s == 'F' or s == 'G'


# Binary_Temp_conn  ->  ‘U’ | ‘R’
def Binary_Temp_conn_check(s):
    return s == 'U' or s == 'R'



# Wff ->  Prop_var | Prop_cons
#                   | Unary_Prop_conn Wff
#	                | Unary_Temp_conn  Interval  Wff
#	             |  Assoc_Prop_conn ‘[‘  Array_entry  ‘]’
#
#                 | ‘(‘ Wff Binary_Prop_conn Wff ‘)’
#                 | ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)

def Wff_check(s):
    len_s = len(s)

    # Prop_var | Prop_cons
    if (Prop_var_check(s) or Prop_cons_check(s)):
        return True

    # Unary_Prop_conn Wff
    if (Unary_Prop_conn_check(Slice_char(s, 0))):
        alpha = Slice(s, 1, len_s-1)
        return  Wff_check(alpha)

    # Unary_Temp_conn  Interval  Wff
    if (Unary_Temp_conn_check(Slice_char(s, 0))):
        begin_interval = 1
        end_interval = 2

        # Parse for end of interval
        while (Slice_char(s, end_interval) != ']' and end_interval <= len_s-1):
            end_interval = end_interval + 1

        interval = Slice(s, begin_interval, end_interval)
        alpha = Slice(s, end_interval+1, len_s-1)
        return Interval_check(interval) and Wff_check(alpha)

    # Assoc_Prop_conn ‘[‘  Array_entry  ‘]’
    if(Assoc_Prop_conn_check(Slice_char(s, 0))):
        begin_array = 1
        end_array = 2

        # Parse for end of array
        while (Slice_char(s, end_array) != ']' and end_array <= len_s-1):
            end_array = end_array + 1

        array_entry = Slice(s, begin_array+1, end_array-1)
        return Slice_char(s, 1) == '[' and Array_entry_check(array_entry) and Slice_char(s, len_s-1) == ']'

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
    if (Slice_char(s, 0) == '(' and Slice_char(s, len_s-1) == ')'):

        # Number of '(' in s
        left_count = 0
        # Number of ')' in s
        right_count = 0


        #    Parse for binary_conn_index in s

        #    When left_count == right_count and s[binary_conn_index] is a binary connective,
        #    we are done parsing and have found binary_conn_index.

        for binary_conn_index in Range(1, len_s-1, 1):
            char = Slice_char(s, binary_conn_index)

            if(char == '('):
                left_count = left_count + 1

            if(char == ')'):
                right_count = right_count + 1

            # Done parsing for binary_conn_index.
            if(left_count == right_count and (Binary_Prop_conn_check(char) or Binary_Temp_conn_check(char))):
                break

        binary_conn = Slice_char(s, binary_conn_index)

        # ‘(‘ Wff Binary_Prop_conn Wff ‘)’
        if (Binary_Prop_conn_check(binary_conn)):
            alpha = Slice(s, 1, binary_conn_index-1)
            beta = Slice(s, binary_conn_index+1, len_s-2)
            return Wff_check(alpha) and Wff_check(beta)

        # ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
        if (Binary_Temp_conn_check(binary_conn)):
            begin_interval = binary_conn_index+1
            end_interval = binary_conn_index+2

            # Parse for end of interval
            while (Slice_char(s, end_interval) != ']' and end_interval <= len_s-1):
                end_interval = end_interval + 1

            alpha = Slice(s, 1, binary_conn_index-1)
            interval = Slice(s, begin_interval, end_interval)
            beta = Slice(s, end_interval+1, len_s-2)
            return Wff_check(alpha) and Interval_check(interval) and Wff_check(beta)

    return False


# Given a well-formed MLTL formula wff,
# return the index of the primary binary connective.
# If this does not occur, return -1.
def primary_binary_conn(wff):
    len_wff = len(wff)

    if (Assoc_Prop_conn_check(Slice_char(wff, 0))):
        return 0

    if (Slice_char(wff, 0) == '(' and Slice_char(wff, len_wff-1) == ')'):
        left_count = 0
        right_count = 0

        for binary_conn_index in Range(1, len_wff-1, 1):
            char = Slice_char(wff, binary_conn_index)

            if(char == '('):
                left_count = left_count + 1

            if(char == ')'):
                right_count = right_count + 1

            if(left_count == right_count and (Binary_Prop_conn_check(char) or Binary_Temp_conn_check(char))):
                break

        return binary_conn_index

    return -1


# Given a well-formed MLTL formula wff,
# return the tuple (begin_interval, comma_index, end_interval), giving the indexs
# for the primary interval occuring in the formula.
# This makes parsing for the temporal indexs easy.
#
# If there is no primary interval for the formula, return -1.
def primary_interval(wff):
    len_wff = len(wff)

    # Unary_Temp_conn  Interval  Wff
    if (Unary_Temp_conn_check(Slice_char(wff, 0))):
        begin_interval = 1

        # Parse for comma_index
        comma_index = begin_interval+1
        while (Num_check(Slice(wff, begin_interval+1, comma_index)) and comma_index <= len_wff-1):
            comma_index = comma_index+1

        # Parse for end_interval
        end_interval = comma_index+1
        while (Num_check(Slice(wff, comma_index+1, end_interval)) and end_interval <= len_wff-1):
            end_interval = end_interval + 1

        return (begin_interval, comma_index, end_interval)


    # ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
    binary_conn_index = primary_binary_conn(wff)
    if (Binary_Temp_conn_check(Slice_char(wff, binary_conn_index))):
        begin_interval = binary_conn_index+1

        # Parse for comma_index
        comma_index = begin_interval+1
        while (Num_check(Slice(wff, begin_interval+1, comma_index)) and comma_index <= len_wff-1):
            comma_index = comma_index+1

        # Parse for end_interval
        end_interval = comma_index+1
        while (Num_check(Slice(wff, comma_index+1, end_interval)) and end_interval <= len_wff-1):
            end_interval = end_interval + 1

        return (begin_interval, comma_index, end_interval)


    return -1




# Test Wff_check, primary_binary_conn, and primary_interval functions.
if __name__ == "__main__":
    wff = input ("Enter MLTL formula : ")
    wff = strip_whitespace(wff)
    assert (Wff_check(wff)), "Not a well-formed formula"

    print("primary_binary_conn: " + str(primary_binary_conn(wff)))
    print("primary_interval: " + str(primary_interval(wff)))
