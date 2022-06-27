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
#include "reg.h"

using namespace std;

//TESTS FOR UTILS.CPP

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

// I have no idea what this is
/*
TEST(test_string_intersect_1) {
    //cout << string_intersect("", "1ss, sss, sss, sss", 3) << endl;
    
    string s1[] = {"s1,     ss", "1s,      ss"};
    vector<string> v1 (s1, s1 + sizeof(s1) / sizeof(string) );
    string s2[] = {"10  ,  ss     ", "s1,  ss   "};
    vector<string> v2 (s2, s2 + sizeof(s2) / sizeof(string) );
    vector<string> v = set_intersect(v1, v2, 2);

    for (int i = 0; i < v.size(); ++i){
        cout << v[i] << endl;
    }
}
*/


TEST(test_pad_1) {
    vector<string> vec;
    vec.push_back("0ss,sss");
    vec.push_back("s0s,sss,000");
    vec.push_back("ss0");
    vector<int> or_vector;
    or_vector = right_or_aux(vec, 3);
     
    for (int i = 0; i < vec.size(); ++i) {
        cout << pad(vec, 3, 3)[i] << endl;
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








//TESTS FOR REG.CPP

TEST(test_set_intersect_1) {
    vector<string> v1 = {"ss", "ss", "ss", "ss"};
    vector<string> v2 = {"ss", "ss", "ss", "ss"};
    vector<string> v_expected = {"ss", "ss", "ss", "ss","ss", "ss", "ss", "ss","ss", "ss", "ss", "ss","ss", "ss", "ss", "ss"};
    vector<string> v_actual = set_intersect(v1, v2, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_intersect_2) {
    vector<string> v1 = {"1", "0"};
    vector<string> v2 = {"0", "1"};
    vector<string> v_actual = set_intersect(v1, v2, 1);
    vector<string> v_expected = {"1", "0"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_intersect_3) {
    vector<string> v1 = {"1", "0"};
    vector<string> v2 = {"0", "1"};
    vector<string> v_actual = set_intersect(v1, v2, 1);
    vector<string> v_expected = {"1", "0"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_intersect_4) {
    vector<string> v1 = {"10", "11"};
    vector<string> v2 = {"00", "01"};
    vector<string> v_actual = set_intersect(v1, v2, 2);
    vector<string> v_expected = {};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_intersect_5) {
    vector<string> v1 = {"10", "11"};
    vector<string> v2 = {"10", "01"};
    vector<string> v_actual = set_intersect(v1, v2, 2);
    vector<string> v_expected = {"10"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_intersect_6) {
    vector<string> v1 = {"10", "s1"};
    vector<string> v2 = {"10", "01"};
    vector<string> v_actual = set_intersect(v1, v2, 2);
    vector<string> v_expected = {"10", "01"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}



TEST(test_set_union_1) {
    vector<string> v1 = {"10", "s1"};
    vector<string> v2 = {"10", "01"};
    vector<string> v_actual = set_union(v1, v2, 2);
    vector<string> v_expected = {"10", "s1"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_union_2) {
    vector<string> v1 = {"ss", "ss", "ss", "ss"};
    vector<string> v2 = {"ss", "ss", "ss", "ss"};
    vector<string> v_actual = set_union(v1, v2, 2);
    vector<string> v_expected = {"ss"};
    vector<string> v = join(v1, v2);
    vector<int> v_int = right_or_aux(v, 2);
    for (int i = 0; i < v_int.size(); ++i) {
        cout << v_int[i] << " ";
    }
    
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_union_3) {
    vector<string> v1 = {"10", "11"};
    vector<string> v2 = {"00", "01"};
    vector<string> v_actual = set_union(v1, v2, 2);
    vector<string> v_expected = {"s0", "s1"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_union_4) {
    vector<string> v1 = {"ss"};
    vector<string> v2 = {"ss"};
    vector<string> v_actual = set_union(v1, v2, 2);
    vector<string> v_expected = {"ss"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_union_5) {
    vector<string> v1 = {"10", "11"};
    vector<string> v2 = {"00"};
    vector<string> v_actual = set_union(v1, v2, 2);
    vector<string> v_expected = {"s0", "11"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_set_union_6) {
    vector<string> v1 = {"10", "11", "11"};
    vector<string> v2 = {"00"};
    vector<string> v_actual = set_union(v1, v2, 2);
    vector<string> v_expected = {"s0", "11"};
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_prop_cons_1) {
    string t = "T";
    vector<string> v_expected = {"ss"};
    vector<string> v_actual = reg_prop_cons(t, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_prop_cons_2) {
    string t = "F";
    vector<string> v_expected = {};
    vector<string> v_actual = reg_prop_cons(t, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_prop_cons_3) {
    string t = "T";
    vector<string> v_expected = vector<string>();
    vector<string> v_actual = reg_prop_cons(t, 0);
    
    ASSERT_EQUAL(v_expected.size(), v_actual.size());
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(reg_prop_var_1) {
    string t = "p0";
    vector<string> v_expected = {"1s"};
    vector<string> v_actual = reg_prop_var(t, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(reg_prop_var_2) {
    string t = "p1";
    vector<string> v_expected = {"s1"};
    vector<string> v_actual = reg_prop_var(t, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(reg_prop_var_3) {
    string t = "p1";
    vector<string> v_expected = {"s1s"};
    vector<string> v_actual = reg_prop_var(t, 3);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(reg_prop_var_4) {
    string t = "~p1";
    vector<string> v_expected = {"s0s"};
    vector<string> v_actual = reg_prop_var(t, 3);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(reg_prop_var_5) {
    string t = "p0";
    vector<string> v_expected = {"1"};
    vector<string> v_actual = reg_prop_var(t, 1);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(reg_prop_var_6) {
    string t = "~p0";
    vector<string> v_expected = {"0"};
    vector<string> v_actual = reg_prop_var(t, 1);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_basic) {
    string s = "F[2,4]p1";
    vector<string> v_expected = {"ss,ss,s1", "ss,ss,ss,s1", "ss,ss,ss,ss,s1"};
    vector<string> v_actual = reg(s, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_a_equals_b) {
    string s = "F[2,2]p0";
    vector<string> v_expected = {"s,s,1"};
    vector<string> v_actual = reg(s, 1);
    
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_finally_negation) {
    string s = "F[2,2]~p0";
    vector<string> v_expected = {"s,s,0"};
    vector<string> v_actual = reg(s, 1);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_two_props) {
    string s = "F[2,2]p0";
    vector<string> v_expected = {"ss,ss,1s"};
    vector<string> v_actual = reg(s, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_zero_props) {
    string s = "F[0,2]T";
    vector<string> v_expected = {};
    vector<string> v_actual = reg(s, 0);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_and_1) {
    string s = "F[0,2](&[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    
    vector<string> v_expected = {"ss,ss,11", "ss,ss,ss,11", "ss,ss,ss,ss,11"};
    vector<string> v_actual = reg(s, 2);
    //print(reg(s, 2));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_and_2) {
    string s = "F[0,2](&[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"sss,sss,11s", "sss,sss,sss,11s", "sss,sss,sss,sss,11s"};
    vector<string> v_actual = reg(s, 3);
    //print(reg(s, 3));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_or_1) {
    string s = "F[2,4](v[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"ss,ss,s1", "ss,ss,ss,s1", "ss,ss,ss,ss,s1",
                                 "ss,ss,1s", "ss,ss,ss,1s", "ss,ss,ss,ss,1s"};
    
    vector<string> v_actual = reg(s, 2);
    //print(reg(s, 2));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_or_2) {
    string s = "F[0,2](v[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"sss,sss,11s", "sss,sss,sss,11s", "sss,sss,sss,sss,11s"};
    vector<string> v_actual = reg(s, 3);
    //print(reg(s, 3));
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_global_basic) {
    
}



TEST_MAIN()
