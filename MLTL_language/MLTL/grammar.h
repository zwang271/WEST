#pragma once
#include <string>
#include <tuple>
#include <stdexcept>

using namespace std;


string Slice(string s, int a, int b);
string Slice_char(string s, int i);
bool Digit_check(string s);
bool Num_check(string s);
bool Interval_check(string s);
bool Prop_var_check(string s);
bool Prop_cons_check(string s);
bool Unary_Prop_conn_check(string s);
bool Binary_Prop_conn_check(string s);
bool Assoc_Prop_conn_check(string s);
bool Wff_check(string s);
bool Array_entry_check(string s);
bool Unary_Temp_conn_check(string s);
bool Binary_Temp_conn_check(string s);
int primary_binary_conn(string wff);
tuple<int, int, int> primary_interval(string wff);
int Comp_len(string wff);
vector<tuple<string, vector<string>>> subformula_regex(string wff, int n);
void print_subformulas(vector< tuple<string, vector<string>> > formulas);
