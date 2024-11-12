#include <Translators.h>

/// @brief getAlphabets
/// @param MLTLFormula, f wrapped in unique pointer
/// @return the alphabet (set) of the formula, e.g., p U[1,2] q has {p,q} as
/// alphabet
void
Translators::getAlphabet(MLTLFormula* f, std::unordered_set<string> &out) {
  queue<MLTLFormula *> Q;
  Q.push(f);
  while (!Q.empty()) {
    auto v = Q.front();
    Q.pop();
    switch (v->op) {
    case MLTLFormula::Atom:
      out.insert(v->prop);
      break;
    case MLTLFormula::False:
    case MLTLFormula::True:
      break;
    case MLTLFormula::Neg:
    case MLTLFormula::Future:
    case MLTLFormula::Global:
      Q.push(v->right.get());
      break;
    case MLTLFormula::Or:
    case MLTLFormula::And:
    case MLTLFormula::Implies:
    case MLTLFormula::Equiv:
    case MLTLFormula::NotEquiv:
    case MLTLFormula::Release:
    case MLTLFormula::Until:
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case MLTLFormula::Ite:
      Q.push(v->left.get());
      Q.push(v->right.get());
      Q.push(v->tern.get());
      break;
    default:
      break;
    }
  }
}

std::string Translators::mltl2PropSmvMaxSAT(const vector<MLTLFormula *> &vec) {
  using namespace std;
  string res = "";
  if (vec.empty()) {
    return "";
  }
  // make the combined formula to find the alphabets:
  std::unordered_set<string> alphSet;
  for (auto f : vec){
    getAlphabet(f,alphSet);
  }
  for (auto a : alphSet) {
    res += "(declare-fun " + a + " (Int) Bool)\n";
  }
  int start = 0;
  for (auto formula : vec) {
    start = formula->setProp(start);
  }
  for (int i = 0; i < start; i++) {
    res += "(declare-fun t" + to_string(i) + " (Int) Bool)\n";
  }
  auto Anded = [](vector<string> fs) {
    string out = "(and ";
    for (auto f : fs) {
      out += f + " ";
    }
    out += ")";
    return out;
  };

  for (auto f : vec){
    auto constraints = getConstraints(f);
    res+= "(assert-soft " + Anded(constraints) + ")\n";
  }
  res += "(check-sat)\n";
  // res += "(get-model)\n";
  res += "(get-objectives)\n";
  return res;
}


std::string Translators::mltl2PropSmv(const unique_ptr<MLTLFormula> &f) {
  using namespace std;
  string res = "";

  // Make alphabet functions:
  std::unordered_set<string> alphSet;
  getAlphabet(f.get(),alphSet);
  for (auto a : alphSet) {
    res += "(declare-fun " + a + " (Int) Bool)\n";
  }

  queue<MLTLFormula *> Q;
  int _id2 = 0;
  Q.push(f.get());
  int i = 0;
  while (!Q.empty()) {
    auto v = Q.front();
    Q.pop();
    switch (v->op) {
    case MLTLFormula::True:
    case MLTLFormula::False:
      break;
    case MLTLFormula::Neg:
    case MLTLFormula::Future:
    case MLTLFormula::Global:
      res += "(declare-fun t" + to_string(_id2++) + " (Int) Bool)\n";
      Q.push(v->right.get());
      break;
    case MLTLFormula::Or:
    case MLTLFormula::And:
    case MLTLFormula::Implies:
    case MLTLFormula::Equiv:
    case MLTLFormula::NotEquiv:
    case MLTLFormula::Release:
    case MLTLFormula::Until:
      res += "(declare-fun t" + to_string(_id2++) + " (Int) Bool)\n";
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case MLTLFormula::Ite:
      res += "(declare-fun t" + to_string(_id2++) + " (Int) Bool)\n";
      Q.push(v->left.get());
      Q.push(v->right.get());
      Q.push(v->tern.get());
      break;
    default:
      break;
    }
  }
  auto constraints = getConstraints(f.get());
  for (auto constraint : constraints) {
    res += "(assert " + constraint + ")\n";
  }
  res += "(check-sat)\n";
  return res;
}

vector<std::string>
Translators::getConstraints(MLTLFormula* f) {
  // First sweep: set the declare fun statements:
  queue<MLTLFormula *> Q;
  vector<std::string> res;
  // Next sweep through and set the assertions
  // First initialization:
  Q.push(f);
  auto v = Q.front();
  Q.pop();
  switch (v->op) {
  case MLTLFormula::True:
    res.push_back("true");
    break;
  case MLTLFormula::False:
    res.push_back("false");
    break;
  case MLTLFormula::Neg:
    res.push_back("(not " + v->right->gf(0) + ")");
    Q.push(v->right.get());
    break;
  case MLTLFormula::And:
    res.push_back("(and " + v->right->gf(0) + " " + v->left->gf(0) + ")");
    Q.push(v->left.get());
    Q.push(v->right.get());
    break;
  case MLTLFormula::Or:
    res.push_back("(or " + v->right->gf(0) + " " + v->left->gf(0) + ")");
    Q.push(v->left.get());
    Q.push(v->right.get());
    break;
  case MLTLFormula::Implies:
    res.push_back("(=> " + v->left->gf(0) + " " + v->right->gf(0) + ")");
    Q.push(v->left.get());
    Q.push(v->right.get());
    break;
  case MLTLFormula::Equiv:
    res.push_back("(= " + v->right->gf(0) + " " + v->left->gf(0) + ")");
    Q.push(v->left.get());
    Q.push(v->right.get());
    break;
  case MLTLFormula::NotEquiv:
    res.push_back("(not (= " + v->right->gf(0) + " " + v->left->gf(0) + "))");
    Q.push(v->left.get());
    Q.push(v->right.get());
    break;
  case MLTLFormula::Ite:
    res.push_back("(ite " + v->left->gf(0) + " " + v->right->gf(0) + " " +
                  v->tern->gf(0) + ")");
    Q.push(v->left.get());
    Q.push(v->right.get());
    Q.push(v->tern.get());
    break;
  case MLTLFormula::Future: {
    auto temp = v->right->gf(v->lb);
    for (int i = v->lb + 1; i <= v->ub; i++) {
      temp = v->right->gf(i) + " " + temp;
    }
    res.push_back("(or " + temp + ")");
    Q.push(v->right.get());
    break;
  }

  case MLTLFormula::Global: {
    auto temp = v->right->gf(v->lb);
    for (int i = v->lb + 1; i <= v->ub; i++) {
      temp = v->right->gf(i) + " " + temp;
    }
    res.push_back("(and " + temp + ")");
    Q.push(v->right.get());
    break;
  }
  case MLTLFormula::Until: {
    auto temp = v->right->gf(v->ub);
    for (int i = v->ub - 1; i >= v->lb; i--) {
      temp = "(or " + v->right->gf(i) + " (and " + v->left->gf(i) + " " + temp +
             "))";
    }
    res.push_back(temp);
    Q.push(f->right.get());
    if (f->lb < f->ub) {
      Q.push(f->left.get());
    }
    break;
  }
  case MLTLFormula::Release: {
    auto temp = v->right->gf(v->ub);
    for (int i = v->ub - 1; i >= v->lb; i--) {
      temp = "(and " + v->right->gf(i) + "  (or " + v->left->gf(i) + " " +
             temp + "))";
    }
    res.push_back(temp);
    Q.push(v->right.get());
    if (v->lb < v->ub) {
      Q.push(v->left.get());
    }
    break;
  }
  default:
    break;
  }

  while (!Q.empty()) {
    auto v = Q.front();
    Q.pop();
    switch (v->op) {
    case MLTLFormula::True:
      break;
    case MLTLFormula::False:
      break;
    case MLTLFormula::Neg:
      for (int i = v->alb; i <= v->aub; i++) {
        res.push_back("(= " + v->gf(i) + " (not " + v->right->gf(i) + ") )");
      }
      Q.push(v->right.get());
      break;
    case MLTLFormula::And:
      for (int i = v->alb; i <= v->aub; i++) {
        res.push_back("(= " + v->gf(i) + " (and " + v->right->gf(i) + " " +
                      v->left->gf(i) + " ))");
      }
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case MLTLFormula::Or:
      for (int i = v->alb; i <= v->aub; i++) {
        res.push_back("(= " + v->gf(i) + " (or " + v->right->gf(i) + " " +
                      v->left->gf(i) + " ))");
      }
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case MLTLFormula::Implies:
      for (int i = v->alb; i <= v->aub; i++) {
        res.push_back("(= " + v->gf(i) + " (=> " + v->left->gf(i) + " " +
                      v->right->gf(i) + " ))");
      }
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case MLTLFormula::Equiv:
      for (int i = v->alb; i <= v->aub; i++) {
        res.push_back("(= " + v->gf(i) + "(= " + v->right->gf(i) + " " +
                      v->left->gf(i) + " ))");
      }
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case MLTLFormula::NotEquiv:
      for (int i = v->alb; i <= v->aub; i++) {
        res.push_back("(= " + v->gf(i) + " (not (= " + v->right->gf(i) + " " +
                      v->left->gf(i) + " )))");
      }
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case MLTLFormula::Ite:
      for (int i = v->alb; i <= v->aub; i++) {
        res.push_back("(= " + v->gf(i) + "(ite " + v->left->gf(i) + " " +
                      v->right->gf(i) + " " + v->tern->gf(i) + "))");
      }
      Q.push(v->left.get());
      Q.push(v->right.get());
      Q.push(v->tern.get());
      break;
    case MLTLFormula::Future: {
      for (int j = v->alb; j <= v->aub; j++) {
        string temp = v->right->gf(v->lb + j);
        for (int i = v->lb + 1; i <= v->ub; i++) {
          temp = v->right->gf(i + j) + " " + temp;
        }
        res.push_back("(= " + v->gf(j) + " (or " + temp + "))");
      }
      Q.push(v->right.get());
      break;
    }

    case MLTLFormula::Global: {
      for (int j = v->alb; j <= v->aub; j++) {
        string temp = v->right->gf(v->lb + j);
        for (int i = v->lb + 1; i <= v->ub; i++) {
          temp = v->right->gf(i + j) + " " + temp;
        }
        res.push_back("(= " + v->gf(j) + " (and " + temp + "))");
      }
      Q.push(v->right.get());
      break;
    }
    case MLTLFormula::Until:
      for (int j = v->alb; j <= v->aub; j++) {
        auto temp = v->right->gf(v->ub + j);
        for (int i = v->ub - 1; i >= v->lb; i--) {
          temp = "(or " + v->right->gf(i + j) + " (and " + v->left->gf(i + j) +
                 " " + temp + "))";
        }
        res.push_back("(= " + v->gf(j) + " " + temp + ")");
      }
      Q.push(v->right.get());
      if (v->lb < v->ub) {
        Q.push(v->left.get());
      }
      break;
    case MLTLFormula::Release:
      for (int j = v->alb; j <= v->aub; j++) {
        auto temp = v->right->gf(v->ub + j);
        for (int i = v->ub - 1; i >= v->lb; i--) {
          temp = "(and " + v->right->gf(i + j) + " (or " + v->left->gf(i + j) +
                 " " + temp + "))";
        }
        res.push_back("(= " + v->gf(j) + " " + temp + ")");
      }
      Q.push(v->right.get());
      if (v->lb < v->ub) {
        Q.push(v->left.get());
      }
      break;
    default:
      break;
    }
  }

  return res;
}

/// The below two functions are taken directly from Li et al.'s CAV2019
/// artifacts for comparison
/// I just modified it to include ite expressions as well.
/*
 * Generate an SMT-LIB v2 expression for the formula
 * k and len in the result string are fixed
 */
string Translators::smtlib_expr(const unique_ptr<MLTLFormula> &f,
                                std::string &loc, std::string &len, int &i) {
  string res = "";

  string quantify_var1 = "i" + std::to_string(i++);
  string quantify_var2 = "i" + std::to_string(i++);

  string new_len1 = "(- " + len + " " + quantify_var1 + ")";
  string new_len2 = "(- " + len + " " + quantify_var2 + ")";

  switch (f->op) {
  case MLTLFormula::True:
    res += "(and (> " + len + " 0) true)";
    break;
  case MLTLFormula::False:
    res += "(and (> " + len + " 0) false)";
    break;
  case MLTLFormula::Neg:
    res += "(and (> " + len + " 0) (not " + smtlib_expr(f->right, loc, len, i) +
           "))";
    break;
  case MLTLFormula::And:
    res += "(and (> " + len + " 0) (and " + smtlib_expr(f->left, loc, len, i) +
           " " + smtlib_expr(f->right, loc, len, i) + "))";
    break;
  case MLTLFormula::Or:
    res += "(and (> " + len + " 0) (or " + smtlib_expr(f->left, loc, len, i) +
           " " + smtlib_expr(f->right, loc, len, i) + "))";
    break;
  case MLTLFormula::Equiv:
    res += "(and (> " + len + " 0) (= " + smtlib_expr(f->left, loc, len, i) +
           " " + smtlib_expr(f->right, loc, len, i) + "))";
    break;
  case MLTLFormula::Implies:
    res += "(and (> " + len + " 0) (=> " + smtlib_expr(f->left, loc, len, i) +
           " " + smtlib_expr(f->right, loc, len, i) + "))";
    break;
  case MLTLFormula::NotEquiv:
    res += "(and (> " + len +
           " 0) (not (= " + smtlib_expr(f->left, loc, len, i) + " " +
           smtlib_expr(f->right, loc, len, i) + ")))";
    break;
  case MLTLFormula::Ite:
    res += "(and (> " + len + " 0) (ite " + smtlib_expr(f->left, loc, len, i) +
           " " + smtlib_expr(f->right, loc, len, i) + " " +
           smtlib_expr(f->tern, loc, len, i) + "))";
    break;
  case MLTLFormula::None:
    cout << "smtlib_expr error: cannot recognize the MLTL formula\n";
    exit(0);
  case MLTLFormula::Global: {
    // (len > (a)) && forall(i, implies(((a + k) <= i) && (i <= (b + k)),
    //  mltl2folLi(f->r_mf(), i, len - i)));

    res += "(and (> " + len + " " + std::to_string(f->lb) + ")(forall ((" +
           quantify_var1 + " Int)) (implies (and (<= (+ " +
           std::to_string(f->lb) + " " + loc + ") " + quantify_var1 +
           ") (<= " + quantify_var1 + " (+ " + std::to_string(f->ub) + " " +
           loc + "))) " + smtlib_expr(f->right, quantify_var1, new_len1, i) +
           ") ) )";

    break;
  }
  case MLTLFormula::Future: {
    res += "(and (> " + len + " " + std::to_string(f->lb) + ") (exists ((" +
           quantify_var1 + " Int)) (and (and (<= (+ " + std::to_string(f->lb) +
           " " + loc + ") " + quantify_var1 + " ) (<= " + quantify_var1 +
           " (+ " + std::to_string(f->ub) + " " + loc + "))) " +
           smtlib_expr(f->right, quantify_var1, new_len1, i) + " ) ) )";
    break;
  }
  case Formula::Until:
    res += "(and (> " + len + " " + std::to_string(f->lb) + ") (exists ((" +
           quantify_var1 + " Int)) (and (and (>= " + quantify_var1 + " " +
           "(+ " + loc + " " + std::to_string(f->lb) + ")" +
           ") (<= " + quantify_var1 + " " + "(+ " + loc + " " +
           std::to_string(f->ub) + ")" + ")) (and " +
           smtlib_expr(f->right, quantify_var1, new_len1, i) + " (forall ((" +
           quantify_var2 + " Int)) (implies (and (>= " + quantify_var2 +
           " (+ " + loc + " " + std::to_string(f->lb) + ")" + ") (< " +
           quantify_var2 + " " + quantify_var1 + ")) " +
           smtlib_expr(f->left, quantify_var2, new_len2, i) + "))))))";
    break;
  case Formula::Release:
    res += "(and (> " + len + " " + std::to_string(f->lb) + ") (forall ((" +
           quantify_var1 + " Int)) (implies (and (>= " + quantify_var1 + " " +
           "(+ " + loc + " " + std::to_string(f->lb) + ")" +
           ") (<= " + quantify_var1 + " " + "(+ " + loc + " " +
           std::to_string(f->ub) + ")" + ")) (or " +
           smtlib_expr(f->right, quantify_var1, new_len1, i) + " (exists ((" +
           quantify_var2 + " Int)) (and (and (>= " + quantify_var2 + " (+ " +
           loc + " " + std::to_string(f->lb) + ")" + ") (< " + quantify_var2 +
           " " + quantify_var1 + ")) " +
           smtlib_expr(f->left, quantify_var2, new_len2, i) + "))))))";
    break;
  case MLTLFormula::Atom:
    res += "(and (> " + len + " 0) (" + f->to_string() + " " + loc + "))";
    break;
  default:
    break;
  }
  return res;
}

std::string Translators::mltl2smtlib(const unique_ptr<MLTLFormula> &f) {
  // FormulaSet V;
  // Make alphabet functions:
  string res = "";
  std::unordered_set<string> alphSet;
  getAlphabet(f.get(),alphSet);
  int i = 0;
  for (auto a : alphSet) {
    res += "(declare-fun " + a + " (Int) Bool)\n";
  }
  std::string start = std::to_string(0);
  std::string len = "len";
  res += "(define-fun f ((k Int) (len Int)) Bool " +
         smtlib_expr(f, start, len, i) + ")\n";

  res += "(assert (exists ((len Int)) (f 0 len)))\n";
  res += "(check-sat)\n";
  return res;
}

// The slow boolean translation:

std::string Translators::mltl2PropSmvSlow(const unique_ptr<MLTLFormula> &f,
                                          int i) {
  string res;
  switch (f->op) {
  case Formula::True:
    res = "true";
    break;
  case Formula::False:
    res = "false";
    break;
  case Formula::Atom:
    res = "(" + f->to_string() + " " + std::to_string(i) + ")";
    break;
  case Formula::Neg:
    res = "(not " + mltl2PropSmvSlow(f->right, i) + ")";
    break;
  case Formula::And:
    res = "(and " + mltl2PropSmvSlow(f->left, i) + " " +
          mltl2PropSmvSlow(f->right, i) + " )";
    break;
  case Formula::Or:
    res = "(or " + mltl2PropSmvSlow(f->left, i) + " " +
          mltl2PropSmvSlow(f->right, i) + " )";
    break;
  case Formula::Equiv:
    res = "(= " + mltl2PropSmvSlow(f->left, i) + " " +
          mltl2PropSmvSlow(f->right, i) + " )";
    break;
  case Formula::Implies:
    res = "(=> " + mltl2PropSmvSlow(f->left, i) + " " +
          mltl2PropSmvSlow(f->right, i) + " )";
    break;
  case Formula::NotEquiv:
    res = "(not (= " + mltl2PropSmvSlow(f->left, i) + " " +
          mltl2PropSmvSlow(f->right, i) + " ))";
    break;
  case Formula::Ite:
    res = "(ite " + mltl2PropSmvSlow(f->left, i) + " " +
          mltl2PropSmvSlow(f->right, i) + " " + mltl2PropSmvSlow(f->tern, i) +
          ")";
    break;
  case Formula::Global: {
    string temp = mltl2PropSmvSlow(f->right, i + f->lb);
    for (int j = f->lb + 1; j <= f->ub; j++) {
      temp = mltl2PropSmvSlow(f->right, i + j) + " " + temp;
    }
    res = "(and " + temp + ")";
    break;
  }
  case Formula::Future: {
    string temp = mltl2PropSmvSlow(f->right, i + f->lb);
    for (int j = f->lb + 1; j <= f->ub; j++) {
      temp = mltl2PropSmvSlow(f->right, i + j) + " " + temp;
    }
    res = "(or " + temp + ")";
    break;
  }
  case Formula::Until: {
    auto temp = mltl2PropSmvSlow(f->right, i + f->ub);
    for (int j = f->ub - 1; j >= f->lb; j--) {
      temp = "(or " + mltl2PropSmvSlow(f->right, i + j) + " (and " +
             mltl2PropSmvSlow(f->left, (i + j)) + " " + temp + "))";
    }
    res = temp;
    break;
  }
  case MLTLFormula::Release: {
    auto temp = mltl2PropSmvSlow(f->right, i + f->ub);
    for (int j = f->ub - 1; j >= f->lb; j--) {
      temp = "(and " + mltl2PropSmvSlow(f->right, i + j) + " (or " +
             mltl2PropSmvSlow(f->left, (i + j)) + " " + temp + "))";
    }
    res = temp;
    break;
  }
  default:
    break;
  }
  return res;
}

std::string Translators::mltl2PropSmvS(const unique_ptr<MLTLFormula> &f) {
  string res = "";
  std::unordered_set<string> alphSet;
  getAlphabet(f.get(),alphSet);
  int i = 0;
  for (auto a : alphSet) {
    res += "(declare-fun " + a + " (Int) Bool)\n";
  }
  res += "(assert " + mltl2PropSmvSlow(f, 0) + ")\n";
  res += "(check-sat)\n";
  return res;
}
// (assert ((and (q 1)  (or false (and (q 2)  (or false (and (q 3)  (or false
// (q 4)))))))))