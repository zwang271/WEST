#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "utils.h"
#include "rest.h"

// Creates regex that satisfies REST of size SIZE with ones decided by array_one, zeros by array_zero
vector<string> rest_regex(int size, vector<int> array_one, vector<int> array_zero) {
    //char regex [size+1][size]; // numRows, numCols
    //for(int i=0; i<size; ++i){ // i IS COLUMN
    //    for (int j=0; j < size+1; ++j) { // j IS ROW
    //        if (array_one[i] == j) {
    //            regex[j][i] = '1';
    //        }
    //        else if (array_zero[i] == j) {
    //            regex[j][i] = '0';
    //        }
    //        else {
    //            regex[j][i] = 'S';
    //        }
    //    }
    //}

    //vector<string> output = vector<string>();
    //string temp;


    // Turning char array into vector of strings
    //for (int i = 0; i < size + 1; ++i) {
    //    temp = "";
    //    for (int j = 0; j < size; ++j) {
    //        temp += regex[i][j];
    //    }
    //    output.push_back(temp);
    //}

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

/*void print(vector<string> v) {
   for (int i = 0; i < v.size(); ++i) {
        cout << v[i] << endl;
    }
}*/

int main() {
    vector<string> regexp = {};
    int m = 16;
    int n = 14;
    int k = 10; 
    string alphabet = "01s";
    for (int row = 0; row < m; row++) {
        string rand_reg = "";
        for (int col = 0; col < n; col++) {
            if (row < k+1) {
                if (col < k) {
                    if (row == k) {
                        rand_reg += "0";
                    }
                    else if (row == col) {
                        rand_reg += "1";
                    }
                    else {
                        rand_reg += "s";
                    }
                }
                else {
                    rand_reg += "1"; 
                }
            }
            else {
                rand_reg += alphabet[rand() % 3];
            }
        }
        regexp.push_back(rand_reg);
    }


    cout << endl;
    print(regexp);

    regexp = strip_commas(regexp);
    cout << "REST_simplify_v2" << endl;
    vector<string> regexp2 = REST_simplify_v2(regexp);
    cout << "\nREST_simplify original" << endl; 
    regexp = REST_simplify(regexp); 
    cout << endl;
    
    print(regexp2);
    cout << endl; 
    print(regexp);


    return 0; 


    //bool run = true;
    //int success_counter = 0;
    //srand(time(0));
    //while (run) {
    //    int size = 15 + (rand() % 5);
    //    cout << "Size of block: " << size << endl;
    //    //cout << "Please enter size of REST block." << endl;
    //    //cin >> size;
    //    /*int array_one[size];
    //    int array_zero[size];*/
    //    vector<int> array_one = {};
    //    vector<int> array_zero = {};

    //    for (int i = 0; i < size; ++i) {
    //        /*array_one[i] =  rand()%(size+1);
    //        array_zero[i] = rand()%(size+1);*/
    //        array_one.push_back(rand() % (size + 1));
    //        array_zero.push_back(rand() % (size + 1));

    //        while (array_one[i] == array_zero[i]) {
    //            array_one[i] = rand() % (size + 1);
    //            array_zero[i] = rand() % (size + 1);
    //        }
    //    }


    //    vector<string> regex = rest_regex(size, array_one, array_zero);
    //    //cout << "Randomly generated input: " << endl;
    //    //print(regex);

    //    //vector<string> regex_expanded = expand(regex);
    //    //cout << "Size of expanded input = " << regex_expanded.size() << endl;

    //    vector<string> simp = REST_simplify(regex);
    //    //cout << "\n After simplifying with REST: " << endl;
    //    //print(simp);
    //    //cout << endl;

    //    if (simp.size() != 1 || simp.at(0).find('1') != string::npos ||
    //        simp.at(0).find('0') != string::npos) {
    //        cout << "REST Failed!" << endl;
    //        run = false;
    //    }

    //    success_counter++;
    //    cout << "Number of successful REST runs: " << success_counter << endl;

    //    //char run_char;
    //    //cout << "Run again? (y/n)" << endl;
    //    //cin >> run_char;
    //    //if (run_char == 'n') {
    //        //run = false;
    //    //}
    //}
    //return 0;
}
