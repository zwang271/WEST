#include "utils.h"
#include "grammar.h"
#include "reg.h"

/*
* Input: Two vectors of computation strings V1 and V2, comma separated
* Output: Computes pairwise string_intersect between all
*		  computation strings in V1 and V2
*/
vector<string> set_intersect(vector<string> v1, vector<string> v2, int n) {
	vector<string> v = vector<string>();
	if (v1 == v or v2 == v) { // Currently v is the empty vector
		return v;
	}

	int len_w = max(v1[0].length(), v2[0].length());
	v1 = pad(v1, n, len_w); v2 = pad(v2, n, len_w);

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
* Input: Vectors V1 and V2 of computation strings
*		 N is number of proposition variables
* Output: Vector of disjoint computation strings
*/
vector<string> set_union(vector<string> v1, vector<string> v2, int n) {
	vector<string> v = join(v1, v2);
    
	return right_or(v, n);
}


/*
* Prop_cons  ->  'T' | 'F'
*/
vector<string> reg_prop_cons(string s, int n) {
	vector<string> v = vector<string>();
	if ((s == "T") && ( n != 0)) {
		v.push_back(string(n, 's'));
	}
	else if (s == "F") {
	} //do nothing
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


vector<string> reg_F(vector<string> alpha, int a, int b, int n) {
	vector<string> comp = vector<string>();

	string pre = "";
	string w = "";
	vector<string> temp_alpha = vector<string>();

	for (int i = 0; i < a; ++i) {
		pre += string(n, 's') + ",";
	}

	for (int i = 0; i <= b - a; ++i) {
		w = pre;
		
		for (int j = 0; j < i; ++j) {
			w += string(n, 's') + ",";
		}
		temp_alpha = list_str_concat_prefix(alpha, w);
		comp = join(comp, temp_alpha);
	}
	return comp;
}


vector<string> reg_G(vector<string> alpha, int a, int b, int n)
{
	string pre = "";
	string w = "";
	vector<string> temp_alpha = vector<string>();

	for (int i = 0; i < a; ++i) {
		pre += string(n, 's') + ",";
	}

	vector<string> comp = list_str_concat_prefix(alpha, pre);

	for (int i = 0; i <= b - a; ++i) {
		w = pre;

		for (int j = 0; j < i; ++j) {
			w += string(n, 's') + ",";
		}
		temp_alpha = list_str_concat_prefix(alpha, w);
		comp = set_intersect(comp, temp_alpha, n);
	}
	return comp;
}


vector<string> reg_U(vector<string> alpha, vector<string> beta, int a, int b, int n) {
	string pre = "";
	for (int i = 0; i < a; ++i) {
		pre += string(n, 's') + ",";
	}

	vector<string> comp = list_str_concat_prefix(beta, pre);
	
	for (int i = a; i <= b - 1; ++i) {
		comp = join(comp, set_intersect(
		reg_G(alpha, a, i, n), reg_G(beta, i+1, i+1, n), n));
	}
	return comp;
}


vector<string> reg_R(vector<string> alpha, vector<string> beta, int a, int b, int n) {
	string pre = "";
	for (int i = 0; i < a; ++i) {
		pre += string(n, 's') + ",";
	}

	vector<string> comp = reg_G(beta, a, b, n);
	
	for (int i = a; i <= b; ++i) {
		comp = join(comp,
			set_intersect(reg_G(beta, a, i, n), reg_G(alpha, i, i, n), n));
	}
	return comp;
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
	int len_s = int(s.length());

	// ?(~) Prop_var 
	if (Prop_var_check(s) or
		(Slice_char(s, 0) == "~" and Prop_var_check(Slice(s, 1, len_s - 1)))) {
		return reg_prop_var(s, n);
	}

	// Prop_cons
	if (Prop_cons_check(s)) {
		return reg_prop_cons(s, n);
	}

	// Unary_Temp_conn  Interval  Nnf
	if (Unary_Temp_conn_check(Slice_char(s, 0))) {
		int begin_interval = 1;
		int end_interval = 2;
		int comma_index = 0;

		// Parse for end of interval
		while (Slice_char(s, end_interval) != "]" and end_interval <= len_s - 1) {
			if (Slice_char(s, end_interval) == ",") {
				comma_index = end_interval;
			}
			end_interval = end_interval + 1;
		}

		string interval = Slice(s, begin_interval, end_interval);
		int a = stoi(Slice(s, begin_interval + 1, comma_index -1));
		int b = stoi(Slice(s, comma_index + 1, end_interval - 1));
		string alpha = Slice(s, end_interval + 1, len_s - 1);
		string unary_temp_con = Slice_char(s, 0);
		
		// Handing the 'finally' temporal operator
		vector<string> reg_alpha = reg(alpha, n); // recursive call
		if (unary_temp_con == "F") {
			return reg_F(reg_alpha, a, b, n);
		}
		else if (unary_temp_con == "G") {
			return reg_G(reg_alpha, a, b, n);
		}
	}

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

	// ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
	if (Slice_char(s, 0) == "(" and Slice_char(s, len_s - 1) == ")") {

		// Number of '(' in s
		int left_count = 0;
		// Number of ')' in s
		int right_count = 0;


		//    Parse for binary_conn_index in s

		//    When left_count == right_count and s[binary_conn_index] is a binary connective,
		//    we are done parsing and have found binary_conn_index.

		int binary_conn_index = 1;
		for (binary_conn_index = 1; binary_conn_index <= len_s - 1; ++binary_conn_index) {
			string c = Slice_char(s, binary_conn_index);

			if (c == "(") {
				++left_count;
			}

			if (c == ")") {
				++right_count;
			}

			// Done parsing for binary_conn_index.
			if (left_count == right_count and (Binary_Prop_conn_check(c) or Binary_Temp_conn_check(c))) {
				break;
			}
		}

		string binary_conn = Slice_char(s, binary_conn_index);

		// ‘(‘ Wff Binary_Prop_conn Wff ‘)’
		if (Binary_Prop_conn_check(binary_conn)) {
			string alpha = Slice(s, 1, binary_conn_index - 1);
			string beta = Slice(s, binary_conn_index + 1, len_s - 2);
			
			if (binary_conn == "&") {
				return set_intersect(reg(alpha, n), reg(beta, n), n);
			}
			else if (binary_conn == "v") {
				return set_union(reg(alpha, n), reg(beta, n), n);
			}
		}

		// ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
		if (Binary_Temp_conn_check(binary_conn)) {
			int begin_interval = binary_conn_index + 1;
			int end_interval = binary_conn_index + 2;
			int comma_index = 0;

			// Parse for end of interval
			while (Slice_char(s, end_interval) != "]" and end_interval <= len_s - 1) {
				if (Slice_char(s, end_interval) == ",") {
					comma_index = end_interval;
				}
				end_interval = end_interval + 1;
			}

			string alpha = Slice(s, 1, binary_conn_index - 1);
			string interval = Slice(s, begin_interval, end_interval);
			string beta = Slice(s, end_interval + 1, len_s - 2);
			int a = stoi(Slice(s, begin_interval + 1, comma_index - 1));
			int b = stoi(Slice(s, comma_index + 1, end_interval - 1));

			if (binary_conn == "U") {
				return reg_U(reg(alpha, n), reg(beta, n), a, b, n);
			}
			else if (binary_conn == "R") {
				return reg_R(reg(alpha, n), reg(beta, n), a, b, n);
			}
		}
	}

	return vector<string>();
}
