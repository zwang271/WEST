from MLTL_semantics import *
import pathlib

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


# Test next_finite_model function on input: wff.
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

    num_states = Comp_len(wff)
    #current_state = int(input ("Enter current state : "))
    #end_state = int(input ("Enter end state : "))

    #try:
    #    assert (0 <= current_state and current_state <= end_state)
    #except AssertionError:
    #    print("0 <= current_state <= end_state")
    #    return -2

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
        eval = Interpretation(wff, finite_model)

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



# Function for comparing output between brute-forcer and regex
# implementations
def comparison_output(wff, output_file, n = -1):
    wff = strip_whitespace(wff)

    try:
        assert (Wff_check(wff))
    except AssertionError:
        print("Not a well-formed formula")
        return -1

    Prop_array = string_To_Prop_array(wff, n)
    num_prop = len(Prop_array)
    # print("num_prop: " + str(num_prop) + '\n')

    num_states = Comp_len(wff)
    # print("num_states: " + str(num_states) + '\n')

    f = open(output_file, 'w')

    finite_model = ['0' * num_prop] * (num_states)

    satisfying_model = []
    for i in Range(1, 2**(num_prop * num_states), 1):
        eval = Interpretation(wff, finite_model, n)

        if eval:
            satisfying_model.append(finite_model.copy())

        finite_model = next_finite_model(num_prop, num_states, finite_model)

    # Write number of satisfying models at top of output_file file
    len_satisfying_model = len(satisfying_model)
    f.write(str(len_satisfying_model) + '\n')
   
    # Write satsifying finite models into output_file file
    for finite_model in satisfying_model:
        output_string = array_To_string(finite_model)
        len_output_string = len(output_string)
        # Remove '[', ']' characters from output_string
        output_string = Slice(output_string, 1, len_output_string-2)
        # Remove whitespace from output_string
        output_string = strip_whitespace(output_string)

        f.write(output_string + '\n')



    f.close()
    return 0


# Old-Driver code
# if __name__ == "__main__":
#     running = True
#     while(running):
#         error_code = -1
#         while error_code != 0:
#             error_code = execute_truth_table_program()

#         print("Enter 'q' to quit or 'r' to re-enter another formula:")
#         choice = input()
#         running = False if choice == 'q' else True


# New-Driver code
if __name__ == "__main__":
    # wff = "(p0 U [0:3] p1)"
    # wff = strip_whitespace(wff)
    # output_file = "0.txt"
    # n = 2
    # comparison_output(wff, output_file, n)  

    path = str(pathlib.Path(__file__).parent.resolve())
    verify_path = path[:-25] + "/MLTL_reg/MLTL/verify/"
    n = 4
    i = 0
    with open(verify_path + "formulas.txt") as f:
        for wff in f:
            output_file = verify_path + "/brute_force_outputs" + "/" + str(i) + ".txt"
            comparison_output(wff, output_file, n)
            print("Wrote to", output_file, "for", wff)
            i += 1
        
        print(i)
