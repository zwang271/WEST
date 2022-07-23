#include <string>
#include <iostream>
#include <vector>
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include "utils.h"

using namespace std;

int main() {
	vector<string> v = { "1,0,1", "s,1,s", "1,s,0", "0,s,s" };
	cout << check_simp(v) << endl; 

	vector<string> expand_v = expand(v);
	cout << expand_v.size() << endl;

	print(simplify(v, 1));

	cout << endl; 
	string wff = "((p0U[1:1](p0U[0:1]p1))=(p0U[1:2]p1))";
	print(reg(wff, 2));

	return 0;
}