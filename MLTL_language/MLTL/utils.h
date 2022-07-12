#pragma once
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

//NOTE: error_check that sum of right bounds must be less than computation bounds
string pad_to_length(string unpadded_s, int length, int n);
vector<string> pad(vector<string> unpadded_v, int n, int m = -1);
string strip_char(string s, char c);
vector<string> strip_commas(vector<string> comma_v);
vector<string> add_commas(vector<string> v, int n);
string string_intersect(string w_1, string w_2, int n);
vector<string> single_char_or(vector<string> V);
vector<string> join(vector<string> A, vector<string> B, int n, bool simp = true);
vector<string> list_str_concat_suffix(vector<string> V, string s);
vector<string> list_str_concat_prefix(vector<string> V, string s);
vector<string> left_expand(vector<string> v, int n, int iteration = 0);
vector<string> right_expand(vector<string> v, int n, int iteration = 0);
void print(vector<string> v);
string simplify_string(string s1, string s2);
vector<string> simplify(vector<string> v, int n);
void print_tree(vector<string> v, string pre_space = "");
string common_left_string(vector<string> v);
void print_all_representations(vector<string> v_actual, int n);
int sum_of_characters(vector<string>);
