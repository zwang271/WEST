#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"

using namespace std;

int main() {
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

		// Get number of prop_vars in wff
		int n = get_n(wff);
		cout << "get_n outputs: " << n << endl;
		
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
		string nnf = Wff_to_Nnf(wff);
		cout << "NNF Formula: " << nnf << endl;
        cout << endl;
        
        // User wants to output regexs of all
		// subformulas of input
		if (subformulas) {
            answer = reg(nnf, n, true, true);
            answer = simplify(answer, n);
            print_subformulas(get_formulas(), n, nnf);
			clear_formulas();
        }

        //User wants to only output regex of input
		else {
            answer = reg(nnf, n, false, true);
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
