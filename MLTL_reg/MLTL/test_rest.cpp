#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "utils.h"
#include "rest.h"

// Creates regex that satisfies REST of size SIZE with ones decided by array_one, zeros by array_zero
vector<string> rest_regex(int size, int array_one[], int array_zero[]) {
    char regex [size+1][size]; // numRows, numCols
    for(int i=0; i<size; ++i){ // i IS COLUMN
        for (int j=0; j < size+1; ++j) { // j IS ROW
            if (array_one[i] == j) {
                regex[j][i] = '1';
            }
            else if (array_zero[i] == j) {
                regex[j][i] = '0';
            }
            else {
                regex[j][i] = 'S';
            }
        }
    }

    vector<string> output = vector<string>();
    string temp;

    // Turning char array into vector of strings
    for (int i = 0; i < size + 1; ++i) {
        temp = regex[i];
        temp += "\0";
        cout << temp << endl;
        output.push_back(temp);
    }

    return output;
}

void print(vector<string> v) {
    for (int i = 0; i < v.size(); ++i) {
        cout << v[i] << endl;
    }
}

int main() {
    int size = 0;
    cout << "Please enter size of REST block." << endl;
    cin >> size;
    int array_one[size];
    int array_zero[size];

    srand(time(0));
    for(int i = 0; i < size; ++i){
        array_one[i] =  rand()%(size+1);
        array_zero[i] = rand()%(size+1);
        while (array_one[i] == array_zero[i]) {
            array_one[i] =  rand()%(size+1);
            array_zero[i] = rand()%(size+1);
        }
    }


    vector<string> regex = rest_regex(size, array_one, array_zero);
    //print(regex);
    //cout << endl;
    print(REST(regex));
    cout << endl;
    return 0;
}
