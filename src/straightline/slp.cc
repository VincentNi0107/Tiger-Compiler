#include "straightline/slp.h"

#include <iostream>

namespace A {
int A::CompoundStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return std::max(stm1->MaxArgs(), stm2->MaxArgs());
}

Table *A::CompoundStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  return stm2->Interp(stm1->Interp(t));
}

int A::AssignStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return exp->MaxArgs();
}

Table *A::AssignStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  IntAndTable *result = exp->Interp(t);
  return result->t->Update(id, result->i);
}

int A::PrintStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return std::max(exps->NumExps(),exps->MaxArgs());
}

Table *A::PrintStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  return exps->Interp(t);
}

int A::IdExp::MaxArgs() const{
  return 0;
}

IntAndTable *A::IdExp::Interp(Table *t) const{
  return new IntAndTable(t->Lookup(id), t);
}

int A::NumExp::MaxArgs() const{
  return 0;
}

IntAndTable *A::NumExp::Interp(Table *t) const{
  return new IntAndTable(num, t);
}

int A::OpExp::MaxArgs() const{
  return std::max(left->MaxArgs(),right->MaxArgs());
}

IntAndTable *A::OpExp::Interp(Table *t) const{
  IntAndTable *tmp = left->Interp(t);
  int leftResult = tmp->i;
  tmp = right->Interp(tmp->t);
  int rightResult = tmp->i;
  int result;
  switch (oper){
    case PLUS:
      result = leftResult + rightResult;
      break;
    case MINUS:
      result = leftResult - rightResult;
      break;
    case TIMES:
      result = leftResult * rightResult;
      break;
    case DIV:
      result = leftResult / rightResult;
      break;
    default:
      break;
  }
  return new IntAndTable(result, tmp->t);
}

int A::EseqExp::MaxArgs() const{
  return std::max(stm->MaxArgs(),exp->MaxArgs());
}

IntAndTable *A::EseqExp::Interp(Table *t) const{
  t = stm->Interp(t);
  return exp->Interp(t);
}

int A::PairExpList::MaxArgs() const{
  return std::max(exp->MaxArgs(), tail->MaxArgs());
}

int A::PairExpList::NumExps() const{
  return 1 + tail->NumExps();
}

Table *A::PairExpList::Interp(Table *t) const{
  IntAndTable *tmp = exp->Interp(t);
  std::cout << tmp->i << ' ';
  return tail->Interp(tmp->t);
}

int A::LastExpList::MaxArgs() const{
  return exp->MaxArgs();
}

int A::LastExpList::NumExps() const{
  return 1;
}

Table *A::LastExpList::Interp(Table *t) const{
  IntAndTable *tmp = exp->Interp(t);
  std::cout << tmp->i << '\n';
  return tmp->t;
}

int Table::Lookup(const std::string &key) const {
  if (id == key) {
    return value;
  } else if (tail != nullptr) {
    return tail->Lookup(key);
  } else {
    assert(false);
  }
}

Table *Table::Update(const std::string &key, int val) const {
  return new Table(key, val, this);
}
}  // namespace A
