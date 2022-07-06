#include <iostream>
#include <vector>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include "simulation.h"

using namespace std;

int main() {
	
	bool is_valid = false;
	bool running = true;
	string wff;
	vector<string> reg;

	while (running) {
		while (!is_valid) {
			getline(cin, wff);
			if (Wff_check(wff)) {
				is_valid = true;
			}
			else {
				cout << "Not a well formed formula!" << endl;
			}
		}

		reg = reg(wff);

	}

    return 0;
}