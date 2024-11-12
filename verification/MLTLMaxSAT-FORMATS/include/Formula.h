#pragma once

#include <memory>
#include <vector>
#include <string>

class Formula {
private:
public:
enum ops {
  Atom,
  Neg,
  And,
  Or,
  Equiv,
  NotEquiv,
  Implies,
  Ite,
  True,
  False,
  Next,
  Global,
  Future,
  Until,
  Release,
  None
};
// Note the order of above and below must match.
// Only to_string() will be affected for a mismatch
std::vector<std::string> names{
  "Atom",
  "!",
  "&&",
  "||",
  "<=>",
  "!=",
  "=>",
  "?",
  "True",
  "False",
  "X",
  "G",
  "F",
  "U",
  "R",
  "None"
};
  virtual std::string to_string();
  int aub;
  int alb;
  int op;
  std::string prop;
  Formula(/* args */);
  virtual void init(){};
  ~Formula();
};
