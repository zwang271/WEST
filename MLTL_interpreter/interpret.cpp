#include <iostream>
#include <vector>
#include <string>
#include "utils.h"
#include "evaluate_mltl.h"

using namespace std;

int main(int argc, char** argv) {
    // should be 2 arguments: formula file and trace file
    if (argc != 3) {
        throw invalid_argument("Incorrect number of arguments.");
    }
    string formula_file = argv[1];
    string trace_file = argv[2];

    // read in formula from file
    vector<string> formula_vec = read_from_file(formula_file);
    if (formula_vec.size() == 0) {
        throw invalid_argument("Formula file is empty.");
    }
    string formula = formula_vec[0];
    if (formula_vec.size() > 1) {
        for (int i = 1; i < formula_vec.size(); ++i) {
            formula = "(" + formula + "&" + formula_vec[i] + ")";
        }
    }
    formula = strip_char(formula, ' ');

    // read in trace from file
    vector<string> trace = read_from_file(trace_file);
    for (int i = 0; i < trace.size(); ++i) {
        trace[i] = strip_char(trace[i], ' ');
        trace[i] = strip_char(trace[i], ',');
    }

    // evaluate formula on trace
    bool eval = evaluate_mltl(formula, trace, false);


    // print results
    cout << "Formula: " << formula << endl;
    cout << "Trace: " << endl;
    print(trace);
    if (eval) {
        cout << "evaluation: true" << endl;
    } else {
        cout << "evaluation: false" << endl;
    }

    return 0;
}