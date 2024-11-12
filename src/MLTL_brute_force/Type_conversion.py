from Custom_notation import *
from grammar import *


def array_To_string(array):
    len_array = len(array)
    if (array == []):
        return '[]'

    string = '['

    for i in Range(0, len_array-2, 1):
        string = string + array[i] + ', '

    string = string + array[len_array-1] + ']'
    return string

# If n = -1:
# Takes in string s, parses out Prop_var with largest natural number k.
# Returns the array: Prop_array = ["p0", "p1", ..., "pk"]
# 
# If n >= 0, returns the array: Prop_array = ["p0", "p1", ..., "p(n-1)"]  
def string_To_Prop_array(s, n = -1):
    len_s = len(s)
    Prop_array = []

    if (n == -1):
        # Parse for the largest Prop_var index in s
        max_index = -1
        for begin_index in Range(0, len_s-1, 1):
            if (Prop_var_check(Slice(s, begin_index, begin_index+1))):
                # Parse for Prop_var index
                begin_index = begin_index+1
                end_index = begin_index
                while(Num_check(Slice(s, begin_index, end_index)) and end_index <= len_s-1):
                    end_index += 1
                end_index -= 1

                # Parsing done, have Prop_var index
                index = int(Slice(s, begin_index, end_index))
                max_index = max(index, max_index)

        for i in Range(0, max_index, 1):
            prop_var = "p" + str(i)
            Prop_array.append(prop_var)

        return Prop_array
    
    else:
        for i in Range(0, n-1, 1):
            prop_var = "p" + str(i)
            Prop_array.append(prop_var)

        return Prop_array


def bit_To_bool(bit):
    if (bit == '0'):
        return False

    if (bit == '1'):
        return True

    return


def string_To_finite_model(s, Prop_array):
    len_s = len(s)
    len_Prop_array = len(Prop_array)
    finite_model = []
    if (s == ''):
        return finite_model

    num_of_entries = 1
    for i in Range(0, len_s-1, 1):
        if (Slice_char(s, i) == ','):
            num_of_entries = num_of_entries + 1


    for i in Range(0, num_of_entries-1, 1):
        finite_model.append(Slice(s, i * (len_Prop_array+1), i * (len_Prop_array+1) + len_Prop_array-1))

    return finite_model
