#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
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
        
        bool subformulas = false;
        char y_or_n = 'q';
        cout << "would you like to generate the truth table? (y / n)" << endl;
        while (y_or_n == 'q') {
               cin >> y_or_n;
               if (y_or_n == 'y') subformulas = true;
               else if (y_or_n == 'n') subformulas = false;
               else cout << "enter 'y' or 'n'";
        }

		// G[0:5] (p0 R[3:5] (p1 & p2))

		string nnf = Wff_to_Nnf_clean(wff);
		cout << "NNF Formula: " << nnf << endl;
		cout << endl << "NNF Check: " << Nnf_check(nnf) << endl << endl;
        
        if (subformulas) {
            answer = reg_subformulas(nnf, n);
            answer = simplify(answer, n);
            push_back_formulas(nnf, reg_subformulas(nnf, n));
            print_subformulas(get_formulas(), n);
        }

        else {
            answer = reg(nnf, n);
            answer = simplify(answer, n);
            print(answer);
            cout << endl;
        }
        
        
		cout << "Finished computing." << endl;
		cout << "Size of vector: " << answer.size() << endl;
		cout << "Number of characters: " << sum_of_characters(answer) << endl;
		/*print_all_representations(answer, n);*/
		//print(simplify(answer, n));
		cin.get();
	}

    return 0;
}
