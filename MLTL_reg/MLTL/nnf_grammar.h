#pragma once
#include <string>
#include <tuple>
#include <stdexcept>

using namespace std;

bool Nnf_Array_entry_check(string s);
bool Nnf_check(string s);

// Given an Nnf-formula nnf, returns the regex
// for the language of nnf.
// For preformance reasons, reg_clean will return a
// NOT NECESSARILY DISJOINT Union.
// To obtain a disjoing union from reg_clean's output,
// use right_or.
//
// Cleaner implementation of reg
// in case original is faulty.
string Wff_to_Nnf_clean(string wff);