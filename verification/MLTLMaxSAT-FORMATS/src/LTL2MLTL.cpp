#include <LTL2MLTL.h>

antlrcpp::Any LTL2MLTL::visitProgram(LTLParser::ProgramContext *ctx) {
  return visitChildren(ctx);
};

antlrcpp::Any LTL2MLTL::visitNeg_expr(LTLParser::Neg_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Neg;
  temp.isUnary = true;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs);
  return 0;
};
antlrcpp::Any LTL2MLTL::visitAnd_expr(LTLParser::And_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::And;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitEquiv_expr(LTLParser::Equiv_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Equiv;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitNotequiv_expr(LTLParser::Notequiv_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::NotEquiv;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitImplies_expr(LTLParser::Implies_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Implies;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitOr_expr(LTLParser::Or_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Or;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};
antlrcpp::Any LTL2MLTL::visitNext_expr(LTLParser::Next_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Next;
  temp.isUnary = true;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs);
  return 0;
};
antlrcpp::Any LTL2MLTL::visitGlobal_expr(LTLParser::Global_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Global;
  temp.isUnary = true;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs);
  return 0;
};
antlrcpp::Any LTL2MLTL::visitFuture_expr(LTLParser::Future_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Future;
  temp.isUnary = true;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitUntil_expr(LTLParser::Until_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Until;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitIte_expr(LTLParser::Ite_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Ite;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  visit(subs[2]);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitRelease_expr(LTLParser::Release_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Release;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitAtom_expr(LTLParser::Atom_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Atom;
  temp.isUnary = true;
  temp.isLeaf = true;
  temp.prop = ctx->getText();
  _Q.push(temp);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitTrue_expr(LTLParser::True_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::True;
  temp.isUnary = true;
  temp.isLeaf = true;
  temp.prop = "true";
  _Q.push(temp);
  return 0;
};

antlrcpp::Any LTL2MLTL::visitFalse_expr(LTLParser::False_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::False;
  temp.isUnary = true;
  temp.isLeaf = true;
  temp.prop = "false";
  _Q.push(temp);
  return 0;
};

void LTL2MLTL::setf(queue<MyExpression> &Q, unique_ptr<MLTLFormula> &f) {
  auto item = Q.front();
  Q.pop();
  if (item.op == Formula::Next) {
    f->op = Formula::Global;
    f->lb = 1;
    f->ub = 1;
  } else {
    f->op = item.op;
    auto num1 = numDist(engn);
    auto num2 = numDist(engn);
    (num1 < num2) ? (f->lb = num1, f->ub = num2) : (f->lb = num2, f->ub = num1);
    f->prop = item.prop;
  }

  if ((item.op == MyExpression::Atom) || (item.op == MyExpression::True) ||
      (item.op == MyExpression::False)) {
    return;
  } else if (item.isUnary) {
    f->right = make_unique<MLTLFormula>();
    setf(Q, f->right);
  } else if (f->op == Formula::Ite){
    f->left = make_unique<MLTLFormula>();
    f->right = make_unique<MLTLFormula>();
    f->tern = make_unique<MLTLFormula>();
    setf(Q, f->left);
    setf(Q, f->right);
    setf(Q, f->tern);
  } else {
    f->left = make_unique<MLTLFormula>();
    f->right = make_unique<MLTLFormula>();
    setf(Q, f->left);
    setf(Q, f->right);
  }
}

// LTL2MLTL::~LTL2MLTL(){};
LTL2MLTL::LTL2MLTL(std::string in, std::unique_ptr<MLTLFormula> &f) : _i(0) {
  uniform_int_distribution<int> dist(0, 10000);
  this->numDist = dist;
  ANTLRInputStream input(in);
  LTLLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  tokens.fill();
  LTLParser parser(&tokens);
  auto t = parser.program();
  visit(t);
  f = make_unique<MLTLFormula>();
  setf(_Q, f);
}
