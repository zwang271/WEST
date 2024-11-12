#pragma once
#include <String2MLTL.h>
#include <MLTLFormula.h>
#include <queue>
#include <string>
#include <unordered_set>


class Translators {
/// @brief getAlphabets
/// @param MLTLFormula, f wrapped in unique pointer
/// @return the alphabet (set) of the formula, e.g., p U[1,2] q has {p,q} as
/// alphabet
public:
void getAlphabet(MLTLFormula* f, std::unordered_set<string> &out);

std::string mltl2PropSmvMaxSAT(const vector<MLTLFormula*> &vec);

std::string mltl2PropSmv(const unique_ptr<MLTLFormula> &f);

/// The below two functions are taken directly from Li et al.'s CAV2019
/// artifacts for comparison
/// I just modified it to include ite expressions as well.
/*
 * Generate an SMT-LIB v2 expression for the formula
 * k and len in the result string are fixed
 */
string smtlib_expr(const unique_ptr<MLTLFormula> &f, std::string &loc,
                   std::string &len, int &i) ;

std::string mltl2smtlib(const unique_ptr<MLTLFormula> &f);

// The slow boolean translation:

vector<std::string> getConstraints(MLTLFormula* f);

std::string mltl2PropSmvSlow(const unique_ptr<MLTLFormula> &f, int i);

std::string mltl2PropSmvS(const unique_ptr<MLTLFormula> &f);
Translators(){};
};