#include <string>
#include "utils.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include "grammar.h"



using namespace std;


/*
* Input: computation string UNPADDED_S separated by commas
*		 LENGTH is target length to pad to
*		 N is number of propositional variables
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
* Output: Pads all comutation strings to the same length as the longest string
*/
vector<string> pad(vector<string> unpadded_v, int n) {
	int unpadded_size = unpadded_v.size();

	// Compute max-length of strings
	int maxLength = 0;
	for (int i = 0; i < unpadded_size; ++i) {
		if (unpadded_v[i].length() > maxLength) {
			maxLength = int(unpadded_v[i].length());
		}
	}

	// Pad each string to maxLength
	vector<string> padded_v;
	for (int j = 0; j < unpadded_size; ++j) {
		padded_v.push_back(pad_to_length(unpadded_v[j], maxLength, n));
	}

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
*		 N is number of propositional variables
* Output: Vector of computation strings with commas
*/
vector<string> add_commas(vector<string> v, int n) {
	for (int i = 0; i < v.size(); i++) {
		int len_w = v[i].length();
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
*		 N is number of propositional variables
* Output: Bit-wise and of the two string of length max(len(w_1), len(w_2))
*/
string string_intersect(string w_1, string w_2, int n) {
	// Remove white-characters from w_1 and w_2
	w_1.erase(remove_if(w_1.begin(),
		w_1.end(), ::isspace), w_1.end());
	w_2.erase(remove_if(w_2.begin(),
		w_2.end(), ::isspace), w_2.end());

	// If either w_1 or w_2 are empty, return empty.
	if (w_1 == "" || w_2 == "") {
		return "";
	}

	// Make w_1 and w_2 the same length.
	vector<string> vec;
	vec.push_back(w_1);
	vec.push_back(w_2);
	pad(vec, n);

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
* Input: Vector of computation strings V, with commas
*	     N is number of propositional variables
* Info: Any computation string w is of the form w = (s,)^k(0,|1,|s,)^m where k is maximal
* Output: Array of indices that computes m for each string in V
*/
vector<int> right_or_aux(vector<string> v, int n) {
	v = pad(v, n);
	v = strip_commas(v);
	int len_w = int(v[0].size());
	vector<int> indices;
	for (int i = 0; i < v.size(); ++i) {
		if (v[i][0] == 's') {
			for (int j = 1; j < len_w; ++j) {
				if (v[i][j] != 's' or (j == len_w-1 and v[i][j] == 's')) {
					indices.push_back(len_w - j -1);
					break;
				}
			}
		}
	}
	return indices;
}


/*
* Input: vector V of length 1 computation strings (bits)
* Output: singleton vector computing or of V
*/
vector<string> single_char_or(vector<string> V) {
	vector<string> ret;
	if (V.size() == 0) {
		return V;
	}
	else if (V.size() == 1 || (V[0] == "s")) {
		ret.push_back(V[0]);
		return ret;
	}
	else {
		for (int i = 1; i < V.size(); ++i) {
			if (V[i] != V[0]) {
				ret.push_back("s");
				return ret;
			}
		}
		ret.push_back(V[0]);
		return ret;
	}
}


/*
* Input: Vectors A and B of computation strings
* Output: Vector of A concatenated with B
*/
vector<string> join(vector<string> A, vector<string> B) {
	vector<string> AB;
	AB.reserve(A.size() + B.size()); // preallocate memory
	AB.insert(AB.end(), A.begin(), A.end());
	AB.insert(AB.end(), B.begin(), B.end());
	return AB;
}


/*
* Input: Vector of computation strings V
*		 String S
* Output: Appends S to each string in V
*/
vector<string> list_str_concat(vector<string> V, string s) {
	for (int i = 0; i < V.size(); ++i) {
		V[i] += s;
	}
	return V;
}


/*
* Input: Vector of computation strings V, with commas
*		 ITERATION describes depth of recursion (MUST CALL WITH 0 INITIALLY)
*		 INIDCES is vector of ints from right_or_aux
*		 N is number of propositional variables
* Output: Vector of disjoint computation strings
*/
vector<string> right_or(vector<string> v, int iteration, vector<int> indices, int n) {
	//strip commas before, or write invariant_check

	if (iteration == 0) {
		v = pad(v, n);
		v = strip_commas(v);
	}

	int len_w = int(v[0].size());

	// Base case
	if (len_w == 1) {
		return single_char_or(v);
	}

	for (int i = 0; i < indices.size(); ++i) {
		if (indices[i] == iteration) {
			string s_w = string(len_w, 's'); // string = 's' repeated len_w times
			if (find(v.begin(), v.end(), s_w) != v.end()) {
				vector<string> ret = { s_w };
				ret = add_commas(ret, n);
				return ret;
			}
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
		return end_zero; // returns an empty vector
	}
	else {
		++iteration;
		v = join(list_str_concat(right_or(end_zero, iteration, indices, n), "0"),
			list_str_concat(right_or(end_one, iteration, indices, n), "1")
		);
		v = add_commas(v, n);
	}

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

