# Makes Slice operator closed instead of half-open interval
def Slice(s, a, b):
    return s[a : b+1]


# Safe way to retrieve char at index i out of string s.
# If i is not in index, returns empty string ''.
def Slice_char(s, i):
  	return Slice(s, i, i)


# Makes range function closed instead of half-open interval
def Range(a, b, n):
    if (n >= 0):
        return range(a, b+1, n)
    if (n < 0):
        return range(a, b-1, n)


def print_array(array):
    len_array = len(array)
    if (array == []):
        print('[]')
        return

    print('[', end = '')
    for i in Range(0, len_array-2, 1):
        print(array[i], end = '')
        print(', ', end = '')

    print(array[len_array-1], end = '')
    print(']', end = '')
    return


# Removes all whitespace from a string s.
def strip_whitespace(s):
	return "".join(s.split())
