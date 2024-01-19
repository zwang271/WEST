#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <algorithm>
#include "utils.h"

using namespace std;

/* 
checks if inputted string is a number
*/
bool digit_check(string s) {
    for (int i = 0; i < s.length(); ++i) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

/*
checks if the inputted string is "p" followed by a digit
*/
bool prop_var_check(string s) {
    if (s.length() < 2) {
        return false;
    }
    if (s[0] != 'p') {
        return false;
    }
    if (!digit_check(s.substr(1, s.length()-1))) {
        return false;
    }
    return true;
}

/* 
finds ub and lb of first occurence of interval bound in formula F
*/
tuple<int, int> find_bounds(string F) {
    int lbrace = F.find('[');
    int comma = F.find(',');
    int rbrace = F.find(']');
    if (lbrace == string::npos || comma == string::npos || rbrace == string::npos) {
        throw invalid_argument("Formula " + F + " is not a valid MLTL formula.");
    }
    int lb = stoi(F.substr(lbrace+1, comma-lbrace-1));
    int ub = stoi(F.substr(comma+1, rbrace-comma-1));
    if (lb > ub) {
        throw invalid_argument("Formula " + F + " is not a valid MLTL formula.");
    }
    return make_tuple(lb, ub);
}

/*
finds index of binary connective in formula F
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
            if (F[i] == '|' || F[i] == '&' || F[i] == '-' || F[i] == 'U' || F[i] == 'R') {
                binary_conn_index = i; 
                break;
            }
        }
    }
    return binary_conn_index;
}

/*
 Input: MLTL formula F
        trace T
 Output: true if and only if F evaluates to true on F
 */
bool evaluate_mltl(string F, vector<string> T, bool verbose=false){
    // cout << F << endl;

    // Prop_cons -> true | false
    if (F == "true") {
        return true;
    } else if (F == "false") {
        return false;
    }

    // Prop_var -> 'p' Num
    else if (prop_var_check(F)) {
        int p = stoi(F.substr(1, F.length()-1));
        if (T.size() == 0) {
            return false;
        }
        if (p >= T[0].length()) {
            cout << T[0] << endl; 
            throw invalid_argument("Propositional variable " + F + " is out of bounds of the trace.");
        }
        if (T[0][p] == '0') {
            return false;
        }
        return true;
    }

    // Unary_Prop_conn -> '~' | '!'
    else if (F[0] == '~' || F[0] == '!') {
        return !evaluate_mltl(F.substr(1, F.length()-1), T);
    }

    // Unary_Temp_conn -> 'F' | 'G'
    else if ((F[0] == 'F') || (F[0] == 'G')) {
        int lb, ub;
        tie(lb, ub) = find_bounds(F);
        int rbrace = F.find(']');
        if (rbrace == string::npos) {   
            throw invalid_argument("Formula " + F + " is not a valid MLTL formula.");
        }
        string subF = F.substr(rbrace+1, F.length()-rbrace-1);
        if (verbose) {
            cout << "lb: " << lb << endl;
            cout << "ub: " << ub << endl;
            cout << "subF: " << subF << endl;
        }
        
        // T |- F[a, b] subF iff |T| > a and there exists i in [a, b] such that T[i:] |- subF
        if (F[0] == 'F') {
            if (T.size() <= lb) {
                return false;
            } // |T| > a
            for (int i = lb; i <= ub; ++i) {
                if (i >= T.size()) {
                    break;
                } // |T| > i
                vector<string> subT = slice(T, i, T.size());
                if (evaluate_mltl(subF, subT)) {
                    return true;
                }
            } // no i in [a, b] such that T[i:] |- subF
            return false;
        }

        // T |- G[a, b] subF iff |T| <= a or for all i in [a, b], T[i:] |- subF
        else { // F[0] == 'G'
            if (T.size() <= lb) {
                return true;
            } // |T| <= a
            for (int i = lb; i <= ub; ++i) {
                if (i > T.size()) {
                    break;
                } // |T| > i
                vector<string> subT = slice(T, i, T.size());
                if (!evaluate_mltl(subF, subT)) {
                    return false;
                }
            } // for all i in [a, b], T[i:] |- subF
            return true;
        }
    }

    // find first occurence of binary connective by counting parentheses
    else if (F[0] == '(') {
        int binary_conn_index = find_binary_conn(F);
        if (binary_conn_index == -1) {
            return evaluate_mltl(F.substr(1, F.length()-2), T);
        }
        string subF1 = F.substr(1, binary_conn_index-1);
        if (verbose) {
            cout << "binary conn index: " << binary_conn_index << endl;
            cout << "binary conn: " << F[binary_conn_index] << endl;
            cout << "subF1: " << subF1 << endl;
            cout << "F: " << F.substr(binary_conn_index, F.length()-binary_conn_index-1) << endl;
        }
        F = F.substr(binary_conn_index, F.length()-binary_conn_index-1);
        
        // &
        if (F[0] == '&') {
            string subF2 = F.substr(1, F.length()-1);
            if (evaluate_mltl(subF1, T) && evaluate_mltl(subF2, T)) {
                return true;
            }
            return false;
        } 

        // | 
        else if (F[0] == '|') {
            string subF2 = F.substr(1, F.length()-1);
            if (evaluate_mltl(subF1, T) || evaluate_mltl(subF2, T)) {
                return true;
            }
            return false;
        }

        // ->
        else if (F[0] == '-') {
            if (F[1] != '>') {
                throw invalid_argument("Formula " + F + " is not a valid MLTL formula.");
            }
            string subF2 = F.substr(2, F.length()-2);
            if (evaluate_mltl(subF1, T) && !evaluate_mltl(subF2, T)) {
                return false;
            }
            return true;
        }

        // binary_temp_conn
        else if (F[0] == 'U' || F[0] == 'R') {
            int lb, ub;
            tie(lb, ub) = find_bounds(F);
            int rbrace = F.find(']');
            if (rbrace == string::npos) {   
                throw invalid_argument("Formula " + F + " is not a valid MLTL formula.");
            }
            string subF2 = F.substr(rbrace+1, F.length()-rbrace-1);
            if (verbose) {
                cout << "lb: " << lb << endl;
                cout << "ub: " << ub << endl;
                cout << "subF2: " << subF2 << endl;
            }
        
            // T |- F1 U[a,b] F2 iff |T| > a and there exists i in [a,b] such that
            // (T[i:] |- F2 and for all j in [a, i-1], T[j:] |- F1)
            if (F[0] == 'U') {
                if (T.size() <= lb) {
                    return false;
                } // |T| <= a
                // find first occurence for which T[i:] |- F2
                int i = -1; 
                for (int k = lb; k <= ub; ++k) {
                    if (k >= T.size()) {
                        break;
                    } // |T| > j
                    vector<string> subT = slice(T, k, T.size());
                    if (evaluate_mltl(subF2, subT)) {
                        i = k;
                        break;
                    }
                } // no i in [a, b] such that T[i:] |- F2
                if (i == -1) {
                    return false;
                }
                // cout << "i: " << i << endl;
                // check that for all j in [a, i-1], T[j:] |- F1
                for (int j = lb; j < i; ++j) {
                    vector<string> subT = slice(T, j, T.size());
                    if (!evaluate_mltl(subF1, subT)) {
                        return false;
                    }
                    // cout << "passed at j = " << j << endl;
                    // print(subT);
                } // for all j in [a, i-1], T[a:j] |- F1
                return true;
            }

            // T |- F1 R[a,b] F2 iff |T| <= a or for all i in [a, b] T[i:] |- F2 or
            // (there exists j in [a, b-1] such that T[j:] |- F1 and for all k in [a, j],
            // T[k:] |- F2)
            else if (F[0] == 'R') {
                if (T.size() <= lb) {
                    return true;
                } // |T| <= a

                // check if all i in [a, b] T[i:] |- F2
                for (int i = lb; i <= ub; ++i) {
                    vector<string> subT = slice(T, i, T.size());
                    if (!evaluate_mltl(subF2, subT)) {
                        break;
                    }
                    if (i == ub || i == T.size()-1) {
                        return true;
                    }
                } // not all i in [a, b] T[i:] |- F2
                
                // find first occurence of j in [a, b-1] for which T[j:] |- F1
                int j = -1;
                for (int k = lb; k < ub; ++k) {
                    vector<string> subT = slice(T, k, T.size());
                    if (evaluate_mltl(subF1, subT) || k == T.size()-1) {
                        j = k;
                        break;
                    }
                } // no j in [a, b-1] such that T[j:] |- F1
                if (j == -1) {
                    return false;
                }
                // check that for all k in [a, j], T[k:] |- F2
                for (int k = lb; k <= j; ++k) {
                    vector<string> subT = slice(T, k, T.size());
                    if (!evaluate_mltl(subF2, subT)) {
                        return false;
                    }
                    if (k == T.size()-1) {
                        break;
                    }
                } // for all k in [a, j], T[k:] |- F2
                return true;
            }
        }
    }

    // Formula is not a valid formula
    else {
        throw invalid_argument("Formula " + F + " is not a valid MLTL formula.");
    }
    return false;
}