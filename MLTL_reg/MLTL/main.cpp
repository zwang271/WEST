#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include "rest.h"

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

			// Convert input to west grammar and read from file
			string python_parser = "python ./parser.py \"" + wff + "\"";
			system(python_parser.c_str());
			ifstream wff_file("./gui/west_wff.txt");
			if (wff_file.is_open()) {
				while (wff_file) {
					getline(wff_file, wff);
				}
			}
			wff_file.close();

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
		
		char simp_reply = 'q';
		bool simp_flag;

		char truth_table_reply = 'q';
        bool truth_table_flag;

        char rest_reply = 'q';
        bool rest_flag;
        

		// Ask user if they want to simplify output of reg
		while (simp_reply != 'y' and simp_reply != 'n') {
			cout << "Would you like to simplify output of reg? (y / n)" << endl;
			// Read in user input
			cin >> simp_reply;
			// Flush the input buffer
    		cin.ignore(numeric_limits<streamsize>::max(),'\n');

			// Turn on simp_flag
			if (simp_reply == 'y'){
				simp_flag = true;
			}
			// Turn off simp_flag
			else if (simp_reply == 'n'){
				simp_flag = false;
			}
			// User did not enter in 'y' or 'n'
			else{ 
				cout << "enter 'y' or 'n'" << endl;
			}
        }


		// Ask user if they want to print out recursive truth-table 
        while (truth_table_reply != 'y' and truth_table_reply != 'n') {
			cout << "Would you like to generate the truth table? (y / n)" << endl;
			// Read in user input
			cin >> truth_table_reply;
			// Flush the input buffer
    		cin.ignore(numeric_limits<streamsize>::max(),'\n');

			// Turn on truth_table_flag
			if (truth_table_reply == 'y'){
				truth_table_flag = true;
			}
			// Turn off truth_table_flag
			else if (truth_table_reply == 'n'){
				truth_table_flag = false;
			}
			// User did not enter in 'y' or 'n'
			else{ 
				cout << "enter 'y' or 'n'" << endl;
			}
        }

        while (rest_reply != 'y' and rest_reply != 'n') {
            cout << "Would you like to use rest? (y / n)" << endl;
            // Read in user input
            cin >> rest_reply;
            // Flush the input buffer
            cin.ignore(numeric_limits<streamsize>::max(),'\n');

            // Turn on rest_flag
            if (rest_reply == 'y'){
                rest_flag = true;
            }
                // Turn off rest_flag
            else if (rest_reply == 'n'){
                rest_flag = false;
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
        
        answer = reg(nnf, n, truth_table_flag, simp_flag);
		
		// User wants to output regexs of all
		// subformulas of input
		if (truth_table_flag) {
            print_subformulas(get_formulas(), n, nnf);
			clear_formulas();
        }

        // User wants to only output regex of input
		else {
            print(answer);
            cout << endl;
        }
        
		cout << "Finished computing." << endl;
		cout << "Size of vector: " << answer.size() << endl;
		cout << "Number of characters: " << sum_of_characters(answer) << endl;

        if (rest_flag) {
            cout << "\nWith REST:" << endl;
            vector<string> rest_answer = REST_simplify(answer);
            print(rest_answer);
        }
	}

    return 0;
}
