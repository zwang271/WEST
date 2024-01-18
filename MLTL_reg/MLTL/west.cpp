#include <string>
#include <fstream>
#include <iostream>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"

using namespace std;

int main(int argc, char** argv) {
    // Expecting 2 arguments: formula file, output file
    if (argc <= 2) { return 0; }
    ifstream formula_file(argv[1]);
    ofstream output_file(argv[2]);
    
    // Read formula from file, should be a single line
    string wff;
    getline(formula_file, wff);
    wff = strip_char(wff, ' ');
    wff = Wff_to_Nnf(wff);

    // compute reg(wff)
    int n = get_n(wff);
    vector<string> answer = reg(wff, n, false, true);

    // Write output to file
    for (int i = 0; i < answer.size(); i++) {
        output_file << answer[i] << endl;
    }
    return 0;
}