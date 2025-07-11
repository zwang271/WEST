// Author: Zili Wang
// Last updated: 06/24/2025
// WEST command line tool, containerized for docker

#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <chrono>
#include <fstream>
#include "reg.h"
#include "utils.h"
#include "parser.h"

using namespace std;


// bool hasExtension(const std::string& filename, const std::string& extension) {
//     if (filename.length() >= extension.length()) {
//         return filename.find(extension) != std::string::npos;
//     }
//     return false;
// }

int main(int argc, char** argv) {
    // Expecting 1 arg: input formula file(.mltl or .txt) or formula string
    if (argc < 2) {
        cout << "Usages:" << endl;
        cout << "\tstring_west <input_file>" << endl;
        cout << "\tstring_west \"<formula_string>\"" << endl;
        return 1;
    }

	string wff; 
	// if (hasExtension(string(argv[1]), ".mltl") || hasExtension(string(argv[1]), ".txt")) {
	// 	// Read input file
	// 	ifstream input_file(argv[1]);
	// 	if (!input_file.is_open()) {
	// 		cout << "Error: could not open input file." << endl;
	// 		return 1;
	// 	}
	// 	// Read input file into string
	// 	while (input_file) {
	// 		getline(input_file, wff);
	// 	}
	// 	input_file.close();
	// }
	// else {
	// 	wff = argv[1];
	// }
    wff = argv[1];
    wff = strip_char(wff, ' ');

    string nnf = wff_to_nnf(wff);
    int n = get_n(nnf);
    int cl = complen(nnf);
    int bits_needed = 2 * n * cl; 

    if (bits_needed > MAXBITS) {
        cout << "Error: Formula is too large to fit in bitset." << endl;
        cout << "\tBits needed: " << bits_needed << endl;
        cout << "\tBits available: " << MAXBITS << endl;
    }

    cout << "\tnnf: " << nnf << endl;
    cout << "\tpropositonal variables: " << n << endl;  
    cout << "\tcomputation length: " << cl << endl;
    cout << "\tBits needed: 2 * " << n << " * " << cl << " = " << bits_needed << endl;
    cout << "\tBits available: " << MAXBITS << endl;
    cout << "\tFormula fits in bitset." << endl;
    cout << "\tUse -OPTIMIZED flag to run bit optimized version of WEST" << endl; 

    auto start = chrono::high_resolution_clock::now();
    vector<bitset<MAXBITS>> bitset_computations = reg(nnf, n);
    auto stop = chrono::high_resolution_clock::now();
    int time = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
    auto computations = bitset_to_reg(bitset_computations, bits_needed);
    computations = add_commas(computations, n);
    cout << "=======================================================" << endl; 
    for (int i = 0; i < min(int(computations.size()), 10); i++) {
        cout << "\t" << computations[i] << endl;
    }
    if (computations.size() > 10) {
        cout << "\t..." << endl;
    }
    cout << "=======================================================" << endl; 
    cout << "\tTime taken: " << time << " milliseconds" << endl;
    cout << "\tNumber of computations: " << computations.size() << endl;
    
    ofstream output_file("./output/output.txt");
    if (output_file.is_open()) {
        output_file << nnf << endl;
        for (int i = 0; i < computations.size(); i++) {
            output_file << computations[i] << endl;
        }
    }
    output_file.close();
    cout << "Output written to ./output/output.txt" << endl;

    auto FORMULAS = get_formulas();
    ofstream formulas_file("./output/subformulas.txt");
    if (formulas_file.is_open()) {
        for (int i = 0; i < FORMULAS.size(); i++) {
            formulas_file << get<0>(FORMULAS[i]) << endl;
            auto regexes = bitset_to_reg(get<1>(FORMULAS[i]), bits_needed);
            regexes = add_commas(regexes, n);
            for (auto regex : regexes) {
                formulas_file << regex << endl;
            }
            formulas_file << endl; 
        }
    }
    formulas_file.close();
    cout << "Subformulas written to ./output/subformulas.txt" << endl << endl;

    // print all outputs to standard output
    // auto FORMULAS = get_formulas();
    // for (int i = 0; i < FORMULAS.size(); i++) {
    //     cout << get<0>(FORMULAS[i]) << endl;
    //     auto regexes = bitset_to_reg(get<1>(FORMULAS[i]), bits_needed);
    //     regexes = add_commas(regexes, n);
    //     for (auto regex : regexes) {
    //         cout << regex << endl;
    //     }
    //     cout << endl;
    // }

    return 0; 
}
