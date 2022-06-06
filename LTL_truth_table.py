from LTL_semantics import *

'''
Given input: wff, num_states, current_state, end_state
this file interprets wff in every possible finite model
of length num_states.

It does this by iterating over all possible finite models of length num_states,
and then printing out the interpretation of wff in that model.
'''


def bit_string_add_one(s):
    len_s = len(s)

    for i in Range(len_s-1, 0, -1):
        if (Slice_char(s, i) == '0'):
            s = Slice(s, 0, i-1) + '1' + Slice(s, i+1, len_s-1)
            break

        if (Slice_char(s, i) == '1'):
            s = Slice(s, 0, i-1) + '0' + Slice(s, i+1, len_s-1)

    return s


def next_finite_model(num_prop, num_states, finite_model):
    for i in Range(num_states-1, 0, -1):
        if (finite_model[i] != ('1' * num_prop)):
            finite_model[i] = bit_string_add_one(finite_model[i])
            break
        else:
            finite_model[i] = bit_string_add_one(finite_model[i])

    return finite_model



# Test next_finite_model function on input: wff, num_states, current_state,
# and end_state.
if __name__ == "__main__":
    wff = input ("Enter LTL formula : ")
    wff = strip_whitespace(wff)
    assert (Wff_check(wff)), "Not a well-formed formula"

    Prop_array = string_To_Prop_array(wff)
    num_prop = len(Prop_array)

    num_states = int(input ("Enter number of states for finite_model : "))
    current_state = int(input ("Enter current state : "))
    end_state = int(input ("Enter end state : "))
    assert (0 <= current_state and current_state <= end_state), "0 <= current_state <= end_state"


    print()    
    print("Prop array: ", end = '')
    print_array(Prop_array)
    print()
    print()


    finite_model = ['0' * num_prop] * (num_states)

    for i in Range(1, 2**(num_prop * num_states), 1):
        eval = Interpretation(wff, Prop_array, current_state, end_state, finite_model)
        print_array(finite_model)
        print('   ' + str(eval))
        finite_model = next_finite_model(num_prop, num_states, finite_model)
