#include <iostream>
#include "utils.h"
#include <vector>

using namespace std;

int main() {
    
    vector<string> vec;
    vec.push_back("0ss,sss");
    vec.push_back("s0s,sss,000");
    vec.push_back("ss0");

    vec = strip_commas(vec);
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i] << endl;
    }

    return 0;
}

