#pragma once
#include <algorithm>
#include <string>
//#include "utils.cpp"
#include <vector>

using namespace std;

//NOTE: error_check that sum of right bounds must be less than computation bounds

// Pad input string from right to specified length.
string pad_to_length(string unpadded_s, int length, int n) {
    // Compute remaining space to pad.
    int diff = int((length - unpadded_s.length()) / (n + 1));
    // Pad remaining space with ",ss...s"
    for (int i = 0; i < diff; i++) {
        unpadded_s += ',' + string(n, 's');
    }
    return unpadded_s;
}


// Pad a vector of strings so all strings have same length.
vector<string> pad(vector<string> unpadded_v, int n) {
  int unpadded_size = unpadded_v.size();

  // Compute max-length of strings
  int maxLength = 0;
  for (int i = 0; i < unpadded_size; ++i) {
    if (unpadded_v[i].length() > maxLength) {
        maxLength = int(unpadded_v[i].length());
    }
  }

  // Pad each string to maxLength
  vector<string> padded_v;
  for (int j = 0; j < unpadded_size; ++j) {
    padded_v.push_back(pad_to_length(unpadded_v[j], maxLength, n));
  }

  return padded_v;
}


// Returns the bit-wise 'and' of two strings.
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

  // Make w_1 and w_2 the same length.
  vector<string> vec = {w_1, w_2};
  pad(vec, n);

  // Bit-wise 'and' w_1 and w_2
  string w = "";
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


// Returns bit-wise 'and' of all strings of vector.
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


//entries in index counts how many char are remaining after removing consecutive sigmas at front
//parse from left, count from the right
//j is first non-sigma
vector<int> right_or_aux(vector<string> v, int n) {
    v = pad(v, n);
    int len_w = int(v[0].size());
    vector<int> indices;
    for (int i = 0; i < v.size(); ++i) {
        if (v[i][0] == 's') {
            for (int j = 1; j < len_w; ++j) {
                if (v[i][j] != 's') {
                    indices.push_back(len_w - j);
                    break;
                }
            }
        }
    }

  return indices;
}



vector<string> strip_commas(vector<string> comma_v) {
    cout << "TEST1" << endl;
    int size = comma_v.size();
    cout << size << endl;
    for (int i = 0; i < size; ++i) {
        cout << "TEST2" << endl;
        //string s = comma_v[i];
        cout << "input: " << comma_v[i] << endl;
        comma_v[i].erase(remove(comma_v[i].begin(), comma_v[i].end(), ','));
        cout << "output: " << comma_v[i] << endl;
    }
    return comma_v;
}

/*
vector<string> right_or(vector<string> v, int iteration, vector<int> indices, int n) {
    //strip commas before, or or write invariant_check
    int len_w = int(v[0].size());
    v = pad(v, n);
    for (int i = 0; i < indices.size(); ++i) {
        if (indices[i] == iteration) {
            string s_w = string(len_w, 's');
            if (find(v.begin(), v.end(), s_w) != v.end()) {
                vector<string> ret = {s_w};
                return ret;
            }
        }







}
}
 */


vector<string> set_union(vector<string> v1, vector<string> v2){
	vector<string> v = vector<string>();

	// Union all elements of v1,v2 into v
    /*
	for (int i = 0; i < v1.size(); ++i){
		for (int j = 0; j < v2.size(); ++j){
			string s = string_intersect(v1[i], v2[j], n);
			if (s != ""){
				v.push_back(s);
			}
		}
	}
     */




	// Remove all duplicate entries from v

	return v;
}
