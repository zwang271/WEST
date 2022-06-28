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

	string s = "F[0,3](p0vp1)";
	int n = 2;
	vector<string> v = pad(reg(s, n), n);

	print(v);

    return 0;
}