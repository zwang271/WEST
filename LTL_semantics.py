from Custom_notation import *
from Type_conversion import *
from LTL_wff import *

'''
This file implements the interpretation of a well-formed LTL formula wff
given input: current state, end state, and string that encodes a finite model.

Let Prop_array be the list of Prop_var's occuring in wff and len_Prop_array
the length of this list (i.e. the number of Prop var's in wff).

A Truth assignment on Prop_array is simply a bit string of length len_Prop_array
while a finite model on Prop_array is simply a list of truth assignments on
Prop_array.
'''


# Determines whether a given string is a bit string
def Bit_string_check(s):
    len_s = len(s)
    for i in Range(0, len_s-1, 1):
        if (Slice_char(s, i) != '0' and Slice_char(s, i) != '1'):
            return False

    return True


# Determines whether a string s encodes a Truth assignment for Prop_array
def Truth_assign_check(s, Prop_array):
    len_s = len(s)
    len_Prop_array = len(Prop_array)

    return (len_s == len_Prop_array) and Bit_string_check(s)


# Determines whether a string s encodes a finite model for Prop_array.
def Finite_model_check(s, Prop_array):
    len_s = len(s)
    len_Prop_array = len(Prop_array)

    for i in Range(0, len_s - (len_Prop_array+1), len_Prop_array+1):
        truth_assign = Slice(s, i, i + len_Prop_array-1)
        comma = Slice_char(s, i + len_Prop_array)

        if (not (Truth_assign_check(truth_assign, Prop_array) and comma == ',' ) ):
            return False

    truth_assign = Slice(s, len_s-len_Prop_array, len_s-1)
    return Truth_assign_check(truth_assign, Prop_array)


def Interpretation (wff, Prop_array, current_state, end_state, finite_model):
    len_wff = len(wff)
    len_finite_model = len(finite_model)

    # Allow unconstrained behavior outside of finite_model's range.
    if (current_state > len_finite_model-1):
        return True

    # Allow unconstrained behavior outside of finite_model's range.
    if (end_state > len_finite_model-1):
        end_state = len_finite_model-1

    # Now 0 <= current_state <= end_state <= len_finite_model-1

    # Prop_var
    if (Prop_var_check(wff)):
        index = Prop_array.index(wff)
        truth_assign = finite_model[current_state]

        return bit_To_bool(Slice_char(truth_assign, index))


    # Prop_cons
    if (Prop_cons_check(wff)):
        if (wff == 'T'):
            return True

        if (wff == 'F'):
            return False


    # Temp_cons
    if (Temp_cons_check(wff)):
        return (current_state == 0)


    # Unary_Prop_conn Wff
    if (Unary_Prop_conn_check(Slice_char(wff, 0))):
        alpha = Slice(wff, 1, len_wff-1)
        return not Interpretation(alpha, Prop_array, current_state, end_state, finite_model)


    # Unary_Temp_conn Wff
    if (Unary_Temp_conn_check(Slice_char(wff, 0))):
        char = Slice_char(wff, 0)
        alpha = Slice(wff, 1, len_wff-1)

        if (char == 'N'):
            return Interpretation(alpha, Prop_array, current_state+1, end_state, finite_model)

        if (char == 'E'):
            for state in Range(current_state, end_state, 1):
                if (Interpretation(alpha, Prop_array, state, end_state, finite_model)):
                    return True

            return False

        if (char == 'A'):
            for state in Range(current_state, end_state, 1):
                if (not Interpretation(alpha, Prop_array, state, end_state, finite_model)):
                    return False

            return True


    binary_conn = primary_binary_conn(wff)
    char = Slice_char(wff, binary_conn)
    alpha = Slice(wff, 1, binary_conn-1)
    beta = Slice(wff, binary_conn+1, len_wff-2)

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’
    if (Binary_Prop_conn_check(char)):
        eval_alpha = Interpretation(alpha, Prop_array, current_state, end_state, finite_model)
        eval_beta = Interpretation(beta, Prop_array, current_state, end_state, finite_model)

        if (char == 'v'):
            return eval_alpha or eval_beta

        if (char == '&'):
            return eval_alpha and eval_beta

        if (char == '='):
            return (eval_alpha and eval_beta) or (not eval_alpha and not eval_beta)

        if (char == '>'):
            return (not eval_alpha) or eval_beta


    # ‘(‘ Wff Binary_Temp_conn Wff ‘)’
    if (Binary_Temp_conn_check(char)):
        if(char == 'U'):
            state = current_state
            eval_alpha = Interpretation(alpha, Prop_array, state, end_state, finite_model)
            eval_beta = Interpretation(beta, Prop_array, state, end_state, finite_model)

            while (state <= end_state-1 and eval_alpha):
                if (eval_beta):
                    return True
                state = state+1
                eval_alpha = Interpretation(alpha, Prop_array, state, end_state, finite_model)
                eval_beta = Interpretation(beta, Prop_array, state, end_state, finite_model)

            return eval_beta

        if(char == 'W'):
            state = current_state
            eval_alpha = Interpretation(alpha, Prop_array, state, end_state, finite_model)
            eval_beta = Interpretation(beta, Prop_array, state, end_state, finite_model)

            while (state <= end_state-1 and eval_alpha):
                if (eval_beta):
                    return True
                state = state+1
                eval_alpha = Interpretation(alpha, Prop_array, state, end_state, finite_model)
                eval_beta = Interpretation(beta, Prop_array, state, end_state, finite_model)

            return eval_beta or eval_alpha


    raise Exception(wff + "is not a well-formed formula.")


# Test Interpretation function on input: wff, current_state, end_state, finite_model_string
if __name__ == "__main__":
    wff = input ("Enter LTL formula : ")
    wff = strip_whitespace(wff)
    assert (Wff_check(wff)), "Not a well-formed formula"


    Prop_array = string_To_Prop_array(wff)


    current_state = int(input ("Enter current state : "))
    end_state = int(input ("Enter end state : "))
    assert (0 <= current_state and current_state <= end_state), "0 <= current_state <= end_state"


    message = "Enter Finite model for " + array_To_string(Prop_array) + " (with entries seperated by ',') :\n"
    finite_model_string = input(message)
    finite_model_string = strip_whitespace(finite_model_string)
    assert (Finite_model_check(finite_model_string, Prop_array)), "Not a Finite model for " + array_To_string(string_To_Prop_array(wff))
    finite_model = string_To_finite_model(finite_model_string, Prop_array)


    print(Interpretation(wff, Prop_array, current_state, end_state, finite_model))
