#pragma once
#include <Formula.h>
#include <iostream>
using namespace std;

/// @brief This class inherits from formula, and extends
/// it to MLTL.
class MLTLFormula : public Formula {
public:
  int ub;
  int lb;
  std::unique_ptr<MLTLFormula> left;
  std::unique_ptr<MLTLFormula> right;
  std::unique_ptr<MLTLFormula> tern;
  // string g;
  std::string to_string() override;
  // void setG();
  void setAccumulated();
  std::string gf(const int& i);
  MLTLFormula():left(nullptr),right(nullptr),tern(nullptr){};
  // Takes the initial number associated with slack variable,
  // returns the last value of number used (if j = setProp(i)), then there are
  // j -i slack variables.
  int setProp(int i); 
  ~MLTLFormula();
};


