#include <stdlib.h>
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


int main(int argc, char** argv) {
	if (argc <= 2) { return 0; }

	std::vector<std::string> argList(argv + 1, argv + argc);
	string func = argList[0];
	string wff = strip_char(argList[1], ' ');

	if (func == "Wff_check") {
		auto output = Wff_check(wff); 
		ofstream outfile("./gui/Wff_check.txt"); 
		outfile << output;  
		outfile.close(); 
	}

	else if (func == "get_n") {
		auto output = get_n(wff); 
		ofstream outfile("./gui/get_n.txt"); 
		outfile << output;
		outfile.close();
	}

	else if (func == "Wff_to_Nnf") {
		auto output = Wff_to_Nnf(wff);
		ofstream outfile("./gui/Wff_to_Nnf.txt");
		outfile << output;
		outfile.close();
	}
	
	else if (func == "reg") {
		if (argc <= 4) { return 0; }

		string simp_flag = argList[2]; 
		string rest_flag = argList[3];

		wff = Wff_to_Nnf(wff); 

		int n = get_n(wff); 
		vector<string> answer = reg(wff, n, true, (simp_flag == "y") ? true : false);
		if (rest_flag == "y") {
			answer = strip_commas(answer); 
			answer = REST_simplify(answer); 
			answer = add_commas(answer, n); 
		}

		ofstream out_formula("./west_output/formula.txt");
		ofstream out_regexp("./west_output/regexp.txt"); 
		
		
		auto FORMULAS = get_formulas(); 
		for (auto tup : FORMULAS) {
			string subformula = get<0>(tup);
			out_formula << subformula << endl;
		
			vector<string> regexp = get<1>(tup); 
			for (string subformula_regexp : regexp) {
				out_regexp << subformula_regexp << endl; 
			}
			out_regexp << endl; 
		}
		
		
		out_formula.close(); 
		out_regexp.close(); 
	}

	else {
		cout << "invalid function call" << endl; 
	}

	return 0; 
}


//int main() {
//
//	string wff = ""; 
//	string input;
//	vector<string> answer; 
//
//	while (true) {
//		cout << "Enter a MLTL formula." << endl;
//		do {
//			getline(cin, wff);
//			wff = strip_char(wff, ' ');
//			if (!Wff_check(wff)) {
//				cout << "Not a well formed formula!" << endl;
//			}
//
//		} while (!Wff_check(wff));
//		int n = get_n(wff);
//
//
//		cout << "Generate truth table for all subformulas? (y / n)" << endl;
//		do {
//			getline(cin, input);
//			if (input != "y" and input != "n") {
//				cout << "Please enter 'y' or 'n'" << endl;
//			}
//		} while (input != "y" and input != "n");
//		bool truth_table_flag = (input == "y") ? true : false;
//
//
//		cout << "Would you like to simplify output of reg? (y / n)" << endl;
//		do {
//			getline(cin, input);
//			if (input != "y" and input != "no") {
//				cout << "Please enter 'y' or 'n'" << endl;
//			}
//		} while (input != "y" and input != "n");
//		bool simp_flag = (input == "y") ? true : false;
//
//
//		cout << "Would you like to apply REST scheme? (y / n) "
//			"Warning: computationally expensive."<< endl;
//		do {
//			getline(cin, input);
//			if (input != "y" and input != "n") {
//				cout << "Please enter 'y' or 'n'" << endl;
//			}
//		} while (input != "y" and input != "n");
//		bool rest_flag = (input == "y") ? true : false;
//
//
//		// Computing regex with WEST algorithm and simplifying as prompted
//		string nnf = Wff_to_Nnf(wff);
//		cout << "NNF Formula: " << nnf << endl;
//		answer = reg(wff, n, truth_table_flag, simp_flag);
//		if (rest_flag) {
//			answer = strip_commas(answer); 
//			answer = REST_simplify(answer); 
//			answer = add_commas(answer, n); 
//		}
//
//
//		cout << "Would you like to view output in GUI? (y/n) "
//			"Otherwise prints output to terminal." << endl;
//		do {
//			getline(cin, input);
//			if (input != "y" and input != "n") {
//				cout << "Please enter 'y' or 'n'" << endl;
//			}
//		} while (input != "y" and input != "n");
//		bool view_gui = (input == "y") ? true : false;
//
//		
//		// print output to commandline
//		if (!view_gui) {
//			if (truth_table_flag) {
//				print_subformulas(get_formulas(), n, nnf);
//				clear_formulas();
//			}
//			else {
//				print(answer);
//				cout << endl;
//			}
//
//			cout << "Finished computing." << endl;
//			cout << "Size of vector: " << answer.size() << endl;
//			cout << "Number of characters: " << sum_of_characters(answer) << endl;
//		}
//		// display outputs in gui
//		else {
//			ofstream out_formula("./west_output/formula.txt"); 
//			ofstream out_regexp("./west_output/regexp.txt");
//			ofstream out_formula_info("./west_output/formula_info.txt");
//
//
//			// Store formula info
//			int time_steps = strip_commas(answer)[0].length() / n;
//			out_formula_info << n << endl; 
//			out_formula_info << time_steps << endl; 
//
//			// subformulas have been generated
//			if (truth_table_flag) {
//				auto FORMULAS = get_formulas(); 
//				for (auto tup : FORMULAS) {
//					string subformula = get<0>(tup);
//					out_formula << subformula << endl;
//
//					vector<string> regexp = get<1>(tup); 
//					for (string subformula_regexp : regexp) {
//						out_regexp << subformula_regexp << endl; 
//					}
//					out_regexp << endl; 
//				}
//			}
//			// no subformulas generated
//			else {
//				out_formula << nnf;
//
//				for (string regexp : answer) {
//					out_regexp << regexp << endl; 
//				}
//			}
//
//			out_formula.close(); 
//			out_regexp.close(); 
//			out_formula_info.close();
//
//
//			system("python gui.py"); 
//		}
//
//	}
//	return 0; 
//}