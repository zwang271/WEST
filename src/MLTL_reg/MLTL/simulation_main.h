#pragma once
#include <random>
#include <string>
#include <iostream>
#include "utils.h"
#include "reg.h"


/*
* Generate a random MLTL formula with a iteration depth
* of MAX_ITER.
* For ex: 'p2' is iteration depth 0, '(p0 & p1)' is iteration depth 1,
* '(((p0 & p1) U [5:10] (p2 R [5:10] p3))' is iteration depth 2, etc.
*/
string rand_function(int iter);


/*
* Writes FUNC_NUM number of random MLTL
* formulas of iterative depth MAX_ITER to the file "formulas"
*/ 
void run_rand_function(string formulas);


/*
* For each MLTL formula alpha in the 'formulas' file, simulate will:
*   1. Calculate the regex for alpha in 'output' variable
*   2. Simplify the regex for alpha using simplify() function (from utils.cpp file)
*      and saves this to 'output' variable
*   3. Writes the amount of time taken to calculate 'output' as-well-as
*      number of characters in 'output' to 'out' file   
*/
void simulate(string formulas, string out);


/*
* Driver function for simulation_main.cpp file
*/
int main();