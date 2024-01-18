#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "utils.h"
#include "evaluate_mltl.h"

using namespace std;

int main(int argc, char** argv) {
    // should be 4 arguments: formula file, trace file, output file, and optional flag for printing
    if (argc != 4 && argc != 5) {
        throw invalid_argument("Incorrect number of arguments.");
    }
    string formula_file = argv[1];
    string trace_file = argv[2];
    string output_file = argv[3];
    bool print = false;
    if (argc == 5) {
        string print_flag = argv[4];
        if (print_flag == "-p") {
            print = true;
        } else {
            throw invalid_argument("Incorrect flag.");
        }
    }

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

    // read in batch of traces from file
    vector<vector<string>> batch = read_batch_from_file(trace_file);
    if (batch.size() == 0) {
        throw invalid_argument("Trace file is empty.");
    }

    // for each trace in batch, evaluate formula on trace
    ofstream out;
    out.open(output_file);
    for (int i = 0; i < batch.size(); ++i) {
        vector<string> trace = batch[i];
        for (int j = 0; j < trace.size(); ++j) {
            trace[j] = strip_char(trace[j], ' ');
            trace[j] = strip_char(trace[j], ',');
        }
        // evaluate formula on trace
        bool eval = evaluate_mltl(formula, trace, false);
        // write to output file
        out << eval << endl;

        // print results
        if (print) {
            cout << "Formula: " << formula << endl;
            cout << "Trace: " << endl;
            for (int i = 0; i < trace.size(); ++i) {
                cout << i << ": " << trace[i] << endl;
            }
            if (eval) {
                cout << "evaluation: true" << endl;
            } else {
                cout << "evaluation: false" << endl;
            }
        }
    }
    out.close();

    return 0;
}