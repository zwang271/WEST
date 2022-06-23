#pragma once
#include <algorithm>
#include <string>
//#include "utils.cpp"
#include <vector>

using namespace std;

string string_intersect(string w_1, string w_2, int n) {
	// Remove white-characters from w_1 and w_2
	w_1.erase(remove_if(w_1.begin(),
						w_1.end(), ::isspace), w_1.end());
	w_2.erase(remove_if(w_2.begin(),
						w_2.end(), ::isspace), w_2.end());

	// If either w_1 or w_2 are empty, return empty.
	if (w_1 == "" || w_2 == "") {
		return "";
	}

	string w = "";
	// Make w_2 same length as w_1
	if (w_1.length() > w_2.length()) {
		int diff = (w_1.length() - w_2.length()) / (n + 1);
		for (int i = 0; i < diff; i++) {
			w_2 += ',' + string(n, 's');
		}
	}
	// Make w_1 same length as w_2
	else if (w_1.length() < w_2.length()) {
		int diff = (w_2.length() - w_1.length()) / (n + 1);
		for (int i = 0; i < diff; i++) {
			w_1 += ',' + string(n, 's');
		}
	}
	// Now w_1.length() == w_2.length()

  // Bit-wise 'and' w_1 and w_2
	for (int i = 0; i < w_1.length(); i++) {
		if (w_1[i] != 's' and w_2[i] != 's') {
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


vector<string> set_intersect(vector<string> v1, vector<string> v2, int n){
	vector<string> v = vector<string>();

	// Bit-wise 'and' all entries of v1 and v2
	for (int i = 0; i < v1.size(); ++i){
		for (int j = 0; j < v2.size(); ++j){
			string s = string_intersect(v1[i], v2[j], n);
			if (s != ""){
				v.push_back(s);
			}
		}
	}

	return v;
}


vector<string> set_union(vector<string> v1, vector<string> v2){
	vector<string> v = vector<string>();

	// Union all elements of v1,v2 into v
	for (int i = 0; i < v1.size(); ++i){
		for (int j = 0; j < v2.size(); ++j){
			string s = string_intersect(v1[i], v2[j], n);
			if (s != ""){
				v.push_back(s);
			}
		}
	}


	// Remove all duplicate entries from v

	return v;
}
