from Custom_notation import *
from Type_conversion import *
from nnf_grammar import *

'''
This file implements the interpretation of a well-formed MLTL formula wff
given input: current state, end state, and string that encodes a finite model.

Let Prop_array be the list of Prop_var's occuring in wff and len_Prop_array
the length of this list (i.e. the number of Prop var's in wff).

A Truth assignment on Prop_array is simply a bit string of length len_Prop_array
while a finite model on Prop_array is simply a list of truth assignments on
Prop_array.
'''

# Wff ->  Prop_var | Prop_cons
#                  | Unary_Prop_conn Wff
#	               | Unary_Temp_conn  Interval  Wff
	            
#                  | ‘(‘ Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ‘)’
#                  | ‘(‘ Wff Binary_Prop_conn Wff ‘)’
#                  | ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)    

# Here Prop_array is the array of Prop_vars in wff, current_state and end_state
# is the sub-interval in finite_model where wff is evaluated in.
# n is an optional parameter and determines the number of Prop_vars being evaluated.
def Interpretation_aux (wff, Prop_array, current_state, end_state, finite_model, n = -1):
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

        if (wff == '!'):
            return False


    # Unary_Prop_conn Wff
    if (Unary_Prop_conn_check(Slice_char(wff, 0))):
        # Unary_Prop_conn is AMBIVALENT to Out-Of-Bounds Behavior:

        alpha = Slice(wff, 1, len_wff-1)
        return not Interpretation_aux(alpha, Prop_array, current_state, end_state, finite_model)


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

        if (char == 'F'):
            # Allow NO Out-Of-Bounds Behavior for F:
            if (current_state > len_finite_model-1):
                return False
            if (end_state > len_finite_model-1):
                end_state = len_finite_model-1

            # Evaluate 'F Interval alpha'
            for state in Range(current_state, end_state, 1):
                if (Interpretation_aux(alpha, Prop_array, state, end_state, finite_model)):
                    return True

            return False

        if (char == 'G'):
            # Allow ALL Out-Of-Bounds Behavior for G:
            if (current_state > len_finite_model-1):
                return True
            if (end_state > len_finite_model-1):
                end_state = len_finite_model-1

            # Evaluate 'G Interval alpha'
            for state in Range(current_state, end_state, 1):
                if (not Interpretation_aux(alpha, Prop_array, state, end_state, finite_model)):
                    return False

            return True


#  ‘(‘ Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ‘)’
    if (Assoc_Prop_conn_check(Slice_char(wff, 1))):
        char = Slice_char(wff, 1)
        array_entry = Slice(wff, 3, len_wff-3)
        subformulas = Array_entry_subformulas(array_entry)

        if (char == "v"):
            # v is AMBIVALENT to Out-Of-Bounds Behavior:

            flag = False
            for alpha in subformulas:
                Prop_alpha_array = string_To_Prop_array(alpha, n)
                eval_alpha = Interpretation_aux(alpha, Prop_alpha_array, current_state, end_state, finite_model)
                flag = flag or eval_alpha
            return flag

        if (char == "&"):
            # & is AMBIVALENT to Out-Of-Bounds Behavior:

            flag = True
            for alpha in subformulas:
                Prop_alpha_array = string_To_Prop_array(alpha, n)
                eval_alpha = Interpretation_aux(alpha, Prop_alpha_array, current_state, end_state, finite_model)
                flag = flag and eval_alpha
            return flag

        if (char == "="):
            # = is AMBIVALENT to Out-Of-Bounds Behavior:

            flag = False
            for alpha in subformulas:
                Prop_alpha_array = string_To_Prop_array(alpha, n)
                eval_alpha = Interpretation_aux(alpha, Prop_alpha_array, current_state, end_state, finite_model)
                flag = (flag and eval_alpha) or (not flag and not eval_alpha)
            return flag


    binary_conn = primary_binary_conn(wff)
    char = Slice_char(wff, binary_conn)

    # ‘(‘ Wff Binary_Prop_conn Wff ‘)’
    if (Binary_Prop_conn_check(char)):
        alpha = Slice(wff, 1, binary_conn-1)
        beta = Slice(wff, binary_conn+1, len_wff-2)
        eval_alpha = Interpretation_aux(alpha, Prop_array, current_state, end_state, finite_model)
        eval_beta = Interpretation_aux(beta, Prop_array, current_state, end_state, finite_model)

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

        # print( char + ", interval tuple: " + str(current_state) + ", " + str(end_state))

        if(char == 'U'):
            # Allow NO Out-Of-Bounds Behavior for U:
            if (current_state > len_finite_model-1):
                return False
            if (end_state > len_finite_model-1):
                end_state = len_finite_model-1

            # Evaluate '( alpha U Interval beta )'
            state = current_state
            eval_alpha = Interpretation_aux(alpha, Prop_array, state, end_state, finite_model)
            eval_beta = Interpretation_aux(beta, Prop_array, state, end_state, finite_model)

            while (state <= end_state-1 and eval_alpha):
                if (eval_beta):
                    return True
                state = state+1
                eval_alpha = Interpretation_aux(alpha, Prop_array, state, end_state, finite_model)
                eval_beta = Interpretation_aux(beta, Prop_array, state, end_state, finite_model)

            return eval_beta

        # αR[a,b]β if and only if |π| ≤ a or ∀s ∈ [a, b], (πs ⊨ β or ∃t ∈ [a, s − 1], πt ⊨ α)    
        if(char == 'R'):
            # Allow ALL Out-Of-Bounds Behavior for R:
            if (current_state > len_finite_model-1):
                return True
            if (end_state > len_finite_model-1):
                end_state = len_finite_model-1

            # Evaluate '( alpha R Interval beta )'
            for state in Range(current_state, end_state, 1):
                # counter_ex_flag = False
                # eval_beta = Interpretation_aux(beta, Prop_array, state, end_state, finite_model)
                # if(not eval_beta):
                #     for t in Range(current_state, state-1, 1):
                #         # eval_alpha = Interpretation_aux(alpha, Prop_array, current_state, t, finite_model)
                #         eval_alpha = Interpretation_aux(alpha, Prop_array, t, end_state, finite_model)
                #         if (eval_alpha):
                #             break

                #         if (not eval_alpha and t == state-1):
                #             counter_ex_flag = True
                
                # if(counter_ex_flag):
                #     return False

                counter_ex_flag = False
                eval_beta = Interpretation_aux(beta, Prop_array, state, end_state, finite_model)

                if (not eval_beta and state == current_state):
                    return False

                if(not eval_beta):
                    for t in Range(current_state, state-1, 1):
                        # eval_alpha = Interpretation_aux(alpha, Prop_array, current_state, t, finite_model)
                        eval_alpha = Interpretation_aux(alpha, Prop_array, t, end_state, finite_model)
                        if (eval_alpha):
                            break

                        if (not eval_alpha and t == state-1):
                            counter_ex_flag = True
                
                if(counter_ex_flag):
                    return False

            return True


    raise Exception(wff + "is not a well-formed formula.")


# n is an optional parameter and determines the number of Prop_vars being evaluated.
def Interpretation(wff : str, finite_model, n = -1):
    Prop_array = string_To_Prop_array(wff, n)
    current_state = 0
    end_state = Comp_len(wff)-1
    return Interpretation_aux(wff, Prop_array, current_state, end_state, finite_model, n)


# Test Interpretation function on input: wff, finite_model_string
if __name__ == "__main__":
    wff = input ("Enter MLTL formula : ")
    wff = strip_whitespace(wff)
    assert (Wff_check(wff)), "Not a well-formed formula"


    Prop_array = string_To_Prop_array(wff)


    #current_state = int(input ("Enter current state : "))
    #end_state = int(input ("Enter end state : "))
    #assert (0 <= current_state and current_state <= end_state), "0 <= current_state <= end_state"


    message = "Enter Finite model for " + array_To_string(Prop_array) + " (with entries seperated by ',') :\n"
    finite_model_string = input(message)
    finite_model_string = strip_whitespace(finite_model_string)
    #assert (Finite_model_check(finite_model_string, Prop_array)), "Not a Finite model for " + array_To_string(string_To_Prop_array(wff))
    finite_model = string_To_finite_model(finite_model_string, Prop_array)

    print(Interpretation(wff, finite_model))
