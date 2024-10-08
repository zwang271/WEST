// Author: Zili Wang
// Last updated: 01/19/2024
// Core WEST functions

#include "reg.h"

// Vector containing all sub-nnfs and their regexs for a given nnf input
vector<tuple<string, vector<bitset<MAXBITS>>>> FORMULAS;
// Returns the global variable FORMULAS of sub-nnfs and their regexs 
vector<tuple<string, vector<bitset<MAXBITS>>>> get_formulas() {return FORMULAS;}
// Records a sub-nnf and its regex to the global variable FORMULAS
void record_formula(string nnf, vector<bitset<MAXBITS>> regex) {
    // check if the nnf is already in the vector
    for (int i = 0; i < FORMULAS.size(); ++i) {
        if (get<0>(FORMULAS[i]) == nnf) {
            return;
        }
    }
    // if not, add it to the vector
    FORMULAS.push_back(make_tuple(nnf, regex));
}


int find_first(bitset<MAXBITS> b) {
    for (int i = 0; i < MAXBITS; ++i) {
        if (b[i]) {
            return i;
        }
    }
    return -1;
}

int find_next(bitset<MAXBITS> b, int i) {
    for (int j = i+1; j < MAXBITS; ++j) {
        if (b[j]) {
            return j;
        }
    }
    return -1;
}

/*
Checks if b1 and b2 can be simplified by bitwise or
*/
bool check_simp(bitset<MAXBITS> b1, bitset<MAXBITS> b2) {
    // compute xor, return true if sum of all resulting bits < 2
    bitset<MAXBITS> b = b1 ^ b2;
    int count = b.count();
    if (count < 2) {    
        return true;
    }
    // if count == 2, make sure the two set bits are adjacent and aligned
    if (count == 2) {
        // int first_set_bit = b._Find_first();
        // int second_set_bit = b._Find_next(first_set_bit);
        int first_set_bit = find_first(b);
        int second_set_bit = find_next(b, first_set_bit);
        if (first_set_bit % 2 == 0 && second_set_bit == first_set_bit + 1) {
            return true;
        }
    }
    return false;
}


/*
Simplifies a vector of bitsets by bitwise or
*/
vector<bitset<MAXBITS>> simplify(vector<bitset<MAXBITS>> B) {
    for (int i = 0; i < B.size(); ++i) {
        for (int j = i + 1; j < B.size(); ++j) {
            if (check_simp(B[i], B[j])) {
                B[i] |= B[j];
                B.erase(B.begin() + j);
                return simplify(B);
            }
        }
    }
    return B;
} 


/*
Computes or of two vectors of bitsets
*/
vector<bitset<MAXBITS>> or_vec(vector<bitset<MAXBITS>> B1, vector<bitset<MAXBITS>> B2) {
    vector<bitset<MAXBITS>> B;
    for (int i = 0; i < B1.size(); ++i) {
        B.push_back(B1[i]);
    }
    for (int i = 0; i < B2.size(); ++i) {
        B.push_back(B2[i]);
    }
    return simplify(B);
}


/*
Computes and of two vectors of bitsets
*/
vector<bitset<MAXBITS>> and_vec(vector<bitset<MAXBITS>> B1, vector<bitset<MAXBITS>> B2){
    vector<bitset<MAXBITS>> B;
    for (int i = 0; i < B1.size(); i++) {
        for (int j = 0; j < B2.size(); j++) {
            bitset<MAXBITS> b = B1[i] & B2[j];
            if (!is_null(b)) {
                B.push_back(b);
            }
        }
    }
    return simplify(B);
}


/*
nnf ->  ?(!) prop_var | prop_cons
                      | unary_temp_conn  interval  nnf
                      | '(' Nnf Binary_Prop_conn Nnf ')'
                      | '(' Nnf Binary_Temp_conn  Interval Nnf ')'
                      | '(' nnf ')'
Input: mLTL formula in NNF (string)
       n is number of propositional variables
Output: Vector of computation strings satisfying the formula
*/
vector<bitset<MAXBITS>> reg(string nnf, int n) {
    SyntaxTree T(nnf);
    // print(node_type_to_string(T.type));
    // print(T.formula);
    vector<bitset<MAXBITS>> answer = {};

    if (T.type == INVALID) {
        nnf = "(" + nnf + ")";
        SyntaxTree T(nnf);
        if (T.type == INVALID) {
            cout << "Invalid formula." << endl;
            return {};
        }
        answer = reg(nnf, n);
    }

    if (T.type == PROP_CONS) {
        bitset<MAXBITS> b; 
        if (T.formula == "false") {
            answer = {};
        }
        if (T.formula == "true") {
            b.flip();
            answer = {b};
        }
    }

    if (T.type == PROP_VAR || T.type == UNARY_CONN) {
        bitset<MAXBITS> b; 
        b.flip();
        if (T.type == PROP_VAR) {
            b.flip(2*T.k+1);
        }
        else if (T.type == UNARY_CONN) {
            b.flip(2*T.wff1->k);
        }
        answer = {b};
    }

    if (T.type == BINARY_PROP_CONN) {
        if (T.op == "->") {
            string neg_wff1 = wff_to_nnf("!" + T.wff1_string);
            string equiv = "(" + neg_wff1 + "|" + T.wff2_string + ")";
            answer = reg(equiv, n);
        }
        if (T.op == "=") {
            string neg_wff1 = wff_to_nnf("!" + T.wff1_string);
            string neg_wff2 = wff_to_nnf("!" + T.wff2_string);
            string wff1_and_wff2 = "(" + T.wff1_string + "&" + T.wff2_string + ")";
            string neg_wff1_and_neg_wff2 = "(" + neg_wff1 + "&" + neg_wff2 + ")";
            string equiv = "(" + wff1_and_wff2 + "|" + neg_wff1_and_neg_wff2 + ")";
            answer = reg(equiv, n);
        }
        auto reg1 = reg(T.wff1_string, n);
        auto reg2 = reg(T.wff2_string, n);
        if (T.op == "|") {
            answer = or_vec(reg1, reg2);
        }
        if (T.op == "&") {
            answer = and_vec(reg1, reg2);
        } 
    }

    if (T.type == UNARY_TEMP_CONN) {
        auto reg1 = reg(T.wff1_string, n);
        auto result = shift(reg1, T.lb*2*n);
        for (int i = T.lb+1; i <= T.ub; ++i) {
            auto term = shift(reg1, i*2*n);
            if (T.op == "F") {
                result = or_vec(result, term);
            }
            if (T.op == "G") {
                result = and_vec(result, term);
            }
        }
        answer = result;
    } 

    if (T.type == BINARY_TEMP_CONN) {
        if (T.op == "U") {
            auto reg2 = reg(T.wff2_string, n);
            auto result = shift(reg2, T.lb*2*n);
            for (int i = T.lb+1; i <= T.ub; ++i) {
                string reg1_interval = "[" + to_string(T.lb) + "," + to_string(i-1) + "]";
                string reg2_interval = "[" + to_string(i) + "," + to_string(i) + "]"; 
                auto G_reg1 = reg("G" + reg1_interval + T.wff1_string, n);
                auto G_reg2 = reg("G" + reg2_interval + T.wff2_string, n);
                result = or_vec(result, and_vec(G_reg1, G_reg2));
            }
            answer = result;
        }
        if (T.op == "R") {
            string interval = "[" + to_string(T.lb) + "," + to_string(T.ub) + "]";
            auto result = reg("G" + interval + T.wff2_string, n);
            for (int i = T.lb; i <= T.ub-1; i++) {
                string reg2_interval = "[" + to_string(T.lb) + "," + to_string(i) + "]";
                string reg1_interval = "[" + to_string(i) + "," + to_string(i) + "]";
                auto G_reg1 = reg("G" + reg1_interval + T.wff1_string, n);
                auto G_reg2 = reg("G" + reg2_interval + T.wff2_string, n);
                result = or_vec(result, and_vec(G_reg1, G_reg2));
            }
            answer = result;
        }
    }

    if (T.type == PAREN_WFF) {
        answer = reg(T.wff1_string, n);
    }

    record_formula(nnf, answer);
    return answer; 
}


/*
Recompiles bit optimized binary and runs the executable
*/
void recompile(string wff) {
    ifstream input_file("./src/bitoptimized/reg_template.txt");
    ofstream output_file("./src/bitoptimized/reg.h");
    string line;
    int line_number = 0;
    while (getline(input_file, line)) {
        ++line_number;
        if (line_number == 5) {
            output_file << "#define MAXBITS " << 2 * get_n(wff) * complen(wff) << endl;
        }
        else {
            output_file << line << endl;
        }
    }
    input_file.close();
    output_file.close();

    string command; 
#ifdef _WIN32
    command = "g++ ./src/bitoptimized/west.cpp ./src/bitoptimized/reg.cpp ./src/bitoptimized/utils.cpp ./src/bitoptimized/parser.cpp -o ./src/bitoptimized/west_bitoptimized.exe -std=c++17";
#else
    command = "g++ ./src/bitoptimized/west.cpp ./src/bitoptimized/reg.cpp ./src/bitoptimized/utils.cpp ./src/bitoptimized/parser.cpp -o ./src/bitoptimized/west_bitoptimized -std=c++17";
#endif
    system(command.c_str());    

#ifdef _WIN32
    command = ".\\src\\bitoptimized\\west_bitoptimized.exe \"" + wff + "\"";
#else
    command = "./src/bitoptimized/west_bitoptimized \"" + wff + "\"";
#endif
    system(command.c_str());
}