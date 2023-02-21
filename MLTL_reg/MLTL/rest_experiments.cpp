#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "utils.h"
#include "rest.h"
#include "rest_experiments.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

double run_experiment(vector<string> rest_comp) {
    auto start = high_resolution_clock::now();
    vector<string> output = REST_simplify(rest_comp);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Write the amount of time to calculate 'output'
    // into 'out' file
    return duration.count();

}

// Creates regex that satisfies REST of size SIZE with ones decided by array_one, zeros by array_zero
vector<string> rest_regex(int size, vector<int> array_one, vector<int> array_zero) {
    vector<string> output; // numRows, numCols
    for(int j=0; j<size+1; ++j){ // j IS ROW
        output.push_back("");
        for (int i=0; i < size; ++i) { // i IS COLUMN
            if (array_one[i] == j) {
                output[j] += "1";
            }
            else if (array_zero[i] == j) {
                output[j] += "0";
            }
            else {
                output[j] += "s";
            }
        }
    }

    return output;
}

int main() {
    cout << "Enter file name to print data to\n";
    string out;
    cin >> out;

    ofstream outfile;
    outfile.open(out);

    cout << "\nEnter min size\n";
    int min_size;
    cin >> min_size;
    cout << "\nenter variation\n";
    int variation;
    cin >> variation;
    int counter = 0;
    srand(time(0));
    while (counter < 100) {
        int size = min_size + (rand() % variation);
        vector<int> array_one;
        vector<int> array_zero;

        for (int i = 0; i < size; ++i) {
            array_one.push_back(rand() % (size + 1));
            array_zero.push_back(rand() % (size + 1));

            while (array_one[i] == array_zero[i]) {
                array_one[i] = rand() % (size + 1);
                array_zero[i] = rand() % (size + 1);
            }
        }
        vector<string> regex = rest_regex(size, array_one, array_zero);
        int num_chars = size * (size + 1);
        outfile << num_chars;
        outfile << " ";
        double duration = run_experiment(regex);
        outfile << duration;
        outfile << "\n";
        cout << " Wrote a line to output file\n";

        cout << counter << "\n";
        counter++;
    }
    outfile.close();
    return 0;
}