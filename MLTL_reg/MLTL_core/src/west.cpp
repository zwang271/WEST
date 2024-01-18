#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <chrono>
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

    cout << "\tnnf: " << nnf << endl;
    cout << "\tpropositonal variables: " << n << endl;  
    cout << "\tcomputation length: " << cl << endl;
    if (bits_needed > MAXBITS) {
        cout << "\tBits needed: 2 * " << n << " * " << cl << " = " << bits_needed << endl;
        cout << "\tBits available: " << MAXBITS << endl;
        cout << "\tFormula too large for bitset." << endl;
    } else {
        cout << "\tBits needed: " << bits_needed << endl;
        cout << "\tBits available: " << MAXBITS << endl;
        cout << "\tFormula fits in bitset." << endl;
    }

    auto start = chrono::high_resolution_clock::now();
    vector<bitset<MAXBITS>> bitset_computations = reg(nnf, n);
    auto stop = chrono::high_resolution_clock::now();
    vector<string> computations = bitset_to_reg(bitset_computations, bits_needed);
    cout << "=======================================================" << endl; 
    for (int i = 0; i < min(int(computations.size()), 10); i++) {
        cout << "\t" << computations[i] << endl;
    }
    if (computations.size() > 10) {
        cout << "\t..." << endl;
    }
    cout << "=======================================================" << endl; 
    int time = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
    cout << "\tTime taken: " << time << " milliseconds" << endl;
    cout << "\tNumber of computations: " << computations.size() << endl;
    // auto b = bitset<MAXBITS>("00000000");
    // print(shift(b, 2));
    
    return 0; 
}
