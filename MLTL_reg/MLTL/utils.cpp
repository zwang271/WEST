#include <string>
#include "utils.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include "grammar.h"
#include "reg.h"


using namespace std;


/*
 * Input: computation string UNPADDED_S separated by commas
 *		 LENGTH is target length to pad to
 *		 n is number of propositional variables
 * Output: computation string separated by commas of length LENGTH
 */
string pad_to_length(string unpadded_s, int length, int n) {
	// Compute remaining space to pad.
	int diff = int((length - unpadded_s.length()) / (n + 1));
	// Pad remaining space with ",ss...s"
	for (int i = 0; i < diff; i++) {
		unpadded_s += ',' + string(n, 's');
	}
	return unpadded_s;
}


/*
 * Input: Vector of computation strings with commas
 *		 n is number of propositional variables
 *		 m is -1 at first, pass in a positive value for m to pad all strings to length m instead
 * Output: Pads all computation strings to the same length as the longest string
 */
vector<string> pad(vector<string> unpadded_v, int n, int m) {
	int unpadded_size = int(unpadded_v.size());

	// Compute max-length of strings
	int maxLength = 0;
	for (int i = 0; i < unpadded_size; ++i) {
		if (unpadded_v[i].length() > maxLength) {
			maxLength = int(unpadded_v[i].length());
		}
	}
	maxLength = max(maxLength, m);

	// Pad each string to maxLength
	vector<string> padded_v;
	for (int j = 0; j < unpadded_size; ++j) {
		padded_v.push_back(pad_to_length(unpadded_v[j], maxLength, n));
	}

	unpadded_v.clear();
	unpadded_v.shrink_to_fit();
	return padded_v;
}


/*
 * Input: string S
 *		 char C
 * Output: S with every instance of C removed
 */
string strip_char(string s, char c)
{
	string w = "";
	for (int j = 0; j < s.length(); ++j) {
		if (s[j] != c) {
			w += s[j];
		}
	}

	return w;
}


/*
 * Input: Vector of computation strings with commas
 * Output: Vector of computation strings without commas
 */
vector<string> strip_commas(vector<string> comma_v) {
	for (int i = 0; i < comma_v.size(); ++i) {
		comma_v[i] = strip_char(comma_v[i], ',');;
	}
	return comma_v;
}


/*
 * Input: Vector of computation strings without commas
 *		 n is number of propositional variables
 * Output: Vector of computation strings with commas
 */
vector<string> add_commas(vector<string> v, int n) {
	for (int i = 0; i < v.size(); i++) {
		int len_w = int(v[i].length());
		string w = "";
		for (int j = 0; j < len_w; j += n) {
			w += v[i].substr(j, n);
			if (j + n < len_w) {
				w += ',';
			}
		}
		v[i] = w;
	}
	return v;
}


/*
 * Input: Two computation strings W_1 and W_2, comma separated
 *		 n is number of propositional variables
 * Output: Bit-wise AND of the two strings of length max(len(w_1), len(w_2))
 */
string string_intersect(string w_1, string w_2, int n) {
	// Remove white-characters from w_1 and w_2
	w_1 = strip_char(w_1, ' ');
	w_2 = strip_char(w_2, ' ');

	//Commented-out legacy code

	// w_1.erase(remove_if(w_1.begin(),
	// 	w_1.end(), ::isspace), w_1.end());
	// w_2.erase(remove_if(w_2.begin(),
	// 	w_2.end(), ::isspace), w_2.end());


	// If either w_1 or w_2 are empty, return empty.
	if (w_1 == "" || w_2 == "") {
		return "";
	}

	// Bit-wise 'and' w_1 and w_2
	string w = "";
	for (int i = 0; i < w_1.length(); i++) {
		if (w_1[i] != 's' and w_2[i] != 's') {
			if (w_1[i] != w_2[i]) {
				return "";
			}
			else {
				w += w_1[i];
			}
		}
		else if (w_1[i] == 's') {
			w += w_2[i];
		}
		else {
			w += w_1[i];
		}

	}

	return w;
}


/*
 * Input: vector V of length 1 computation strings (bits)
 * Output: singleton vector computing OR of V
 */
vector<string> single_char_or(vector<string> V) {
	vector<string> ret;
	if (V.size() == 0) {
		return V;
	}
	else if (V.size() == 1 || (V[0] == "s")) {
		ret.push_back(V[0]);
		V.clear();
		V.shrink_to_fit();
		return ret;
	}
	else {
		for (int i = 1; i < V.size(); ++i) {
			if (V[i] != V[0]) {
				ret.push_back("s");
				V.clear();
				V.shrink_to_fit();
				return ret;
			}
		}
		ret.push_back(V[0]);
		V.clear();
		V.shrink_to_fit();
		return ret;
	}
}


/*
 * Input: Vector of computation strings V
 *		 String S
 * Output: Appends S to each string in V
 */
vector<string> list_str_concat_suffix(vector<string> V, string s) {
	for (int i = 0; i < V.size(); ++i) {
		V[i] += s;
	}
	return V;
}


/*
 * Input: Vector of computation strings V
 *		 String S
 * Output: Prepends S to each string in V
 */
vector<string> list_str_concat_prefix(vector<string> V, string s) {
	for (int i = 0; i < V.size(); ++i) {
		V[i] = s + V[i];
	}
	return V;
}


/*
 * Input: Vector of computation strings V, with commas
 *		 ITERATION describes depth of recursion (MUST CALL WITH 0 INITIALLY)
 *		 n is number of propositional variables
 * Output: Vector of disjoint computation strings
 * Info: Called left_expand() because the function begins the recursion at the leftmost character of the strings
 */
vector<string> left_expand(vector<string> v, int n, int iteration) {
	// Single union and empty union are disjoint
	if (v.size() == 0 or v.size() == 1) {
		return v;
	}

	//strip commas before, or write invariant_check
	if (iteration == 0) {
		// Pad all strings to length of longest string
		v = pad(v, n);
		v = strip_commas(v);
	}

	// Length of all strings
	int len_w = int(v[0].size());

	// Base case of single char strings
	if (len_w == 1) {
		return single_char_or(v);
	}


	// Searching for s^len_w in input
	string s_lenw = string(len_w, 's');
	for (int i = 0; i < v.size(); ++i) {
		if (v[i] == s_lenw) {
			vector<string> ret = { s_lenw };
			v.clear();
			v.shrink_to_fit();
			return ret;
		}
	}

	vector<string> begin_zero;
	vector<string> begin_one;

	// Cut-off left-most char in all strings, and assign either to
	// begin_zero or begin_one based on char 
	for (int i = 0; i < v.size(); ++i) {
		// w = c + rest
		string w = v[i];
		string c = Slice_char(w, 0);
		string rest = Slice(w, 1, len_w - 1);
		if (c == "0") {
			// Slice(w, 1, len_w-1)
			begin_zero.push_back(rest);
		}
		else if (c == "1") {
			begin_one.push_back(rest);
		}
		else {
			begin_zero.push_back(rest);
			begin_one.push_back(rest);
		}
	}

	if (begin_zero.size() == 0 and begin_one.size() == 0) {
		begin_zero.shrink_to_fit();
		begin_one.shrink_to_fit();
		return {}; // returns an empty vector
	}
	else {
		++iteration;
		v = join(list_str_concat_prefix(left_expand(begin_zero, n, iteration), "0"),
			list_str_concat_prefix(left_expand(begin_one, n, iteration), "1"), n, false);
	}

	// Final return will be in iteration 1, add commas before returning
	if (iteration == 1) {
		v = add_commas(v, n);
	}

	begin_zero.clear();
	begin_zero.shrink_to_fit();
	begin_one.clear();
	begin_one.shrink_to_fit();
	return v;
}


/*
 * Input: Vector of computation strings V, with commas
 *		 ITERATION describes depth of recursion (MUST CALL WITH 0 INITIALLY)
 *		 n is number of propositional variables
 * Output: Vector of disjoint computation strings
 * Info: Called right_expand() because the function begins the recursion at the rightmost character of the strings
 */
vector<string> right_expand(vector<string> v, int n, int iteration) {
	//strip commas before, or write invariant_check
	if (v.size() == 0 or v.size() == 1) {
		return v;
	}

	if (iteration == 0) {
		v = pad(v, n);
		v = strip_commas(v);
	}

	int len_w = int(v[0].size());

	// Base case
	if (len_w == 1) {
		return single_char_or(v);
	}


	// Searching for s^len_w in input
	string s_lenw = string(len_w, 's');
	for (int i = 0; i < v.size(); ++i) {
		if (v[i] == s_lenw) {
			vector<string> ret = { s_lenw };
			v.clear();
			v.shrink_to_fit();
			return ret;
		}
	}

	vector<string> end_zero;
	vector<string> end_one;

	for (int i = 0; i < v.size(); ++i) {
		string w = v[i];
		if (w[len_w - 1] == '0') {
			end_zero.push_back(w.substr(0, len_w - 1));
		}
		else if (w[len_w - 1] == '1') {
			end_one.push_back(w.substr(0, len_w - 1));
		}
		else {
			end_zero.push_back(w.substr(0, len_w - 1));
			end_one.push_back(w.substr(0, len_w - 1));
		}
	}

	if (end_zero.size() == 0 and end_one.size() == 0) {
		end_zero.shrink_to_fit();
		end_one.shrink_to_fit();
		return end_zero; // returns an empty vector
	}
	else {
		++iteration;
		v = join(list_str_concat_suffix(right_expand(end_zero, n, iteration), "0"),
			list_str_concat_suffix(right_expand(end_one, n, iteration), "1"), n, false);
	}

	// Final return will be in iteration 1, add commas before returning
	if (iteration == 1) {
		v = add_commas(v, n);
	}

	end_zero.clear();
	end_zero.shrink_to_fit();
	end_one.clear();
	end_one.shrink_to_fit();
	return v;
}


/*
 * Prints each element of a vector of strings on a new line
 */
void print(vector<string> v) {
	for (int i = 0; i < v.size(); ++i) {
		cout << v[i] << endl;
	}
}


/*
 * Input: computation strings s1 and s2
 *	If the following is possible:
 *	s1 = w1 + 'c1' + v1
 *	s2 = w1 + 'c2' + v1
 *	Here, c1 and c2 are the first differing character from the left
 *	return w1 + single_char_or(c1, c2) + v1
 * Otherwise output: "FAIL"
 */
string simplify_string(string s1, string s2)
{
	if (s1.length() != s2.length()) {
		cout << "simplify_string called on strings of unequal length" << endl;
		exit(-1);
	}

	int len_s = int(s1.length());
	for (int i = 0; i < len_s; ++i) {
		// s1 = w1 + 'c1' + v1
		string w1 = Slice(s1, 0, i - 1);
		string c1 = Slice_char(s1, i);
		string v1 = Slice(s1, i + 1, len_s - 1);

		// s2 = w2 + 'c2' + v2
		string w2 = Slice(s2, 0, i - 1);
		string c2 = Slice_char(s2, i);
		string v2 = Slice(s2, i + 1, len_s - 1);

		if (w1 == w2 and v1 == v2) {
			// c1|c2 of the form s|0 or s|1 or 0|1
			if (c1 != c2) {
				return w1 + "s" + v1;
			}

			// Otherwise c1 == c2 and return s1 == s2
			else {
				return s1;
			}
		}
	}

	return "FAIL";
}


/*
 * Removes element in INDEX from vector v
 */
template <typename T>
void remove(vector<T>& v, size_t index) {
	v.erase(v.begin() + index);
}


/*
* Removes duplicate entries from a vector.
* Mutates vector.
*/
template <typename T>
void remove_duplicates(vector<T>* reg_alpha){
  // Convert vector to a set
  set<T> s( (*reg_alpha).begin(), (*reg_alpha).end() );
  // Assign set back to vector
  (*reg_alpha).assign( s.begin(), s.end() );
  
  return ;
}


/*
 * Input: Vector of disjoint computation strings v
 * Output: Simplifies strings in v pairwise as much as possible
 */
vector<string> simplify(vector<string> v, int n) {
	v = pad(v, n);
	if (v.size() <= 1) {
		return v;
	}

	// CAN OPTIMIZE BY STRIPPING COMMAS
	int i = int(v.size() - 1);
	int j = i - 1;


	// v = v0, v1, v2, ..., v_n-1, v_n
	// i starts at v_n, j starts at v_n-1
	// if v[i] and v[j] can simplify, then v[i] is destroyed and 
	//		v[j] = simplify_string(v[i], v[j])
	// iteratively decrement j until out of bounds from left (j < 0), then decrement i 
	// and assign j = i - 1

START:
	while (i >= 1) {
		while (j >= 0) {
			string simplified = simplify_string(v[i], v[j]);
			if (simplified != "FAIL") {
				v[j] = simplified;
				remove(v, i);
				i = int(v.size() - 1);
				j = i - 1;
				goto START;
			}
			--j;
		}
		--i;
		j = i - 1;
	}

	return v;
}


/*
 * Prints vector of computations strings as a tree.
 * String pre_space is printed in front of the computation strings.
 */
void print_tree(vector<string> v, string pre_space) {
	// Base case: print out v with pre-concatenated pre_space
	if (v.size() == 1) {
		v = list_str_concat_prefix(v, pre_space);
		print(v);
		return;
	}
	// Exit from function if v is empty
	else if (v.size() == 0) { return; }

	// Remove all "" from v
	for (int i = 0; i < v.size(); ++i) {
		if (v[i] == "") {
			remove(v, i);
		}
	}

	v = strip_commas(v);

	// Cases for how the next character after left_common of strings in v
	vector<string> differ_zero = {};
	vector<string> differ_one = {};
	vector<string> differ_s = {};

	// Find the longest common substring from the left
	string left_common = common_left_string(v);

	for (string w : v) {
		// w = left_common + rest
		// next_char is first letter of rest, could possibly be ""
		string next_char = Slice_char(w, left_common.length());
		string rest = Slice(w, left_common.length(), w.length() - 1);

		if (next_char == "0") {
			differ_zero.push_back(rest);
		}
		else if (next_char == "1") {
			differ_one.push_back(rest);
		}
		else if (next_char == "s") {
			differ_s.push_back(rest);
		}
		// next_char can't be ","
		// no if blocks matched means next_char = ""
	}

	cout << pre_space + left_common << endl;

	string pre = string(left_common.length(), ' ') + pre_space;
	print_tree(differ_zero, pre);
	print_tree(differ_one, pre);
	print_tree(differ_s, pre);
}


/*
 * Returns the longest common left substring of all strings in v.
 */
string common_left_string(vector<string> v) {
	// Return entire string if v has only 1 string
	if (v.size() == 1) {
		return v[0];
	}
	// Return empty string if v is empty
	else if (v.size() == 0) { return ""; }

	int len_w = v[0].length();
	string left_common = "";
	bool found_longest = false;
	for (int i = 0; i < len_w; i++) {
		// Deepen search for common left string
		left_common += Slice_char(v[0], i);

		// Check if left_common is still a common left substring 
		for (string w : v) {
			// Check if left_common no longer matches w
			if (Slice(w, 0, left_common.length() - 1) != left_common) {
				found_longest = true;
				break;
			}
		}

		if (found_longest) {
			// Remove the last character to get obtain long left common substring
			left_common = Slice(left_common, 0, left_common.length() - 2);
			break;
		}
	}
	v.clear();
	v.shrink_to_fit();
	return left_common;
}

/*
 * Prints all computation strings in a vector v_actual.
 * n is number of propositional variables
 */
void print_all_representations(vector<string> v_actual, int n) {

	cout << endl << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;
	cout << "reg" << endl;
	print(v_actual);

	cout << endl << "simplify" << endl;
	print(simplify(v_actual, n));

	cout << endl << "right_expand" << endl;
	print(right_expand(v_actual, n));

	cout << endl << "simplify right_expand" << endl;
	print(simplify(right_expand(v_actual, n), n));

	cout << endl << "left_expand" << endl;
	print(left_expand(v_actual, n));

	cout << endl << "simplify left_expand" << endl;
	print(simplify(left_expand(v_actual, n), n));

	cout << endl << "tree" << endl;
	print_tree(simplify(left_expand(v_actual, n), n));

	cout << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl << endl;
}


/*
* Input: Vector of strings V
* Output: sum of length of all strings 
*/
int sum_of_characters(vector<string> v) {
	int sum = 0;
	for (int i = 0; i < v.size(); ++i) {
		sum += v[i].length();
	}
	return sum;
}

void print_subformulas(vector< tuple<string, vector<string>> > formulas, int n) {
    for (int i = 0; i < formulas.size(); ++i) {
        cout << "Subformula: " << get<0>(formulas[i]) << endl;
        //print(simplify(right_expand(get<1>(formulas[i]), n), n));
        print(get<1>(formulas[i]));
        cout << endl;
        }
}

