#include <String2MLTL.h>

antlrcpp::Any String2MLTL::visitProgram(MLTLParser::ProgramContext *ctx) {
  return visitChildren(ctx);
};

antlrcpp::Any String2MLTL::visitNeg_expr(MLTLParser::Neg_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Neg;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = true;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs);
  return 0;
};
antlrcpp::Any String2MLTL::visitAnd_expr(MLTLParser::And_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::And;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any String2MLTL::visitNotequiv_expr(MLTLParser::Notequiv_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::NotEquiv;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any String2MLTL::visitEquiv_expr(MLTLParser::Equiv_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Equiv;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any
String2MLTL::visitImplies_expr(MLTLParser::Implies_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Implies;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any String2MLTL::visitOr_expr(MLTLParser::Or_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Or;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};
antlrcpp::Any
String2MLTL::visitGlobal_expr(MLTLParser::Global_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Global;
  auto bnds = ctx->Number();
  temp.lb = stoi(string(bnds[0]->getText()));
  temp.ub = stoi(string(bnds[1]->getText()));
  temp.isUnary = true;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs);
  return 0;
};
antlrcpp::Any
String2MLTL::visitFuture_expr(MLTLParser::Future_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Future;
  auto bnds = ctx->Number();
  temp.lb = stoi(string(bnds[0]->getText()));
  temp.ub = stoi(string(bnds[1]->getText()));
  temp.isUnary = true;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs);
  return 0;
};

antlrcpp::Any String2MLTL::visitUntil_expr(MLTLParser::Until_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Until;
  auto bounds = ctx->Number();
  auto bnds = ctx->Number();
  temp.lb = stoi(string(bnds[0]->getText()));
  temp.ub = stoi(string(bnds[1]->getText()));
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any
String2MLTL::visitRelease_expr(MLTLParser::Release_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Release;
  auto bnds = ctx->Number();
  temp.lb = stoi(string(bnds[0]->getText()));
  temp.ub = stoi(string(bnds[1]->getText()));
  temp.isUnary = false;
  temp.prop = "t" + to_string(_i++);
  _Q.push(temp);
  auto subs = ctx->expr();
  visit(subs[0]);
  visit(subs[1]);
  return 0;
};

antlrcpp::Any String2MLTL::visitIte_expr(MLTLParser::Ite_exprContext *ctx) {
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

antlrcpp::Any String2MLTL::visitAtom_expr(MLTLParser::Atom_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::Atom;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = true;
  temp.isLeaf = true;
  temp.prop = ctx->getText();
  _Q.push(temp);
  return 0;
};

antlrcpp::Any String2MLTL::visitTrue_expr(MLTLParser::True_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::True;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = true;
  temp.isLeaf = true;
  temp.prop = "true";
  _Q.push(temp);
  return 0;
};

antlrcpp::Any String2MLTL::visitFalse_expr(MLTLParser::False_exprContext *ctx) {
  MyExpression temp;
  temp.op = MyExpression::False;
  temp.lb = 0;
  temp.ub = 0;
  temp.isUnary = true;
  temp.isLeaf = true;
  temp.prop = "false";
  _Q.push(temp);
  return 0;
};

void String2MLTL::setf(queue<MyExpression> &Q, unique_ptr<MLTLFormula> &f) {
  auto item = Q.front();
  Q.pop();
  f->op = item.op;
  f->lb = item.lb;
  f->ub = item.ub;
  f->prop = item.prop;
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

// String2MLTL::~String2MLTL(){};
String2MLTL::String2MLTL(std::string in, std::unique_ptr<MLTLFormula> &f)
    : _i(0) {
  ANTLRInputStream input(in);
  MLTLLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  tokens.fill();
  MLTLParser parser(&tokens);
  auto t = parser.program();
  visit(t);
  f = make_unique<MLTLFormula>();
  setf(_Q, f);
}
