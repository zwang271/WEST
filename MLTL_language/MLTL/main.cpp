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

	int n = 2;
	string s = "(p0U[0,1](p0U[0,1]p1))";
	vector<string> v = reg(s, n);
	v = right_or(v, n);
	print(v);
	cout << endl; 
	print(simplify(v, n));

    return 0;
}