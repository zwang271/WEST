#pragma once
#include<string>
#include "utils.cpp"

using namespace std;

string string_intersect(string w_1, string w_2, int n) {
	w_1.erase(std::remove_if(w_1.begin(),
		w_1.end(), ::isspace), w_1.end());
	w_2.erase(std::remove_if(w_2.begin(),
		w_2.end(), ::isspace), w_2.end());

	if (w_1 == "" || w_2 == "") {
		return "";
	}

	string w = "";
	if (w_1.length() > w_2.length()) {
		int diff = (w_1.length() - w_2.length()) / (n + 1);
		for (int i = 0; i < diff; i++) {
			w_2 += ',' + string(n, 's');
		}
	}
	else if (w_1.length() < w_2.length()) {
		int diff = (w_2.length() - w_1.length()) / (n + 1);
		for (int i = 0; i < diff; i++) {
			w_1 += ',' + string(n, 's');
		}
	}

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




