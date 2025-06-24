// Author: Zili Wang
// Last updated: 01/19/2024
// Parser for MLTL formulas

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <stdexcept>
#include <memory>
#include "utils.h"

using namespace std;

enum NodeType {
    PROP_VAR, // p1, p2, ...
    PROP_CONS, // true, false
    UNARY_CONN, // ! 
    UNARY_TEMP_CONN, // F, G
    BINARY_PROP_CONN, // &, |
    BINARY_TEMP_CONN, // U, R
    INTERVAL, // [a, b]
    PAREN_WFF, // (wff)
    INVALID, // invalid syntax
};

/*
Converts the NodeType to a string
*/
string node_type_to_string(NodeType type);

class SyntaxTree {
public:
    NodeType type;
    string op; // F, G, &, |, ->, U, R
    string formula; // a well formed formula
    string wff1_string; // a well formed formula
    string wff2_string; // a well formed formula
    int k; // an integer representing the propositional variable index
    int lb; // an integer representing the lower bound of an interval
    int ub; // an integer representing the upper bound of an interval
    string ERROR_MESSAGE; // error message
    unique_ptr<SyntaxTree> wff1 = nullptr; // a pointer to a well formed formula
    unique_ptr<SyntaxTree> wff2 = nullptr; // a pointer to a well formed formula
    int level; // the level of the node in the syntax tree

    // constructor
    SyntaxTree(string wff, int level = 0);

    // destructor
    ~SyntaxTree() = default;

    // prints all the nodes in the syntax tree
    void print();
};


