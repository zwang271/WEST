#pragma once
#include <random>
#include <string>
#include <iostream>
#include "utils.h"
#include "reg.h"


/*
* Runs right_or on {"0, s, 0, s, ...", "s, 0, s, 0, ..."}
*/
void right_or_PT1(int iterations);
void run_rand_function(string formulas);
void simulate(string formulas, string out);
int main();