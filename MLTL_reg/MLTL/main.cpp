#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include "simulation.h"

using namespace std;

int main() {
	/*string wff_test = "((F[0:3]p0 > F[0:3]p1) > F[0:3](p0 > p1))";
	wff_test = strip_char(wff_test, ' ');
	vector<string> wff_expand = expand(reg(wff_test, 2));
	cout << wff_expand.size() << endl; */

	vector<string> v = { "s,s,1,1", "s,s,1,0", "s,s,0,1", "s,s,0,0" };
	print(simplify(v, 1));

	bool running = true;
	vector<string> answer;
	vector<string> display;
	string wff = "";

	while (running) {
		bool is_valid = false;
		
		while (!is_valid) {
			cout << "Please enter a MLTL formula." << endl;
			// Write user input to wff variable
			// Since using getline, don't need to flush input buffer
			getline(cin, wff);
			// Strip whitespace from input
			wff = strip_char(wff, ' ');
			if (Wff_check(wff)) {
				// Exit user input loop
				is_valid = true;
			}
			else {
				cout << "Not a well formed formula!" << endl;
			}
		}

		//cout << "Please enter number of propositional variables." << endl;
		int n = get_n(wff);
		// while (n < 0) {
		// 	string in;
		// 	getline(cin, in);
		// 	n = stoi(in);
		// 	if (n < 0) {
		// 		cout << "n must be a positive integer." << endl;
		// 	}
		// }
        
        bool subformulas = false;
        char y_or_n = 'q';
        while (y_or_n != 'y' and y_or_n != 'n') {
			cout << "Would you like to generate the truth table? (y / n)" << endl;
			// Read in user input
			cin >> y_or_n;
			// Flush the input buffer
    		cin.ignore(numeric_limits<streamsize>::max(),'\n');

			// Turn on subformulas regex flag
			if (y_or_n == 'y'){
				subformulas = true;
			}
			// Turn off subformulas regex flag
			else if (y_or_n == 'n'){
				subformulas = false;
			}
			// User did not enter in 'y' or 'n'
			else{ 
				cout << "enter 'y' or 'n'" << endl;
			}
        }

		// Convert input to Nnf form
		string nnf = Wff_to_Nnf_clean(wff);
		cout << "NNF Formula: " << nnf << endl;
        cout << endl;
        
        // User wants to output regexs of all
		// subformulas of input
		if (subformulas) {
            answer = reg_sub(nnf, n);
            answer = simplify(answer, n);
            print_subformulas(get_formulas(), n, nnf);
			clear_formulas();
        }

        //User wants to only output regex of input
		else {
            answer = reg(nnf, n);
            answer = simplify(answer, n);
            print(answer);
            cout << endl;
        }
        
		cout << "Finished computing." << endl;
		cout << "Size of vector: " << answer.size() << endl;
		cout << "Number of characters: " << sum_of_characters(answer) << endl;
	}

    return 0;
}
