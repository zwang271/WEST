#include "string_grammar.h"
#include "string_nnf_grammar.h"
#include <string>
#include <tuple>
#include <stdexcept>
#include <iostream>


/*
Context-Free Grammar for a mLTL wff in Negation normal-form (Nnf):
Here, ‘Eventually’, ‘Always’, ‘Until’, and ‘Release’ are represented by the letters ‘F’, ‘G’, ‘U’, and ‘R’.

Alphabet = { ‘0’, ‘1’, …, ‘9’, ‘p’, ‘(‘, ‘)’, ‘[’, ‘]’, ':', ‘,’ ,
                       ‘T’, ‘!’,                
                       ‘~’, ‘F’, ‘G’,
                       ‘v’, ‘&’, ‘=’, ‘>’, ‘U’, ‘R’ }

Digit  ->  ‘0’ | ‘1’ | … |’9’
Num  ->  Digit Num |  Digit
Interval  ->  ‘[’  Num ‘:’ Num ‘]’  
Prop_var  ->  ‘p’ Num

Prop_cons  ->  ‘T’ | ‘!’
Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’

Assoc_Prop_conn -> ‘v’ | ‘&’ | ‘=’
Nnf_Array_entry -> Nnf ‘,’ Nnf_Array_entry  |  Nnf 

Unary_Temp_conn  ->  ‘F’ | ‘G’
Binary_Temp_conn  ->  ‘U’ | ‘R’

Nnf ->  ?('~')  Prop_var | Prop_cons
	                   | Unary_Temp_conn  Interval  Nnf

	                   | '(' Assoc_Prop_conn ‘[‘ Nnf_Array_entry ‘]’ ')'
                       | ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’
                       | ‘(‘ Nnf Binary_Temp_conn  Interval Nnf  ‘)’

*/

using namespace std;


/*
 * Nnf_Array_entry -> Nnf ‘,’ Nnf_Array_entry  |  Nnf
 * Checks that an inputted string is an array of formulas in NNF.
 */
bool Nnf_Array_entry_check(string s){
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
        if (left_count == right_count and c == ","){
            break;
        }
    }

    return (Nnf_check(Slice(s, 0, comma_index-1)) and Slice_char(s, comma_index) == "," 
        and Nnf_Array_entry_check(Slice(s, comma_index+1, len_s-1))) or Nnf_check(s) ;
}


/*
 *  Nnf ->   ?('~') Prop_var | Prop_cons
 *          | Unary_Temp_conn  Interval  Nnf
 *          | '(' Assoc_Prop_conn ‘[‘ Nnf_Array_entry ‘]’ ')'
 *          | ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’
 *          | ‘(‘ Nnf Binary_Temp_conn  Interval Nnf  ‘)’
 *          | ‘(‘ Nnf ‘)’
 * Checks that the inputted string is a WWF in NNF.
 */
bool Nnf_check(string s){
    int len_s = int(s.length());

    // ?('~') Prop_var | Prop_cons
    if (Prop_var_check(s) or (Slice_char(s,0) == "~" and Prop_var_check(Slice(s, 1, len_s-1))) 
                          or Prop_cons_check(s)){
        return true;
    }  

    // Unary_Temp_conn  Interval  Nnf
    if (Unary_Temp_conn_check(Slice_char(s, 0))){
        int begin_interval = 1;
        int end_interval = 2;

        // Parse for end of interval
        while (Slice_char(s, end_interval) != "]" and end_interval <= len_s-1){
            end_interval = end_interval + 1;
        }

        string interval = Slice(s, begin_interval, end_interval);
        string alpha = Slice(s, end_interval+1, len_s-1);
        return Interval_check(interval) and Nnf_check(alpha);
    }

    // '(' Assoc_Prop_conn ‘[‘ Nnf_Array_entry ‘]’ ')'
    if(Assoc_Prop_conn_check(Slice_char(s, 1))){
        int begin_array = 2;
        int end_array = len_s-2;

        string array_entry = Slice(s, begin_array+1, end_array-1);
        return Slice_char(s, 0) == "(" 
            and Slice_char(s, 2) == "["
            and Nnf_Array_entry_check(array_entry)
            and Slice_char(s, len_s - 2) == "]"
            and Slice_char(s, len_s - 1) == ")"; 
    }

//    ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’ | ‘(‘ Nnf Binary_Temp_conn  Interval Nnf  ‘)’
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

        // ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’
        if (Binary_Prop_conn_check(binary_conn)){
            string alpha = Slice(s, 1, binary_conn_index-1);
            string beta = Slice(s, binary_conn_index+1, len_s-2);
            return Nnf_check(alpha) and Nnf_check(beta);
        }

        // ‘(‘ Nnf Binary_Temp_conn  Interval Nnf  ‘)’
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
            return Nnf_check(alpha) and Interval_check(interval) and Nnf_check(beta);
        }
    }

    // ‘(‘ Nnf ‘)’
    if (Slice_char(s, 0) == "(" and Slice_char(s, len_s-1) == ")"){
        string alpha = Slice(s, 1, len_s-2);
        return Nnf_check(alpha);
    }

    return false;
}


/*
 * Converts a WFF to its equivalent NNF.
 */
string Wff_to_Nnf(string wff){
    int len_wff = int(wff.length());

    //Determine if first symbol is '~' or not.
    if (Slice_char(wff, 0) != "~"){
        // Prop_var
        if (Prop_var_check(wff)){
            return wff;
        }

        // Prop_cons
        if (Prop_cons_check(wff)){
            return wff;
        }

        // Unary_Temp_conn Interval wff
        if (Unary_Temp_conn_check(Slice_char(wff, 0))){
            string unary_temp_conn = Slice_char(wff, 0);
            tuple<int, int, int> interval_tuple = primary_interval(wff);
            int begin_interval = get<0>(interval_tuple); 
            int end_interval =  get<2>(interval_tuple);
            string alpha = Slice(wff, end_interval+1, len_wff-1);

            // Input: unary_temp_conn Interval alpha
            // Return: unary_temp_conn Interval Wff_to_Nnf(alpha)
            return unary_temp_conn + Slice(wff, begin_interval, end_interval) + Wff_to_Nnf(alpha);
        } 

        // '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
        if (Assoc_Prop_conn_check(Slice_char(wff, 1))){
            // Parse through '[' wff_1 ',' wff_2 ',' ... ',' wff_n ']' entry-by-entry
            // and iteratively compute: return_string = '(' Assoc_Prop_conn '[' Wff_to_Nnf(wff_1) ',' ... ',' Wff_to_Nnf(wff_n) ']' ')'
            int begin_entry = 3;
            string return_string = Slice(wff, 0, 2); 
            for (int end_entry = 3; end_entry <= len_wff-1; ++end_entry){
                if (Wff_check(Slice(wff, begin_entry, end_entry))){
                    string alpha = Slice(wff, begin_entry, end_entry);
                    
                    // Add Wff_to_Nnf(alpha) to return string
                    return_string = return_string + Wff_to_Nnf(alpha) + ",";

                    // Update begin_entry so it has index of the first char of the next entry.
                    begin_entry = end_entry + 2;
                }
            }

            // Remove extra comma at right end
            return_string = Slice(return_string, 0, int(return_string.length()-2));

            // Input: '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
            // Return: '(' Assoc_Prop_conn '[' Wff_to_Nnf(wff_1) ',' ... ',' Wff_to_Nnf(wff_n) ']' ')'
            return_string = return_string + "])";
            return return_string;
        }

        // ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)    
        int binary_conn_index = primary_binary_conn(wff);
        string binary_conn = Slice_char(wff, binary_conn_index);

        // ‘(‘ Wff Binary_Prop_conn Wff ‘)’
        if (Binary_Prop_conn_check(binary_conn)){
            string alpha = Slice(wff, 1, binary_conn_index-1);
            string beta = Slice(wff, binary_conn_index+1, len_wff-2);

            // Input: '(' alpha binary_conn beta ')' 
            // Return: '(' Wff_to_Nnf(alpha) + binary_conn + Wff_to_Nnf(beta)
            return "(" + Wff_to_Nnf(alpha) + binary_conn + Wff_to_Nnf(beta) + ")";
        }

        // ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)'
        if (Binary_Temp_conn_check(binary_conn)){
            tuple<int, int, int> interval_tuple = primary_interval(wff);
            int begin_interval = get<0>(interval_tuple);
            int end_interval = get<2>(interval_tuple);

            string alpha = Slice(wff, 1, binary_conn_index-1);
            string beta = Slice(wff, end_interval+1, len_wff-2);

            // Input: ‘(‘ alpha binary_conn  Interval beta ‘)'  
            // Return: '(' Wff_to_Nnf(alpha) binary_conn Interval Wff_to_Nnf(beta) ')'
            return "(" + Wff_to_Nnf(alpha) + binary_conn 
                + Slice(wff, begin_interval, end_interval) + Wff_to_Nnf(beta) + ")";
        }    
        
    }

    // Slice_char(wff, 0) == "~"
    if (Slice_char(wff, 0) == "~"){
        // '~' Prop_var
        if (Prop_var_check(Slice(wff, 1, len_wff-1))){
            return wff;
        }

        // '~' Prop_cons
        if (Prop_cons_check(Slice(wff, 1, len_wff-1))){
            // Prop_cons -> 'T'
            if (Slice(wff, 1, len_wff-1) == "T"){
                return "!";
            }

            // Prop_cons -> '!'
            if (Slice(wff, 1, len_wff-1) == "!"){
                return "T";
            }
        }

        // '~' Unary_Prop_conn Wff
        if (Unary_Prop_conn_check(Slice_char(wff, 1))){
            string alpha = Slice(wff, 2, len_wff-1);
            return Wff_to_Nnf(alpha);
        }

        // '~' Unary_Temp_conn  Interval  Wff
        if (Unary_Temp_conn_check(Slice_char(wff, 1))){
            string unary_temp_conn = Slice_char(wff, 1);

            tuple<int, int, int> interval_tuple = primary_interval(Slice(wff, 1, len_wff-1));
            // Add 1 to every entry of interval tuple to accomodate sliced-off '~' char.
            interval_tuple = make_tuple(get<0>(interval_tuple) + 1, get<1>(interval_tuple) + 1, get<2>(interval_tuple) + 1);

            int begin_interval = get<0>(interval_tuple);
            int end_interval = get<2>(interval_tuple);
            string alpha =  Slice(wff, end_interval+1, len_wff-1);

            // Switch 'F', 'G' with corresponding dual: 'G', 'F'
            if (unary_temp_conn == "F"){
                unary_temp_conn = "G";
            }
            else if (unary_temp_conn == "G"){
                unary_temp_conn = "F";
            }  

            // Input: '~' unary_Temp_conn  Interval  alpha 
            // Return: dual(unary_Temp_conn) Interval Wff_to_Nnf("~" + alpha)
            return unary_temp_conn + Slice(wff, begin_interval, end_interval) + Wff_to_Nnf("~" + alpha);
        }

        // '~' ‘(‘ Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ‘)’
        if (Assoc_Prop_conn_check(Slice_char(wff, 2))){
            string assoc_prop_conn = Slice_char(wff, 2);

            // ~ (...((wff_1 assoc_prop_conn wff_2) assoc_prop_conn wff_3) ... assoc_prop_conn wff_n)
            // is equiv to ~(assoc_prop_conn [wff_1, ..., wff_n])
            int begin_entry = 4;
            string equiv_formula = ""; 
            for (int end_entry = 4; end_entry <= len_wff-1; ++end_entry){
                if (Wff_check(Slice(wff, begin_entry, end_entry))){   
                    string alpha = Slice(wff, begin_entry, end_entry);
                    
                    // First entry obtained
                    if (begin_entry == 4){
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

            // Add on "~" on front
            equiv_formula = "~" + equiv_formula;
            
            
            return Wff_to_Nnf(equiv_formula);
        }

        // '~' ‘(‘ Wff Binary_Prop_conn Wff ‘)’ | '~' ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)

        // Add 1 to accomodate for sliced-off '~'    
        int binary_conn_index = primary_binary_conn(Slice(wff, 1, len_wff-1)) + 1;
        string binary_conn = Slice_char(wff, binary_conn_index);

        // '~' ‘(‘ Wff Binary_Prop_conn Wff ‘)’
        if (Binary_Prop_conn_check(binary_conn)){
            // '~' ‘(‘ Wff 'v' Wff ‘)’ | '~' ‘(‘ Wff '&' Wff ‘)
            if (binary_conn == "v" or binary_conn == "&"){ 

                // Switch 'v', '&' with corresponding dual: '&', 'v'
                if (binary_conn == "v"){
                    binary_conn = "&";
                }
                else if (binary_conn == "&"){
                    binary_conn = "v";
                } 

                string nega_alpha = "~" + Slice(wff, 2, binary_conn_index-1);
                string nega_beta = "~" + Slice(wff, binary_conn_index+1, len_wff-2);
                return "(" + Wff_to_Nnf(nega_alpha) + binary_conn + Wff_to_Nnf(nega_beta) + ")";
            }

            // '~' ‘(‘ Wff '=' Wff ')'
            if (binary_conn == "="){
                string alpha = Slice(wff, 2, binary_conn_index-1);
                string beta = Slice(wff, binary_conn_index+1, len_wff-2);
                string nega_alpha = "~" + alpha;
                string nega_beta = "~" + beta;

                // Input: '~' ‘(‘ alpha '=' beta ')'
                // Return: "((" + Wff_to_Nnf(alpha) + "v" + Wff_to_Nnf(beta) ")"
                // + "&" + "(" "Wff_to_Nnf(neg_alpha)" + "v" + "Wff_to_Nnf(neg_beta)" "))"
                return "((" + Wff_to_Nnf(alpha) + "v" + Wff_to_Nnf(beta) + ")" 
                    + "&" + "(" + Wff_to_Nnf(nega_alpha) + "v" + Wff_to_Nnf(nega_beta) + "))";
            }

            // '~' '(' Wff '>' Wff ')'
            if (binary_conn == ">"){
                string alpha = Slice(wff, 2, binary_conn_index-1);
                string beta = Slice(wff, binary_conn_index+1, len_wff-2);
                string neg_beta = "~" + beta;

                // Input: "~(" + alpha + ">" + beta ")"
                // Return: "(" + Wff_to_Nnf(alpha) + "&" + Wff_to_Nnf(neg_beta) + ")"
                return "(" + Wff_to_Nnf(alpha) + "&" + Wff_to_Nnf(neg_beta) + ")";
            }
        }


        //'~' ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)
        if (Binary_Temp_conn_check(binary_conn)){
            tuple<int, int, int> interval_tuple = primary_interval(Slice(wff, 1, len_wff-1));
            // Add 1 to accomodate for sliced-off '~' 
            int start_interval = get<0>(interval_tuple) + 1;
            int end_interval = get<2>(interval_tuple) + 1;
            if (binary_conn == "U" or binary_conn == "R"){ 
                
                // Binary_Temp_conn -> ‘U’ | ‘R’
                // Switch 'U', 'R' with corresponding dual: 'R', 'U'
                if (binary_conn == "U"){
                    binary_conn = "R";
                }
                else if (binary_conn == "R"){
                    binary_conn = "U";
                } 

                string neg_alpha = "~" + Slice(wff, 2, binary_conn_index-1);
                string neg_beta = "~" + Slice(wff, end_interval+1, len_wff-2);

                // Input: "~(" + alpha + binary_conn + beta + ")"
                //Return: "(" + Wff_to_Nnf(neg_alpha) + dual(binary_conn) 
                //          + interval + Wff_to_Nnf(neg_beta) + ")"
                return "(" + Wff_to_Nnf(neg_alpha) + binary_conn
                    + Slice(wff, start_interval, end_interval) + Wff_to_Nnf(neg_beta) + ")";
            }
        }

        // '~' '(' Wff ')'
        if (Slice_char(wff, 1) == "(" and Slice_char(wff, len_wff-1) == ")"){
            string alpha = Slice(wff, 2, len_wff-2);
            return "(" + Wff_to_Nnf("~"+alpha) + ")";
        }
    }

    // '(' Wff ')'
    if (Slice_char(wff, 0) == "(" and Slice_char(wff, len_wff-1) == ")"){
        string alpha = Slice(wff, 1, len_wff-2);
        return "(" + Wff_to_Nnf(alpha) + ")";
    }

    else{
        string error_string = wff + " is not a well-formed formula, can't convert to nnf.\n";
        throw invalid_argument(error_string);
    }
    return "";
}
