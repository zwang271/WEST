#include "simulation_main.h"
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#define MAX_ITER 3
// Maximum distance of any interval
// in MLTL formula
#define DELTA 5
// Maximum upperbound of any interval
// in MLTL formula
#define INTERVAL_MAX 10
// Number of random MLTL formulas
// to be generated
#define FUNC_NUM 1000
#define NUM_PROP_VAR 5

using namespace std;
using namespace std::chrono;


/*
* Generate a random MLTL formula with a iteration depth
* of MAX_ITER.
* For ex: 'p2' is iteration depth 0, '(p0 & p1)' is iteration depth 1,
* '(((p0 & p1) U [5:10] (p2 R [5:10] p3))' is iteration depth 2, etc.
*/
string rand_function(int iter) {
    // Maximum iteration depth hit, fill-in with propositional variables 
    if (iter == MAX_ITER) {
        return "p" + to_string((rand() % NUM_PROP_VAR));
    }

    // Determines which inductive case to follow
    int op_type = (rand() % 4);

    // Return: "~" + rand_function(iter + 1)
    if (op_type == 0) {
        return "~" + rand_function(iter + 1);
    }

    // Return: "(" + rand_function(iter + 1) + binary_prop + rand_function(iter + 1) + ")"
    else if (op_type == 1) {
        int binary_prop_gen = rand() % 4;
        string binary_prop = "";
        if (binary_prop_gen == 0) {
            binary_prop = "v";
        }
        else if (binary_prop_gen == 1) {
            binary_prop = "&";
        }
        else if (binary_prop_gen == 2) {
            binary_prop = ">";
        }
        else if (binary_prop_gen == 3) {
            binary_prop = "=";
        }

        return "(" + rand_function(iter + 1) + binary_prop + rand_function(iter + 1) + ")";
    }

    // Return: unary_temp + "[" + to_string(a) + ":" + to_string(b) + "]" + rand_function(iter + 1)
    // where a,b are random upper and lower bounds
    else if (op_type == 2) {
        int unary_temp_gen = rand() % 2;
        string unary_temp = "";
        if (unary_temp_gen == 0) {
            unary_temp = "F";
        }
        else if (unary_temp_gen == 1) {
            unary_temp = "G";
        }

        // Random upperbound
        int a = rand() % INTERVAL_MAX;
        // Random lowerbound
        int b = (rand() % min(DELTA, INTERVAL_MAX - a)) + a;

        return unary_temp + "[" + to_string(a) + ":" + to_string(b) + "]" + rand_function(iter + 1);
    }

    // Return: "(" + rand_function(iter + 1) + binary_temp + "[" + to_string(a) + ":" + to_string(b) + "]" + rand_function(iter + 1) + ")"
    // where a,b are random upper and lower bounds
    else if (op_type == 3) {
        int binary_temp_gen = rand() % 2;
        string binary_temp = "";
        if (binary_temp_gen == 0) {
            binary_temp = "U";
        }
        else if (binary_temp_gen == 1) {
            binary_temp = "R";
        }

        // Random upperbound
        int a = rand() % INTERVAL_MAX;
        // Random lowerbound
        int b = (rand() % min(DELTA, INTERVAL_MAX - a)) + a;

        return "(" + rand_function(iter + 1) + binary_temp + "[" + to_string(a) + ":" +
            to_string(b) + "]" + rand_function(iter + 1) + ")";
    }
    return "";
}


/*
* Writes FUNC_NUM number of random MLTL
* formulas of iterative depth MAX_ITER to the file "formulas"
*/ 
void run_rand_function(string formulas) {
    ofstream myfile;
    myfile.open(formulas);
    for (int i = 0; i < FUNC_NUM; ++i) {
        myfile << rand_function(0);
        myfile << "\n";
    }
    myfile.close();
}


/*
* For each MLTL formula alpha in the 'formulas' file, simulate will:
*   1. Calculate the regex for alpha in 'output' variable
*   2. Simplify the regex for alpha using simplify() function (from utils.cpp file)
*      and saves this to 'output' variable
*   3. Writes the amount of time taken to calculate 'output' as-well-as
*      number of characters in 'output' to 'out' file   
*/
void simulate(string formulas, string out) {
    ofstream outfile;
    outfile.open(out);
    ifstream infile(formulas);
    string line;
    vector<string> output;
    // Keeps track of which formula is currently being processed
    // from 'formulas' file
    int formula_num = 1;
    while (getline(infile, line)) {
        outfile << line.size();
        outfile << " ";
        auto start = high_resolution_clock::now();
        output = reg(Wff_to_Nnf(line), NUM_PROP_VAR);
        output = simplify(output, NUM_PROP_VAR);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        // Write the amount of time to calculate 'output'
        // into 'out' file
        outfile << duration.count();
        outfile << " ";
        // Write the amount of characters in 'output'
        // into 'out' file
        outfile << sum_of_characters(output);
        outfile << "\n";
        cout << formula_num << " Wrote a line to complexities.txt\n";
        ++formula_num;
    }
    infile.close();
    outfile.close();
}


/*
* Driver function for simulation_main.cpp file
*/
int main() {
    string formulas = "random_mltl.txt";
    string out = "complexities.txt";
    srand(time(NULL));
    // Write FUNC_NUM number of random MLTL formulas
    // to "random_mltl.txt"
    run_rand_function(formulas);
    cout << "Wrote to random_mltl.txt\n";
    // Write simulation results to "complexities.txt"
    simulate(formulas, out);
    cout << "Wrote to complexities.txt\n";
    return 0;
}
