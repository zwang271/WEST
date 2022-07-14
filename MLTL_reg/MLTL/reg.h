#pragma once
#include "utils.h"
#include <vector>

/*
 * Input: Two vectors of computation strings V1 and V2, comma separated
 * Output: Computes pairwise string_intersect between all
 *		  computation strings in V1 and V2
 */
vector<string> set_intersect(vector<string> v1, vector<string> v2, int n, bool simp = false);


/*
 * Input: Vectors A and B of computation strings
 * Output: Vector A concatenated with B
 */
vector<string> join(vector<string> A, vector<string> B, int n, bool simp = false);


/*
 * Prop_cons  ->  'T' | '!'
 * Input: mLTL formula in NNF form (string)
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying "T" or "!"
 */
vector<string> reg_prop_cons(string s, int n);


/*
 * Prop_var -> 'p' Num | '~' 'p' Num
 * Input: mLTL formula in NNF form (string)
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying a prop. var.
 */
vector<string> reg_prop_var(string s, int n);


/*
 * Input: vector reg_alpha of computation strings satisfying MLTL NNF formula alpha
 *      a is lower bound of interval
 *      b is upper bound of interval
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying F[a,b]alpha
 */
vector<string> reg_F(vector<string> alpha, int a, int b, int n);


/*
 * Input: vector reg_alpha of computation strings satisfying mLTL NNF formula alpha
 *      a is lower bound of interval
 *      b is upper bound of interval
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying G[a,b]alpha
 */
vector<string> reg_G(vector<string> alpha, int a, int b, int n);


/*
 * Input: vector reg_alpha of computation strings satisfying mLTL NNF formula alpha
 *      vector reg_beta of comp. strings satisfying mLTL NNF formula beta
 *      a is lower bound of interval
 *      b is upper bound of interval
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying alphaU[a,b]beta
 */
vector<string> reg_U(vector<string> reg_alpha, vector<string> reg_beta, int a, int b, int n);


/*
 * Input: vector reg_alpha of computation strings satisfying mLTL NNF formula alpha
 *      vector reg_beta of comp. strings satisfying mLTL NNF formula beta
 *      a is lower bound of interval
 *      b is upper bound of interval
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying alphaR[a,b]beta
 */
vector<string> reg_R(vector<string> alpha, vector<string> beta, int a, int b, int n);


/*
 * Nnf ->  ?(~) Prop_var | Prop_cons
 *	                     | Unary_Temp_conn  Interval  Nnf
 *
 *	                     | '(' Assoc_Prop_conn '['  Nnf_Array_entry  ']' ')'
 *                       | '(' Nnf Binary_Prop_conn Nnf ')'
 *                       | '(' Nnf Binary_Temp_conn  Interval Nnf ')'
 * Input: mLTL formula in NNF (string)
 *     n is number of propositional variables
 * Output: Vector of computation strings satisfying the formula
 */
vector<string> reg(string s, int n);



bool find_formula(vector<tuple<string, vector<string>>> v, string s);

vector<tuple<string, vector<string>>> get_formulas();

void clear_formulas();

void push_back_formulas(string s, vector<string> v, int n);

//vector<string> reg_subformulas(string nnf, int n);

vector<string> reg_sub(string nnf, int n, bool sub = true);
