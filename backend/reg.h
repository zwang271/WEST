// Author: Zili Wang
// Last updated: 01/19/2024
// Core WEST functions

# pragma once
#include <vector>
#include <tuple>
#include <bitset>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#define MAXBITS 4096

#include "utils.h"

using namespace std;


/*
Checks if b1 and b2 can be simplified by bitwise or
*/
bool check_simp(bitset<MAXBITS> b1, bitset<MAXBITS> b2);


/*
Simplifies a vector of bitsets by bitwise or
*/
vector<bitset<MAXBITS>> simplify(vector<bitset<MAXBITS>> B);


/*
Computes or of two vectors of bitsets
*/
vector<bitset<MAXBITS>> or_vec(vector<bitset<MAXBITS>> B1, vector<bitset<MAXBITS>> B2);


/*
computes and of two vectors of bitsets
*/
vector<bitset<MAXBITS>> and_vec(vector<bitset<MAXBITS>> B1, vector<bitset<MAXBITS>> B2);


/*
nnf ->  literal 
        | unary_temp_conn  interval  nnf
        | '(' Nnf Binary_Prop_conn Nnf ')'
        | '(' Nnf Binary_Temp_conn  Interval Nnf ')'
        | '(' nnf ')'
Input: MLTL formula in NNF (string)
       n is number of propositional variables
Output: Vector of computation strings satisfying the formula
*/
vector<bitset<MAXBITS>> reg(string nnf, int n);


/*
Recompiles bit optimized binary and runs the executable
*/
void recompile(string wff);


/*
Returns a vector of tuples of formulas and their corresponding regular expressions
*/
vector<tuple<string, vector<bitset<MAXBITS>>>> get_formulas();