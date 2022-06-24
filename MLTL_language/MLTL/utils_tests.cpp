//
//  utils_tests.cpp
//  tempLogic
//
//  Created by Jenna Elwing on 6/24/22.
//

#include <stdio.h>
#include "utils.h"
#include "unit_test_framework.h"
#include <iostream>


using namespace std;

TEST(test_right_or_aux_1) {
    vector<string> vec;
    vec.push_back("0ss,sss");
    vec.push_back("s0s,sss,000");
    vec.push_back("ss0");
    vector<int> or_vector;
    or_vector = right_or_aux(vec, 3);
    
    for (int i = 0; i < or_vector.size(); ++i) {
        cout << or_vector[i] << endl;
    }
}

TEST(test_right_or_aux_2) {
    vector<string> vec;
    vec.push_back("sss1011s1");
    vector<int> or_vector;
    or_vector = right_or_aux(vec, 9);
    for (int i = 0; i < or_vector.size(); ++i) {
        cout << or_vector[i] << endl;
    }

}

TEST(test_right_or_1) {
    
}

TEST(test_string_intersect_1) {
    cout << string_intersect("", "1ss, sss, sss, sss", 3) << endl;
    
    string s1[] = {"s1,     ss", "1s,      ss"};
    vector<string> v1 (s1, s1 + sizeof(s1) / sizeof(string) );
    string s2[] = {"10  ,  ss     ", "s1,  ss   "};
    vector<string> v2 (s2, s2 + sizeof(s2) / sizeof(string) );
    vector<string> v = set_intersect(v1, v2, 2);

    for (int i = 0; i < v.size(); ++i){
        cout << v[i] << endl;
    }
}

TEST(test_pad_1) {
    vector<string> vec;
    vec.push_back("0ss,sss");
    vec.push_back("s0s,sss,000");
    vec.push_back("ss0");
    vector<int> or_vector;
    or_vector = right_or_aux(vec, 3);
     
    for (int i = 0; i < vec.size(); ++i) {
        cout << pad(vec, 3)[i] << endl;
    }
}

TEST(test_strip_commas_1) {
    vector<string> vec;
    vec.push_back("0ss,sss");
    vec.push_back("s0s,sss,000");
    vec.push_back("ss0");
    vector<int> or_vector;
    or_vector = right_or_aux(vec, 3);
   
    for (int i = 0; i < vec.size(); ++i) {
        cout << strip_commas(vec)[i] << " ";
    }
}

TEST(test_pad_to_length_1) {
    string s = "";
    string expected = ",,,,,,,,,,,,,,,,,,,";
    string actual = pad_to_length(s, 19, 0);
    ASSERT_EQUAL(expected, actual);
    ASSERT_EQUAL(expected.length(), actual.length());
}

TEST(test_pad_to_length_2) {
    string s = "1,0,0";
    string actual = pad_to_length(s, 9, 1);
    string expected = "1,0,0,s,s";
    
    ASSERT_EQUAL(expected, actual);
    ASSERT_EQUAL(expected.length(), actual.length());
}

TEST_MAIN()
