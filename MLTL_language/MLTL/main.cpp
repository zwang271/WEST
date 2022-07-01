#include <iostream>
#include <vector>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include "simulation.h"

using namespace std;

int main() {
	vector<string> v1 = {"ss0,s1s,000", "ss1,s1s"};
	vector<string> v2 = {"000,s1s,0ss", "sss,s1s,1ss"};
	print(simplify(left_or(v2, 3), 3));
	cout << endl;
	print(simplify(left_or(v2, 3), 3));

    return 0;
}