#pragma once
#include "utils.h"

vector<string> set_intersect(vector<string> v1, vector<string> v2, int n, bool simp = true);
vector<string> set_union(vector<string> v1, vector<string> v2, int n);
vector<string> reg_prop_cons(string s, int n);
vector<string> reg_prop_var(string s, int n);
vector<string> reg_F(vector<string> alpha, int a, int b, int n);
vector<string> reg_G(vector<string> alpha, int a, int b, int n);

vector<string> reg(string s, int n);
vector<string> reg_U(vector<string> reg_alpha, vector<string> reg_beta, int a, int b, int n);
vector<string> reg_R(vector<string> alpha, vector<string> beta, int a, int b, int n);
// Cleaner implementation of reg
// in case original is faulty
vector<string> reg_clean(string s, int n);
