#include "simulation.h"
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
#define MAX_ITER 2
#define DELTA 5
#define MISSION_END 10
#define FUNC_NUM 1000
#define NUM_PROP_VAR 5

using namespace std;

void right_or_PT1(int iterations) {
	int n = 1;
	vector<string> v = { "s", "0" };
	for (int i = 0; i < iterations; ++i) {
		if (i % 2 == 0) {
			v[0] += ",0";
			v[1] += ",s";
		}
		else {
			v[0] += ",s";
			v[1] += ",0";
		}

		print(v);
		cout << i << "\t input length: " << v[0].length() << "\t output vector size: " << simplify(v, n).size() << endl;
		print(simplify(v, n));
		cout << endl << endl;
	}
}

string rand_function(int iter) {
    if (iter == MAX_ITER) {
        return "p"+ to_string((rand() % NUM_PROP_VAR));
    }

    int op_type = (rand() % 4);
    if (op_type == 0) {
        return "~" + rand_function(iter + 1);
    }
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

        return "(" + rand_function(iter + 1) + binary_prop + rand_function(iter+1) + ")";
    }
    else if (op_type == 2) {
        int unary_temp_gen = rand() % 2;
        string unary_temp = "";
        if (unary_temp_gen == 0) {
            unary_temp = "F";
        }
        else if (unary_temp_gen == 1) {
            unary_temp = "G";
        }

        int a = rand() % MISSION_END;
        int b = (rand() % min(DELTA, MISSION_END - a)) + a;

        return unary_temp + "[" + to_string(a) + ":" + to_string(b) + "]" + rand_function(iter + 1);
    }
    else if (op_type == 3) {
        int binary_temp_gen = rand() % 2;
        string binary_temp = "";
        if (binary_temp_gen == 0){
            binary_temp = "U";
        }
        else if (binary_temp_gen == 1) {
            binary_temp = "R";
        }

        int a = rand() % MISSION_END;
        int b = (rand() % min(DELTA, MISSION_END - a)) + a;

        return "(" + rand_function(iter + 1) + binary_temp + "[" + to_string(a) + ":" +
            to_string(b) + "]" + rand_function(iter + 1) + ")";
    }
    return "";
}

void run_rand_function() {
    ofstream myfile;
    myfile.open("random_mltl.txt");
    for (int i = 0; i < FUNC_NUM; ++i){
        myfile << rand_function(0);
        myfile << "\n";
    }
    myfile.close();
}

void simulate() {
    ofstream outfile;
    outfile.open("complexities.txt");
    ifstream infile("random_mltl.txt");
    string line;
    vector<string> output;
    int iter = 1;
    while (getline(infile,line)) {
        outfile << line.size();
        outfile << " ";
        output = reg(Wff_to_Nnf_clean(line), NUM_PROP_VAR);
        output = simplify(output, NUM_PROP_VAR);
        outfile << sum_of_characters(output);
        outfile << "\n";
        cout << iter << " Wrote a line to complexities.txt\n";
        ++iter;
    }
    infile.close();
    outfile.close();
}

int main() {
    //srand(1);
    /*run_rand_function();
    cout << "Wrote to random_mltl.txt\n";*/
    simulate();
    cout << "Wrote to complexities.txt\n";
    return 0;
}

