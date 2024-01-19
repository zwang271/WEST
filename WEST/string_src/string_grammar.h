#pragma once
#include <string>
#include <tuple>
#include <stdexcept>
#include <vector>

using namespace std;


/*
 * Takes substring of given string from a to b.
 */
string Slice(string s, int a, int b);


/*
 * Returns length 1 string at index i.
 */
string Slice_char(string s, int i);


/*
 * Digit  ->  ‘0’ | ‘1’ | … |’9’
 * Checks that the inputted string is a digit.
 */
bool Digit_check(string s);


/*
 * Num  ->  Digit Num |  Digit
 * Checks that the inputted string is of length 1 and then runs digit_check().
 */
bool Num_check(string s);


/*
 * Interval  ->  ‘[’  Num ‘,’ Num ‘]’
 * Checks that the inputted string is of the form of an interval.
 */
bool Interval_check(string s);


/*
 * Prop_var  ->  ‘p’ Num
 * Checks that the inputted string is a propositional variable.
 */
bool Prop_var_check(string s);


/*
 * Prop_cons  ->  ‘T’ | ‘!’
 * Checks that the inputted string is a propositional constant.
 */
bool Prop_cons_check(string s);


/*
 * Unary_Prop_conn  ->  ‘~’
 * Checks that the inputted string is the negation symbol (the unary prop. connective).
 */
bool Unary_Prop_conn_check(string s);


/*
 * Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’
 * Checks that the inputted string is a binary prop. connective (or, and, equivalence, implication).
 */
bool Binary_Prop_conn_check(string s);


/*
 * Assoc_Prop_conn -> ‘v’ | ‘&’ | ‘=’
 * Checks that the inputted string is an associative prop. connective (or, and, equivalence).
 */
bool Assoc_Prop_conn_check(string s);


/*
 * Array_entry -> Wff ‘,’ Array_entry  |  Wff
 * Checks that the inputted string is an array of WFFs.
 * We use an array of WFFs, for example, when ANDing >2 formulas.
 */
bool Array_entry_check(string s);


/*
 * Unary_Temp_conn  ->  ‘F’ | ‘G’
 * Checks that the inputted string is F or G (the unary temporal connectives).
 */
bool Unary_Temp_conn_check(string s);


/*
 * Binary_Temp_conn  ->  ‘U’ | ‘R’
 * Checks that the inputted string is U or R (the binary temporal connectives).
 */
bool Binary_Temp_conn_check(string s);


/*
 *  Wff ->  Prop_var | Prop_cons
 *                  | Unary_Prop_conn Wff
 *                  | Unary_Temp_conn  Interval  Wff
 *                  | '(' Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ')'
 *                  | ‘(‘ Wff Binary_Prop_conn Wff ‘)’
 *                  | ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)
 *  Checks that an inputted string is a WFF.
 */
bool Wff_check(string s);


/*
 * Returns the index of the primary binary connective of a WFF.
 */
int primary_binary_conn(string wff);


/*
 * Returns the indices where the primary interval is in a given WFF.
 */
tuple<int, int, int> primary_interval(string wff);


/*
 * Determines the minimum computation length needed for a given WFF to not have out-of-bounds behavior.
 */
int Comp_len(string wff);