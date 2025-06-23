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

using namespace std;


/*
* Generates test_suite template
* a - prop_var or negation prop_var
* b - prop_cons (T or !)
* c - prop_var
*/
vector<string> generate_test_template(int depth, int n, int a = 0, int b = 2, bool large = false) {
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
	vector<string> v = generate_test_template(depth - 1, n, a, b, large);
	string interval = "[" + to_string(a) + ":" + to_string(b) + "]";

	for (string alpha_1 : v) {
		test.push_back("G" + interval + alpha_1);
		test.push_back("F" + interval + alpha_1);

		vector<string> w = generate_test_template(depth - 1, n, a, b, large);
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
* Returns a random propositional variable p_num
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


vector<string> generate_test(int depth, int n, int a = 0, int b = 2, bool large = false) {
	vector<string> T = generate_test_template(depth, n, a, b, large);

	for (int i = 0; i < T.size(); i++) {
		string w = "";
		for (int j = 0; j < T[i].length(); j++) {
			if (T[i][j] == 'a') {
				w = w + rand_prop_var(n, true);
			}
			else if (T[i][j] == 'b') {
				w = w + rand_prop_cons();
			}
			else if (T[i][j] == 'c') {
				w = w + rand_prop_var(n, false);
			}
			else {
				w = w + T[i][j];
			}
		}
		T[i] = w;
	}

	return T;
}


int main() {
	string formulas_file; 

	int n;
	//cout << "Enter number of propositional variables" << endl;
	//cin >> n;

	char gen_formulas;
	cout << "Generate new formulas? (y/n)" << endl; 
	cin >> gen_formulas;
	if (gen_formulas == 'y') {
		int a = 0;
		int b = 2;
		int depth;
		bool large = false;

		cout << "Enter depth of formula to be generated" << endl; 
		cin >> depth;
		n = pow(2, depth);

		cout << "Enter name of file to write formulas to" << endl; 
		cin >> formulas_file;

		srand(time(NULL));
		vector<string> test = generate_test(depth, n, a, b, large);
		for (string wff : test) {
			if (!Wff_check(wff)) {
				cout << wff << " failed wff check" << endl;
			}
		}
		write_to_file(test, formulas_file, false);
		cout << "Formulas written to " + formulas_file << endl;
	}
	else {
		cout << "Enter number of propositional variables" << endl;
		cout << "This should be equal to 2^(depth of formulas to test)" << endl;
		cin >> n;

		string verify_reg = "./verify/reg_outputs_d1/";
		string verify_brute_force = "./verify/brute_force_outputs_d1/";

		cout << "Enter folder to write reg expanded ouputs to" << endl;
		cin >> verify_reg;
		cout << "Enter folder containing brute force outputs" << endl;
		cin >> verify_brute_force;

		ifstream formulas;
		formulas.open(formulas_file);
		int formula_count = 0;
		while (!formulas.eof()) {
			string wff;
			getline(formulas, wff);

			if (wff == "") {
				break;
			}

			vector<string> reg_wff = reg(wff, n, false, true);
			cout << "formula: " << wff << endl;
			print(reg_wff);
			int cp = Comp_len(wff);
			reg_wff = pad(reg_wff, n, cp * (n + 1) - 1);

			reg_wff = expand(reg_wff);
			cout << "wrote to" << verify_reg + to_string(formula_count) + ".txt" << endl;
			cout << "comp length: " << cp << endl;
			write_to_file(reg_wff, verify_reg + to_string(formula_count) + ".txt");
			formula_count++;
		}

		cout << endl << endl << "checking output files" << endl;
		cout << "======================================" << endl;

		//int formula_count = 2;
		for (int i = 0; i < formula_count; i++) {
			string f1 = verify_brute_force + to_string(i) + ".txt";
			string f2 = verify_reg + to_string(i) + ".txt";
			compare_files(f1, f2);
		}
	}

	return 0;
}
