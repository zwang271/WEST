#include <stdio.h>
#include "utils.h"
#include "unit_test_framework.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include <iostream>
#include "reg.h"

using namespace std;

// UNIT FUNCTIONAL TEST CASES

// we might need to specify that minimum 1 prop_var
// should be specified even when there are none in the formula
TEST(basic_true) {
    string s = "T";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 1);
    vector<string> v_expected = {"s"};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_false) {
    string s = "!";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 1);
    vector<string> v_expected = {};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_single_prop_var) {
    string s = "p0";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 1);
    vector<string> v_expected = {"1"};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_single_negated_prop_var) {
    string s = "~p0";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 1);
    vector<string> v_expected = {"0"};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_finally) {
    string s = "F[2:4]p1";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"ss,ss,s1", "ss,ss,ss,s1", "ss,ss,ss,ss,s1"};
    vector<string> v_actual = reg(s, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_global) {
    string s = "G[0:1]p0";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"1,1"};
    vector<string> v_actual = reg(s, 1);
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_or) {
    string s = "(v[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"1s", "s1"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_and) {
    string s = "(&[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"11"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_until) {
    string s = "(p0U[0:1]p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"s1", "1s,s1"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(basic_release) {
    string s = "(p0R[0:1]p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 2);
    vector<string> v_expected = {"s1,s1", "11"};
    ASSERT_EQUAL(v_expected, v_actual);
}






//// EDGE CASE TESTS

TEST(test_p_equals_p) {
    string s = "(p0=p0)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = simplify(reg(s, 1), 1);
    vector<string> v_expected = {"s"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_true_equals_false) {
    string s = "(T=!)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 0);
    vector<string> v_expected = {};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_p0_equals_p1) {
    string s = "(p0=p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 2);
    vector<string> v_expected = { "11", "00"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_assoc_and_0) {
    int n = 2;
    string s = "(&[p0])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_reg = reg(s, n);
    vector<string> v_actual = { "1s" };
    ASSERT_EQUAL(v_reg, v_actual)
}

TEST(test_assoc_and_1) {
    int n = 2;
    string s = "(&[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_reg = reg(s, n);
    vector<string> v_actual = { "11" };
    ASSERT_EQUAL(v_reg, v_actual)
}



TEST(test_finally_a_equals_b) {
    string s = "F[2:2]p0";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"s,s,1"};
    vector<string> v_actual = reg(s, 1);
    
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_finally_negation) {
    string s = "F[2:2]~p0";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"s,s,0"};
    vector<string> v_actual = reg(s, 1);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_two_props) {
    string s = "F[2:2]p0";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"ss,ss,1s"};
    vector<string> v_actual = reg(s, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_zero_props) {
    string s = "F[0:2]T";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {};
    vector<string> v_actual = reg(s, 0);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_and_1) {
    string s = "F[0:2](&[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    
    vector<string> v_expected = {"11", "ss,11", "ss,ss,11"};
    vector<string> v_actual = reg(s, 2);
    //print(reg(s, 2));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_and_2) {
    string s = "F[0:2](&[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"11s", "sss,11s", "sss,sss,11s"};
    vector<string> v_actual = reg(s, 3);
    //print(reg(s, 3));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_or_1) {
    string s = "F[2:4](v[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"ss,ss,1s", "ss,ss,s1", "ss,ss,ss,1s", "ss,ss,ss,s1", "ss,ss,ss,ss,1s", "ss,ss,ss,ss,s1"};
    vector<string> v_actual = reg(s, 2);
    //print(reg(s, 2));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_or_2) {
    string s = "F[0:2](v[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"1ss", "s1s", "sss,1ss", "sss,s1s", "sss,sss,1ss", "sss,sss,s1s"};
    vector<string> v_actual = reg(s, 3);
    //print(reg(s, 3));
    ASSERT_EQUAL(v_expected, v_actual);
}



TEST(test_global_basic_2) {
    string s = "G[0:1]p0";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"1s,1s"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_global_with_or) {
    string s = "G[0:1](v[p1,p2])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"s1s,s1s", "s1s,ss1", "ss1,s1s", "ss1,ss1"};
    vector<string> v_actual = reg(s, 3);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_global_with_or_2) {
    string s = "G[0:1](v[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"1s,1s", "1s,s1", "s1,1s", "s1,s1"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_global_with_and) {
    string s = "G[0:1](&[p1,p2])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"s11,s11"};
    vector<string> v_actual = reg(s, 3);
    ASSERT_EQUAL(v_expected, v_actual);
}



TEST(test_until_many_props) {
    string s = "(p0U[0:1]p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 5);
    vector<string> v_expected = {"s1sss", "1ssss,s1sss"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_until_swapped_props) {
    string s = "(p1U[0:1]p0)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 2);
    vector<string> v_expected = {"1s", "s1,1s"};
    ASSERT_EQUAL(v_expected, v_actual);
}



TEST(test_until_nested_diff_complen) {
    string s = "((p0U[0:1]p1)U[0:2]p2)";
    int n = 3;
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, n);
    v_actual = simplify(v_actual, n);

    //print_all_representations(v_actual, n);

    vector<string> v_expected = {"ss1,sss,sss", "s1s,ss1,sss",
                    "1ss,s11,sss", "s1s,s1s,ss1", "s1s,1ss,s11", 
                    "1ss,s1s,ss1", "1ss,11s,s11"};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_p_until_p_until_q) {
    string s = "(p0U[0:1](p0U[0:1]p1))";
    //should return comps for (p0U[0,2]p1)
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = simplify(reg(s, 2), 2);
    vector<string> v_expected = {"s1,ss,ss", "1s,s1,ss", "1s,1s,s1"};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_and_with_until_diff_complen) {
    string s = "(&[(p0U[0:1]p1),(p0U[0:2]p1)])";
    int n = 2;
    //should return comps for (p0U[0,1]p1)
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, n);

    //print_all_representations(v_actual, n);

    v_actual = simplify(right_expand(v_actual, n), n);
    vector<string> v_expected = {"s1,ss,ss", "10,s1,ss"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_until_nested_same_complen) {
    string s1 = "(p0U[0:3](p0U[0:3]p1))";
    string s2 = "(p0U[0:6]p1)";
    ASSERT_TRUE(Wff_check(s1));
    ASSERT_TRUE(Nnf_check(s1));
    ASSERT_TRUE(Wff_check(s2));
    ASSERT_TRUE(Nnf_check(s2));
    vector<string> v1 = simplify(reg(s1, 2), 2);
    vector<string> v2 = simplify(reg(s2, 2), 2);
    ASSERT_EQUAL(v1, v2);
}

TEST(test_until_nested_a_not_zero) {
    string s1 = "(p0U[1:3](p0U[0:3]p1))";
    string s2 = "(p0U[1:6]p1)";
    ASSERT_TRUE(Wff_check(s1));
    ASSERT_TRUE(Nnf_check(s1));
    ASSERT_TRUE(Wff_check(s2));
    ASSERT_TRUE(Nnf_check(s2));
    vector<string> v1 = simplify(reg(s1, 2), 2);
    vector<string> v2 = simplify(reg(s2, 2), 2);
    ASSERT_EQUAL(v1, v2);
}

TEST(test_implies_basic) {
    string s = "(p0>p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = simplify(reg(s, 2), 2);
    vector<string> v_expected = {"0s", "s1"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_1) {
    string s = "G[0:1](&[(p0>G[1:1]~p0),(~p0>G[1:1]p0)])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = simplify(reg(s, 1), 1);
    vector<string> v_expected = {"0,1,0", "1,0,1"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_2) {
    string s = "G[0:2](&[(p0>G[1:1]~p0),(~p0>G[1:1]p0)])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = simplify(reg(s, 1), 1);
    vector<string> v_expected = {"0,1,0,1", "1,0,1,0"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_3) {
    string s = "G[0:0](&[(p0>G[1:1]~p0),(~p0>G[1:1]p0)])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = simplify(reg(s, 1), 1);
    vector<string> v_expected = {"0,1", "1,0"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_4) {
    string s = "G[0:0](&[(p0>G[1:1]~p0),(~p0>G[1:1]p0)])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = simplify(reg(s, 2), 2);
    vector<string> v_expected = {"0s,1s", "1s,0s"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_5) {
    string s = "G[0:1](&[(p1>G[1:1]~p1),(~p1>G[1:1]p1)])";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = simplify(reg(s, 2), 2);
    vector<string> v_expected = {"s0,s1,s0", "s1,s0,s1"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_induction) {
    string s = "(G[0:3](p0>G[1:1]p0)>(p0>G[0:4]p0))";
    int n = 1;
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, n);

    //print_all_representations(v_actual, n);

    v_actual = simplify(right_expand(v_actual, n), n);

    vector<string> v_expected = {"s,s,s,s,s"};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_equivalent_1) {
    string s = "(G[0:0]p0=F[0:0]p0)";
    int n = 1;
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, n);
    v_actual = simplify(v_actual, n);
    vector<string> v_expected = {"s"};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_equivalent_2) {
    string s = "(G[1:1]p0=F[1:1]p0)";
    int n = 1;
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, n);
    v_actual = simplify(v_actual, n);

    vector<string> v_expected = {"s,s"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_intuitive_equivalence_1) {
    string s1 = "G[1:1]p0";
    string s2 = "F[1:1]p0";
    ASSERT_TRUE(Wff_check(s1));
    ASSERT_TRUE(Nnf_check(s1));
    ASSERT_TRUE(Wff_check(s2));
    ASSERT_TRUE(Nnf_check(s2));
    vector<string> v1 = reg(s1, 1);
    vector<string> v2 = reg(s2, 1);
    /*print(v1);
    cout << endl;
    print(v2);*/
    ASSERT_EQUAL(v1, v2);
}

TEST(test_intuitive_equivalence_2) {
    string s1 = "G[0:0]p0";
    string s2 = "F[0:0]p0";
    ASSERT_TRUE(Wff_check(s1));
    ASSERT_TRUE(Nnf_check(s1));
    ASSERT_TRUE(Wff_check(s2));
    ASSERT_TRUE(Nnf_check(s2));
    vector<string> v1 = reg(s1, 1);
    vector<string> v2 = reg(s2, 1);
   /* print(v1);
    cout << endl;
    print(v2);*/
    ASSERT_EQUAL(v1, v2);
}


TEST(test_finally_and_release_1) {
    string s = "F[0:1](p0R[0:1]p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 2);
    //print(v_actual);
    vector<string> v_expected = {"s1,s1", "11", "ss,s1,s1", "ss,11"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_and_release_2) {
    string s = "F[1:2](p0R[0:1]p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, 2);
    /*print(v_actual);*/
    vector<string> v_expected = {"ss,s1,s1", "ss,11", "ss,ss,s1,s1", "ss,ss,11"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_and_until_1) {
    string s = "F[0:1](p0U[0:1]p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"s1", "1s,s1", "ss,s1", "ss,1s,s1"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_and_until_2) {
    string s = "F[1:2](p0U[0:1]p1)";
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_expected = {"ss,s1", "ss,1s,s1", "ss,ss,s1", "ss,ss,1s,s1"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_global_and_release) {
    string s = "G[1:1](p0R[0:1]p1)";
    int n = 2;
    ASSERT_TRUE(Wff_check(s));
    ASSERT_TRUE(Nnf_check(s));
    vector<string> v_actual = reg(s, n);
    
    //print_all_representations(v_actual, n);

    vector<string> v_expected = {"ss,s1,s1", "ss,11"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_chiara_dual_release_equivalence_check){
    string a = "( (v[G[1:3]p1, (G[1:1]p0 & G[1:1]p1), (G[2:2]p0 & G[1:2]p1), (G[3:3]p0 & G[1:3]p1)])";
	string b = "= (&[(F[1:0]p0 v G[1:1]p1), (F[1:1]p0 v G[2:2]p1), (F[1:3]p0 v G[3:3]p1)]) )";
	string s1 = strip_char(a + b, ' ');
	/*cout << s1 << endl;
	cout << "Wff_check: " << Wff_check(s1) << endl;
	cout << "Nnf_check: " << Nnf_check(s1) << endl;*/

	vector<string> reg_s1 = reg(s1, 2);
	//print_all_representations(reg_s1, 2);

	s1 = "(v[G[1:3]p1, (G[1:1]p0 & G[1:1]p1), (G[2:2]p0 & G[1:2]p1), (G[3:3]p0 & G[1:3]p1)])";
	string s2 = "(&[(F[1:0]p0 v G[1:1]p1), (F[1:1]p0 v G[2:2]p1), (F[1:3]p0 v G[3:3]p1)])";
	s1 = strip_char(s1, ' ');
	s2 = strip_char(s2, ' ');
	/*cout << "Wff_check: " << Wff_check(s1) << endl;
	cout << "Nnf_check: " << Nnf_check(s1) << endl;
	cout << "Wff_check: " << Wff_check(s2) << endl;
	cout << "Nnf_check: " << Nnf_check(s2) << endl;
	cout << "Comp_len(s1): " << Comp_len(s1) << endl;
	cout << "Comp_len(s2): " << Comp_len(s2) << endl; */
}

TEST(test_wff_to_nnf_1) {
    string s = "p0";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_2) {
    string s = "~p0";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_3) {
    string s = "T";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_4) {
    string s = "!";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_5) {
    string s = "~~p1";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_F_1) {
    string s = "F[0:1]p0";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_F_2) {
    string s = "F[0:1]~p0";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_F_3) {
    string s = "~F[0:1]p0";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_G_1) {
    string s = "G[0:1]p0";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_G_2) {
    string s = "G[0:1]~p0";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_G_3) {
    string s = "~G[0:1]p0";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_or_1) {
    string s = "(~p0v~p1)";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_or_2) {
    string s = "~(~p0v~p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_or_3) {
    string s = "~(p0v~p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_or_4) {
    string s = "~(~p0vp1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_or_5) {
    string s = "~(p0vp1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_and_1) {
    string s = "(~p0&~p1)";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_and_2) {
    string s = "~(~p0&~p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_and_3) {
    string s = "~(p0&~p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_and_4) {
    string s = "~(~p0&p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_and_5) {
    string s = "~(p0&p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_equals_1) {
    string s = "(~p0=~p1)";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_equals_2) {
    string s = "~(~p0=~p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_equals_3) {
    string s = "~(p0=~p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_equals_4) {
    string s = "~(~p0=p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_nnf_equals_5) {
    string s = "~(p0=p1)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_implies_1) {
    string s = "(p1>p2)";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_implies_2) {
    string s = "(p1>~p2)";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_implies_3) {
    string s = "(~p1>p2)";
    ASSERT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_implies_4) {
    string s = "~(~p1>~p2)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_implies_5) {
    string s = "~(~p1>~p2)";
    ASSERT_NOT_EQUAL(Wff_to_Nnf(s), s);
}

TEST(test_wff_to_implies_6) {
    string not_nnf = "~(~p1>~p2)";
    string nnf = "(~p1&p2)";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_1) {
    string not_nnf = "~G[0:1]~p0";
    string nnf = "F[0:1]p0";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_2) {
    string not_nnf = "~F[0:1]~p0";
    string nnf = "G[0:1]p0";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_3) {
    string not_nnf = "~(~p0U[0:1]~p1)";
    string nnf = "(p0R[0:1]p1)";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_4) {
    string not_nnf = "~(~p0R[0:1]~p1)";
    string nnf = "(p0U[0:1]p1)";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_5) {
    string not_nnf = "~(p0U[0:1]p1)";
    string nnf = "(~p0R[0:1]~p1)";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_6) {
    string not_nnf = "~(p0R[0:1]p1)";
    string nnf = "(~p0U[0:1]~p1)";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_7) {
    string not_nnf = "~G[1:1]p0";
    string nnf = "F[1:1]~p0";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_8) {
    string not_nnf = "~F[1:1]p0";
    string nnf = "G[1:1]~p0";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_9) {
    string not_nnf = "~G[1:4]p0";
    string nnf = "F[1:4]~p0";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_wff_to_nnf_dual_10) {
    string not_nnf = "~F[1:4]p0";
    string nnf = "G[1:4]~p0";
    ASSERT_EQUAL(Wff_to_Nnf(not_nnf), nnf);
}

TEST(test_sum) {
    vector<string> v = {"1", "11"};
    ASSERT_TRUE(sum_of_characters(v) == 3);
}










TEST_MAIN()


