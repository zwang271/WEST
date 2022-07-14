#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <set>

using namespace std;


/*
 * Input: computation string UNPADDED_S separated by commas
 *		 LENGTH is target length to pad to
 *		 n is number of propositional variables
 * Output: computation string separated by commas of length LENGTH
 */
string pad_to_length(string unpadded_s, int length, int n);


/*
 * Input: Vector of computation strings with commas
 *		 n is number of propositional variables
 *		 m is -1 at first, pass in a positive value for m to pad all strings to length m instead
 * Output: Pads all computation strings to the same length as the longest string
 */
vector<string> pad(vector<string> unpadded_v, int n, int m = -1);


/*
 * Input: string S
 *		 char C
 * Output: S with every instance of C removed
 */
string strip_char(string s, char c);


/*
 * Input: Vector of computation strings with commas
 * Output: Vector of computation strings without commas
 */
vector<string> strip_commas(vector<string> comma_v);


/*
 * Input: Vector of computation strings without commas
 *		 n is number of propositional variables
 * Output: Vector of computation strings with commas
 */
vector<string> add_commas(vector<string> v, int n);


/*
 * Input: Two computation strings W_1 and W_2, comma separated
 *		 n is number of propositional variables
 * Output: Bit-wise AND of the two strings of length max(len(w_1), len(w_2))
 */
string string_intersect(string w_1, string w_2, int n);


/*
 * Input: vector V of length 1 computation strings (bits)
 * Output: singleton vector computing OR of V
 */
vector<string> single_char_or(vector<string> V);


/*
 * Input: Vector of computation strings V
 *		 String S
 * Output: Appends S to each string in V
 */
vector<string> list_str_concat_suffix(vector<string> V, string s);


/*
 * Input: Vector of computation strings V
 *		 String S
 * Output: Prepends S to each string in V
 */
vector<string> list_str_concat_prefix(vector<string> V, string s);


/*
 * Input: Vector of computation strings V, with commas
 *		 ITERATION describes depth of recursion (MUST CALL WITH 0 INITIALLY)
 *		 n is number of propositional variables
 * Output: Vector of disjoint computation strings
 * Info: Called left_expand() because the function begins the recursion at the leftmost character of the strings
 */
vector<string> left_expand(vector<string> v, int n, int iteration = 0);


/*
 * Input: Vector of computation strings V, with commas
 *		 ITERATION describes depth of recursion (MUST CALL WITH 0 INITIALLY)
 *		 n is number of propositional variables
 * Output: Vector of disjoint computation strings
 * Info: Called right_expand() because the function begins the recursion at the rightmost character of the strings
 */
vector<string> right_expand(vector<string> v, int n, int iteration = 0);


/*
 * Prints each element of a vector of strings on a new line
 */
void print(vector<string> v);


/*
 * Input: computation strings s1 and s2
 *	If the following is possible:
 *	s1 = w1 + 'c1' + v1
 *	s2 = w1 + 'c2' + v1
 *	Here, c1 and c2 are the first differing character from the left
 *	return w1 + single_char_or(c1, c2) + v1
 * Otherwise output: "FAIL"
 */
string simplify_string(string s1, string s2);


/*
 * Removes element in INDEX from vector v
 */
template <typename T>
void remove(vector<T>& v, size_t index);


/*
* Removes duplicate entries from a vector.
* Mutates vector.
*/
template <typename T>
void remove_duplicates(vector<T>* reg_alpha);


/*
 * Input: Vector of disjoint computation strings v
 * Output: Simplifies strings in v pairwise as much as possible
 */
vector<string> simplify(vector<string> v, int n);


/*
 * Prints vector of computations strings as a tree.
 * String pre_space is printed in front of the computation strings.
 */
void print_tree(vector<string> v, string pre_space = "");


/*
 * Returns the longest common left substring of all strings in v.
 */
string common_left_string(vector<string> v);


/*
 * Prints all computation strings in a vector v_actual.
 * n is number of propositional variables
 */
void print_all_representations(vector<string> v_actual, int n);


/*
* Input: Vector of strings V
* Output: sum of length of all strings
*/
int sum_of_characters(vector<string>);

void print_subformulas(vector< tuple<string, vector<string>> > formulas, int n);
