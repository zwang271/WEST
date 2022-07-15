#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"

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


vector<string> generate_test(int depth, int n, int a = 0, int b = 2) {
	if (depth == 0) {
		vector<string> test_d0 = {};
		
		string prop_var = "p" + to_string(rand() % n);
		if (rand() % 2 == 0) {
			test_d0.push_back(prop_var);
		}
		else {
			test_d0.push_back("~" + prop_var);
		}

		if (rand() % 2 == 0) {
			test_d0.push_back("T");
		} 
		else {
			test_d0.push_back("!");
		}

		return test_d0;
	}

	vector<string> test = {};



	return test;
}


int main() {

	string out = "./verify/reg1.txt";
	srand(time(NULL));

	/*vector<string> v = { "1ss", "s1s", "ss1" };
	vector<string> expanded = expand(v);
	print(expanded);
	write_to_file(expanded, out);*/

	print(generate_test(0, 4));

	return 0;
}