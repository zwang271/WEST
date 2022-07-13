#include <string>
#include <tuple>
#include <stdexcept>
#include <iostream>
#include "grammar.h"
#include <vector>
#include "reg.h"
#include "nnf_grammar.h"
#include "utils.h"

/*
This file implements the following Context-Free Grammar:
For a well-formed mLTL formula,

Alphabet = { ‘0’, ‘1’, …, ‘9’, ‘p’, ‘(‘, ‘)’, ‘[’, ‘]’, ':', ‘,’ ,
                       ‘T’, ‘!’,
                       ‘~’, ‘F’, ‘G’,
                       ‘v’, ‘&’, ‘=’, ‘>’, ‘U’, ‘R’ }

Digit  ->  ‘0’ | ‘1’ | … |’9’
Num  ->  Digit Num |  Digit
Interval  ->  ‘[’  Num ‘:’ Num ‘]’
Prop_var  ->  ‘p’ Num

Prop_cons  ->  ‘T’ | ‘!’
Unary_Prop_conn  ->  ‘~’
Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’

Assoc_Prop_conn -> ‘v’ | ‘&’ | ‘=’
Array_entry -> Wff ‘,’ Array_entry  |  Wff

Unary_Temp_conn  ->  ‘F’ | ‘G’
Binary_Temp_conn  ->  ‘U’ | ‘R’


Wff ->  Prop_var | Prop_cons
                 | Unary_Prop_conn Wff
	             | Unary_Temp_conn  Interval  Wff

	             | '(' Assoc_Prop_conn ‘[‘  Array_entry  ']' ')'
                 | '(' Wff Binary_Prop_conn Wff ')'
                 | '(' Wff Binary_Temp_conn  Interval Wff ')'


The Prop constants: True, False are represented by: ‘T’, ‘F’.
The Unary Prop connective: Negation is represented by: ‘~’.
The Binary Prop connectives: Or, And, Iff, Implies are represented by: ‘v’, ‘&’, ‘=’, ‘>’.

The Unary Temp connectives: Eventually, Always are represented by: ‘F’, ‘G’.
The Binary Temp connectives: Until, Weak until are represented by: ‘U’, ‘R’.

*/

using namespace std;


/*
 * Takes substring of given string from a to b.
 */
string Slice(string s, int a, int b){
	int s_len = int(s.length());
  	
  	if (a < 0){
		a = 0;      
    }
  	// Now 0 <= a
  	
  	if (b > s_len-1){
    	b = s_len-1;	
    }
  	// Now b <= s_len-1
  	
  	if (a > b){
      return "";
    }
  	// Now a <= b

	return s.substr(a, b-a+1);
}


/*
 * Returns length 1 string at index i.
 */
string Slice_char(string s, int i){
    return Slice(s, i, i);
}


/*
 * Digit  ->  ‘0’ | ‘1’ | … |’9’
 * Checks that the inputted string is a digit.
 */
bool Digit_check(string s){
    return s == "0" or s == "1" or s == "2" or s == "3" 
        or s == "4" or s == "5" or s == "6" or s == "7" or s == "8" or s == "9";
}


/*
 * Num  ->  Digit Num |  Digit
 * Checks that the inputted string is of length 1 and then runs digit_check().
 */
bool Num_check(string s){
    int len_s = int(s.length());
    if (len_s == 1){
        return Digit_check(s);
    }

    string c = Slice_char(s, 0);
    string alpha = Slice(s, 1, len_s-1);
    return Digit_check(c) and Num_check(alpha);
}


/*
 * Interval  ->  ‘[’  Num ‘,’ Num ‘]’
 * Checks that the inputted string is of the form of an interval.
 */
bool Interval_check(string s){
    int len_s = int(s.length());

    string left_bracket = Slice_char(s, 0);
    string right_bracket = Slice_char(s, len_s-1);

    // Parse for comma index
    int comma_index = 1;
    while(Num_check(Slice(s, 1, comma_index)) and comma_index <= len_s-1){
        ++comma_index;
    }

    string num_1 = Slice(s, 1, comma_index-1);
    string comma = Slice_char(s, comma_index);
    string num_2 = Slice(s, comma_index+1, len_s-2);
    return left_bracket == "[" and Num_check(num_1) and comma == ":" 
        and Num_check(num_2) and right_bracket == "]";
}


/*
 * Prop_var  ->  ‘p’ Num
 * Checks that the inputted string is a propositional variable.
 */
bool Prop_var_check(string s){
    int len_s = int(s.length());

    string c = Slice_char(s, 0);
    string alpha = Slice(s, 1, len_s-1);
    return c == "p" and Num_check(alpha);
}


/*
 * Prop_cons  ->  ‘T’ | ‘!’
 * Checks that the inputted string is a propositional constant.
 */
bool Prop_cons_check(string s){
    return s == "T" or s == "!";
}


/*
 * Unary_Prop_conn  ->  ‘~’
 * Checks that the inputted string is the negation symbol (the unary prop. connective).
 */
bool Unary_Prop_conn_check(string s){
    return s == "~";
}


/*
 * Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’
 * Checks that the inputted string is a binary prop. connective (or, and, equivalence, implication).
 */
bool Binary_Prop_conn_check(string s){
    return s == "v" or s == "&" or s == "=" or s == ">";
}


/*
 * Assoc_Prop_conn -> ‘v’ | ‘&’ | ‘=’
 * Checks that the inputted string is an associative prop. connective (or, and, equivalence).
 */
bool Assoc_Prop_conn_check(string s){
    return s == "v" or s == "&" or s == "=";
}


/*
 * Array_entry -> Wff ‘,’ Array_entry  |  Wff
 * Checks that the inputted string is an array of WFFs.
 * We use an array of WFFs, for example, when ANDing >2 formulas.
 */
bool Array_entry_check(string s){
    int len_s = int(s.length());

    // Number of '(' in s
    int left_count = 0;
    // Number of ')' in s
    int right_count = 0;


    //    Parse for comma_index in s
    //    When left_count == right_count, we are done parsing and have found comma_index.
    int comma_index = 0;
    for (comma_index = 0; comma_index <= len_s-1; ++comma_index){

        string c = Slice_char(s, comma_index);

        if (c == "(") {
            ++left_count;
        }
        else if (c == ")"){
            ++right_count;
        }

        // Done parsing for comma_index.
        
        //cout << s << " " << comma_index << " " << left_count << " " << right_count << endl;

        if (left_count == right_count and c == ","){
            break;
        }
    }

    return (Wff_check(Slice(s, 0, comma_index-1)) and Slice_char(s, comma_index) == "," 
        and Array_entry_check(Slice(s, comma_index+1, len_s-1))) or Wff_check(s) ;
}


/*
 * Unary_Temp_conn  ->  ‘F’ | ‘G’
 * Checks that the inputted string is F or G (the unary temporal connectives).
 */
bool Unary_Temp_conn_check(string s){
    return s == "F" or s == "G";
}


/*
 * Binary_Temp_conn  ->  ‘U’ | ‘R’
 * Checks that the inputted string is U or R (the binary temporal connectives).
 */
bool Binary_Temp_conn_check(string s){
    return s == "U" or s == "R";
}


/*
 *  Wff ->  Prop_var | Prop_cons
 *                  | Unary_Prop_conn Wff
 *                  | Unary_Temp_conn  Interval  Wff
 *                  | '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
 *                  | ‘(‘ Wff Binary_Prop_conn Wff ‘)’
 *                  | ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)
 *  Checks that an inputted string is a WFF.
 */

bool Wff_check(string s){
    int len_s = int(s.length());

    // Prop_var | Prop_cons
    if (Prop_var_check(s) or Prop_cons_check(s)){
        return true;
    }

    // Unary_Prop_conn Wff
    if (Unary_Prop_conn_check(Slice_char(s, 0))){
        string alpha = Slice(s, 1, len_s-1);
        return  Wff_check(alpha);
    }    

    // Unary_Temp_conn  Interval  Wff
    if (Unary_Temp_conn_check(Slice_char(s, 0))){
        int begin_interval = 1;
        int end_interval = 2;

        // Parse for end of interval
        while (Slice_char(s, end_interval) != "]" and end_interval <= len_s-1){
            end_interval = end_interval + 1;
        }

        string interval = Slice(s, begin_interval, end_interval);
        string alpha = Slice(s, end_interval+1, len_s-1);
        return Interval_check(interval) and Wff_check(alpha);
    }

    // '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
    if(Assoc_Prop_conn_check(Slice_char(s, 1))){
        int begin_array = 2;
        int end_array = len_s-2;

        string array_entry = Slice(s, begin_array+1, end_array-1);
        return Slice_char(s, 0) == "(" 
            and Slice_char(s, 2) == "["
            and Array_entry_check(array_entry)
            and Slice_char(s, len_s - 2) == "]"
            and Slice_char(s, len_s - 1) == ")"; 
    }

    // ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
    if (Slice_char(s, 0) == "(" and Slice_char(s, len_s-1) == ")"){

        // Number of '(' in s
        int left_count = 0;
        // Number of ')' in s
        int right_count = 0;


        //    Parse for binary_conn_index in s

        //    When left_count == right_count and s[binary_conn_index] is a binary connective,
        //    we are done parsing and have found binary_conn_index.

        int binary_conn_index = 1;
        for (binary_conn_index = 1; binary_conn_index <= len_s-1; ++binary_conn_index){
            string c = Slice_char(s, binary_conn_index);

            if(c == "("){
                ++left_count;
            }

            if(c == ")"){
                ++right_count;
            }

            // Done parsing for binary_conn_index.
            if(left_count == right_count and (Binary_Prop_conn_check(c) or Binary_Temp_conn_check(c))){
                break;
            }
        }

        string binary_conn = Slice_char(s, binary_conn_index);

        // ‘(‘ Wff Binary_Prop_conn Wff ‘)’
        if (Binary_Prop_conn_check(binary_conn)){
            string alpha = Slice(s, 1, binary_conn_index-1);
            string beta = Slice(s, binary_conn_index+1, len_s-2);
            return Wff_check(alpha) and Wff_check(beta);
        }

        // ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
        if (Binary_Temp_conn_check(binary_conn)){
            int begin_interval = binary_conn_index+1;
            int end_interval = binary_conn_index+2;

            // Parse for end of interval
            while (Slice_char(s, end_interval) != "]" and end_interval <= len_s-1){
                end_interval = end_interval + 1;
            }

            string alpha = Slice(s, 1, binary_conn_index-1);
            string interval = Slice(s, begin_interval, end_interval);
            string beta = Slice(s, end_interval+1, len_s-2);
            return Wff_check(alpha) and Interval_check(interval) and Wff_check(beta);
        }
    }

    return false;
}


/*
 * Returns the index of the primary binary connective of a WFF.
 */
int primary_binary_conn(string wff){
    int len_wff = int(wff.length());

    // '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
    if (Assoc_Prop_conn_check(Slice_char(wff, 1))){
        return 1;
    }

    // ‘(‘ Wff Binary_Prop_conn Wff ‘)’  |  ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)'
    if (Slice_char(wff, 0) == "(" and Slice_char(wff, len_wff-1) == ")"){
        int left_count = 0;
        int right_count = 0;

        int binary_conn_index = 1;
        for (binary_conn_index = 1; binary_conn_index <= len_wff-1; ++binary_conn_index){
            string c = Slice_char(wff, binary_conn_index);

            if(c == "("){
                ++left_count;
            }

            if(c == ")"){
                ++right_count;
            }

            if(left_count == right_count and (Binary_Prop_conn_check(c) or Binary_Temp_conn_check(c))){
                break;
            }
        }

        return binary_conn_index;
    }

    else{
        string error_string = wff + " does not have a primary binary connective.\n";
        throw invalid_argument(error_string);
        return -1;
    }
}


/*
 * Returns the indices where the primary interval is in a given WFF.
 */
tuple<int, int, int> primary_interval(string wff){
    int len_wff = int(wff.length());

    // Unary_Temp_conn  Interval  Wff
    if (Unary_Temp_conn_check(Slice_char(wff, 0))){
        int begin_interval = 1;

        // Parse for comma_index
        int comma_index = begin_interval+1;
        while (Num_check(Slice(wff, begin_interval+1, comma_index)) and comma_index <= len_wff-1){
            ++comma_index;
        }

        // Parse for end_interval
        int end_interval = comma_index+1;
        while (Num_check(Slice(wff, comma_index+1, end_interval)) and end_interval <= len_wff-1){
            ++end_interval;
        }

        return make_tuple(begin_interval, comma_index, end_interval);
    }


    // ‘(‘ Wff Binary_Temp_conn Interval Wff ‘)
    int binary_conn_index = primary_binary_conn(wff);
    if (Binary_Temp_conn_check(Slice_char(wff, binary_conn_index))){
        int begin_interval = binary_conn_index+1;

        // Parse for comma_index
        int comma_index = begin_interval+1;
        while (Num_check(Slice(wff, begin_interval+1, comma_index)) and comma_index <= len_wff-1){
            ++comma_index;
        }

        // Parse for end_interval
        int end_interval = comma_index+1;
        while (Num_check(Slice(wff, comma_index+1, end_interval)) and end_interval <= len_wff-1){
            ++end_interval;
        }

        return make_tuple(begin_interval, comma_index, end_interval);
    }

   
    else{
        string error_string = wff + " does not have a primary interval.\n";
        throw invalid_argument(error_string);
    }
}


/*
 * Determines the minimum computation length needed for a given WFF to not have out-of-bounds behavior.
 */
int Comp_len(string wff){
    int len_wff = int(wff.length());

    // Prop_var
    if (Prop_var_check(wff)){
        return 1;
    }

    // Prop_cons
    if (Prop_cons_check(wff)){
        return 0; 
    }

    string c = Slice_char(wff, 0);
    // Unary_Prop_conn Wff
    if (Unary_Prop_conn_check(c)){
        string alpha = Slice(wff, 1, len_wff-1);
        return Comp_len(alpha);
    }

    // Unary_Temp_conn  Interval  Wff
    if (Unary_Temp_conn_check(c)){
        tuple<int, int, int> interval = primary_interval(wff);
        int comma_index = get<1>(interval);
        int end_interval = get<2>(interval);
        int upperbound = stoi(Slice(wff, comma_index+1, end_interval-1));
        string alpha = Slice(wff, end_interval+1, len_wff-1);
        return upperbound + Comp_len(alpha); 
    }

    // '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
    c = Slice_char(wff, 1);
    if (Assoc_Prop_conn_check(c)){
        // Parse through '[' wff_1 ',' wff_2 ',' ... ',' wff_n ']' entry-by-entry
        // and iteratively compute: return_value = max(Comp_len(wff_1), ..., Comp_len(wff_n))
        int begin_entry = 3;
        int return_value = 0;
        for (int end_entry = 3; end_entry <= len_wff-1; ++end_entry){
            if (Wff_check(Slice(wff, begin_entry, end_entry))){
                string alpha = Slice(wff, begin_entry, end_entry);
                int Comp1 = Comp_len(alpha);

                // Take max of current return_value and Comp_len(alpha)
                if (return_value < Comp1){
                    return_value = Comp1;
                }

                // Update begin_entry so it has index of the first char of the next entry.
                begin_entry = end_entry + 2;
            }
        }

        return return_value;
    }

    // ‘(‘ Wff Binary_Prop_conn Wff ‘)’  |  ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)'
    int binary_conn_index = primary_binary_conn(wff);
    string binary_conn = Slice_char(wff, binary_conn_index);

    // ‘(‘ Wff Binary_Prop_conn Wff ‘)’
    if (Binary_Prop_conn_check(binary_conn)){
        string alpha = Slice(wff, 1, binary_conn_index-1); 
        int Comp_alpha = Comp_len(alpha); 
        string beta = Slice (wff, binary_conn_index+1, len_wff-2);
        int Comp_beta = Comp_len(beta);

        return max(Comp_alpha, Comp_beta);
    }

    // ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)'
    if (Binary_Temp_conn_check(binary_conn)){
        tuple<int, int, int> interval = primary_interval(wff);
        int comma_index = get<1>(interval);
        int end_interval = get<2>(interval);
        int upper_bound = stoi(Slice(wff, comma_index + 1, end_interval - 1));

        string alpha = Slice(wff, 1, binary_conn_index-1);
        int Comp_alpha = Comp_len(alpha); 
        string beta = Slice(wff, end_interval+1, len_wff-2);
        int Comp_beta = Comp_len(beta);
        
        return max((upper_bound-1) + Comp_alpha, upper_bound + Comp_beta);
    }

    
    else{
        string error_string = wff + " is not a well-formed formula.\n";
        throw invalid_argument(error_string);
    }
}

//Nnf ->  ?( ‘~’) Prop_var | Prop_cons
//                            | Unary_Temp_conn  Interval  Nnf
//
//                            | '(' Assoc_Prop_conn ‘[‘  Nnf_Array_entry ‘]’ ')'
//                                      | ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’
//                                     | ‘(‘ Nnf Binary_Temp_conn  Interval Nnf  ‘)’


vector<tuple<string, vector<string>>> subformula_regex(string wff, int n) {
    vector<tuple<string, vector<string>>> formulas;
    string nnf = Wff_to_Nnf_clean(wff);
    int len_nnf = int(nnf.length());
    
    //if ( ‘~’) Prop_var
    if (Slice_char(nnf, 0) == "~" and Prop_var_check(Slice(nnf, 1, len_nnf - 1))) {
        string prop_var = Slice(nnf, 1, len_nnf - 1);
        vector<string> prop_var_regex = reg(prop_var, n);
        tuple<string, vector<string>> prop_var_tuple = make_tuple(prop_var, prop_var_regex);
        formulas.push_back(prop_var_tuple);
        
        vector<string> nnf_regex = reg(nnf, n);
        tuple<string, vector<string>> nnf_tuple = make_tuple(nnf, nnf_regex);
        formulas.push_back(nnf_tuple);
        // Return formulas = (prop_var_tuple, nnf_tuple)
        return formulas;
    }
    
    //if Prop_var | Prop_cons
    if (Prop_var_check(nnf) or Prop_cons_check(nnf)) {
        vector<string> regex = reg(nnf, n);
        tuple<string, vector<string>> regex_tuple = make_tuple(nnf, regex);
        formulas.push_back(regex_tuple);
        // Return formulas = (regex_tuple)
        return formulas;
    }
    
    //recursion starts, think about how to do that
    //if Unary_Temp_conn  Interval  Nnf
    if (Unary_Temp_conn_check(Slice_char(nnf, 0))) {
        tuple<int, int, int> interval_tuple = primary_interval(nnf);
        int begin_interval = get<0>(interval_tuple);
        int colon_index = get<1>(interval_tuple);
        int end_interval = get<2>(interval_tuple);
        string interval = Slice(nnf, begin_interval, end_interval);
        int lower_bound = stoi(Slice(nnf, begin_interval+1, colon_index-1));
        int upper_bound = stoi(Slice(nnf, colon_index+1, end_interval-1));
        string alpha = Slice(nnf, end_interval+1, len_nnf-1);
        if (Interval_check(interval) and Nnf_check(alpha)) {
            //# prop var of alpha == # prop var of unary_temp_conn interval alpha
            vector<string> alpha_regex = reg(alpha, n);
            tuple<string, vector<string>> alpha_regex_tuple = make_tuple(alpha, alpha_regex);
            formulas.push_back(alpha_regex_tuple);
            
            //F interval alpha
            if (Slice_char(nnf, 0) == "F") {
                vector<string> nnf_regex_F = reg_F(alpha_regex, lower_bound, upper_bound, n);
                tuple<string, vector<string>> F_tuple = make_tuple(nnf, nnf_regex_F);
                formulas.push_back(F_tuple);
                // Return formulas = (alpha_regex_tuple, F_tuple)
                return formulas;
            }
            
            //G interval alpha
            else if (Slice_char(nnf, 0) == "G") {
                vector<string> nnf_regex_G = reg_G(alpha_regex, lower_bound, upper_bound, n);
                tuple<string, vector<string>> G_tuple = make_tuple(nnf, nnf_regex_G);
                formulas.push_back(G_tuple);
                // Return formulas = (alpha_regex_tuple, G_tuple)
                return formulas;
            }
            else {
                string error_string = nnf + " does not have a unary temp connective.\n";
                throw invalid_argument(error_string);
            }
        }
    }
    
    
    //(&[p0, p1])
    //if '(' Assoc_Prop_conn ‘[‘  Nnf_Array_entry ‘]’ ')'
    if (Slice_char(nnf, 0) == "(" and Assoc_Prop_conn_check(Slice_char(nnf, 1)) and Slice_char(nnf, 2) == "[" and
        Nnf_Array_entry_check(Slice(nnf, 3, len_nnf-3)) and Slice(nnf, len_nnf-2, len_nnf-1) == "])") {
        
        int num_open_bracket = 1;
        int num_close_bracket = 0;
        string prop_con = Slice_char(nnf, 1);
        //(&[p0,p1,p2])
        string alpha = "";
        for (int i = 3; i < len_nnf; ++i) {
            if (Slice_char(nnf, i) == "]") ++num_close_bracket;
            if (Slice_char(nnf, i) == "[") ++num_open_bracket;
            
            //concatenate alpha
            if (Slice_char(nnf, i) != "," && (Slice_char(nnf, i) != "]" || num_open_bracket != num_close_bracket)) {
                alpha += Slice_char(nnf, i);
            }
            
            else if (Slice_char(nnf, i) == "," || Slice_char(nnf, i) == "]") {
                //(&[p1,p2,(p4R[0:1]p5)])
                if (Nnf_check(alpha) && (num_close_bracket == num_open_bracket || Slice_char(nnf, i) == ",")) {
                    vector<string> alpha_regex = reg(alpha, n);
                    tuple<string, vector<string>> alpha_tuple = make_tuple(alpha, alpha_regex);
                    formulas.push_back(alpha_tuple);
                }
                alpha = "";
            }
        }
        
        vector<string> nnf_regex = reg(nnf, n);
        tuple<string, vector<string>> nnf_tuple = make_tuple(nnf, nnf_regex);
        formulas.push_back(nnf_tuple);
        //return formulas = (alpha_tuple, ... , alpha_tuple, nnf_tuple)
        return formulas;
    }
    
    //if ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’
    int binary_con_index = primary_binary_conn(nnf);
    string alpha = Slice(nnf, 1, binary_con_index-1);
    string beta = Slice(nnf, binary_con_index+1, len_nnf-2);
    if (Slice_char(nnf, 0) == "(" and  Nnf_check(alpha) and
        Binary_Prop_conn_check(Slice_char(nnf, binary_con_index)) and
        Nnf_check(beta) and Slice_char(nnf, len_nnf-1) == ")") {
        //# prop var of alpha,beta == # prop var of ‘(‘ alpha Binary_Prop_conn beta ‘)’
        vector<string> alpha_regex = reg(alpha, n);
        tuple<string, vector<string>> alpha_tuple = make_tuple(alpha, alpha_regex);
        formulas.push_back(alpha_tuple);
        
        vector<string> beta_regex = reg(beta, n);
        tuple<string, vector<string>> beta_tuple = make_tuple(beta, beta_regex);
        formulas.push_back(beta_tuple);
        
        string binary_con = Slice_char(nnf, binary_con_index);
        //(alpha v beta)
        if (binary_con == "v")  {
            vector<string> nnf_regex = join(alpha_regex, beta_regex, n);
            tuple<string, vector<string>> nnf_tuple = make_tuple(nnf, nnf_regex);
            formulas.push_back(nnf_tuple);
            // Return formulas = (alpha_tuple, beta_tuple, nnf_tuple)
            return formulas;
        }
        
        //(alpha & beta)
        if (binary_con == "&") {
            vector<string> nnf_regex = set_intersect(alpha_regex, beta_regex, n);
            tuple<string, vector<string>> nnf_tuple = make_tuple(nnf, nnf_regex);
            formulas.push_back(nnf_tuple);
            // Return formulas = (alpha_tuple, beta_tuple, nnf_tuple)
            return formulas;
        }
        
        //(alpha = beta)
        if (binary_con == "=") {
            vector<string> nnf_regex = reg(nnf, n);
            tuple<string, vector<string>> nnf_tuple = make_tuple(nnf, nnf_regex);
            formulas.push_back(nnf_tuple);
            // Return formulas = (alpha_tuple, beta_tuple, nnf_tuple)
            return formulas;
        }
        
        //(alpha > beta)
        if (binary_con == ">") {
            vector<string> nnf_regex = reg(nnf, n);
            tuple<string, vector<string>> nnf_tuple = make_tuple(nnf, nnf_regex);
            formulas.push_back(nnf_tuple);
            // Return formulas = (alpha_tuple, beta_tuple, nnf_tuple)
            return formulas;
        }
        
        return formulas;
        
    
    }
        
    //if ‘(‘ Nnf Binary_Temp_conn  Interval Nnf  ‘)’
    binary_con_index = primary_binary_conn(nnf);
    string binary_temp_con = Slice_char(nnf, binary_con_index);
    tuple<int, int, int> interval_tuple = primary_interval(nnf);
    int begin_interval = get<0>(interval_tuple);
    int colon_index = get<1>(interval_tuple);
    int end_interval = get<2>(interval_tuple);
    string interval = Slice(nnf, begin_interval, end_interval);
    int lower_bound = stoi(Slice(interval, begin_interval+1, colon_index-1));
    int upper_bound = stoi(Slice(interval, colon_index+1, end_interval-1));
    alpha = Slice(nnf, 1, binary_con_index-1);
    beta = Slice(nnf, end_interval+1, len_nnf-2);
    if (Slice_char(nnf, 0) == "(" and  Nnf_check(alpha) and
        Binary_Temp_conn_check(Slice_char(nnf, binary_con_index))
        and Interval_check(interval) and
        Nnf_check(beta) and Slice_char(nnf, len_nnf-1) == ")") {
        
        vector<string> alpha_regex = reg(alpha, n);
        tuple<string, vector<string>> alpha_tuple = make_tuple(alpha, alpha_regex);
        formulas.push_back(alpha_tuple);
        vector<string> beta_regex = reg(beta, n);
        tuple<string, vector<string>> beta_tuple = make_tuple(beta, beta_regex);
        formulas.push_back(beta_tuple);
        
        if (binary_temp_con == "U") {
            vector<string> nnf_regex = reg_U(alpha_regex, beta_regex, lower_bound, upper_bound, n);
            tuple<string, vector<string>> nnf_tuple = make_tuple(alpha, nnf_regex);
            formulas.push_back(nnf_tuple);
            // Return formulas = (alpha_tuple, beta_tuple, nnf_tuple)
            return formulas;
        }
        
        if (binary_temp_con == "R") {
            vector<string> nnf_regex = reg_R(alpha_regex, beta_regex, lower_bound, upper_bound, n);
            tuple<string, vector<string>> nnf_tuple = make_tuple(alpha, nnf_regex);
            formulas.push_back(nnf_tuple);
            // Return formulas = (alpha_tuple, beta_tuple, nnf_tuple)
            return formulas;
        }
    }
        
    
    return formulas;
}

//vector<tuple<string, vector<string>>> subformula_regex(string wff, int n)
void print_subformulas(vector< tuple<string, vector<string>> > formulas) {
    for (int i = 0; i < formulas.size(); ++i) {
        cout << get<0>(formulas[i]) << endl;
        print(get<1>(formulas[i]));
        cout << endl;
        }
}


