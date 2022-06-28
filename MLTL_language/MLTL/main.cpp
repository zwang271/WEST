#include <iostream>
#include <vector>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"

using namespace std;

int main() {
	// string s = "F[2,4]p1";
	// string s = "F[2,4]~p1";
	string s = "(p0R[1,3]p1)";
	int n = 2;

	vector<string> v = {"ss,ss,ss,s1", "s1"};
	print(simplify(v, 2));

    return 0;
}
