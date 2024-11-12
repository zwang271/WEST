#pragma once
#include <string>
#include <tuple>
#include <stdexcept>

using namespace std;

/*
 * Nnf_Array_entry -> Nnf ‘,’ Nnf_Array_entry  |  Nnf
 * Checks that an inputted string is an array of formulas in NNF.
 */
bool Nnf_Array_entry_check(string s);


/*
 *  Nnf ->   ?('~') Prop_var | Prop_cons
 *          | Unary_Temp_conn  Interval  Nnf
 *          | '(' Assoc_Prop_conn ‘[‘ Nnf_Array_entry ‘]’ ')'
 *          | ‘(‘ Nnf Binary_Prop_conn Nnf ‘)’
 *          | ‘(‘ Nnf Binary_Temp_conn  Interval Nnf  ‘)’
 * Checks that the inputted string is a WWF in NNF.
 */
bool Nnf_check(string s);


/*
 * Converts a WFF to its equivalent NNF.
 */
string Wff_to_Nnf(string wff);