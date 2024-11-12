/// @brief genMLTLBenchmarks.cpp
/// Input: A file name that contains LTL formulas.
/// Output: A file is dumped into Benchmarks/BenchmarksMLTL/
/// with same file name, but with ".mltl" extension.

#include "antlr4-runtime.h"
#include <LTL2MLTL.h>
#include <MLTLFormula.h>
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

using namespace antlrcpp;
using namespace antlr4;
using namespace std;
namespace fs = std::filesystem;

void tester(std::string left) {
  unique_ptr<MLTLFormula> f;
  LTL2MLTL formula(left, f);
  cout << f->to_string() << endl;
  //   f->setAccumulated();
  //   cout << "Orig.: " << left << ", now: " << f->to_string() << endl;
  //   auto out = mltl2PropSmv(f);
  //   cout << out << endl;
};

int main(int argc, char **argv) {
  if (argc == 1) {
    try {
      tester("F p");
      tester("! G (! p)");
      tester("!((!p) U (!q))");
      tester("p V q");
      tester("((TRUE) U (q))");
      tester("F q");
      tester("((FALSE) V (q))");
      tester("G q");
      tester("(p && q)");
      tester("!((!p) || (!q))");
      tester("F (p | q)");
      tester("(F (p)) | (F(q))");
      tester("G (p & q)");
      tester("G (p) & G(q)");
      // tester("X G (p)", "G X (p)");
      // tester("G (p)", "p & X G (p)");
      // tester("F (p)", "p | X F (p)");
    } catch (std::exception &ex) {
      cout << ex.what() << endl;
    }
  } else {
    // Get the input string:
    string temp = argv[1];
    auto p = fs::path(temp);
    auto p1 = p.filename().replace_extension("mltl");
    ifstream ifile(p);
    ofstream ofile("../Benchmarks/MLTLSatisfiability/" + p1.generic_string()+"00");
    string line;
    int i = 0;
    int bre;
    while (getline(ifile, line)) {
        cout << "count : " << i++ << endl;
        cout << line << endl;
      unique_ptr<MLTLFormula> f;
      LTL2MLTL formula(line, f);
      ofile << f->to_string() << endl;
        // cin >> bre;
    }
    ofile.close();
    ifile.close();
  }

  return 0;
}
