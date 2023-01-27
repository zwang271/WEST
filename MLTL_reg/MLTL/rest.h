#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"

vector<string> REST(vector<string> regexp);

vector<vector<int>> combinations(int n, int r);

vector<string> simplify_subsets(vector<string> regexp);

vector<string> REST_simplify(vector<string> regexp);
