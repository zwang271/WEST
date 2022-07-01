#include <iostream>
#include <vector>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include "simulation.h"

using namespace std;

int main() {
	/*string s = "F[2,4]p1";
	string s = "F[2,4]~p1";*/

	/*int n = 2;
	vector<string> v = {"s1", "1s,s1", "1s,s1", "1s,11,s1", "1s,1s,s1"};
	v = pad(v, n);
	print(simplify(v, n));*/

	// int n = 2;
	// string s = "(p0U[0,1](p0U[0,1]p1))";
	// vector<string> v = reg(s, n);
	// v = right_or(v, n);
	// print(v);
	// cout << endl; 
	// print(simplify(v, n));

	string a = "( (v[G[1,3]p1, (G[1,1]p0 & G[1,1]p1), (G[2,2]p0 & G[1,2]p1), (G[3,3]p0 & G[1,3]p1)])";
	string b = "= (&[(F[1,0]p0 v G[1,1]p1), (F[1,1]p0 v G[2,2]p1), (F[1,3]p0 v G[3,3]p1)]) )";
	string s1 = strip_char(a + b, ' ');
	cout << s1 << endl;
	cout << "Wff_check: " << Wff_check(s1) << endl;
	cout << "Nnf_check: " << Nnf_check(s1) << endl;

	vector<string> reg_s1 = reg(s1, 2);
	print_all_representations(reg_s1, 2);

	cout << "Comp_len(s1)" << endl; 

    return 0;
}