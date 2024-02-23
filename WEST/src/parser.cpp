// Author: Zili Wang
// Last updated: 01/19/2024
// Parser for MLTL formulas

#include "parser.h"

/*
Converts the NodeType to a string
*/
string node_type_to_string(NodeType type) {
    switch (type) {
        case PROP_VAR:
            return "PROP_VAR";
        case PROP_CONS:
            return "PROP_CONS";
        case UNARY_CONN:
            return "UNARY_CONN";
        case UNARY_TEMP_CONN:
            return "UNARY_TEMP_CONN";
        case BINARY_PROP_CONN:
            return "BINARY_PROP_CONN";
        case BINARY_TEMP_CONN:
            return "BINARY_TEMP_CONN";
        case INTERVAL:
            return "INTERVAL";
        case PAREN_WFF:
            return "PAREN_WFF";
        case INVALID:
            return "INVALID";
        default:
            return "INVALID";
    }
}

/*
Checks if a string is a digit
*/
bool is_digit(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

struct Bound {
    bool valid; // true if valid, false if invalid
    int lb; // lower bound
    int ub; // upper bound
    int lbrace; // index for left brace
    int comma; // index for comma
    int rbrace; // index for right brace
};

/*
Finds ub and lb of first occurence of interval bound in formula F
returns tuple of (lb, ub, lbrace, comma, rbrace)
*/
Bound find_bounds(string F) {
    Bound b;
    b.valid = false;
    b.lbrace = F.find('[');
    b.comma = F.find(',');
    b.rbrace = F.find(']');
    if (b.lbrace == string::npos || 
        b.comma == string::npos || 
        b.rbrace == string::npos) {
        return b;
    }
    string lb = F.substr(b.lbrace+1, b.comma-b.lbrace-1);
    string ub = F.substr(b.comma+1, b.rbrace-b.comma-1);
    if (!is_digit(lb) || !is_digit(ub)) {
        return b;
    }
    b.lb = stoi(lb);
    b.ub = stoi(ub);
    if (b.lb > b.ub) {
        return b;
    }
    b.valid = true;
    return b;
}

/*
Finds index of binary connective in formula F
*/
int find_binary_conn(string F) {
    int binary_conn_index = -1;
    int pcounter = 0;
    for (int i = 1; i < F.length()-1; ++i) {
        if (F[i] == '(') {
            ++pcounter;
        } else if (F[i] == ')') {
            --pcounter;
        }
        if (pcounter == 0) {
            if (F[i] == '|' || F[i] == '&' || F[i] == '-' || F[i] == '='
                            || F[i] == 'U' || F[i] == 'R') {
                binary_conn_index = i; 
                break;
            }
        }
    }
    return binary_conn_index;
}

/*
Constructor: builds a syntax tree from a well formed formula
*/
SyntaxTree::SyntaxTree(string wff, int level){
    type = INVALID;
    formula = wff;
    this->level = level;

    if (wff == "true" || wff == "false") {
        type = PROP_CONS;
    }

    else if (wff[0] == 'p') {
        if (wff.length() < 2) {
            // throw invalid_argument("Propositional variable index is missing.");
            ERROR_MESSAGE = "Propositional variable index is missing.";
            return;
        }
        string k = wff.substr(1, wff.length() - 1);
        if (!is_digit(k)) { 
            // throw invalid_argument("Propositional variable index is not valid.");
            ERROR_MESSAGE = "Propositional variable index is not valid.";
            return;
        }
        type = PROP_VAR;
        this->k = stoi(k);
    }

    else if (wff[0] == '!') {
        type = UNARY_CONN;
        op = "!";
        wff1_string = wff.substr(1, wff.length() - 1);
        wff1 = make_unique<SyntaxTree>(wff1_string, level+1);
    }

    else if (wff[0] == 'F' || wff[0] == 'G') {
        Bound b = find_bounds(wff);
        if (!b.valid) {
            // throw invalid_argument("Invalid interval bound.");
            ERROR_MESSAGE = "Invalid interval bound.";
            return;
        }
        type = UNARY_TEMP_CONN;
        op = wff[0];
        wff1_string = wff.substr(b.rbrace + 1, wff.length() - 1);
        wff1 = make_unique<SyntaxTree>(wff1_string, level+1);
        lb = b.lb;
        ub = b.ub;
    }

    else if (wff[0] == '(' && wff[wff.length()-1] == ')') {
        int binary_conn_index = find_binary_conn(wff);
        if (binary_conn_index == -1) { // no binary connective
            type = PAREN_WFF;
            wff1_string = wff.substr(1, wff.length()-2);
            wff1 = make_unique<SyntaxTree>(wff1_string, level+1);
        }

        else if (wff[binary_conn_index] == '|' 
                || wff[binary_conn_index] == '&'
                || wff[binary_conn_index] == '=') {
            type = BINARY_PROP_CONN;
            op = wff[binary_conn_index];
            wff1_string = wff.substr(1, binary_conn_index-1);
            wff2_string = wff.substr(binary_conn_index+1, wff.length()-2-binary_conn_index);
            wff1 = make_unique<SyntaxTree>(wff1_string, level+1);
            wff2 = make_unique<SyntaxTree>(wff2_string, level+1);
        }

        else if (wff[binary_conn_index] == '-' && wff[binary_conn_index+1] == '>') {
            type = BINARY_PROP_CONN;
            op = "->";
            wff1_string = wff.substr(1, binary_conn_index-1);
            wff2_string = wff.substr(binary_conn_index+2, wff.length()-3-binary_conn_index);
            wff1 = make_unique<SyntaxTree>(wff1_string, level+1);
            wff2 = make_unique<SyntaxTree>(wff2_string, level+1);
        }

        else if (wff[binary_conn_index] == 'U' || wff[binary_conn_index] == 'R') {
            // node.wff1 = wff.substr(1, binary_conn_index-1);
            wff1_string = wff.substr(1, binary_conn_index-1);
            op = wff[binary_conn_index];
            wff = wff.substr(binary_conn_index+1, wff.length()-2-binary_conn_index);
            Bound b = find_bounds(wff);
            if (!b.valid) {
                // throw invalid_argument("Invalid interval bound.");
                ERROR_MESSAGE = "Invalid interval bound.";
                return;
            }
            type = BINARY_TEMP_CONN;
            wff2_string = wff.substr(b.rbrace+1, wff.length()-1);
            wff1 = make_unique<SyntaxTree>(wff1_string, level+1);
            wff2 = make_unique<SyntaxTree>(wff2_string, level+1);
            lb = b.lb;
            ub = b.ub;
        }
    } 
    
    // throw invalid_argument("Invalid syntax.");
    ERROR_MESSAGE = "Invalid syntax.";
}

/*
Prints the syntax tree by doing dfs traversal
Indentation level is determined by the level of the node
*/
void SyntaxTree::print() {
    for (int i = 0; i < level; ++i) {
        cout << "  ";
    }
    cout << formula << endl;
    if (type == INVALID) {
        cout << "ERROR: " << ERROR_MESSAGE << endl;
        return;
    }

    if (wff1 != nullptr) {
        wff1->print();
    }
    if (wff2 != nullptr) {
        wff2->print();
    }
}
    
