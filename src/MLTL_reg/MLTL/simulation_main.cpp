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
#define FUNC_NUM 1000
int NUM_PROP_VAR;
int MAX_ITER;
int DELTA;
int INTERVAL_MAX;

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
        output = reg(Wff_to_Nnf(line), NUM_PROP_VAR, false, true);
        output = simplify(output, NUM_PROP_VAR);
        auto stop = high_resolution_clock::now();
        // cast to microseconds
        duration<double, std::micro> ms = stop - start;

        // Write the amount of time to calculate 'output'
        // into 'out' file
        outfile << ms.count() / 1000000; 
        outfile << " ";
        // Write the amount of characters in 'output'
        // into 'out' file
        outfile << sum_of_characters(output);
        outfile << "\n";
        cout << formula_num << " Wrote a line to output file\n";
        ++formula_num;
    }
    infile.close();
    outfile.close();
}


/*
* Driver function for simulation_main.cpp file
*/
int main() {
    cout << "Generate formulas? (y/n) \n";
    char gen;
    cin >> gen;
    string formulas;

    if (gen == 'y') {
        cout << "NUM_PROP_VAR: ";
        cin >> NUM_PROP_VAR;

        cout << "MAX_ITER: ";
        cin >> MAX_ITER;

        // Maximum distance of any interval
        cout << "DELTA: ";
        cin >> DELTA;

        // Maximum upperbound of any interval
        cout << "INTERVAL_MAX: ";
        cin >> INTERVAL_MAX;

        cout << "Enter name of formula output file \n";
        cin >> formulas;
        srand(time(NULL));
        // Write FUNC_NUM number of random MLTL formulas
        run_rand_function(formulas);
        cout << "Wrote to formula output file\n";
    } else {
        cout << "Enter pathname of formula file \n";
        cin >> formulas;

        cout << "NUM_PROP_VAR: ";
        cin >> NUM_PROP_VAR;
    }
    cout << "Enter name of output file \n";
    string out;
    cin >> out;
    // Write simulation results to out
    simulate(formulas, out);
    cout << "Wrote to output file\n";
    return 0;
}

// ./complexity_graph/random_mltl2.txt