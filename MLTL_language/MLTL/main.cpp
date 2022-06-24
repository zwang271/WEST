#include <iostream>
#include "utils.h"
#include <vector>

using namespace std;

int main() {
    
    /*
	cout << string_intersect("", "1ss, sss, sss, sss", 3) << endl;
    
	string s1[] = {"s1,     ss", "1s,      ss"};
	vector<string> v1 (s1, s1 + sizeof(s1) / sizeof(string) );
	string s2[] = {"10  ,  ss     ", "s1,  ss   "};
	vector<string> v2 (s2, s2 + sizeof(s2) / sizeof(string) );
	vector<string> v = set_intersect(v1, v2, 2);

	for (int i = 0; i < v.size(); ++i){
		cout << v[i] << endl;
	}
    */
    
    vector<string> vec;
    vec.push_back("0sssss");
    vec.push_back("s0ssss000000");
    vec.push_back("ss0");
    vector<int> or_vector;
    or_vector = right_or_aux(vec, 3);
    /*
    for (int i = 0; i < or_vector.size(); ++i) {
        cout << or_vector[i] << endl;
    }
     
    for (int i = 0; i < vec.size(); ++i) {
        cout << pad(vec, 3)[i] << endl;
    }
     */
    vec = add_commas(vec, 3);
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << endl;
    }
    
    
    
     
    /*
    vector<string> vec;
    vec.push_back("sss1011s1");
    vector<int> or_vector;
    or_vector = right_or_aux(vec);
    for (int i = 0; i < or_vector.size(); ++i) {
        cout << or_vector[i] << endl;
    }

    string s = "";
    cout << pad_to_length(s, 19, 0) << endl;
    cout << pad_to_length(s, 19, 0).length();
     */
	return 0;
}

