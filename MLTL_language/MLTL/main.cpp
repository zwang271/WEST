#include <iostream>
#include <vector>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include "simulation.h"

using namespace std;

int main() {
	vector<string> v1 = { "100,s11,ss0", "100,s1s,ss1", "s10,ss1,sss",
							 "s10,100,s11", "s10,s10,ss1", "ss1,sss,sss"};
	print(v1);
	cout << endl;
	v1 = pad(v1, 3);
	print(v1);
	cout << endl;
	v1 = strip_commas(v1);
	print(v1);
	cout << endl;

	print_tree(v1);

    return 0;
}