#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"

using namespace std;

// REST functions
// ***************************** 


/*
* Input: vector of strings representing regular expressions
* Output: returns the arbitrary computation if regexp satisfies REST, otherwise returns regexp
*/
vector<string> REST(vector<string> regexp) {
	if (regexp.size() == 0) {
		return {};
	}
	
	int n = regexp.size() - 1;
	if (regexp[0].length() != n) {
		cout << "cannot use REST" << endl;
		return {};
	}

	// counter is used to check condition for REST theorem
	vector<vector<int>> counter;
	for (int i = 0; i < n; i++) {
		// vec[0], vec[1], vec[2] counts the number of '0', '1', and 's', respectively
		vector<int> vec = { 0, 0, 0 };
		counter.push_back(vec);
	}

	for (int row = 0; row < n+1; row++) {
		for (int col = 0; col < n; col++) {
			char val = regexp[row][col];
			if (val == '0') {
				counter[col][0] += 1; 
			}
			else if (val == '1') {
				counter[col][1] += 1;
			}
			else if (val == 's') {
				counter[col][2] += 1; 
			}
		}
	}

	// v represents the desired column in which there is one '0', one '1', and n-1 's'
	vector<int> v = { 1, 1, n - 1 };

	// Debugging information 
	/*for (int i = 0; i < n; i++) {
		for (int j = 0; j < 3; j++) {
			cout << counter[i][j];
		}
		cout << endl; 
	}*/

	for (int col = 0; col < n; col++) {
		if (counter[col] != v) {
			return regexp;
		}
	}

	return { string(n, 's')};
}


/*
* Input: integers n and r
* Output: vector of all r combinations from the set [0, ..., n-1]
*/
vector<vector<int>> combinations(int n, int r) {
	vector<bool> v(n);
	fill(v.end() - r, v.end(), true);

	vector<vector<int>> all_comb = {};

	do {
		vector<int> comb = {};
		for (int i = 0; i < n; i++) {
			if (v[i]) {
				comb.push_back(i);
			}
		}
		all_comb.push_back(comb);
	} while (next_permutation(v.begin(), v.end()));

	// Debugging info
	/*cout << "n = " << n << ", r = " << r << endl;
	int size = 0;
	for (vector<int> comb : all_comb) {
		for (int x: comb) {
			cout << x << " ";
		}
		cout << endl;
		size++;
	}
	cout << "number of combinations is " << size << endl; */

	return all_comb;
}


/*
* Input: vector of regular expression strings
* Output: returns the simplified input after checking for subsets
* 
* Example: simplify_subsets({ss1, 1s1}) = {ss1}
*/
vector<string> simplify_subsets(vector<string> regexp) {
	if (regexp.size() <= 1) {
		return regexp;
	}

	start: 
	int m = regexp.size();
	int n = regexp[0].length();
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < m; j++) {
			if (i == j) {
				continue; 
			}

			// Check if regexp[i] is a subset of regexp[j]
			bool is_subset = true; 
			for (int col = 0; col < n; col++) {
				if (regexp[i][col] != 's') { // regexp[i][col] is '0' or '1'
					if (regexp[i][col] != regexp[j][col] and regexp[j][col] != 's') {
						is_subset = false; 
						break; 
					}
				}
				else if (regexp[j][col] != 's') { // regexp[i][col] is 's'
					is_subset = false; 
					break;
				}
			}

			if (is_subset) {
				regexp.erase(regexp.begin() + i);
				goto start; 
			}
		}
	}

	return regexp;
}


vector<string> REST_simplify(vector<string> regexp) {
	if (regexp.size() == 0) {
		return {};
	}

	if (regexp.size() == 1) {
		return regexp;
	}

    if (regexp.size() == 2) {
        if ((regexp[0] == "1" && regexp[1] == "0") || (regexp[0] == "0" && regexp[1] == "1")) {
            return {"s"};
        }
    }



start:
	int m = regexp.size(); // number of strings in regexp
	int n = regexp[0].length(); // length of each string in regexp

	// cout << "m=" << m << " n=" << n << endl;

	for (int r = 3; r <= min(m, n + 1); r++) {
		vector<vector<int>> all_combs = combinations(m, r);
		for (vector<int>comb : all_combs) {
			
			vector<string> v = {}; // v is an r-subset of regular expression strings of regexp
			for (int index : comb) {
				v.push_back(regexp[index]);
			}

			/*print(v);
			cout << endl; */

			vector<int> diff_col_v = {}; // indices of the columns of v that aren't equal
			for (int col = 0; col < n; col++) {
				string first_string = v[0];
				for (int i = 1; i < r; i++) {
					if (v[i][col] != first_string[col]) {
						diff_col_v.push_back(col);
						break;
					}
				}
				if (diff_col_v.size() > r - 1) {
					break; 
				}
			}

			/*for (int col : diff_col_v) {
				cout << col << " ";
			}
			cout << endl; */

			if (diff_col_v.size() == r - 1) {
				// Now extract the r-1 different columns from the r rows and check REST
				vector<string> rest_regexp = {};
				for (int i = 0; i < r; i++) {
					string temp = "";
					for (int col : diff_col_v) {
						temp += v[i][col];
					}
					rest_regexp.push_back(temp);
				}
				rest_regexp = REST(rest_regexp);

				/*print(rest_regexp);
				cout << endl; */

				if (rest_regexp.size() == 1) {
					// Simplify first occurence
					for (int col : diff_col_v) {
						regexp[comb[0]][col] = 's';
					}

					// Delete all the rest from 
					for (int comb_index = comb.size() - 1; comb_index > 0; comb_index--) {
						int index = comb[comb_index];
						regexp.erase(regexp.begin() + index);
					}
					goto start; 
				}
			}
		}
	}

	regexp = simplify_subsets(regexp);
	return regexp;
}   


// *****************************

/*int main() {
	bool running = true;
	vector<string> answer;
	vector<string> display;
	string wff = "";


	// vector<string> regexp = { "1,0,1", "s,1,s", "1,s,0", "0,s,s" };

	// vector<string> regexp = { "1,1,1", "0,s,s", "s,0,s", "s,s,0" };

	*//*vector<string> regexp = {
		"0,1,s,1,s,0,s,s,1",
		"0,s,1,1,s,0,s,s,1",
		"0,s,s,1,s,0,1,s,1",
		"0,s,s,1,s,0,s,1,1",
		"0,0,0,1,s,0,0,0,1"
	};*//*

	vector<string> regexp = {

	};

	cout << endl; 

	regexp = simplify(regexp, 1);
	// print(regexp);

	regexp = strip_commas(regexp);
	regexp = REST_simplify(regexp);
	regexp = add_commas(regexp, 2);
	cout << endl; 


	cout << "size after simplifying with REST: " << expand(regexp).size() << endl;
	print(regexp);
	cout << endl;

	return 0; 
}*/
