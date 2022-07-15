from Custom_notation import *

# This file implements the following Context-Free Grammar:
# For a well-formed mLTL formula,

# Alphabet = { ‘0’, ‘1’, …, ‘9’, ‘p’, ‘(‘, ‘)’, ‘[’, ‘]’, ':', ‘,’ ,
#                        ‘T’, ‘!’,
#                        ‘~’, ‘F’, ‘G’,
#                        ‘v’, ‘&’, ‘=’, ‘>’, ‘U’, ‘R’ }

# Digit  ->  ‘0’ | ‘1’ | … |’9’
# Num  ->  Digit Num |  Digit
# Interval  ->  ‘[’  Num ‘:’ Num ‘]’
# Prop_var  ->  ‘p’ Num

# Prop_cons  ->  ‘T’ | ‘!’
# Unary_Prop_conn  ->  ‘~’
# Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’

# Assoc_Prop_conn -> ‘v’ | ‘&’ | ‘=’
# Array_entry -> Wff ‘,’ Array_entry  |  Wff

# Unary_Temp_conn  ->  ‘F’ | ‘G’
# Binary_Temp_conn  ->  ‘U’ | ‘R’


# Wff ->  Prop_var | Prop_cons
#                  | Unary_Prop_conn Wff
# 	             | Unary_Temp_conn  Interval  Wff

# 	             | '(' Assoc_Prop_conn ‘[‘  Array_entry  ']' ')'
#                  | '(' Wff Binary_Prop_conn Wff ')'
#                  | '(' Wff Binary_Temp_conn  Interval Wff ')'


# The Prop constants: True, False are represented by: ‘T’, ‘F’.
# The Unary Prop connective: Negation is represented by: ‘~’.
# The Binary Prop connectives: Or, And, Iff, Implies are represented by: ‘v’, ‘&’, ‘=’, ‘>’.

# The Unary Temp connectives: Eventually, Always are represented by: ‘F’, ‘G’.
# The Binary Temp connectives: Until, Weak until are represented by: ‘U’, ‘R’.



#  Digit  ->  ‘0’ | ‘1’ | … |’9’
#  Checks that the inputted string is a digit.
def Digit_check(s : str):
    return s == "0" or s == "1" or s == "2" or s == "3" or s == "4" \
                    or s == "5" or s == "6" or s == "7" or s == "8" or s == "9"


#  Num  ->  Digit Num |  Digit
#  Checks that the inputted string is of length 1 and then runs digit_check().  
def Num_check(s : str):
    len_s = len(s)
    if (len_s == 1):
        return Digit_check(s)

    c = Slice_char(s, 0)
    alpha = Slice(s, 1, len_s-1)
    return Digit_check(c) and Num_check(alpha)


#  Interval  ->  ‘[’  Num ‘,’ Num ‘]’
#  Checks that the inputted string is of the form of an interval.
def Interval_check(s : str):
    len_s = len(s)

    left_bracket = Slice_char(s, 0)
    right_bracket = Slice_char(s, len_s-1)

    # Parse for comma index
    comma_index = 1
    while(Num_check(Slice(s, 1, comma_index)) and comma_index <= len_s-1):
        comma_index += 1

    num_1 = Slice(s, 1, comma_index-1)
    comma = Slice_char(s, comma_index)
    num_2 = Slice(s, comma_index+1, len_s-2)
    return left_bracket == "[" and Num_check(num_1) and comma == ":" \
        and Num_check(num_2) and right_bracket == "]"



#   Prop_var  ->  ‘p’ Num
#  Checks that the inputted string is a propositional variable.
 
def Prop_var_check(s : str):
    len_s = len(s)

    c = Slice_char(s, 0)
    alpha = Slice(s, 1, len_s-1)
    return c == "p" and Num_check(alpha)



#  Prop_cons  ->  ‘T’ | ‘!’
#  Checks that the inputted string is a propositional constant.
 
def Prop_cons_check(s : str):
    return s == "T" or s == "!"



#   Unary_Prop_conn  ->  ‘~’
#   Checks that the inputted string is the negation symbol (the unary prop. connective).
 
def Unary_Prop_conn_check(s : str):
    return s == "~"



#   Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’
#   Checks that the inputted string is a binary prop. connective (or, and, equivalence, implication).
 
def Binary_Prop_conn_check(s : str):
    return s == "v" or s == "&" or s == "=" or s == ">"



#   Assoc_Prop_conn -> ‘v’ | ‘&’ | ‘=’
#   Checks that the inputted string is an associative prop. connective (or, and, equivalence).
 
def Assoc_Prop_conn_check(s : str):
    return s == "v" or s == "&" or s == "="


#   Array_entry -> Wff ‘,’ Array_entry  |  Wff
#   Checks that the inputted string is an array of WFFs.
#   We use an array of WFFs, for example, when ANDing >2 formulas.
 
def Array_entry_check(s : str):
    len_s = len(s)

    # Number of '(' in s
    left_count = 0
    # Number of ')' in s
    right_count = 0


    # Parse for comma_index in s
    # When left_count == right_count, we are done parsing and have found comma_index.
    for comma_index in Range(0, len_s-1, 1):
        c = Slice_char(s, comma_index)

        if (c == "("):
            left_count += 1

        elif (c == ")"):
            right_count += 1

        # Done parsing for comma_index.
        if (left_count == right_count and c == ","):
            break

    return (Wff_check(Slice(s, 0, comma_index-1)) and Slice_char(s, comma_index) == "," \
        and Array_entry_check(Slice(s, comma_index+1, len_s-1))) or Wff_check(s)



#   Unary_Temp_conn  ->  ‘F’ | ‘G’
#   Checks that the inputted string is F or G (the unary temporal connectives).
 
def Unary_Temp_conn_check(s : str):
    return s == "F" or s == "G"



#   Binary_Temp_conn  ->  ‘U’ | ‘R’
#   Checks that the inputted string is U or R (the binary temporal connectives).
 
def Binary_Temp_conn_check(s : str):
    return s == "U" or s == "R"



#  Wff ->  Prop_var | Prop_cons
#                   | Unary_Prop_conn Wff
#                   | Unary_Temp_conn  Interval  Wff
#                   | '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
#                   | ‘(‘ Wff Binary_Prop_conn Wff ‘)’
#                   | ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)
#   Checks that an inputted string is a WFF.
def Wff_check(s : str):
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
        while (Slice_char(s, end_interval) != "]" and end_interval <= len_s-1):
            end_interval = end_interval + 1

        interval = Slice(s, begin_interval, end_interval)
        alpha = Slice(s, end_interval+1, len_s-1)
        return Interval_check(interval) and Wff_check(alpha)

    # '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
    if (Assoc_Prop_conn_check(Slice_char(s, 1))):
        begin_array = 2
        end_array = len_s-2

        array_entry = Slice(s, begin_array+1, end_array-1)
        return Slice_char(s, 0) == "(" \
            and Slice_char(s, 2) == "[" \
            and Array_entry_check(array_entry) \
            and Slice_char(s, len_s - 2) == "]" \
            and Slice_char(s, len_s - 1) == ")" \

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
    if (Slice_char(s, 0) == "(" and Slice_char(s, len_s-1) == ")"):

        # Number of '(' in s
        left_count = 0
        # Number of ')' in s
        right_count = 0


        #    Parse for binary_conn_index in s

        #    When left_count == right_count and s[binary_conn_index] is a binary connective,
        #    we are done parsing and have found binary_conn_index.

        for binary_conn_index in Range(1, len_s-1, 1):
            c = Slice_char(s, binary_conn_index)

            if(c == "("):
                left_count += 1

            if(c == ")"):
                right_count += 1

            # Done parsing for binary_conn_index.
            if(left_count == right_count and (Binary_Prop_conn_check(c) or Binary_Temp_conn_check(c))):
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
            while (Slice_char(s, end_interval) != "]" and end_interval <= len_s-1):
                end_interval = end_interval + 1

            alpha = Slice(s, 1, binary_conn_index-1)
            interval = Slice(s, begin_interval, end_interval)
            beta = Slice(s, end_interval+1, len_s-2)
            return Wff_check(alpha) and Interval_check(interval) and Wff_check(beta)

    return False


# Returns the index of the primary binary connective of a WFF. 
def primary_binary_conn(wff : str):
    len_wff = len(wff)

    # '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
    if (Assoc_Prop_conn_check(Slice_char(wff, 1))):
        return 1

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’  |  ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)'
    if (Slice_char(wff, 0) == "(" and Slice_char(wff, len_wff-1) == ")"):
        left_count = 0
        right_count = 0

        for binary_conn_index in Range(1, len_wff-1, 1):
            c = Slice_char(wff, binary_conn_index)

            if(c == "("):
                left_count += 1

            if(c == ")"):
                right_count += 1

            if(left_count == right_count and (Binary_Prop_conn_check(c) or Binary_Temp_conn_check(c))):
                break

        return binary_conn_index

    else:
        error_string = wff + " does not have a primary binary conn.\n"
        raise Exception(error_string)



# Returns the indices where the primary interval is in a given WFF.
def primary_interval(wff : str):
    len_wff = len(wff)

    # Unary_Temp_conn  Interval  Wff
    if (Unary_Temp_conn_check(Slice_char(wff, 0))):
        begin_interval = 1

        # Parse for comma_index
        comma_index = begin_interval+1
        while (Num_check(Slice(wff, begin_interval+1, comma_index)) and comma_index <= len_wff-1):
            comma_index += 1

        # Parse for end_interval
        end_interval = comma_index+1
        while (Num_check(Slice(wff, comma_index+1, end_interval)) and end_interval <= len_wff-1):
            end_interval += 1

        return (begin_interval, comma_index, end_interval)


    # ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
    binary_conn_index = primary_binary_conn(wff)
    if (Binary_Temp_conn_check(Slice_char(wff, binary_conn_index))):
        begin_interval = binary_conn_index+1

        # Parse for comma_index
        comma_index = begin_interval+1
        while (Num_check(Slice(wff, begin_interval+1, comma_index)) and comma_index <= len_wff-1):
            comma_index += 1

        # Parse for end_interval
        end_interval = comma_index+1
        while (Num_check(Slice(wff, comma_index+1, end_interval)) and end_interval <= len_wff-1):
            end_interval += 1

        return (begin_interval, comma_index, end_interval)

   
    else:
        error_string = wff + " does not have a primary interval.\n"
        raise Exception(error_string)



# Determines the minimum computation length needed for a given WFF to not have out-of-bounds behavior.
def Comp_len(wff : str):
    len_wff = len(wff)

    # Prop_var
    if (Prop_var_check(wff)):
        return 1

    # Prop_cons
    if (Prop_cons_check(wff)):
        return 0

    c = Slice_char(wff, 0)
    # Unary_Prop_conn Wff
    if (Unary_Prop_conn_check(c)):
        alpha = Slice(wff, 1, len_wff-1)
        return Comp_len(alpha)

    # Unary_Temp_conn  Interval  Wff
    if (Unary_Temp_conn_check(c)):
        interval = primary_interval(wff)
        comma_index = interval[1]
        end_interval = interval[2]
        upperbound = int(Slice(wff, comma_index+1, end_interval-1))
        alpha = Slice(wff, end_interval+1, len_wff-1)
        return upperbound + Comp_len(alpha) 

    # '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
    c = Slice_char(wff, 1)
    if (Assoc_Prop_conn_check(c)):
        # Parse through '[' wff_1 ',' wff_2 ',' ... ',' wff_n ']' entry-by-entry
        # and iteratively compute: return_value = max(Comp_len(wff_1), ..., Comp_len(wff_n))
        begin_entry = 3
        return_value = 0
        for end_entry in Range(3, len_wff-1, 1):
            if (Wff_check(Slice(wff, begin_entry, end_entry))):
                alpha = Slice(wff, begin_entry, end_entry)
                Comp1 = Comp_len(alpha)

                # Take max of current return_value and Comp_len(alpha)
                if (return_value < Comp1):
                    return_value = Comp1

                # Update begin_entry so it has index of the first char of the next entry.
                begin_entry = end_entry + 2

        return return_value

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’  |  ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)'
    binary_conn_index = primary_binary_conn(wff)
    binary_conn = Slice_char(wff, binary_conn_index)

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’
    if (Binary_Prop_conn_check(binary_conn)):
        alpha = Slice(wff, 1, binary_conn_index-1) 
        Comp_alpha = Comp_len(alpha)
        beta = Slice(wff, binary_conn_index+1, len_wff-2)
        Comp_beta = Comp_len(beta)

        return max(Comp_alpha, Comp_beta)

    # ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)'
    if (Binary_Temp_conn_check(binary_conn)):
        interval = primary_interval(wff)
        comma_index = interval[1]
        end_interval = interval[2]
        upper_bound = int(Slice(wff, comma_index + 1, end_interval - 1))

        alpha = Slice(wff, 1, binary_conn_index-1)
        Comp_alpha = Comp_len(alpha) 
        beta = Slice(wff, end_interval+1, len_wff-2)
        Comp_beta = Comp_len(beta)
        
        return max((upper_bound-1) + Comp_alpha, upper_bound + Comp_beta)

    
    else:
        error_string = wff + " is not a well-formed formula.\n"
        raise Exception(error_string)