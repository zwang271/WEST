//
//  utils_tests.cpp
//  tempLogic
//
//  Created by Jenna Elwing on 6/24/22.
//

#include <stdio.h>
#include "utils.h"
#include "unit_test_framework.h"
#include "grammar.h"
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
    vec.push_back("sss,101,1s1");
    vector<int> or_vector;
    or_vector = right_or_aux(vec, 9);
    for (int i = 0; i < or_vector.size(); ++i) {
        cout << or_vector[i] << endl;
    }
}

TEST(test_right_or_1) {
    vector<string> vec = {"0", "1"};
    vector<string> or_vector;
    or_vector = right_or(vec, 0, right_or_aux(vec, 1), 1);
    vector<string> answer = { "s" };
    for (int i = 0; i < or_vector.size(); ++i) {
        ASSERT_EQUAL(or_vector[i], answer[i]);
    }
}

TEST(test_right_or_2) {
    vector<string> vec = { "0,1", "1,s" };
    vector<string> or_vector;
    or_vector = right_or(vec, 0, right_or_aux(vec, 1), 1);
    vector<string> answer = { "1,0", "s,1"};
    for (int i = 0; i < or_vector.size(); ++i) {
        ASSERT_EQUAL(or_vector[i], answer[i]);
    }
}


TEST(test_single_char_or_1) {
    vector<string> vec = { "0", "1" };
    vector<string> or_vector = single_char_or(vec);
    vector<string> answer = { "s" };
    for (int i = 0; i < answer.size(); ++i) {
        ASSERT_EQUAL(or_vector[i], answer[i]);
    }
}

TEST(test_single_char_or_2) {
    vector<string> vec = { "1", "1", "1", "1", "1"};
    vector<string> or_vector = single_char_or(vec);
    vector<string> answer = { "1" };
    for (int i = 0; i < answer.size(); ++i) {
        ASSERT_EQUAL(or_vector[i], answer[i]);
    }
}

TEST(test_single_char_or_3) {
    vector<string> vec = { "1", "1", "s", "0", "s" };
    vector<string> or_vector = single_char_or(vec);
    vector<string> answer = { "s" };
    for (int i = 0; i < answer.size(); ++i) {
        ASSERT_EQUAL(or_vector[i], answer[i]);
    }
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

TEST(WFF_check_global) {
    string f = "G[1,3](&[p0,p1,p2,p3])";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_nonsense) {
    string f = "hoagopsdw398wioags09 -8yt3pgwek;";
    ASSERT_FALSE(Wff_check(f));
}

TEST(WFF_check_global_until) {
    string f = "G[1,3]((p0U[1,6]p4)&(p1&(p2&p3)))";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_negation_1) {
    string f = "G[1,3]~p0";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_negation_2) {
    string f = "~p0";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_nested_negation) {
    string f = "(~(~p0U[1,2]~p1)=(p0R[1,2]p1))";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_binary_operator_parentheses) {
    string f = "~(~p0U[1,2]~p1)=(p0R[1,2]p1)";
    ASSERT_FALSE(Wff_check(f));
}

TEST(WFF_check_empty_string) {
    string f = "";
    ASSERT_FALSE(Wff_check(f));
}

TEST(WFF_check_double_negation) {
    string f = "~~p0";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_implies_1) {
    string f = "(p0>G[1,1]p1)";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_implies_2) {
    string f = "(p0=>G[1,1]p1)";
    ASSERT_FALSE(Wff_check(f));
}

TEST(WFF_check_oscillation) {
    string f = "G[1,10](&[(p0>G[1,1]~p0),(~p0>G[1,1]p0)])";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_true_in_binary_operator) {
    string f = "(TU[2,3]p0)";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_false_in_binary_operator) {
    string f = "(TR[2,3]F)";
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_induction_example) {
    string f = "( G[1,10](p0>G[1,1]p0) > (p0>G[1,10]p0) )";
    f = strip_char(f, ' ');
    ASSERT_TRUE(Wff_check(f));
}

TEST(WFF_check_whitespace_1) {
    string f = "p0 = ~p1";
    ASSERT_FALSE(Wff_check(f));
}

TEST(WFF_check_whitespace_2) {
    string f = "p0 > ~p1";
    ASSERT_FALSE(Wff_check(f));
}

TEST(WFF_check_whitespace_3) {
    string f = "p0 U ~p1";
    ASSERT_FALSE(Wff_check(f));
}

TEST(WFF_check_whitespace_4) {
    string f = "p0 R ~p1";
    ASSERT_FALSE(Wff_check(f));
}

TEST(WFF_check_unary_operator_parenthese) {
    string f = "~(~p1)";
    ASSERT_FALSE(Wff_check(f));
}

TEST(Comp_len_0) {
    string f = "((p0&p1)vp3)";
    ASSERT_EQUAL(Comp_len(f), 1);
}

// add 1 to last time step to get length
TEST(Comp_len_1) {
    string f = "G[1,3](&[p0,p1,p2,p3])";
    ASSERT_EQUAL(Comp_len(f), 4);
}

TEST(Comp_len_2) {
    string f = "G[1,3](&[(p0vp4),p1,p2,p3])";
    ASSERT_EQUAL(Wff_check(f), true);
    ASSERT_EQUAL(Comp_len(f), 4);
}

TEST(Comp_len_3) {
    string f = "G[1,3](&[(p0U[1,6]p4),p1,p2,p3])";
    ASSERT_EQUAL(Comp_len(f), 10);
}

TEST(Comp_len_induction) {
    string f = "( G[1,10](p0>G[1,1]p0) > (p0>G[1,10]p0) )";
    f = strip_char(f, ' ');
    ASSERT_EQUAL(Comp_len(f), 12);
}

TEST(Comp_len_oscillation) {
    string f = "G[1,10](&[(p0>G[1,1]~p0),(~p0>G[1,1]p0)])";
    ASSERT_EQUAL(Comp_len(f), 12);
}



TEST_MAIN()
