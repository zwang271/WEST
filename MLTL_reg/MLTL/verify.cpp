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
* Writes all elements of v to out, one item per line
*/
void write_to_file(vector<string> v, string out) {
	string line;
	ofstream outfile;
	outfile.open(out);

	for (string w : v) {
		outfile << w << endl;
	}
}


/*
* Converts n to a binary string
*/
string binary(int n) {
	string b = "";

	if (n == 0) {
		return "0";
	}

	while (n > 0) {
		b = to_string(n % 2) + b;
		n = int(n / 2);
	}

	return b;
}


/*
* Return a vector representing the expansion of w into bit strings
*/
vector<string> expand_string(string w) {
	vector<string> v = {};
	vector<int> indices = {};

	for (int i = 0; i < w.length(); i++) {
		if (w[i] == 's') {
			indices.push_back(i);
		}
	}

	for (int i = 0; i < pow(2, indices.size()); i++) {
		string b = binary(i); 
		b = string(indices.size() - b.length(), '0') + b;

		string w_copy = w;
		for (int j = 0; j < indices.size(); j++) {
			w_copy[indices[j]] = b[j];
		}

		v.push_back(w_copy);
	}


	return v;
}


/*
* Removes duplicate entries from a vector.
* Mutates vector.
*/
template <typename T>
void remove_duplicates(vector<T>* reg_alpha) {
	// Convert vector to a set
	set<T> s((*reg_alpha).begin(), (*reg_alpha).end());
	// Assign set back to vector
	(*reg_alpha).assign(s.begin(), s.end());

	return;
}


/*
* Expand out all s-strings in v
*/
vector<string> expand(vector<string> v) {
	vector<string> expanded = {};

	for (string w : v) {
		expanded = join(expanded, expand_string(w), 0, false);
	}

	remove_duplicates(&expanded);

	return expanded;
}


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

	string formulas_file = "./verify/formulas/formulas.txt";
	srand(time(NULL));

	vector<string> test = generate_test(2, 4, 0, 2, false);
	
	for (string wff : test) {
		if (!Wff_check(wff)) {
			cout << wff << " failed wff check" << endl; 
		}
	}

	write_to_file(test, formulas_file);


	/*vector<string> v = { "1ss", "s1s", "ss1" };
	vector<string> expanded = expand(v);
	print(expanded);
	write_to_file(expanded, out);*/

	return 0;
}