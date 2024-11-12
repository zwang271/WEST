#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include <cmath>
#include <ctime>


/*
* Generates test_suite template
* a - prop_var or negation prop_var
* b - prop_cons (T or !)
* c - prop_var
*/
vector<string> generate_test_template(int depth, int a = 0, int b = 2, bool large = false) {
	if (depth == 0) {
		vector<string> test_d0 = {};
		
		if (large) {
			test_d0.push_back("c");
			test_d0.push_back("~c");
			test_d0.push_back("T");
			test_d0.push_back("!");
		}
		else {
			test_d0.push_back("a");
			test_d0.push_back("b");
		}
		return test_d0;
	}

	vector<string> test = {};
	vector<string> v = generate_test_template(depth - 1, a, b, large);
	string interval = "[" + to_string(a) + ":" + to_string(b) + "]";

	for (string alpha_1 : v) {
		test.push_back("G" + interval + alpha_1);
		test.push_back("F" + interval + alpha_1);

		vector<string> w = generate_test_template(depth - 1, a, b, large);
		for (string alpha_2 : w) {
			test.push_back("(" + alpha_1 + "R" + interval + alpha_2 + ")");
			test.push_back("(" + alpha_1 + "U" + interval + alpha_2 + ")");
			test.push_back("(" + alpha_1 + "v" + alpha_2 + ")");
			test.push_back("(" + alpha_1 + "&" + alpha_2 + ")");
		}
	}

	return test;
}


/*
* Negation flag set to true: returns a random propositional literal
* Negation flag set to false: returns a random propositional variable p_num
*/
string rand_prop_var(int n, bool negation) {
	string prop_var = "p" + to_string(rand() % n);
	if (negation) {
		if (rand() % 2 == 0) {
			return prop_var;
		}
		else {
			return "~" + prop_var;
		}
	}
	else {
		return prop_var;
	}
}


/*
* Returns a random propositional constant
*/
string rand_prop_cons() {
	if (rand() % 2 == 0) {
		return "T";
	}
	else {
		return "!";
	}
}


/*
* Fills in a test template T with random: propositional variables, propositional constants,
* and propositional literals
*/
vector<string> generate_test(int depth, int n, int a = 0, int b = 2, bool large = false) {
	vector<string> T = generate_test_template(depth, a, b, large);

	for (int i = 0; i < T.size(); i++) {
		string w = "";
		for (int j = 0; j < T[i].length(); j++) {
			// Fill-in random propositional literal into T
			if (T[i][j] == 'a') {
				w = w + rand_prop_var(n, true);
			}
			// Fill-in random propositional constant into T 
			else if (T[i][j] == 'b') {
				w = w + rand_prop_cons();
			}
			// Fill-in random propositional variable into T
			else if (T[i][j] == 'c') {
				w = w + rand_prop_var(n, false);
			}
			// Leave character as is
			else {
				w = w + T[i][j];
			}
		}
		T[i] = w;
	}

	// Return filled-in test template
	return T;
}


int main(){
    int n = 4;
	int depth = 2;
	int a = 0; 
	int b = 2;
	bool large = false;

	string formulas_path = "./verify/formulas_d" + to_string(depth) + ".txt";
	string verify_reg = "./verify/reg_outputs_d" + to_string(depth) + "/";
	string verify_brute_force = "./verify/brute_force_outputs_d" + to_string(depth) + "/";

	// Seed rand() with current time
	srand(time(NULL));
	vector<string> test = generate_test(depth, n, a, b, large);
	// Check for non-well formed strings in test
	for (string wff : test) {
		if (!Wff_check(wff)) {
			cout << wff << " failed wff check" << endl; 
		}
	}
	write_to_file(test, formulas_path, false);
	cout << "Formulas written to " + formulas_path << endl; 

	return 0;
}