from Custom_notation import *
from Type_conversion import *
from MLTL_wff import *

'''
This file implements the interpretation of a well-formed MLTL formula wff
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

    # Prop_var
    if (Prop_var_check(wff)):
        # Allow NO Out-Of-Bounds Behavior for Prop_var:
        if (current_state > len_finite_model-1):
            return False
        if (end_state > len_finite_model-1):
            end_state = len_finite_model-1

        # Evaluate Prop_var
        index = Prop_array.index(wff)
        truth_assign = finite_model[current_state]

        return bit_To_bool(Slice_char(truth_assign, index))


    # Prop_cons
    if (Prop_cons_check(wff)):
        # Prop_cons is AMBIVALENT to Out-Of-Bounds Behavior:

        if (wff == 'T'):
            return True

        if (wff == 'F'):
            return False


    # Temp_cons
    if (Temp_cons_check(wff)):
        # Temp_cons is AMBIVALENT to Out-Of-Bounds Behavior:

        return (current_state == 0)


    # Unary_Prop_conn Wff
    if (Unary_Prop_conn_check(Slice_char(wff, 0))):
        # Unary_Prop_conn is AMBIVALENT to Out-Of-Bounds Behavior:

        alpha = Slice(wff, 1, len_wff-1)
        return not Interpretation(alpha, Prop_array, current_state, end_state, finite_model)


    # Unary_Temp_conn Interval Wff
    if (Unary_Temp_conn_check(Slice_char(wff, 0))):
        # Update current and end states with temporal bounds
        old_current_state = current_state
        interval_tuple = primary_interval(wff)
        lower_bound = int(Slice(wff, interval_tuple[0]+1, interval_tuple[1]-1))
        upper_bound = int(Slice(wff, interval_tuple[1]+1, interval_tuple[2]-1))
        current_state = old_current_state + lower_bound
        end_state = old_current_state + upper_bound

        char = Slice_char(wff, 0)
        alpha = Slice(wff, interval_tuple[2]+1, len_wff-1)

        print(char + ", interval tuple: " + str(current_state) + ", " + str(end_state))

        if (char == 'E'):
            # Allow NO Out-Of-Bounds Behavior for E:
            if (current_state > len_finite_model-1):
                return False
            if (end_state > len_finite_model-1):
                end_state = len_finite_model-1

            # Evaluate 'E interval alpha'
            for state in Range(current_state, end_state, 1):
                if (Interpretation(alpha, Prop_array, state, end_state, finite_model)):
                    return True

            return False

        if (char == 'A'):
            # Allow ALL Out-Of-Bounds Behavior for A:
            if (current_state > len_finite_model-1):
                return True
            if (end_state > len_finite_model-1):
                end_state = len_finite_model-1

            # Evaluate 'A interval alpha'
            for state in Range(current_state, end_state, 1):
                if (not Interpretation(alpha, Prop_array, state, end_state, finite_model)):
                    return False

            return True


    binary_conn = primary_binary_conn(wff)
    char = Slice_char(wff, binary_conn)

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’
    if (Binary_Prop_conn_check(char)):
        alpha = Slice(wff, 1, binary_conn-1)
        beta = Slice(wff, binary_conn+1, len_wff-2)
        eval_alpha = Interpretation(alpha, Prop_array, current_state, end_state, finite_model)
        eval_beta = Interpretation(beta, Prop_array, current_state, end_state, finite_model)

        if (char == 'v'):
            # v is AMBIVALENT to Out-Of-Bounds Behavior:

            return eval_alpha or eval_beta

        if (char == '&'):
            # & is AMBIVALENT to Out-Of-Bounds Behavior:

            return eval_alpha and eval_beta

        if (char == '='):
            # = is AMBIVALENT to Out-Of-Bounds Behavior:

            return (eval_alpha and eval_beta) or (not eval_alpha and not eval_beta)

        if (char == '>'):
            # > is AMBIVALENT to Out-Of-Bounds Behavior:

            return (not eval_alpha) or eval_beta


    # ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)’
    if (Binary_Temp_conn_check(char)):
        # Update current and end states with temporal bounds
        old_current_state = current_state
        interval_tuple = primary_interval(wff)
        lower_bound = int(Slice(wff, interval_tuple[0]+1, interval_tuple[1]-1))
        upper_bound = int(Slice(wff, interval_tuple[1]+1, interval_tuple[2]-1))
        current_state = old_current_state + lower_bound
        end_state = old_current_state + upper_bound

        alpha = Slice(wff, 1, binary_conn-1)
        beta = Slice(wff, interval_tuple[2]+1, len_wff-2)

        print( char + ", interval tuple: " + str(current_state) + ", " + str(end_state))

        if(char == 'U'):
            # Allow NO Out-Of-Bounds Behavior for U:
            if (current_state > len_finite_model-1):
                return False
            if (end_state > len_finite_model-1):
                end_state = len_finite_model-1

            # Evaluate '( alpha U interval beta )'
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
            # Allow ALL Out-Of-Bounds Behavior for W:
            if (current_state > len_finite_model-1):
                return True
            if (end_state > len_finite_model-1):
                end_state = len_finite_model-1

            # Evaluate '( alpha W interval beta )'
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
    wff = input ("Enter MLTL formula : ")
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
