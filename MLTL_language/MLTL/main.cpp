#include <iostream>
#include "utils.h"

using namespace std;

int main() {

	cout << string_intersect("", "1ss, sss, sss, sss", 3) << endl;

	string s1[] = {"s1,     ss", "1s,      ss"};
	vector<string> v1 (s1, s1 + sizeof(s1) / sizeof(string) );
	string s2[] = {"10  ,  ss     ", "s1,  ss   "};
	vector<string> v2 (s2, s2 + sizeof(s2) / sizeof(string) );
	vector<string> v = set_intersect(v1, v2, 2);

	for (int i = 0; i < v.size(); ++i){
		cout << v[i] << endl;
	}

	return 0;
}
