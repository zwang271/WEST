// This is now a translator from MLTL to propositional logic in this script
// No more a sat solver.
// But the propositional encoding is in z3. Just for convenience. Can change
// later.
#include "antlr4-runtime.h"
#include <MLTLFormula.h>
#include <String2MLTL.h>
#include <Translators.h>
#include <algorithm>
#include <ezOptionParser.h>
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

int main(int argc, const char * argv[]) {

  // Get the input string:
  ez::ezOptionParser opt;

  opt.overview = "All translators that we have implemented so far with command "
                 "line arguments";
  opt.syntax = "./main -f [FILENAME] -t [TRANSLATOR] -o [OUTPUT_FILE]";
  opt.example = "./main -f file.mltl -t smt\n\n";
  opt.footer = "Gokul Hariharan (C) 2023\n";

  opt.add("", // Default.
          0,  // Required?
          0,  // Number of args expected.
          0,  // Delimiter if expecting multiple args.
          "Display usage instructions.", // Help description.
          "-h",                          // Flag token.
          "-help",                       // Flag token.
          "--help",                      // Flag token.
          "--usage"                      // Flag token.
  );

  opt.add(
      "", // Default.
      1,  // Required?
      1,  // Number of args expected.
      0,  // Delimiter if expecting multiple args.
      "Translator, needs one option among SMT, boolSlow, boolFast, MaxSAT", // Help description.
      "-t" // Flag token.
  );

  opt.add(
      "", // Default.
      1,  // Required?
      1,  // Number of args expected.
      0,  // Delimiter if expecting multiple args.
      "Formula file to translate. Must contain a single MLTL formula in a "
      "single "
      "line. For MaxSAT instances, you can have multiple formulas, each in a "
      "new line. Every line is then considered a MaxSAT clause.", // Help
                                                                  // description.
      "-f" // Flag token.
  );

  opt.add("",                 // Default.
          0,                  // Required?
          1,                  // Number of args expected.
          0,                  // Delimiter if expecting multiple args.
          "Output file name, to store the translation into file instead of solving with Z3", // Help description.
          "-o"                // Flag token.
  );

  opt.parse(argc, argv);

  if (opt.isSet("-h") || (argc==1)) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
    return 1;
  }

  string output;
  string input;

  if (opt.isSet("-f")) {
    opt.get("-f")->getString(input);
  } else {
    cout << "You need to supply a -f option with file with the mltl formula. Checktou ./main --help\n";
    return 0;
  }
  ifstream infile(input);

  if (opt.isSet("-t")) {
    string translatorType;
    opt.get("-t")->getString(translatorType);
    if (strcmp(translatorType.c_str(), "SMT") == 0) {
      string temp;
      getline(infile,temp);
      unique_ptr<MLTLFormula> f;
      String2MLTL formula(temp, f);
      f->setAccumulated();
      Translators t;
      output = t.mltl2smtlib(f);
    } else if (strcmp(translatorType.c_str(), "boolSlow") == 0) {
      string temp;
      getline(infile,temp);
      unique_ptr<MLTLFormula> f;
      String2MLTL formula(temp, f);
      f->setAccumulated();
      Translators t;
      output = t.mltl2PropSmvS(f);
    } else if (strcmp(translatorType.c_str(), "boolFast") == 0) {
      string temp;
      getline(infile,temp);
      unique_ptr<MLTLFormula> f;
      String2MLTL formula(temp, f);
      f->setAccumulated();
      Translators t;
      output = t.mltl2PropSmv(f);
    } else if (strcmp(translatorType.c_str(), "MaxSAT") == 0) {
      string line;
      vector<MLTLFormula *> formulas;
      vector<string> lines;
      while (getline(infile, line)) {
        lines.push_back(line);
      }
      vector<unique_ptr<MLTLFormula>> uqformulas(lines.size());
      for (int i = 0; i < lines.size(); i++) {
        String2MLTL formula(lines[i], uqformulas[i]);
        uqformulas[i]->setAccumulated();
        formulas.push_back(uqformulas[i].get());
      }
      Translators t;
      output = t.mltl2PropSmvMaxSAT(formulas);
    }
  } else {
    cout << "Needs at least one translator option. Checkout ./main --help\n";
    return 0;
  }


if (opt.isSet("-o")){
  string outFileName;
  opt.get("-o")->getString(outFileName);
  ofstream outfile(outFileName);
  outfile << output;
} else {
  ofstream tempfile("temp.smt");

  tempfile << output << endl;
  tempfile.close();
  system("./usr/bin/z3 -smt2 temp.smt");
}
  return 0;
}
