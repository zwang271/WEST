#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
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

	int len_w = int(max(v1[0].length(), v2[0].length()));
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
* Prop_cons  ->  'T' | '!'
*/
vector<string> reg_prop_cons(string s, int n) {
	vector<string> v = vector<string>();
	if ((s == "T") && ( n != 0)) {
		v.push_back(string(n, 's'));
	}
	else if (s == "!") {
	} //do nothing
	return v;
}


/*
* Prop_var -> �p� Num | '~' 'p' Num
*/
vector<string> reg_prop_var(string s, int n)
{
	int k = 0;
	vector<string> v = vector<string>();

	if (s[0] == 'p') {
		k = stoi(s.substr(1, s.length() - 1));
		string temp = string(k, 's') + "1" + string(n - k - 1, 's');
		v.push_back(temp);
	}
	else if (s[0] == '~') {
		k = stoi(s.substr(2, s.length() - 2));
		v.push_back(string(k, 's') + '0' + string(n - k - 1, 's'));
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
*	                    | '(' Assoc_Prop_conn �[�  Nnf_Array_entry  �]� ')'
*                       | �(� Nnf Binary_Prop_conn Nnf �)�
*                       | �(� Nnf Binary_Temp_conn  Interval Nnf �)
*/
vector<string> reg(string nnf, int n) {
	int len_nnf = int(nnf.length());

	// ?(~) Prop_var 
	if (Prop_var_check(nnf) or
		(Slice_char(nnf, 0) == "~" and Prop_var_check(Slice(nnf, 1, len_nnf - 1)))) {
		return reg_prop_var(nnf, n);
	}

	// Prop_cons
	if (Prop_cons_check(nnf)) {
		return reg_prop_cons(nnf, n);
	}

	// Unary_Temp_conn  Interval  Nnf
	if (Unary_Temp_conn_check(Slice_char(nnf, 0))) {
		string unary_temp_con = Slice_char(nnf, 0);

		int begin_interval = 1;
		int end_interval = 2;
		int comma_index = 0;

		// Parse for end of interval
		while (Slice_char(nnf, end_interval) != "]" and end_interval <= len_nnf - 1) {
			if (Slice_char(nnf, end_interval) == ",") {
				comma_index = end_interval;
			}
			end_interval = end_interval + 1;
		}

		string interval = Slice(nnf, begin_interval, end_interval);
		int a = stoi(Slice(nnf, begin_interval + 1, comma_index -1));
		int b = stoi(Slice(nnf, comma_index + 1, end_interval - 1));
		string alpha = Slice(nnf, end_interval + 1, len_nnf - 1);
		
		
		vector<string> reg_alpha = reg(alpha, n); // recursive call
		// Unary_Temp_conn -> 'F'
		if (unary_temp_con == "F") {
			return reg_F(reg_alpha, a, b, n);
		}

		// Unary_Temp_conn -> 'G'
		if (unary_temp_con == "G") {
			return reg_G(reg_alpha, a, b, n);
		}
	}

	// '(' Assoc_Prop_conn '['  Nnf_Array_entry ']' ')'
	if (Assoc_Prop_conn_check(Slice_char(nnf, 1))) {
		string assoc_prop_conn = Slice_char(nnf, 1);

		// (...((wff_1 assoc_prop_conn wff_2) assoc_prop_conn wff_3) ... assoc_prop_conn wff_n)
		// is equiv to (assoc_prop_conn [wff_1, ..., wff_n])
		int begin_entry = 3;
		string equiv_formula = "";
		for (int end_entry = 3; end_entry <= len_nnf - 1; ++end_entry) {
			if (Wff_check(Slice(nnf, begin_entry, end_entry))) {
				string alpha = Slice(nnf, begin_entry, end_entry);

				// First entry obtained
				if (begin_entry == 3) {
					// Add wff_1 to equiv_formula
					equiv_formula = equiv_formula + alpha;
				}

				// Not first entry
				else {
					// Add wff_n to equiv_formula, where n >= 2
					equiv_formula = "(" + equiv_formula + assoc_prop_conn + alpha + ")";
				}

				// Update begin_entry so it has index of the first char of the next entry.
				begin_entry = end_entry + 2;
			}
		}

		return reg_clean(equiv_formula, n);
	}

	// �(� Nnf Binary_Prop_conn Nnf �)� | �(� Nnf Binary_Temp_conn Interval Nnf �)
	if (Slice_char(nnf, 0) == "(" and Slice_char(nnf, len_nnf - 1) == ")") {

		// Number of '(' in nnf
		int left_count = 0;
		// Number of ')' in nnf
		int right_count = 0;


		//    Parse for binary_conn_index in nnf

		//    When left_count == right_count and nnf[binary_conn_index] is a binary connective,
		//    we are done parsing and have found binary_conn_index.

		int binary_conn_index = 1;
		for (binary_conn_index = 1; binary_conn_index <= len_nnf - 1; ++binary_conn_index) {
			string c = Slice_char(nnf, binary_conn_index);

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

		string binary_conn = Slice_char(nnf, binary_conn_index);

		// �(� Nnf Binary_Prop_conn Nnf �)�
		if (Binary_Prop_conn_check(binary_conn)) {
			string alpha = Slice(nnf, 1, binary_conn_index - 1);
			vector<string> reg_alpha = reg(alpha, n);
			string beta = Slice(nnf, binary_conn_index + 1, len_nnf - 2);
			vector<string> reg_beta = reg(beta, n);
			
			if (binary_conn == "&") {
				return set_intersect(reg_alpha, reg_beta, n);
			}


			if (binary_conn == "v") {
				return set_union(reg_alpha, reg_beta, n);
			}

			if (binary_conn == "="){
				vector<string> neg_reg_alpha = reg("~" + alpha, n);
				vector<string> neg_reg_beta = reg("~" + beta, n);
				return set_union(set_intersect(reg_alpha, reg_beta, n), set_intersect(neg_reg_alpha, neg_reg_beta, n), n);
			}
		}

		// �(� Wff Binary_Temp_conn Interval Wff �)
		if (Binary_Temp_conn_check(binary_conn)) {
			int begin_interval = binary_conn_index + 1;
			int end_interval = binary_conn_index + 2;
			int comma_index = 0;

			// Parse for end of interval
			while (Slice_char(nnf, end_interval) != "]" and end_interval <= len_nnf - 1) {
				if (Slice_char(nnf, end_interval) == ",") {
					comma_index = end_interval;
				}
				end_interval = end_interval + 1;
			}

			string alpha = Slice(nnf, 1, binary_conn_index - 1);
			string interval = Slice(nnf, begin_interval, end_interval);
			string beta = Slice(nnf, end_interval + 1, len_nnf - 2);
			int a = stoi(Slice(nnf, begin_interval + 1, comma_index - 1));
			int b = stoi(Slice(nnf, comma_index + 1, end_interval - 1));

			if (binary_conn == "U") {
				return reg_U(reg(alpha, n), reg(beta, n), a, b, n);
			}
			else if (binary_conn == "R") {
				return reg_R(reg(alpha, n), reg(beta, n), a, b, n);
			}
		}
	}

	else{
        string error_string = nnf + " is not in Negation-normal form.\n";
        throw invalid_argument(error_string);
    }
    return vector<string>();
}





// Given an Nnf-formula nnf, returns the regex
// for the language of nnf.
// For preformance reasons, reg_clean will return a
// NOT NECESSARILY DISJOINT Union.
// To obtain a disjoint union from reg_clean's output,
// use right_or.
//
// Cleaner implementation of reg
// in case original is faulty.
vector<string> reg_clean(string nnf, int n) {
	int len_nnf = int(nnf.length());

	// ?(~) Prop_var 
	if (Prop_var_check(nnf) or
		(Slice_char(nnf, 0) == "~" and Prop_var_check(Slice(nnf, 1, len_nnf-1)))) {

		// Prop_var -> 'p' Num		
		if (Prop_var_check(nnf)) {
			string num = Slice(nnf, 1, len_nnf-1);
			int k = stoi(num);
			string ret_string = string(k, 's') + "1" + string(n-k-1, 's');
			
			//Output
			return {ret_string};
		}

		// '~' Prop_var -> '~' 'p' Num
		if (Slice_char(nnf, 0) == "~" and Prop_var_check(Slice(nnf, 1, len_nnf-1))) {
			string num = Slice(nnf, 2, len_nnf-1);
			int k = stoi(num);
			string ret_string = string(k, 's') + '0' + string(n-k-1, 's');
			return {ret_string};
		}
	}

	// Prop_cons
	if (Prop_cons_check(nnf)) {
		// Prop_cons -> 'T'
		if(nnf == "T"){
			string ret_string = string(n, 's');
			return {ret_string};
		}

		// Prop_cons -> '!'
		if (nnf == "!"){
			return {};
		}
	}

	// Unary_Temp_conn  Interval  Nnf
	if (Unary_Temp_conn_check(Slice_char(nnf, 0))) {
		string unary_temp_conn = Slice_char(nnf, 0);

		tuple<int,int,int> interval_tuple = primary_interval(nnf);
		int begin_interval = get<0>(interval_tuple); 
		int comma_index = get<1>(interval_tuple); 
		int end_interval = get<2>(interval_tuple); 
		int lower_bound = stoi(Slice(nnf, begin_interval+1, comma_index-1));
		int upper_bound = stoi(Slice(nnf, comma_index+1, end_interval-1));
		string alpha = Slice(nnf, end_interval+1, len_nnf-1);
		vector<string> reg_alpha = reg_clean(alpha, n);

		// Empty interval
		if (lower_bound > upper_bound){
			// vacously false
			if(unary_temp_conn == "F"){
				return {};
			}

			// vacously true
			if(unary_temp_conn == "G"){
				string ret_string = string(n, 's');
				return {ret_string};
			}

		}


		vector<string> reg_nnf = {}; 
		string pre = "";

		// [0, lower_bound-1] time steps don't matter.
		for (int i = 0; i <= lower_bound-1; ++i) {
			pre += string(n, 's') + ",";
		}


		// Determine which operation to use based on initial
		// Unary Temp character

		// If Unary_Temp_conn is 'F', operation is join
		if (unary_temp_conn == "F"){
			// Compute regex for 'F' [0, upper_bound-lower_bound] alpha
			// which is: reg_nnf = join [reg(alpha), 's...s' + reg(alpha), ..., ('s...s')^(upper_bound-lower_bound) + reg(alpha)] 
			for (int i = 0; i <= upper_bound-lower_bound; ++i){
				// Vector for ('s...s')^(i) + reg(alpha)
				vector<string> current_step_alpha = list_str_concat_prefix(reg_alpha, string(i, 's') + ",");

				// Join current_step_alpha to reg_nnf
				reg_nnf = join(reg_nnf, current_step_alpha);
			}
		}

		// If Unary_Temp_conn is 'G', operation is set_intersection
		if (unary_temp_conn == "G"){
			// Compute regex for 'G' [0, upper_bound-lower_bound] alpha
			// which is: reg_nnf = set_intersection [reg(alpha), 's...s' + reg(alpha), ..., ('s...s')^(upper_bound-lower_bound) + reg(alpha)] 
			for (int i = 0; i <= upper_bound-lower_bound; ++i){
				// Vector for ('s...s')^(i) + reg(alpha)
				vector<string> current_step_alpha = list_str_concat_prefix(reg_alpha, string(i, 's') + ",");

				// Set_intersect current_step_alpha to reg_nnf
				reg_nnf = set_intersect(reg_nnf, current_step_alpha, n);
			}
		}

		// Regex for unary_temp_conn [lower_bound, upper_bound] alpha is pre + reg_nnf
		return list_str_concat_prefix(reg_alpha, pre);
	}

	// '(' Assoc_Prop_conn '['  Nnf_Array_entry ']' ')'
	if (Assoc_Prop_conn_check(Slice_char(nnf, 1))) {
		string assoc_prop_conn = Slice_char(nnf, 1);

		// (...((wff_1 assoc_prop_conn wff_2) assoc_prop_conn wff_3) ... assoc_prop_conn wff_n)
		// is equiv to (assoc_prop_conn [wff_1, ..., wff_n])
		int begin_entry = 3;
		string equiv_formula = ""; 
		for (int end_entry = 3; end_entry <= len_nnf-1; ++end_entry){
			if (Wff_check(Slice(nnf, begin_entry, end_entry))){   
				string alpha = Slice(nnf, begin_entry, end_entry);
				
				// First entry obtained
				if (begin_entry == 3){
					// Add wff_1 to equiv_formula
					equiv_formula = equiv_formula + alpha;
				}

				// Not first entry
				else {
					// Add wff_n to equiv_formula, where n >= 2
					equiv_formula = "(" + equiv_formula + assoc_prop_conn + alpha + ")";
				}

				// Update begin_entry so it has index of the first char of the next entry.
				begin_entry = end_entry + 2;
			}
		}
		
		return reg_clean(equiv_formula, n);
	}
	
	// '(' Nnf Binary_Prop_conn Nnf ')' | '(' Nnf Binary_Temp_conn Interval Nnf ')'
	int binary_conn_index = primary_binary_conn(nnf);
	string binary_conn = Slice_char(nnf, binary_conn_index);

	// '(' Nnf Binary_Prop_conn Nnf ')'
	if (Binary_Prop_conn_check(binary_conn)) {
		string alpha = Slice(nnf, 1, binary_conn_index - 1);
		vector<string> reg_alpha = reg_clean(alpha, n);
		string beta = Slice(nnf, binary_conn_index + 1, len_nnf - 2);
		vector<string> reg_beta = reg_clean(beta, n);
		
		if (binary_conn == "&") {
			return set_intersect(reg_alpha, reg_beta, n);
		}


		if (binary_conn == "v") {
			return join(reg_alpha, reg_beta);
		}

		if (binary_conn == "="){
			// (alpha = beta) is equiv to ((alpha & beta) v (Wff_to_Nnf_clean(~alpha) & Wff_to_Nnf_clean(~beta)))
			// ((alpha & beta) v (Wff_to_Nnf_clean(~alpha) & Wff_to_Nnf_clean(~beta))) is in Nnf-form
			string equiv_nnf_formula = "((" + alpha + "&" + beta + ")v(" 
				+ Wff_to_Nnf_clean("~" + alpha) + "&" 
				+ Wff_to_Nnf_clean("~" + beta) + "))";
			return reg_clean(equiv_nnf_formula, n);
		}

		if (binary_conn == ">"){
			// (alpha > beta) is equiv to (Wff_to_Nnf(~alpha) v beta))
			// (Wff_to_Nnf(~alpha) v beta)) is in Nnf-form
			string equiv_nnf_formula = "(" + Wff_to_Nnf_clean("~" + alpha) + "v" + beta + ")";
			return reg_clean(equiv_nnf_formula, n);
		}
	}

	// '(' Nnf Binary_Temp_conn Interval Nnf ')'
	if (Binary_Temp_conn_check(binary_conn)) {
		tuple<int,int,int> interval_tuple = primary_interval(nnf);
		int begin_interval = get<0>(interval_tuple); 
		int comma_index = get<1>(interval_tuple); 
		int end_interval = get<2>(interval_tuple); 
		int lower_bound = stoi(Slice(nnf, begin_interval+1, comma_index-1));
		int upper_bound = stoi(Slice(nnf, comma_index+1, end_interval-1));
		string alpha = Slice(nnf, 1, binary_conn_index-1);
		string beta = Slice(nnf, end_interval+1, len_nnf-2);		

		if (binary_conn == "U") {
			// '(' alpha 'U' [a,b] beta ')' is equiv
			// to ( (v[F [a,a-1] Wff_to_Nnf(~alpha), ..., F [a,b-1] Wff_to_Nnf(~alpha)]) v F [a,b] beta).
			// ( (v[F [a,a-1] Wff_to_Nnf(~alpha), ..., F [a,b-1] Wff_to_Nnf(~alpha)]) v F [a,b] beta) is
			// in Nnf-form.
			string equiv_nnf_formula = "";
			string nnf_neg_alpha = Wff_to_Nnf_clean("~" + alpha);
			for (int i = lower_bound-1; i <= upper_bound-1; ++i){
				// Add F [a,i] Wff_to_Nnf(~alpha) to equiv_nnf_formula
				equiv_nnf_formula = equiv_nnf_formula + "F[" 
					+ to_string(lower_bound) + "," + to_string(i) + "]" + nnf_neg_alpha + ",";
			}

			// Remove extra comma
			equiv_nnf_formula = Slice(equiv_nnf_formula, 0, int(equiv_nnf_formula.length()-2));

			equiv_nnf_formula = "((v[" + equiv_nnf_formula + "])" 
			                    + "vF[" + to_string(lower_bound) + "," + to_string(upper_bound) + "]" + beta + ")";
			return reg_clean(equiv_nnf_formula, n);
		}


		if (binary_conn == "R") {
			// '(' alpha 'R' [a,b] beta ')' is equiv
			// to ( (&[G [a,a-1] Wff_to_Nnf(~alpha), ..., G [a,b-1] Wff_to_Nnf(~alpha)]) & G [a,b] beta).
			// ( (&[G [a,a-1] Wff_to_Nnf(~alpha), ..., G [a,b-1] Wff_to_Nnf(~alpha)]) & G [a,b] beta) is
			// in Nnf-form.
			string equiv_nnf_formula = "";
			string nnf_neg_alpha = Wff_to_Nnf_clean("~" + alpha);
			for (int i = lower_bound-1; i <= upper_bound-1; ++i){
				// Add G [a,i] Wff_to_Nnf(~alpha) to equiv_nnf_formula
				equiv_nnf_formula = equiv_nnf_formula + "G[" + to_string(lower_bound) + "," + to_string(i) + "]" + nnf_neg_alpha + ",";
			}

			// Remove extra comma
			equiv_nnf_formula = Slice(equiv_nnf_formula, 0, int(equiv_nnf_formula.length()-2));

			equiv_nnf_formula = "((&[" + equiv_nnf_formula + "])" 
			                    + "&G[" + to_string(lower_bound) + "," + to_string(upper_bound) + "]" + beta + ")";
			return reg_clean(equiv_nnf_formula, n); 
		}
	}

	else{
        string error_string = nnf + " is not in Negation-normal form.\n";
        throw invalid_argument(error_string);
    }
}