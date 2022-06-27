#pragma once
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

//NOTE: error_check that sum of right bounds must be less than computation bounds
string pad_to_length(string unpadded_s, int length, int n);
vector<string> pad(vector<string> unpadded_v, int n, int m);
string strip_char(string s, char c);
vector<string> strip_commas(vector<string> comma_v);
vector<string> add_commas(vector<string> v, int n);
string string_intersect(string w_1, string w_2, int n);
vector<int> right_or_aux(vector<string> v, int n);
vector<string> single_char_or(vector<string> V);
vector<string> join(vector<string> A, vector<string> B);
vector<string> list_str_concat_suffix(vector<string> V, string s);
vector<string> list_str_concat_prefix(vector<string> V, string s);
vector<string> right_or(vector<string> v, int iteration, vector<int> indices, int n);
void print(vector<string> v);
