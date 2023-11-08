#pragma once
#include <string>
#include <vector>
#include "utils.h"

using namespace std;

/*
 * Input: MLTL formula F
 *        trace T
 * Output: true if and only if F evaluates to true on F
 */
bool evaluate_mltl(string F, vector<string> T, bool verbose=false);