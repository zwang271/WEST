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
	vector<string> answer;
	vector<string> display;
	string wff;

	while (running) {
		cout << "Please enter a MLTL formula." << endl;
		wff = ""; 
		is_valid = false;
		while (!is_valid) {
			getline(cin, wff);
			wff = strip_char(wff, ' ');
			if (Wff_check(wff)) {
				is_valid = true;
			}
			else {
				cout << "Not a well formed formula!" << endl;
			}
		}
		cout << "Please enter number of propositional variables." << endl;
		int n = -1;
		while (n < 0) {
			string in;
			getline(cin, in);
			n = stoi(in);
			if (n < 0) {
				cout << "n must be a positive integer." << endl;
			}
		}
		string nnf = Wff_to_Nnf_clean(wff);
		cout << endl << Nnf_check(nnf) << "\t" << nnf << endl << endl;
		answer = reg(nnf, n);
		print(answer);
		cout << "Finished computing." << answer.size() << endl;
		/*print_all_representations(answer, n);*/
		//print(simplify(answer, n));
		cin.get();
	}

    return 0;
}