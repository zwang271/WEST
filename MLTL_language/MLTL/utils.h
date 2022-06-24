#pragma once
#include <algorithm>
#include <string>
//#include "utils.cpp"
#include <vector>

using namespace std;

//NOTE: error_check that sum of right bounds must be less than computation bounds


/*
* Input: computation string UNPADDED_S separated by commas 
*		 LENGTH is target length to pad to
*		 N is number of propositional variables
* Output: computation string separated by commas of length LENGTH
*/
string pad_to_length(string unpadded_s, int length, int n) {
    int diff = int((length - unpadded_s.length()) / (n + 1));
    for (int i = 0; i < diff; i++) {
        unpadded_s += ',' + string(n, 's');
    }
    return unpadded_s;
}


/*
* Input: Vector of computation strings with commas
* Output: Pads all comutation strings to the same length as the longest string
*/
vector<string> pad(vector<string> unpadded_v, int n) {
    int maxLength = 0;
    for (int i = 0; i < unpadded_v.size(); ++i) {
        if (unpadded_v[i].length() > maxLength) {
            maxLength = int(unpadded_v[i].length());
        }
    }
    for (int j = 0; j < unpadded_v.size(); ++j) {
        unpadded_v[j] = pad_to_length(unpadded_v[j], maxLength, n);
    }
    
    vector<string> padded_v = unpadded_v;
    return padded_v;
}


/*
* Input: Two computation strings W_1 and W_2, comma separated 
*		 N is number of propositional variables
* Output: Bit-wise and of the two string of length max(len(w_1), len(w_2))
*/
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
    vector<string> vec = {w_1, w_2};
    pad(vec, n);
	string w = "";
	// Make w_2 same length as w_1
	if (w_1.length() > w_2.length()) {
		int diff = int((w_1.length() - w_2.length()) / (n + 1));
		for (int i = 0; i < diff; i++) {
			w_2 += ',' + string(n, 's');
		} 
	}
	// Make w_1 same length as w_2
	else if (w_1.length() < w_2.length()) {
		int diff = int((w_2.length() - w_1.length()) / (n + 1));
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


/*
* Input: Two vectors of computation strings V1 and V2, comma separated
* Output: Computes pairwise string_intersect between all 
*		  computation strings in V1 and V2
*/
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


/*
* Input: Vector of computation strings V, NO COMMAS
*	     N is number of propositional variables
* Info: Any computation string w is of the form w = s^k(0|1)^m
* Output: Array of indices that computes m for each string in V
*/
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


/*
* Input: Vector of computation strings with commas
* Output: Vector of computation strings without commas
*/
vector<string> strip_commas(vector<string> comma_v) {
    for (int i = 0; i < comma_v.size(); ++i) {
		int len_w = comma_v[i].length();
		string w = "";

		for (int j = 0; j < len_w; ++j) {
			if (comma_v[i][j] != ',') {
				w += comma_v[i][j];
			}
		}

		comma_v[i] = w;
    }
    return comma_v;
}

/*
* Input: Vector of computation strings without commas
*		 N is number of propositional variables
* Output: Vector of computation strings with commas
*/
vector<string> add_commas(vector<string> v, int n) {
	for (int i = 0; i < v.size(); i++) {
		int len_w = v[i].length();
		string w = "";
		for (int j = 0; j < len_w; j += n) {
			w += v[i].substr(j,n);
			if (j + n < len_w) {
				w += ',';
			}
		}
		v[i] = w;
	}
	return v;
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
