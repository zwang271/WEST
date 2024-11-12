/// Run MaxSAT to simply check the regular syntax
/// To run MaxSAT on MLTL formulas, the input is a file with each line containing a clause
/// Run MaxSAT path/to/file.mltl on the terminal.
/// Output is z3 output, suppose there are 16 clauses, and MaxSAT returns "objectives (2)", that means
/// that 16 - 2 = 14 clauses are simultaneously satisfiable.

#include "antlr4-runtime.h"
#include <MLTLFormula.h>
#include <String2MLTL.h>
#include <Translators.h>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <streambuf>
#include <string>
#include <unordered_map>
namespace fs = std::filesystem;
using namespace antlrcpp;
using namespace antlr4;
using namespace std;

void tester(std::string left) {
  unique_ptr<MLTLFormula> f;
  String2MLTL formula(left, f);
  f->setAccumulated();
  cout << "Orig.: " << left << ", now: " << f->to_string() << endl;
  Translators t;
  auto out = t.mltl2PropSmvS(f);
  cout << out << endl;
  ofstream outfile("temp.smt");
  outfile << out << endl;
  outfile.close();
  int bre;
  system("./usr/bin/z3 -smt2 temp.smt");
  // cin >> bre;
};

int main(int argc, char **argv) {
  if (argc == 1) {
    try {
      tester("F[1,2] p");
      tester("! G[1,2] (! p)");
      tester("!((!p) U[1,4] (!q))");
      tester("p V[1,4] q");
      tester("((TRUE) U[1,4] (q))");
      tester("F[1,4] q");
      tester("((FALSE) V[1,4] (q))");
      tester("G[1,4] q");
      tester("(p && q)");
      tester("!((!p) || (!q))");
      tester("F[1,4] (p | q)");
      tester("(F[1,4] (p)) | (F[1,4](q))");
      tester("G[1,4] (p & q)");
      tester("G[1,4] (p) & G[1,4](q)");
      tester("G[1,4] (p) == G[1,4](q)");
      tester("G[1,4] (p) != G[1,4](q)");
      tester("G[1,4] (p) ? G[1,4](q) : G[1,2](p)");
      tester("G[1,2](G[1,4] (p) ? G[1,4](q) : G[1,2](p))");
      tester("G[1,2](G[1,4] (p) => (G[1,4](q) => G[1,2](p)))");
      // tester("X G (p)", "G X (p)");
      // tester("G (p)", "p & X G (p)");
      // tester("F (p)", "p | X F (p)");
    } catch (std::exception &ex) {
      cout << ex.what() << endl;
    }
  } else {
    // Get the input string:
    string temp = argv[1];
    auto filepath = fs::path(temp);
    ifstream infile(filepath);
    string line;
    vector<MLTLFormula *> formulas;
    vector<string> lines;
    while (getline(infile, line)) {
      lines.push_back(line);
    }
    int bre;
    vector<unique_ptr<MLTLFormula>> uqformulas(lines.size());
    for (int i = 0; i < lines.size(); i++) {
      String2MLTL formula(lines[i], uqformulas[i]);
      uqformulas[i]->setAccumulated();
      formulas.push_back(uqformulas[i].get());
    }
    Translators t;
    auto out = t.mltl2PropSmvMaxSAT(formulas);
    ofstream outfile("temp.smt");
    outfile << out << endl;
    outfile.close();

    system("./usr/bin/z3 -smt2 temp.smt");
  }
  return 0;
}