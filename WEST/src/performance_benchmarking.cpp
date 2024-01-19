#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <chrono>
#define MAXSIZE 1024

using namespace std;


// STRING BASED IMPLEMENTATION
/**********************************************************/
string string_intersect(string w_1, string w_2) {
	// If either w_1 or w_2 are empty, return empty.
	if (w_1 == "" || w_2 == "") {
		return "";
	}

	// Bit-wise 'and' w_1 and w_2
	string w = "";
	for (int i = 0; i < w_1.length(); i++) {
		if (w_1[i] != 's' && w_2[i] != 's') {
			if (w_1[i] != w_2[i]) {
				return "";
			}
			else {
				w += w_1[i];
			}
		}
		else if (w_1[i] == 's') {
			w += w_2[i];
		}
		else {
			w += w_1[i];
		}

	}

	return w;
}

/*
Input: computation strings s1 and s2
Returns true if the following is possible:
s1 = w1 + 'c1' + v1
s2 = w1 + 'c2' + v1
*/
bool string_check_simp(string s1, string s2) {
    if (s1.length() != s2.length()) {
        return false;
    }
    for (int i = 0; i < s1.length(); ++i) {
        string w1 = s1.substr(0, i);
        string c1 = s1.substr(i, 1);
        string v1 = s1.substr(i + 1, s1.length() - i - 1);
        string w2 = s2.substr(0, i);
        string c2 = s2.substr(i, 1);
        string v2 = s2.substr(i + 1, s2.length() - i - 1);
        if (w1 == w2 && v1 == v2) {
            return true;
        }
    }
    return false;
}

// BITSET BASED IMPLEMENTATION
/**********************************************************/
bool bitset_check_simp(bitset<MAXSIZE> b1, bitset<MAXSIZE> b2) {
    // compute xor, return true if and only if sum of all resulting bits <= 2
    bitset<MAXSIZE> b = b1 ^ b2;
    int count = b.count();
    if (count < 2) {    
        return true;
    }
    if (count == 2) {
        int first_set_bit = b._Find_first();
        int second_set_bit = b._Find_next(first_set_bit);
        if ((first_set_bit + second_set_bit) % 4 == 1) {
            return true;
        }
    }
    return false;
}


int main(int argc, char** argv) {
    const int n = 100;
    int iterations = 1000000;

    // initialize b1, b2, s1, s2, v1, v2 all to be the same
    bitset<MAXSIZE> b1, b2; 
    string s1 = string(n/2, '0');
    string s2 = string(n/2, '0');
    // b1.flip(50);
    // s1[25] = '1';

    // print out initial values and lengths
    cout << "b1: " << b1 << endl;
    cout << "b2: " << b2 << endl;
    cout << "s1: " << s1 << endl;
    cout << "s2: " << s2 << endl;
    cout << "bitvector length: " << b1.size() << endl;
    cout << "string length: " << s1.length() << endl << endl;

    // Compare performance of bitwise_and
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) { 
        b1 = b1 & b2; // bitwise and 
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    float bitset_time = duration.count() / 1000000.0;
    cout << "bitwise_and Using bitset: " << bitset_time << endl;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        s1 = string_intersect(s1, s2); // bitwise and
    }
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    float string_time = duration.count() / 1000000.0;
    cout << "bitwise_and Using string: " << string_time << endl;
    cout << "Speedup: " << string_time / bitset_time << endl << endl; 

    // Compare performance of check_simp
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        bitset_check_simp(b1, b2);
    }
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    bitset_time = duration.count() / 1000000.0;
    cout << "check_simp Using bitset: " << bitset_time << endl;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        string_check_simp(s1, s2);
    }
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    string_time = duration.count() / 1000000.0;
    cout << "check_simp Using string: " << string_time << endl;
    cout << "Speedup: " << string_time / bitset_time << endl << endl;


    return 0; 
}
