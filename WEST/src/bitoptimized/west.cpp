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

int main(int argc, char** argv) {
    string wff = argv[1];
    wff = strip_char(wff, ' ');

    string nnf = wff_to_nnf(wff);
    int n = get_n(nnf);
    int cl = complen(nnf);
    int bits_needed = 2 * n * cl; 

    cout << "Running bit optimized version of WEST" << endl;
    cout << "\tnnf: " << nnf << endl;
    cout << "\tpropositonal variables: " << n << endl;  
    cout << "\tcomputation length: " << cl << endl;
    cout << "\tBits needed: 2 * " << n << " * " << cl << " = " << bits_needed << endl;
    cout << "\tBits available: " << MAXBITS << endl;

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
        cout << "Output written to ./output/output.txt" << endl;
    }
    output_file.close();

    return 0; 
}
