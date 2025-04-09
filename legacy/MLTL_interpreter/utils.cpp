#include <string>
#include "utils.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <cctype>
#include <fstream>
#include <tuple>
#include <filesystem>


using namespace std;


/*
 * Input: string S
 *		 char C
 * Output: S with every instance of C removed
 */
string strip_char(string s, char c)
{
	string w = "";
	for (int j = 0; j < s.length(); ++j) {
		if (s[j] != c) {
			w += s[j];
		}
	}
	return w;
}


/*
 * Prints each element of a vector of strings on a new line
 */
void print(vector<string> v) {
	for (int i = 0; i < v.size(); ++i) {
		cout << v[i] << endl;
	}
}
void print(vector<int> v) {
	for (int i = 0; i < v.size(); ++i) {
		cout << v[i] << endl;
	}
}


/*
 * Removes element in INDEX from vector v
 */
template <typename T>
void remove(vector<T>& v, size_t index) {
	v.erase(v.begin() + index);
}


/*
* Removes duplicate entries from a vector.
* Mutates vector.
*/
template <typename T>
void remove_duplicates(vector<T>* reg_alpha) {
	// Convert vector to a set
	set<T> s((*reg_alpha).begin(), (*reg_alpha).end());
	// Assign set back to vector
	(*reg_alpha).assign(s.begin(), s.end());
	return;
}


/*
* Writes all elements of v to out, one item per line
*/
void write_to_file(vector<string> v, string out, bool size) {
	string line;
	ofstream outfile;
	outfile.open(out);
	if (size) {
		outfile << v.size() << endl;
	}
	for (string w : v) {
		outfile << w << endl;
	}
	outfile.close();
}


/*
* Read from file and return vector of strings
*/
vector<string> read_from_file(string in) {
    string line;
    ifstream infile;
    infile.open(in);
    vector<string> v;
    while (getline(infile, line)) {
        v.push_back(line);
    }
    infile.close();
    return v;
}

/*
Read a batch of traces from a directory and return a vector of vectors of strings
Run read_from_file on each file in the directory
Output: vector of NamedTrace
*/
vector<NamedTrace> read_batch_from_file(string in) {
	vector<NamedTrace> batch;
	// iterate over files in directory
	for (const auto & entry : filesystem::directory_iterator(in)) {
		// read from file
		vector<string> trace = read_from_file(entry.path());
		// create NamedTrace
		NamedTrace nt;
		nt.name = entry.path();
		nt.trace = trace;
		// add to batch
		batch.push_back(nt);
	}
	return batch;
}





/*
* Function to slice a given vector
* from range X to Y
*/ 
vector<string> slice(vector<string>& arr, int X, int Y)
{
 
    // Starting and Ending iterators
    auto start = arr.begin() + X;
    auto end = arr.begin() + Y;
 
    // To store the sliced vector
    vector<string> result(Y - X);
 
    // Copy vector using copy function()
    copy(start, end, result.begin());
 
    // Return the final sliced vector
    return result;
}