// This is now a translator from MLTL to propositional logic in this script
// No more a sat solver.
// But the propositional encoding is in z3. Just for convenience. Can change
// later.
// This one creates Li's version smt formulas
#include "antlr4-runtime.h"
#include <MLTLFormula.h>
#include <String2MLTL.h>
#include <Translators.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <streambuf>
#include <string>
#include <unordered_map>

using namespace antlrcpp;
using namespace antlr4;
using namespace std;

void tester(std::string left) {
  unique_ptr<MLTLFormula> f;
  String2MLTL formula(left, f);
  f->setAccumulated();
  cout << "Orig.: " << left << ", now: " << f->to_string() << endl;
  Translators t;
  auto out = t.mltl2smtlib(f);
  cout << out << endl;
  ofstream outfile("temp.smt");
  outfile << out << endl;
  outfile.close();
  int bre;
  system("./usr/bin/z3 -smt2 temp.smt");
  cin >> bre; 
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
    unique_ptr<MLTLFormula> f;
    String2MLTL formula(temp, f);
    f->setAccumulated();
    Translators t;
    auto out = t.mltl2smtlib(f);
    cout << out << endl;
  }

  return 0;
}
