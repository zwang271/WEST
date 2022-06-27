#include <string>
#include <tuple>
#include <stdexcept>
#include <iostream>
#include "grammar.h"
#include "nnf_grammar.h"

/*
Context-Free Grammar for a MLTL wff in Negation normal-form (Nnf).
Here, ‘Eventually’, ‘Always’, ‘Until’, and ‘Release’ are represented by the letters ‘F’, ‘G’, ‘U’, and ‘R’.

Alphabet = { ‘0’, ‘1’, …, ‘9’, ‘p’, ‘(‘, ‘)’, ‘[’, ‘]’, ‘,’ ,
                       ‘T’, ‘F’,                
                       ‘~’, ‘F’, ‘G’,
                       ‘v’, ‘&’, ‘=’, ‘>’, ‘U’, ‘R’ }

Digit  ->  ‘0’ | ‘1’ | … |’9’
Num  ->  Digit Num |  Digit
Interval  ->  ‘[’  Num ‘,’ Num ‘]’  
Prop_var  ->  ‘p’ Num

Prop_cons  ->  ‘T’ | ‘F’
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


// Nnf_Array_entry -> Nnf ‘,’ Nnf_Array_entry  |  Nnf
bool Nnf_Array_entry_check(string s){
    int len_s = s.length();

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


// Nnf ->   ?('~') Prop_var | Prop_cons
//	                      | Unary_Temp_conn  Interval  Nnf
//
//	                      | '(' Assoc_Prop_conn ‘[‘ Nnf_Array_entry ‘]’ ')'
//                        | ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’
//                        | ‘(‘ Nnf Binary_Temp_conn  Interval Nnf  ‘)’
bool Nnf_check(string s){
    int len_s = s.length();

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

    return false;
}