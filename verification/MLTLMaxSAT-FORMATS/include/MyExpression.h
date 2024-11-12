#pragma once
#include <string>
struct MyExpression {
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
  int op;
  int lb;
  int ub;
  std::string prop;
  bool isUnary;
  bool isLeaf = false;
};  