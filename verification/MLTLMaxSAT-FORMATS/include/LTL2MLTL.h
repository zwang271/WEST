#include "LTLBaseListener.h"
#include "LTLBaseVisitor.h"
#include "LTLLexer.h"
#include "LTLParser.h"
#include "LTLVisitor.h"
#include "antlr4-runtime.h"
#include "MLTLFormula.h"
#include <string>
#include <queue>
#include <functional>
#include <iostream>
#include <random>
#include <MyExpression.h>
using namespace antlrcpp;
using namespace antlr4;
using namespace std;



class LTL2MLTL : public LTLBaseVisitor {
private:
  queue<MyExpression> _Q;
  int _i;
  uniform_int_distribution<int> numDist;
  random_device engn;
  void setf(queue<MyExpression> &vec, unique_ptr<MLTLFormula>&  f);
public:
  LTL2MLTL(std::string in);
  LTL2MLTL(std::string in, std::unique_ptr<MLTLFormula> & f);
  // void compute(const string &in);
  Any visitProgram(LTLParser::ProgramContext *ctx) override;
  Any visitNeg_expr(LTLParser::Neg_exprContext *ctx) override;
  Any visitAnd_expr(LTLParser::And_exprContext *ctx) override;
  Any visitOr_expr(LTLParser::Or_exprContext *ctx) override;
  Any visitEquiv_expr(LTLParser::Equiv_exprContext *ctx) override;
  Any visitNotequiv_expr(LTLParser::Notequiv_exprContext *ctx) override;
  Any visitImplies_expr(LTLParser::Implies_exprContext *ctx) override;
  Any visitNext_expr(LTLParser::Next_exprContext *ctx) override;
  Any visitGlobal_expr(LTLParser::Global_exprContext *ctx) override;
  Any visitFuture_expr(LTLParser::Future_exprContext *ctx) override;
  Any visitUntil_expr(LTLParser::Until_exprContext *ctx) override;
  Any visitIte_expr(LTLParser::Ite_exprContext *ctx) override;
  Any visitAtom_expr(LTLParser::Atom_exprContext *ctx) override;
  Any visitTrue_expr(LTLParser::True_exprContext *ctx) override;
  Any visitFalse_expr(LTLParser::False_exprContext *ctx) override;
  Any visitRelease_expr(LTLParser::Release_exprContext *ctx) override;
  ~LTL2MLTL(){};
};

