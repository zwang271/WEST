#include "utils.h"

/*
 * Input: string S
 *		 char C
 * Output: S with every instance of C removed
 */
string strip_char(string s, char c) {
	string w = "";
	for (int j = 0; j < s.length(); ++j) {
		if (s[j] != c) {
			w += s[j];
		}
	}
	return w;
}


/*
Returns the (largest prop var index) + 1 in wff
*/
int get_n(string wff) {
    int n = 0;
    for (int i = 0; i < wff.length(); i++) {
        if (wff[i] == 'p') {
            int j = i + 1;
            while (j < wff.length() && isdigit(wff[j])) {
                j++;
            }
            int p = stoi(wff.substr(i + 1, j - i - 1));
            if (p > n) {
                n = p;
            }
        }
    }
    return n + 1;
}


/*
Recursively converts a well formed formula (wff) to negation normal form (nnf)
!p -> !p
!F wff -> G !wff
!G wff -> F !wff
!!wff -> wff
!(wff1 & wff2) -> (!wff1 | !wff2)
!(wff1 | wff2) -> (!wff1 & !wff2)
!(wff1 -> wff2) -> (wff1 & !wff2)
!(wff1 U wff2) -> (!wff1 R !wff2)
!(wff1 R wff2) -> (!wff1 U !wff2)
*/
string wff_to_nnf(string wff) {
    // cout << wff << endl; 
    SyntaxTree T(wff);
    if (T.type == INVALID) {
        wff = "(" + wff + ")";
        SyntaxTree T(wff);
        if (T.type == INVALID) {
            throw invalid_argument("Invalid syntax.");
        }
    }

    if (T.type == PAREN_WFF) {
        return wff_to_nnf(T.wff1_string);
    }

    if (T.type == PROP_CONS || T.type == PROP_VAR) {
        return wff;
    }

    if (T.type == UNARY_TEMP_CONN) {
        string interval = "[" + to_string(T.lb) + "," + to_string(T.ub) + "]";
        return T.op + interval + wff_to_nnf(T.wff1_string);
    }

    if (T.type == BINARY_PROP_CONN) {
        return "(" + wff_to_nnf(T.wff1_string) + T.op + wff_to_nnf(T.wff2_string) + ")";
    }

    if (T.type == BINARY_TEMP_CONN) {
        string interval = "[" + to_string(T.lb) + "," + to_string(T.ub) + "]";
        return "(" + wff_to_nnf(T.wff1_string) + T.op + interval + wff_to_nnf(T.wff2_string) + ")";
    }

    if (T.type == UNARY_CONN) {
        if (T.op == "!") {
            SyntaxTree T2(T.wff1_string);
            if (T2.type == UNARY_TEMP_CONN) {
                string interval = "[" + to_string(T2.lb) + "," + to_string(T2.ub) + "]";
                if (T2.op == "F") {
                    return "G" + interval + wff_to_nnf("!"+T2.wff1_string);
                }
                if (T2.op == "G") {
                    return "F" + interval + wff_to_nnf("!"+T2.wff1_string);
                }
            }
            if (T2.type == BINARY_TEMP_CONN) {
                string interval = "[" + to_string(T2.lb) + "," + to_string(T2.ub) + "]";
                if (T2.op == "U") {
                    return "(" + wff_to_nnf("!"+T2.wff1_string) + "R" + interval + wff_to_nnf("!"+T2.wff2_string) + ")";
                }
                if (T2.op == "R") {
                    return "(" + wff_to_nnf("!"+T2.wff1_string) + "U" + interval + wff_to_nnf("!"+T2.wff2_string) + ")";
                }
            }
            if (T2.type == BINARY_PROP_CONN) {
                if (T2.op == "&") {
                    return "(" + wff_to_nnf("!"+T2.wff1_string) + "|" + wff_to_nnf("!"+T2.wff2_string) + ")";
                }
                if (T2.op == "|") {
                    return "(" + wff_to_nnf("!"+T2.wff1_string) + "&" + wff_to_nnf("!"+T2.wff2_string) + ")";
                }
                if (T2.op == "->") {
                    return "(" + wff_to_nnf(T2.wff1_string) + "&" + wff_to_nnf("!"+T2.wff2_string) + ")";
                }
            }
            if (T2.type == UNARY_CONN) {
                return wff_to_nnf(T2.wff1_string);
            }
            if (T2.type == PAREN_WFF) {
                return wff_to_nnf("!"+T2.wff1_string);
            }
            if (T2.type == PROP_CONS || T2.type == PROP_VAR) {
                return wff;
            }
        }
    }

    return wff;
}


/*
Computes the computation length of a well formed formula
*/
int complen(string wff) {
    SyntaxTree T(wff);

    if (T.type == PROP_CONS || T.type == PROP_VAR) {
        return 1;
    }

    if (T.type == UNARY_CONN) {
        return complen(T.wff1_string);
    }

    if (T.type == BINARY_PROP_CONN){
        return max(complen(T.wff1_string), complen(T.wff2_string));
    }

    if (T.type == UNARY_TEMP_CONN) {
        return T.ub + complen(T.wff1_string);
    }

    if (T.type == BINARY_TEMP_CONN) {
        return T.ub + max(complen(T.wff1_string), complen(T.wff2_string));
    }

    return 0;
}


/*
converts bitset to regular expression
*/
string bitset_to_reg(bitset<MAXBITS> b, int bits_needed){
    string s = "";
    // iterate through b two bits at a time
    for (int i = 0; i < bits_needed; i += 2) {
        if (b[i] && b[i + 1]) {
            s += "s";
        }
        else if (b[i] && !b[i + 1]) {
            s += "1";
        }
        else if (!b[i] && b[i + 1]) {
            s += "0";
        }
        else {
            return "";
        }
    }
    return s;
}
vector<string> bitset_to_reg(vector<bitset<MAXBITS>> B, int bits_needed) {
    vector<string> S;
    for (int i = 0; i < B.size(); i++) {
        S.push_back(bitset_to_reg(B[i], bits_needed));
    }
    return S;
}

/*
Converts a bitset to string using the following scheme, one pair of bits at a time
11 -> "s"
10 -> "1"
01 -> "0"
00 detected within input immediately returns ""
*/
string bitset_to_string(bitset<MAXBITS> b) {
    return bitset_to_reg(b, MAXBITS);
}
vector<string> bitset_to_string(vector<bitset<MAXBITS>> B) {
    vector<string> S;
    for (int i = 0; i < B.size(); ++i) {
        S.push_back(bitset_to_string(B[i]));
    }
    return S;
}
/*
converts string to bitset
*/
bitset<MAXBITS> stb(string s){
    if (2*s.length() > MAXBITS) {
        throw invalid_argument("String too long.");
    }
    bitset<MAXBITS> b;
    b.flip();
    for (int i = 0; i < s.length(); ++i) {
        if (s[i] == 's') {
            b[2 * i] = 1;
            b[2 * i + 1] = 1;
        }
        else if (s[i] == '1') {
            b[2 * i] = 1;
            b[2 * i + 1] = 0;
        }
        else if (s[i] == '0') {
            b[2 * i] = 0;
            b[2 * i + 1] = 1;
        }
        else {
            throw invalid_argument("Invalid string.");
        }
    }
    return b;
}


/*
Checks if a bitset contains 00
*/
bool is_null(bitset<MAXBITS> b){
    for (int i = 0; i < b.size(); i += 2) {
        if (!b[i] && !b[i + 1]) {
            return true;
        }
    }
    return false; 
}


/*
Shifts a bitset to the left by m bits, padding with 1s
*/
bitset<MAXBITS> shift(bitset<MAXBITS> b, int m){
    auto c = b.flip();
    c <<= m;
    c.flip();
    return c;
}
vector<bitset<MAXBITS>> shift(vector<bitset<MAXBITS>> b, int m) {
    vector<bitset<MAXBITS>> B;
    for (int i = 0; i < b.size(); ++i) {
        B.push_back(shift(b[i], m));
    }
    return B;
}

/*
Prints out a vector of strings line by line
WARNING: FLIPS THE ORDER OF THE BITSETS
This is because bitset<MAXBITS>[0] is the right most bit
*/
void print(string s){
    cout << s << endl; 
}
void print(bitset<MAXBITS> b){
    string s = b.to_string();
    reverse(s.begin(), s.end());
    print(s);
}
void print(vector<string> S) {
    for (int i = 0; i < S.size(); ++i) {
        print(S[i]);
    }
}
void print(vector<bitset<MAXBITS>> B) {
    for (int i = 0; i < B.size(); ++i) {
        print(B[i]);
    }
}


