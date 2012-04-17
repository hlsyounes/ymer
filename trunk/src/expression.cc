// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2012 Google Inc
//
// This file is part of Ymer.
//
// Ymer is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Ymer is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ymer; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#include "expression.h"

#include <stdexcept>
#include <typeinfo>

#include "cudd.h"

Expression::Expression()
    : ref_count_(0) {
}

Expression::~Expression() {
}

void Expression::Accept(ExpressionVisitor* visitor) const {
  DoAccept(visitor);
}

void Expression::ref(const Expression* e) {
  if (e != NULL) {
    ++e->ref_count_;
  }
}

void Expression::destructive_deref(const Expression* e) {
  if (e != NULL) {
    --e->ref_count_;
    if (e->ref_count_ == 0) {
      delete e;
    }
  }
}

namespace {

// An expression visitor that prints an expression to an output stream.
class ExpressionPrinter : public ExpressionVisitor {
 public:
  explicit ExpressionPrinter(std::ostream* os);

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitVariable(const Variable& expr);
  virtual void DoVisitComputation(const Computation& expr);

  std::ostream* os_;
  std::set<Computation::Operator> need_parentheses_;
};

ExpressionPrinter::ExpressionPrinter(std::ostream* os)
    : os_(os) {
}

void ExpressionPrinter::DoVisitLiteral(const Literal& expr) {
  *os_ << expr.value();
}

void ExpressionPrinter::DoVisitVariable(const Variable& expr) {
  *os_ << 'v' << expr.low_bit();
  if (expr.low_bit() != expr.high_bit()) {
    *os_ << '_' << expr.high_bit();
  }
}

void ExpressionPrinter::DoVisitComputation(const Computation& expr) {
  std::set<Computation::Operator> need_parentheses;
  swap(need_parentheses_, need_parentheses);
  const bool outer = need_parentheses.find(expr.op()) != need_parentheses.end();
  if (outer) {
    *os_ << '(';
  }
  if (expr.op() == Computation::MULTIPLY || expr.op() == Computation::DIVIDE) {
    need_parentheses_.insert(Computation::PLUS);
    need_parentheses_.insert(Computation::MINUS);
  }
  expr.operand1().Accept(this);
  switch (expr.op()) {
    case Computation::MULTIPLY:
      *os_ << '*';
      break;
    case Computation::DIVIDE:
      *os_ << '/';
      break;
    case Computation::PLUS:
      *os_ << '+';
      break;
    case Computation::MINUS:
      *os_ << '-';
      break;
  }
  need_parentheses_.clear();
  if (expr.op() != Computation::PLUS) {
    need_parentheses_.insert(Computation::PLUS);
    need_parentheses_.insert(Computation::MINUS);
  }
  if (expr.op() == Computation::DIVIDE) {
    need_parentheses_.insert(Computation::MULTIPLY);
    need_parentheses_.insert(Computation::DIVIDE);
  }
  expr.operand2().Accept(this);
  if (outer) {
    *os_ << ')';
  }
  swap(need_parentheses_, need_parentheses);
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const Expression& e) {
  ExpressionPrinter printer(&os);
  e.Accept(&printer);
  return os;
}

Computation::Computation(Operator op,
                         const Expression& operand1,
                         const Expression& operand2)
    : op_(op), operand1_(&operand1), operand2_(&operand2) {
  ref(operand1_);
  ref(operand2_);
}

Computation::~Computation() {
  destructive_deref(operand1_);
  destructive_deref(operand2_);
}

void Computation::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitComputation(*this);
}

Addition::Addition(const Expression& term1, const Expression& term2)
    : Computation(PLUS, term1, term2) {
}

Addition::~Addition() {
}

const Expression& Addition::make(const Expression& term1,
                                 const Expression& term2) {
  const Literal* v1 = dynamic_cast<const Literal*>(&term1);
  if (v1 != NULL) {
    const Literal* v2 = dynamic_cast<const Literal*>(&term2);
    if (v2 != NULL) {
      const Literal& value = *new Literal(v1->value() + v2->value());
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new Addition(term1, term2);
}

TypedValue Addition::value(const ValueMap& values) const {
  return operand1().value(values) + operand2().value(values);
}

const Expression& Addition::substitution(const ValueMap& values) const {
  const Expression& e1 = operand1().substitution(values);
  const Expression& e2 = operand2().substitution(values);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}

const Addition& Addition::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = operand1().substitution(subst);
  const Expression& e2 = operand2().substitution(subst);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return *new Addition(e1, e2);
  } else {
    return *this;
  }
}

DdNode* Addition::mtbdd(const DecisionDiagramManager& dd_man) const {
  DdNode* dd1 = operand1().mtbdd(dd_man);
  DdNode* dd2 = operand2().mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man.manager(), Cudd_addPlus, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return ddc;
}

DdNode* Addition::primed_mtbdd(const DecisionDiagramManager& dd_man) const {
  DdNode* dd1 = operand1().primed_mtbdd(dd_man);
  DdNode* dd2 = operand2().primed_mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man.manager(), Cudd_addPlus, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return ddc;
}

Subtraction::Subtraction(const Expression& term1, const Expression& term2)
    : Computation(MINUS, term1, term2) {
}

Subtraction::~Subtraction() {
}

const Expression& Subtraction::make(const Expression& term1,
                                    const Expression& term2) {
  const Literal* v1 = dynamic_cast<const Literal*>(&term1);
  if (v1 != NULL) {
    const Literal* v2 = dynamic_cast<const Literal*>(&term2);
    if (v2 != NULL) {
      const Literal& value = *new Literal(v1->value() - v2->value());
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new Subtraction(term1, term2);
}

TypedValue Subtraction::value(const ValueMap& values) const {
  return operand1().value(values) - operand2().value(values);
}

const Expression& Subtraction::substitution(const ValueMap& values) const {
  const Expression& e1 = operand1().substitution(values);
  const Expression& e2 = operand2().substitution(values);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}

const Subtraction& Subtraction::substitution(
    const SubstitutionMap& subst) const {
  const Expression& e1 = operand1().substitution(subst);
  const Expression& e2 = operand2().substitution(subst);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return *new Subtraction(e1, e2);
  } else {
    return *this;
  }
}

DdNode* Subtraction::mtbdd(const DecisionDiagramManager& dd_man) const {
  DdNode* dd1 = operand1().mtbdd(dd_man);
  DdNode* dd2 = operand2().mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man.manager(), Cudd_addMinus, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return ddc;
}

DdNode* Subtraction::primed_mtbdd(const DecisionDiagramManager& dd_man) const {
  DdNode* dd1 = operand1().primed_mtbdd(dd_man);
  DdNode* dd2 = operand2().primed_mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man.manager(), Cudd_addMinus, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return ddc;
}

Multiplication::Multiplication(const Expression& factor1,
                               const Expression& factor2)
    : Computation(MULTIPLY, factor1, factor2) {
}

Multiplication::~Multiplication() {
}

const Expression& Multiplication::make(const Expression& factor1,
                                       const Expression& factor2) {
  const Literal* v1 = dynamic_cast<const Literal*>(&factor1);
  if (v1 != NULL) {
    const Literal* v2 = dynamic_cast<const Literal*>(&factor2);
    if (v2 != NULL) {
      const Literal& value = *new Literal(v1->value() * v2->value());
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new Multiplication(factor1, factor2);
}

TypedValue Multiplication::value(const ValueMap& values) const {
  return operand1().value(values) * operand2().value(values);
}

const Expression& Multiplication::substitution(const ValueMap& values) const {
  const Expression& e1 = operand1().substitution(values);
  const Expression& e2 = operand2().substitution(values);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}

const Multiplication& Multiplication::substitution(
    const SubstitutionMap& subst) const {
  const Expression& e1 = operand1().substitution(subst);
  const Expression& e2 = operand2().substitution(subst);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return *new Multiplication(e1, e2);
  } else {
    return *this;
  }
}

DdNode* Multiplication::mtbdd(const DecisionDiagramManager& dd_man) const {
  DdNode* dd1 = operand1().mtbdd(dd_man);
  DdNode* dd2 = operand2().mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man.manager(), Cudd_addTimes, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return ddc;
}

DdNode* Multiplication::primed_mtbdd(
    const DecisionDiagramManager& dd_man) const {
  DdNode* dd1 = operand1().primed_mtbdd(dd_man);
  DdNode* dd2 = operand2().primed_mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man.manager(), Cudd_addTimes, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return ddc;
}

Division::Division(const Expression& factor1, const Expression& factor2)
    : Computation(DIVIDE, factor1, factor2) {
}

Division::~Division() {
}

const Expression& Division::make(const Expression& factor1,
                                 const Expression& factor2) {
  const Literal* v1 = dynamic_cast<const Literal*>(&factor1);
  if (v1 != NULL) {
    const Literal* v2 = dynamic_cast<const Literal*>(&factor2);
    if (v2 != NULL) {
      if (v2->value() == 0) {
        throw std::invalid_argument("division by zero");
      }
      const Literal& value = *new Literal(v1->value() / v2->value());
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
      return value;
    }
  }
  return *new Division(factor1, factor2);
}

TypedValue Division::value(const ValueMap& values) const {
  return operand1().value(values) / operand2().value(values);
}

const Expression& Division::substitution(const ValueMap& values) const {
  const Expression& e1 = operand1().substitution(values);
  const Expression& e2 = operand2().substitution(values);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}

const Division& Division::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = operand1().substitution(subst);
  const Expression& e2 = operand2().substitution(subst);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return *new Division(e1, e2);
  } else {
    return *this;
  }
}

DdNode* Division::mtbdd(const DecisionDiagramManager& dd_man) const {
  DdNode* dd1 = operand1().mtbdd(dd_man);
  DdNode* dd2 = operand2().mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man.manager(), Cudd_addDivide, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return ddc;
}

DdNode* Division::primed_mtbdd(const DecisionDiagramManager& dd_man) const {
  DdNode* dd1 = operand1().primed_mtbdd(dd_man);
  DdNode* dd2 = operand2().primed_mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man.manager(), Cudd_addDivide, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return ddc;
}

Variable::Variable()
    : mtbdd_(NULL), primed_mtbdd_(NULL), identity_bdd_(NULL) {
}

Variable::Variable(int low, int high, int start, int low_bit)
    : low_(low), high_(high), start_(start), mtbdd_(NULL), primed_mtbdd_(NULL),
      identity_bdd_(NULL) {
  set_low_bit(low_bit);
}

Variable::~Variable() {
}

void Variable::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitVariable(*this);
}

void Variable::set_low(int low) {
  low_ = low;
}

void Variable::set_high(int high) {
  high_ = high;
}

void Variable::set_start(int start) {
  start_ = start;
}

void Variable::set_low_bit(int low_bit) {
  low_bit_ = low_bit;
  int h = high() - low();
  int nbits = 0;
  while (h > 0) {
    h >>= 1;
    nbits++;
  }
  high_bit_ = low_bit_ + nbits - 1;
}

TypedValue Variable::value(const ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(this);
  if (vi != values.end()) {
    return (*vi).second;
  } else {
    throw std::logic_error("unbound variable");
  }
}

const Expression& Variable::substitution(const ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(this);
  if (vi != values.end()) {
    return *new Literal((*vi).second);
  } else {
    return *this;
  }
}

const Variable& Variable::substitution(const SubstitutionMap& subst) const {
  SubstitutionMap::const_iterator si = subst.find(this);
  if (si != subst.end()) {
    return *(*si).second;
  } else {
    return *this;
  }
}

DdNode* Variable::mtbdd(const DecisionDiagramManager& dd_man) const {
  if (mtbdd_ == NULL) {
    ADD dd = dd_man.GetConstant(0);
    for (int i = high_bit(); i >= low_bit(); --i) {
      dd = dd + (dd_man.GetAddVariable(2*i) *
                 dd_man.GetConstant(1 << (high_bit() - i)));
    }
    mtbdd_ = (dd + dd_man.GetConstant(low())).release();
  } else {
    Cudd_Ref(mtbdd_);
  }
  return mtbdd_;
}

DdNode* Variable::primed_mtbdd(const DecisionDiagramManager& dd_man) const {
  if (primed_mtbdd_ == NULL) {
    ADD dd = dd_man.GetConstant(0);
    for (int i = high_bit(); i >= low_bit(); --i) {
      dd = dd + (dd_man.GetAddVariable(2*i + 1) *
                 dd_man.GetConstant(1 << (high_bit() - i)));
    }
    primed_mtbdd_ = (dd + dd_man.GetConstant(low())).release();
  } else {
    Cudd_Ref(primed_mtbdd_);
  }
  return primed_mtbdd_;
}

DdNode* Variable::identity_bdd(const DecisionDiagramManager& dd_man) const {
  if (identity_bdd_ == NULL) {
    mtbdd(dd_man);
    primed_mtbdd(dd_man);
    DdNode* dde =
        Cudd_addApply(dd_man.manager(), Cudd_addMinus, mtbdd_, primed_mtbdd_);
    Cudd_Ref(dde);
    Cudd_RecursiveDeref(dd_man.manager(), mtbdd_);
    Cudd_RecursiveDeref(dd_man.manager(), primed_mtbdd_);
    identity_bdd_ = Cudd_addBddInterval(dd_man.manager(), dde, 0, 0);
    Cudd_Ref(identity_bdd_);
    Cudd_RecursiveDeref(dd_man.manager(), dde);
  } else {
    Cudd_Ref(identity_bdd_);
  }
  return identity_bdd_;
}

DdNode* Variable::range_bdd(const DecisionDiagramManager& dd_man) const {
  DdNode* range;
  if (high() - low() == (1 << (high_bit() - low_bit() + 1)) - 1) {
    range = dd_man.GetConstant(1).release();
  } else {
    mtbdd(dd_man);
    DdNode* ddr = Cudd_addBddInterval(dd_man.manager(), mtbdd_, low(), high());
    Cudd_Ref(ddr);
    Cudd_RecursiveDeref(dd_man.manager(), mtbdd_);
    primed_mtbdd(dd_man);
    DdNode* ddp =
        Cudd_addBddInterval(dd_man.manager(), primed_mtbdd_, low(), high());
    Cudd_Ref(ddp);
    Cudd_RecursiveDeref(dd_man.manager(), primed_mtbdd_);
    range = Cudd_bddAnd(dd_man.manager(), ddr, ddp);
    Cudd_Ref(range);
    Cudd_RecursiveDeref(dd_man.manager(), ddr);
    Cudd_RecursiveDeref(dd_man.manager(), ddp);
  }
  return range;
}

void Variable::uncache_dds(const DecisionDiagramManager& dd_man) const {
  if (mtbdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), mtbdd_);
    mtbdd_ = NULL;
  }
  if (primed_mtbdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), primed_mtbdd_);
    primed_mtbdd_ = NULL;
  }
  if (identity_bdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), identity_bdd_);
    identity_bdd_ = NULL;
  }
}

Literal::Literal(const TypedValue& value)
    : value_(value) {
}

Literal::~Literal() {
}

void Literal::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitLiteral(*this);
}

TypedValue Literal::value(const ValueMap& values) const {
  return value();
}

const Literal& Literal::substitution(const ValueMap& values) const {
  return *this;
}

const Literal& Literal::substitution(const SubstitutionMap& subst) const {
  return *this;
}

DdNode* Literal::mtbdd(const DecisionDiagramManager& dd_man) const {
  return dd_man.GetConstant(value().value<double>()).release();
}

DdNode* Literal::primed_mtbdd(const DecisionDiagramManager& dd_man) const {
  return dd_man.GetConstant(value().value<double>()).release();
}

ExpressionVisitor::ExpressionVisitor() {
}

ExpressionVisitor::ExpressionVisitor(const ExpressionVisitor&) {
}

ExpressionVisitor& ExpressionVisitor::operator=(const ExpressionVisitor&) {
}

ExpressionVisitor::~ExpressionVisitor() {
}

void ExpressionVisitor::VisitLiteral(const Literal& expr) {
  DoVisitLiteral(expr);
}

void ExpressionVisitor::VisitVariable(const Variable& expr) {
  DoVisitVariable(expr);
}

void ExpressionVisitor::VisitComputation(const Computation& expr) {
  DoVisitComputation(expr);
}
