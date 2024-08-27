#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <tuple>

using namespace std;

/*
 * Input: string S
 *		 char C
 * Output: S with every instance of C removed
 */
string strip_char(string s, char c);


/*
 * Prints each element of a vector of strings on a new line
 */
void print(vector<string> v);
void print(vector<int> v);


/*
 * Removes element in INDEX from vector v
 */
template <typename T>
void remove(vector<T>& v, size_t index);


/*
* Removes duplicate entries from a vector.
* Mutates vector.
*/
template <typename T>
void remove_duplicates(vector<T>* reg_alpha);


/*
* Writes all elements of v to out, one item per line
*/
void write_to_file(vector<string> v, string out, bool size = true);


/*
* Read from file and return vector of strings
*/
vector<string> read_from_file(string in);


struct NamedTrace {
	string name;
	vector<string> trace;
};
/*
Read a batch of traces form a file and return a vector of vectors of strings
*/
vector<NamedTrace> read_batch_from_file(string in);


/*
* Function to slice a given vector
* from range X to Y
*/ 
vector<string> slice(vector<string>& arr, int X, int Y);