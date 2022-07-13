from MLTL_semantics import *

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
def execute_truth_table_program():
    wff = input ("Enter MLTL formula : ")
    wff = strip_whitespace(wff)

    try:
        assert (Wff_check(wff))
    except AssertionError:
        print("Not a well-formed formula")
        return -1

    Prop_array = string_To_Prop_array(wff)
    num_prop = len(Prop_array)

    num_states = int(input ("Enter number of states for finite_model : "))
    current_state = int(input ("Enter current state : "))
    end_state = int(input ("Enter end state : "))

    try:
        assert (0 <= current_state and current_state <= end_state)
    except AssertionError:
        print("0 <= current_state <= end_state")
        return -2

    f = open('output.txt', 'w')
    f.write(f"Formula: " + wff)
    f.write("\nProp array: " + array_To_string(Prop_array) + '\n\n')

    true_output = open('true_output.txt', 'w')
    true_output.write(f"Formula: " + wff)
    true_output.write("\nProp array: " + array_To_string(Prop_array) + '\n\n')

    false_output = open('false_output.txt', 'w')
    false_output.write(f"Formula: " + wff)
    false_output.write("\nProp array: " + array_To_string(Prop_array) + '\n\n')

    finite_model = ['0' * num_prop] * (num_states)

    for i in Range(1, 2**(num_prop * num_states), 1):
        eval = Interpretation(wff, Prop_array, current_state, end_state, finite_model)

        f.write(array_To_string(finite_model) + '   ' + str(eval) + '\n')
        if eval:
            true_output.write(array_To_string(finite_model) + '   ' + str(eval) + '\n')
        else:
            false_output.write(array_To_string(finite_model) + '   ' + str(eval) + '\n')

        finite_model = next_finite_model(num_prop, num_states, finite_model)


    f.close()
    true_output.close()
    false_output.close()
    return 0


# Driver code
if __name__ == "__main__":
    running = True
    while(running):
        error_code = -1
        while error_code != 0:
            error_code = execute_truth_table_program()

        print("Enter 'q' to quit or 'r' to re-enter another formula:")
        choice = input()
        running = False if choice == 'q' else True
