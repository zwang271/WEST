#pragma once
#include "MLTLBaseListener.h"
#include "MLTLBaseVisitor.h"
#include "MLTLLexer.h"
#include "MLTLParser.h"
#include "MLTLVisitor.h"
#include "antlr4-runtime.h"
#include <MLTLFormula.h>
#include <string>
#include <queue>
#include <functional>
#include <iostream>
#include <MyExpression.h>
using namespace antlrcpp;
using namespace antlr4;
using namespace std;



class String2MLTL : public MLTLBaseVisitor {
private:
  queue<MyExpression> _Q;
  int _i;
  void setf(queue<MyExpression> &vec, unique_ptr<MLTLFormula>&  f);
  // friend MLTLFormula;
public:
  String2MLTL(std::string in);
  String2MLTL(std::string in, std::unique_ptr<MLTLFormula> & f);
  // void compute(const string &in);
  Any visitProgram(MLTLParser::ProgramContext *ctx) override;
  Any visitNeg_expr(MLTLParser::Neg_exprContext *ctx) override;
  Any visitAnd_expr(MLTLParser::And_exprContext *ctx) override;
  Any visitOr_expr(MLTLParser::Or_exprContext *ctx) override;
  Any visitEquiv_expr(MLTLParser::Equiv_exprContext *ctx) override;
  Any visitNotequiv_expr(MLTLParser::Notequiv_exprContext *ctx) override;
  Any visitImplies_expr(MLTLParser::Implies_exprContext *ctx) override;
  Any visitGlobal_expr(MLTLParser::Global_exprContext *ctx) override;
  Any visitFuture_expr(MLTLParser::Future_exprContext *ctx) override;
  Any visitUntil_expr(MLTLParser::Until_exprContext *ctx) override;
  Any visitIte_expr(MLTLParser::Ite_exprContext *ctx) override;
  Any visitAtom_expr(MLTLParser::Atom_exprContext *ctx) override;
  Any visitTrue_expr(MLTLParser::True_exprContext *ctx) override;
  Any visitFalse_expr(MLTLParser::False_exprContext *ctx) override;
  Any visitRelease_expr(MLTLParser::Release_exprContext *ctx) override;
  ~String2MLTL(){};
};

