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


bool hasExtension(const std::string& filename, const std::string& extension) {
    if (filename.length() >= extension.length()) {
        return filename.find(extension) != std::string::npos;
    }
    return false;
}

int main(int argc, char** argv) {
    // Expecting at least 1: input formula file(.mltl or .txt) or formula string
    // Optional 2nd argument: -OPTIMIZED
    if (argc < 2) {
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
    wff = strip_char(wff, ' ');

    bool optimized = false;
    if (argc == 3 && string(argv[2]) == "-OPTIMIZED") {
        optimized = true;
    }

    string nnf = wff_to_nnf(wff);
    int n = get_n(nnf);
    int cl = complen(nnf);
    int bits_needed = 2 * n * cl; 

    if (optimized || bits_needed > MAXBITS) {
        // recompiles optimized binary and runs the executable
        recompile(wff); 
        return 0;
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
    vector<string> computations = bitset_to_reg(bitset_computations, bits_needed);
    int time = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
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
    cout << "Output written to ./output/output.txt" << endl << endl;

    return 0; 
}
