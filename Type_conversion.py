from Custom_notation import *
from MLTL_wff import *


def array_To_string(array):
    len_array = len(array)
    if (array == []):
        return '[]'

    string = '['

    for i in Range(0, len_array-2, 1):
        string = string + array[i] + ', '

    string = string + array[len_array-1] + ']'
    return string


def string_To_Prop_array(s):
    len_s = len(s)
    Prop_array = []

    # Append every Prop_var found in s to Prop_array
    for i in Range(0, len_s-1, 1):
        if (Prop_var_check(Slice(s, i, i+1))):
            j = i+2
            while(Prop_var_check(Slice(s, i, j)) and j <= len_s-1):
                j = j+1
            j = j-1
            Prop_array.append(Slice(s, i, j))

    # Remove duplicate entries in Prop_array
    res = []
    [res.append(x) for x in Prop_array if x not in res]
    Prop_array = res
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
