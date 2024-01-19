#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "string_utils.h"
#include "string_grammar.h"
#include "string_nnf_grammar.h"
#include "string_reg.h"

vector<string> REST(vector<string> regexp);

vector<vector<int>> combinations(int n, int r);

vector<string> simplify_subsets(vector<string> regexp);

vector<string> REST_simplify(vector<string> regexp);

vector<string> REST_simplify_v2(vector<string> regexp); 
