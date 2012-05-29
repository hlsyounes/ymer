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

#include <ostream>
#include <set>
#include <stdexcept>
#include <string>

#include "ddutil.h"
#include "typed-value.h"

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

ADD CompileVariable(const DecisionDiagramManager& manager,
                    int low, int low_bit, int high_bit, bool primed) {
  ADD result = manager.GetConstant(0);
  const int offset = primed ? 1 : 0;
  for (int i = high_bit; i >= low_bit; --i) {
    result = result + (manager.GetAddVariable(2*i + offset) *
                       manager.GetConstant(1 << (high_bit - i)));
  }
  return result + manager.GetConstant(low);
}

class ExpressionCompiler : public ExpressionVisitor {
 public:
  ExpressionCompiler(const DecisionDiagramManager* manager, bool primed);

  ADD mtbdd() const { return mtbdd_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitVariable(const Variable& expr);
  virtual void DoVisitComputation(const Computation& expr);

  const DecisionDiagramManager* manager_;
  bool primed_;
  ADD mtbdd_;
};

ExpressionCompiler::ExpressionCompiler(const DecisionDiagramManager* manager,
                                       bool primed)
    : manager_(manager), primed_(primed), mtbdd_(manager->GetConstant(0)) {
}

void ExpressionCompiler::DoVisitLiteral(const Literal& expr) {
  mtbdd_ = manager_->GetConstant(expr.value().value<double>());
}

void ExpressionCompiler::DoVisitVariable(const Variable& expr) {
  mtbdd_ = CompileVariable(
      *manager_, expr.low(), expr.low_bit(), expr.high_bit(), primed_);
}

void ExpressionCompiler::DoVisitComputation(const Computation& expr) {
  expr.operand1().Accept(this);
  ADD operand1 = mtbdd_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case Computation::PLUS:
      mtbdd_ = operand1 + mtbdd_;
      break;
    case Computation::MINUS:
      mtbdd_ = operand1 - mtbdd_;
      break;
    case Computation::MULTIPLY:
      mtbdd_ = operand1 * mtbdd_;
      break;
    case Computation::DIVIDE:
      mtbdd_ = operand1 / mtbdd_;
      break;
  }
}

}  // namespace

ADD mtbdd(const DecisionDiagramManager& manager, const Expression& e) {
  ExpressionCompiler compiler(&manager, false /* primed */);
  e.Accept(&compiler);
  return compiler.mtbdd();
}

ADD primed_mtbdd(const DecisionDiagramManager& manager, const Expression& e) {
  ExpressionCompiler compiler(&manager, true /* primed */);
  e.Accept(&compiler);
  return compiler.mtbdd();
}

ADD variable_mtbdd(const DecisionDiagramManager& manager,
                   int low, int low_bit, int high_bit) {
  return CompileVariable(manager, low, low_bit, high_bit, false /* primed */);
}

ADD variable_primed_mtbdd(const DecisionDiagramManager& manager,
                          int low, int low_bit, int high_bit) {
  return CompileVariable(manager, low, low_bit, high_bit, true /* primed */);
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
  *os_ << expr.name();
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
    case Computation::PLUS:
      *os_ << '+';
      break;
    case Computation::MINUS:
      *os_ << '-';
      break;
    case Computation::MULTIPLY:
      *os_ << '*';
      break;
    case Computation::DIVIDE:
      *os_ << '/';
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

Variable::Variable(const std::string& name)
    : name_(name) {
}

Variable::~Variable() {
}

void Variable::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitVariable(*this);
}

void Variable::SetVariableProperties(
    int low, int high, int start, int index, int low_bit) {
  low_ = low;
  high_ = high;
  start_ = start;
  index_ = index;
  low_bit_ = low_bit;
  high_bit_ = low_bit + Log2(high - low);
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

ExpressionVisitor::ExpressionVisitor() {
}

ExpressionVisitor::ExpressionVisitor(const ExpressionVisitor&) {
}

ExpressionVisitor& ExpressionVisitor::operator=(const ExpressionVisitor&) {
  return *this;
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
