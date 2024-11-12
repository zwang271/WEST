#include <MLTLFormula.h>
#include <queue>
#include<assert.h>
MLTLFormula::~MLTLFormula(){};

std::string MLTLFormula::to_string() {
  string interval;

  if (op > Formula::Next && op < Formula::None) {
    interval = "[" + std::to_string(lb) + "," + std::to_string(ub) + "]";
  }

  if (op == Formula::Ite){
    return "(" + left->to_string() + "?" + right->to_string() + ":" + tern->to_string() + ")";
  }
  if (left == nullptr && right == nullptr)
    return prop;
  if (left == NULL)
    return "(" + names[op] + interval + " " + right->to_string() + ")";
  if (right == NULL)
    return "(" + left->to_string() + " " + names[op] + interval + ")";
  return "(" + left->to_string() + " " + names[op] + interval + " " +
         right->to_string() + ")";
}

void MLTLFormula::setAccumulated() {
  queue<MLTLFormula *> Q;
  this->alb = 0;
  this->aub = 0;
  Q.push(this);
  while (!Q.empty()) {
    auto v = Q.front();
    Q.pop();
    switch (v->op) {
    case Formula::Neg:
      v->right->alb = v->alb;
      v->right->aub = v->aub;
      Q.push(v->right.get());
      break;
    case Formula::Implies:
    case Formula::Equiv:
    case Formula::NotEquiv:
    case Formula::And:
    case Formula::Or:
      v->left->alb = v->alb;
      v->left->aub = v->aub;
      v->right->alb = v->alb;
      v->right->aub = v->aub;
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case Formula::Ite:
      v->left->alb = v->alb;
      v->left->aub = v->aub;
      v->right->alb = v->alb;
      v->right->aub = v->aub;
      v->tern->alb = v->alb;
      v->tern->aub = v->aub;
      Q.push(v->left.get());
      Q.push(v->right.get());
      Q.push(v->tern.get());
      break;
    case Formula::Future:
    case Formula::Global:
      assert(v->lb <= v->ub);
      v->right->alb = v->alb + (v->lb);
      v->right->aub = v->aub + (v->ub);
      Q.push(v->right.get());
      break;
    case Formula::Until:
    case Formula::Release:
      assert(v->lb <= v->ub);
      v->left->alb = v->alb + (v->lb);
      v->left->aub = v->aub + (v->ub) - 1;
      v->right->alb = v->alb + (v->lb);
      v->right->aub = v->aub + (v->ub);
      Q.push(v->right.get());
      if (v->lb < v->ub) {
        Q.push(v->left.get());
      }
      break;
    default:
      break;
    }
  }
}

std::string MLTLFormula::gf(const int& i){
  if ((this->op == Formula::True) || (this->op == Formula::False)){
    return (this->prop);
  } else {
    return ("(" + this->prop + " " + std::to_string(i) + ")");
  }
}

int MLTLFormula::setProp(int i){
  queue<MLTLFormula *> Q;
  Q.push(this);
  while (!Q.empty()) {
    auto v = Q.front();
    Q.pop();
    switch (v->op) {
    case Formula::Future:
    case Formula::Global:
    case Formula::Neg:
      v->prop = "t" + std::to_string(i++);
      Q.push(v->right.get());
      break;
    case Formula::Implies:
    case Formula::Equiv:
    case Formula::NotEquiv:
    case Formula::And:
    case Formula::Or:
      v->prop = "t" + std::to_string(i++);
      Q.push(v->left.get());
      Q.push(v->right.get());
      break;
    case Formula::Ite:
      v->prop = "t" + std::to_string(i++);
      Q.push(v->left.get());
      Q.push(v->right.get());
      Q.push(v->tern.get());
      break;
    case Formula::Until:
    case Formula::Release:
      assert(v->lb <= v->ub);
      v->prop = "t" + std::to_string(i++);
      Q.push(v->right.get());
      if (v->lb < v->ub) {
        Q.push(v->left.get());
      }
      break;
    default:
      break;
    }
  }
  return i;
}