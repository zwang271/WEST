#include "stdlib.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

  // Will need to update these to run:
  string prefix = "/home/gokul/";
  string tempSh = prefix + "temp.sh";
  string tempMaxBound = prefix + "maxbound.txt";
  string tempSmvFile = prefix + "temp.smv";
  string nuXmvExe = prefix + "nuXmv-2.0.0-Linux/bin/nuXmv ";
  string mltlConverterExe =
      prefix + "artifact/translator/src/MLTLConvertor ";

  ifstream infile(prefix + "MLTLMaxSAT-FORMATS/Benchmarks/MLTLSatisfiability/randomList.mltl");
  ofstream ouic3(prefix + "MLTLMaxSAT-FORMATS/BenchmarkResults/randomList.ic3");
  ofstream oubmc(prefix + "MLTLMaxSAT-FORMATS/BenchmarkResults/randomList.bmc");
  // doing ic3 check:
  string line;
  while (getline(infile, line)) {
    auto start = high_resolution_clock::now();
    system(
        (mltlConverterExe + " -smv \"" + line + "\" > " + tempSmvFile).c_str());
    auto stop = high_resolution_clock::now();
    auto elapsedTranslation = duration_cast<milliseconds>(stop - start);
    ofstream bashFile(tempSh);

    string cmd2 =
        "-source <(cat <<END\nset on_failure_script_quits 1\nset input_file " +
        tempSmvFile +
        "\nread_model\nflatten_hierarchy\nencode_variables\nbuild_"
        "boolean_model \ncheck_ltlspec_ic3 -d\nquit\nEND\n)\n";
    bashFile << nuXmvExe;
    bashFile << cmd2;
    bashFile.close();
    system(("chmod u+x " + tempSh).c_str());
    start = high_resolution_clock::now();
    system(("timeout 5m bash " + tempSh + " && echo $? > some.txt").c_str());
    stop = high_resolution_clock::now();
    auto elapsedSat = duration_cast<milliseconds>(stop - start);
    ouic3 << elapsedTranslation.count() << "\t\t" << elapsedSat.count()
            << endl << flush;
    system(
        (mltlConverterExe + " -max \"" + line + "\" > " + tempMaxBound).c_str());
    ifstream infile2(tempMaxBound);
    string maxbound;
    infile2 >> maxbound;
    bashFile.open(tempSh);
    string bmcComd = nuXmvExe +
                     "-source <(cat "
                     "<<END\nset on_failure_script_quits 1\nset input_file " +
                     tempSmvFile +
                     "\nread_model\nflatten_hierarchy\nencode_variables\nbuild_"
                     "boolean_model\nbmc_setup\ngo_bmc\ncheck_ltlspec_bmc -k " +maxbound+
                     "\nquit\nEND\n)";
    bashFile << bmcComd;
    bashFile.close();

    system(("chmod u+x " + tempSh).c_str());
    start = high_resolution_clock::now();
    system(("timeout 5m bash " + tempSh).c_str());
    stop = high_resolution_clock::now();
    elapsedSat = duration_cast<milliseconds>(stop - start);

    oubmc << elapsedTranslation.count() << "\t\t" << elapsedSat.count()
            << endl << flush;
  }
  ouic3.close();
  oubmc.close();
  return 0;
}