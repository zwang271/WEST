#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
  string path2z3 = "./usr/bin/z3";

  // these need not be updated if the path2proj is correct:
  string path2smt2 = "./MLTL2LiSMT";
  string path2prop = "./MLTL2propFast";
  string path2propS = "./MLTL2propSlow";
  string resultsdir = "../BenchmarkResults/";

  // Temporary files
  string tempFile = "temp.smt2";
  string tempSh = "temp.sh";
  string tempout = "out.txt";

  if (argc == 1)
  {
    cout << "For 0 arguments, the program generates results directory.\n";
    fs::create_directories(resultsdir);
    cout << "Otherwise, pass the file name to begin tests.\n";
    return 0;
  }

  // Get the input file:
  auto filepath = fs::path(string(argv[1]));

  auto filename_smt = filepath.filename().replace_extension("smt2");
  auto filename_pro = filepath.filename().replace_extension("prop");
  auto filename_proS = filepath.filename().replace_extension("propS");

  filename_smt = resultsdir + filename_smt.generic_string();
  filename_pro = resultsdir + filename_pro.generic_string();
  filename_proS = resultsdir + filename_proS.generic_string();

  ifstream ifile(filepath);

  ofstream smt2(filename_smt), prop(filename_pro), propS(filename_proS);
  string line;
  int i = 0;
  while (getline(ifile, line))
  {
    // Do smt2 check:

    // Translation:
    auto startT = high_resolution_clock::now();
    system((path2smt2 + " \"" + line + "\" > " + tempFile).c_str());
    auto stopT = high_resolution_clock::now();

    // Solving:
    string cmd2 = path2z3 + " -T:300 -smt2 " + tempFile + " > " + tempout + "\n";
    auto startS = high_resolution_clock::now();
    system(cmd2.c_str());
    auto stopS = high_resolution_clock::now();

    // Store in file:
    auto elapsedTranslation = duration_cast<milliseconds>(stopT - startT);
    auto elapsedSolving = duration_cast<milliseconds>(stopS - startS);
    ifstream result(tempout);
    string res;
    result >> res;
    smt2 << res << "\t\t" << elapsedTranslation.count() << "\t\t"
         << elapsedSolving.count() << "\n"
         << flush;

    // Do prop check:

    // Translation:
    startT = high_resolution_clock::now();
    system((path2prop + " \"" + line + "\" > " + tempFile).c_str());
    stopT = high_resolution_clock::now();

    // Solving:
    cmd2 = path2z3 + " -T:300 -smt2 " + tempFile + " > " + tempout + "\n";
    startS = high_resolution_clock::now();
    system(cmd2.c_str());
    stopS = high_resolution_clock::now();

    // Store in file:
    elapsedTranslation = duration_cast<milliseconds>(stopT - startT);
    elapsedSolving = duration_cast<milliseconds>(stopS - startS);
    result.close();
    result.open(tempout);
    res = "";
    result >> res;
    prop << res << "\t\t" << elapsedTranslation.count() << "\t\t"
         << elapsedSolving.count() << "\n"
         << flush;

    // Do propS check:

    // Translation:
    startT = high_resolution_clock::now();
    system((path2propS + " \"" + line + "\" > " + tempFile).c_str());
    stopT = high_resolution_clock::now();

    // Solving:
    cmd2 = path2z3 + " -T:300 -smt2 " + tempFile + " > " + tempout + "\n";
    startS = high_resolution_clock::now();
    system(cmd2.c_str());
    stopS = high_resolution_clock::now();

    // Store in file:
    elapsedTranslation = duration_cast<milliseconds>(stopT - startT);
    elapsedSolving = duration_cast<milliseconds>(stopS - startS);
    result.close();
    result.open(tempout);
    res = "";
    result >> res;
    propS << res << "\t\t" << elapsedTranslation.count() << "\t\t"
          << elapsedSolving.count() << "\n"
          << flush;

    cout << "Count: " << i++ << endl;

    // if (i > 2) {
    //   break;
    // }
  }

  smt2.close();
  prop.close();
  propS.close();

  return 0;
}