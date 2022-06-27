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
* Input: Two vectors of computation strings V1 and V2, comma separated
* Output: Computes pairwise string_intersect between all
*		  computation strings in V1 and V2
*/
vector<string> set_intersect(vector<string> v1, vector<string> v2, int n) {
	vector<string> v = vector<string>();

	// Bit-wise 'and' all entries of v1 and v2
	for (int i = 0; i < v1.size(); ++i) {
		for (int j = 0; j < v2.size(); ++j) {
			string s = string_intersect(v1[i], v2[j], n);
			if (s != "") {
				v.push_back(s);
			}
		}
	}

	return v;
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
				if (v[i][j] != 's') {
					indices.push_back(len_w - j);
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
* Prop_cons  ->  'T' | 'F' 
*/
vector<string> reg_prop_cons(string s, int n)
{
	vector<string> v = vector<string>();
	if (s == "T") {
		v.push_back(string(n, 's'));
	} 
	else if (s == "F") {
	}
	return v;
}

/*
* Prop_var -> ‘p’ Num | '~' 'p' Num
*/
vector<string> reg_prop_var(string s, int n)
{
	int k = 0;
	vector<string> v = vector<string>();

	if (s[0] == 'p') {
		k = stoi(s.substr(1, s.length() - 1)) + 1;
		string temp = string(k - 1, 's') + "1" + string(n - k, 's');
		v.push_back(temp);
	}
	else if (s[0] == '~') {
		k = stoi(s.substr(2, s.length() - 2)) + 1;
		v.push_back(string(k - 1, 's') + '0' + string(n - k, 's'));
	}
	return v;
}


/*
* Input: Vectors V1 and V2 of computation strings
*		 N is number of proposition variables
* Output: Vector of disjoint computation strings
*/
vector<string> set_union(vector<string> v1, vector<string> v2, int n){
	vector<string> v = join(v1, v2);
	return right_or(v, 0, right_or_aux(v, n), n);
}

/*DOCUMENT THIS*/
void print(vector<string> v) {
	for (int i = 0; i < v.size(); ++i) {
		cout << v[i] << endl;
	}
}


/*
* Nnf ->  ?(~) Prop_var | Prop_cons
*	                    | Unary_Temp_conn  Interval  Nnf
* 
*	                    | '(' Assoc_Prop_conn ‘[‘  Array_entry_Nnf  ‘]’ ')'
*                       | ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’
*                       | ‘(‘ Nnf Binary_Temp_conn  Interval Nnf ‘)
*/
vector<string> reg(string s, int n) {

	int len_s = s.length();

	// ?(~) Prop_var 
	if (Prop_var_check(s) or 
		(Slice_char(s, 0) == "~" and Prop_var_check(Slice(s, 1, len_s - 1)))) {
		return reg_prop_var(s, n);
	}
		
	// Prop_cons
	if (Prop_cons_check(s)) {
		return reg_prop_cons(s, n);
	}

	//// Unary_Temp_conn  Interval  Nnf
	//if (Unary_Temp_conn_check(Slice_char(s, 0))) {
	//	int begin_interval = 1;
	//	int end_interval = 2;

	//	// Parse for end of interval
	//	while (Slice_char(s, end_interval) != "]" and end_interval <= len_s - 1) {
	//		end_interval = end_interval + 1;
	//	}

	//	string interval = Slice(s, begin_interval, end_interval);
	//	string alpha = Slice(s, end_interval + 1, len_s - 1);
	//	return Interval_check(interval) and Nnf_check(alpha);
	//}

	//// '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
	//if (Assoc_Prop_conn_check(Slice_char(s, 1))) {
	//	int begin_array = 2;
	//	int end_array = len_s - 2;

	//	string array_entry = Slice(s, begin_array + 1, end_array - 1);
	//	return Slice_char(s, 0) == "("
	//		and Slice_char(s, 2) == "["
	//		and Array_entry_check(array_entry)
	//		and Slice_char(s, len_s - 2) == "]"
	//		and Slice_char(s, len_s - 1) == ")";
	//}

	//// ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
	//if (Slice_char(s, 0) == "(" and Slice_char(s, len_s - 1) == ")") {

	//	// Number of '(' in s
	//	int left_count = 0;
	//	// Number of ')' in s
	//	int right_count = 0;


	//	//    Parse for binary_conn_index in s

	//	//    When left_count == right_count and s[binary_conn_index] is a binary connective,
	//	//    we are done parsing and have found binary_conn_index.

	//	int binary_conn_index = 1;
	//	for (binary_conn_index = 1; binary_conn_index <= len_s - 1; ++binary_conn_index) {
	//		string c = Slice_char(s, binary_conn_index);

	//		if (c == "(") {
	//			++left_count;
	//		}

	//		if (c == ")") {
	//			++right_count;
	//		}

	//		// Done parsing for binary_conn_index.
	//		if (left_count == right_count and (Binary_Prop_conn_check(c) or Binary_Temp_conn_check(c))) {
	//			break;
	//		}
	//	}

	//	string binary_conn = Slice_char(s, binary_conn_index);

	//	// ‘(‘ Wff Binary_Prop_conn Wff ‘)’
	//	if (Binary_Prop_conn_check(binary_conn)) {
	//		string alpha = Slice(s, 1, binary_conn_index - 1);
	//		string beta = Slice(s, binary_conn_index + 1, len_s - 2);
	//		return Wff_check(alpha) and Wff_check(beta);
	//	}

	//	// ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
	//	if (Binary_Temp_conn_check(binary_conn)) {
	//		int begin_interval = binary_conn_index + 1;
	//		int end_interval = binary_conn_index + 2;

	//		// Parse for end of interval
	//		while (Slice_char(s, end_interval) != "]" and end_interval <= len_s - 1) {
	//			end_interval = end_interval + 1;
	//		}

	//		string alpha = Slice(s, 1, binary_conn_index - 1);
	//		string interval = Slice(s, begin_interval, end_interval);
	//		string beta = Slice(s, end_interval + 1, len_s - 2);
	//		return Wff_check(alpha) and Interval_check(interval) and Wff_check(beta);
	//	}
	//}

	return vector<string>();
}
