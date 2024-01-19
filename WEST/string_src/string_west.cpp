#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <chrono>
#include "string_utils.h"
#include "string_grammar.h"
#include "string_nnf_grammar.h"
#include "string_reg.h"
#include "string_rest.h"

using namespace std;

bool hasExtension(const std::string& filename, const std::string& extension) {
    if (filename.length() >= extension.length()) {
        return filename.find(extension) != std::string::npos;
    }
    return false;
}

int main(int argc, char** argv) {
    // Expecting 1: input formula file(.mltl or .txt) or formula string
    if (argc != 2) {
        cout << "Usages:" << endl;
        cout << "\tstring_west <input_file>" << endl;
        cout << "\tstring_west \"<formula_string>\"" << endl;
        return 1;
    }

	string wff; 
	if (hasExtension(string(argv[1]), ".mltl") || hasExtension(string(argv[1]), ".txt")) {
		// Read input file
		ifstream input_file(argv[1]);
		if (!input_file.is_open()) {
			cout << "Error: could not open input file." << endl;
			return 1;
		}
		// Read input file into string
		while (input_file) {
			getline(input_file, wff);
		}
		input_file.close();
	}
	else {
		wff = argv[1];
	}

    // Strip whitespace from input, check wff
    string python_parser = "python ./string_src/parser.py \"" + wff + "\"";
    system(python_parser.c_str());
    ifstream wff_file("./string_gui/west_wff.txt");
    if (wff_file.is_open()) {
        while (wff_file) {
            getline(wff_file, wff);
        }
    }
    wff_file.close();
    wff = strip_char(wff, ' ');
    if (!Wff_check(wff)) {
        cout << "Error: not a well formed formula." << endl;
        return 1;
    }

    // Get number of prop_vars in wff
	int n = get_n(wff);
    string nnf = Wff_to_Nnf(wff);
    cout << "\tnnf: " << nnf << endl;
    cout << "\tpropositonal variables: " << n << endl;
    
    auto start = chrono::high_resolution_clock::now();
    vector<string> answer = reg(nnf, n, true, true);
    auto stop = chrono::high_resolution_clock::now();
    vector<tuple<string, vector<string>>> formulas = get_formulas(); 
    int time = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
    cout << "=======================================================" << endl;
    for (int i = 0; i < min(int(answer.size()), 10); i++) {
        cout << "\t" << answer[i] << endl;
    }
    if (answer.size() > 10) {
        cout << "\t..." << endl;
    }
    cout << "=======================================================" << endl;
    cout << "\tTime taken: " << time << " milliseconds" << endl;
    cout << "\tNumber of computations: " << answer.size() << endl;

    // Create two files: subformulas.txt and output.txt
    // subformulas.txt contains all subformulas of the input formula
    // output.txt contains the output of the formula
    ofstream subformulas_file("./output/string_subformulas.txt");
    for (int i = 0; i < formulas.size(); ++i) {
        //if the substring is present in the nnf, print it out
        if (nnf.find(get<0>(formulas[i])) != -1) {
            if (subformulas_file.is_open()) {
                subformulas_file << get<0>(formulas[i]) << endl;
                for (int j = 0; j < get<1>(formulas[i]).size(); ++j) {
                    subformulas_file << get<1>(formulas[i])[j] << endl;
                }
                subformulas_file << endl;
            }               
        }
    }
    subformulas_file.close();
    cout << "Output for subformulas written to ./output/string_subformulas.txt" << endl;

    ofstream output_file("./output/string_output.txt");
    if (output_file.is_open()) {
        output_file << nnf << endl;
        for (int i = 0; i < answer.size(); ++i) {
            output_file << answer[i] << endl;
        }
    }
    output_file.close();
    cout << "Output for formula written to ./output/string_output.txt" << endl << endl; 

    return 0;
}