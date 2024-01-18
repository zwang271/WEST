#pragma once
#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "parser.h"
#include "reg.h"

using namespace std;


/*
 * Input: string S
 *		 char C
 * Output: S with every instance of C removed
 */
string strip_char(string s, char c);


/*
Returns the (largest prop var index) + 1 in wff
*/
int get_n(string wff);


/*
Converts a well formed formula (wff) to negation normal form (nnf)
Input: wff (string)
Output: nnf (string)
*/
string wff_to_nnf(string wff);

/*
Computes the computation length of a well formed formula
*/
int complen(string wff);


/*
converts bitset to regular expression
*/
string bitset_to_reg(bitset<MAXBITS> b, int bits_needed);
vector<string> bitset_to_reg(vector<bitset<MAXBITS>> B, int bits_needed);
/*
Converts a bitset to string using the following scheme, one pair of bits at a time
11 -> "s"
10 -> "1"
01 -> "0"
00 detected within input immediately returns ""
*/
string bitset_to_string(bitset<MAXBITS> b);
vector<string> bitset_to_string(vector<bitset<MAXBITS>> B); 

/*
converts string to bitset
*/
bitset<MAXBITS> stb(string s);


/*
Checks if a bitset contains 00
*/
bool is_null(bitset<MAXBITS> b);


/*
Shifts a bitset to the left by m bits, padding with 1s
*/
bitset<MAXBITS> shift(bitset<MAXBITS> b, int m);
vector<bitset<MAXBITS>> shift(vector<bitset<MAXBITS>> b, int m);

/*
Prints the argument on a separate line,
or each item in a vector in a separate line
*/
void print(string s);
void print(bitset<MAXBITS> b);
void print(vector<string> S);
void print(vector<bitset<MAXBITS>> B);
