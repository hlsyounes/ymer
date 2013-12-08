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

#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>

#include "glog/logging.h"

#include "ddutil.h"
#include "typed-value.h"

Expression::~Expression() = default;

void Expression::Accept(ExpressionVisitor* visitor) const {
  DoAccept(visitor);
}

VariableProperties::VariableProperties(int min_value, int low_bit, int high_bit)
    : min_value_(min_value), low_bit_(low_bit), high_bit_(high_bit) {
  CHECK_LE(low_bit, high_bit);
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
  ExpressionCompiler(
      const DecisionDiagramManager* manager,
      const std::map<std::string, VariableProperties>* variable_properties,
      bool primed);

  ADD mtbdd() const { return mtbdd_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitComputation(const Computation& expr);

  const DecisionDiagramManager* manager_;
  const std::map<std::string, VariableProperties>* variable_properties_;
  bool primed_;
  ADD mtbdd_;
};

ExpressionCompiler::ExpressionCompiler(
    const DecisionDiagramManager* manager,
    const std::map<std::string, VariableProperties>* variable_properties,
    bool primed)
    : manager_(manager), variable_properties_(variable_properties),
      primed_(primed), mtbdd_(manager->GetConstant(0)) {
}

void ExpressionCompiler::DoVisitLiteral(const Literal& expr) {
  mtbdd_ = manager_->GetConstant(expr.value().value<double>());
}

void ExpressionCompiler::DoVisitIdentifier(const Identifier& expr) {
  auto i = variable_properties_->find(expr.name());
  CHECK(i != variable_properties_->end());
  const VariableProperties& p = i->second;
  mtbdd_ = CompileVariable(
      *manager_, p.min_value(), p.low_bit(), p.high_bit(), primed_);
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

ADD mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Expression& e) {
  ExpressionCompiler compiler(
      &manager, &variable_properties, false /* primed */);
  e.Accept(&compiler);
  return compiler.mtbdd();
}

ADD primed_mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Expression& e) {
  ExpressionCompiler compiler(
      &manager, &variable_properties, true /* primed */);
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
  virtual void DoVisitIdentifier(const Identifier& expr);
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

void ExpressionPrinter::DoVisitIdentifier(const Identifier& expr) {
  *os_ << expr.name();
}

void ExpressionPrinter::DoVisitComputation(const Computation& expr) {
  std::set<Computation::Operator> need_parentheses;
  std::swap(need_parentheses_, need_parentheses);
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
  std::swap(need_parentheses_, need_parentheses);
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const Expression& e) {
  ExpressionPrinter printer(&os);
  e.Accept(&printer);
  return os;
}

Computation::Computation(Operator op,
                         std::unique_ptr<const Expression>&& operand1,
                         std::unique_ptr<const Expression>&& operand2)
    : op_(op), operand1_(std::move(operand1)), operand2_(std::move(operand2)) {
}

Computation::~Computation() = default;

std::unique_ptr<const Expression> Computation::Create(
    Operator op,
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  const Literal* v1 = dynamic_cast<const Literal*>(operand1.get());
  if (v1 != nullptr) {
    const Literal* v2 = dynamic_cast<const Literal*>(operand2.get());
    if (v2 != nullptr) {
      TypedValue value(0);
      switch (op) {
        case PLUS:
          value = v1->value() + v2->value();
          break;
        case MINUS:
          value = v1->value() - v2->value();
          break;
        case MULTIPLY:
          value = v1->value() * v2->value();
          break;
        case DIVIDE:
          CHECK(v2->value() != 0);
          value = v1->value() / v2->value();
          break;
      }
      return std::unique_ptr<const Expression>(new Literal(value));
    }
  }
  return std::unique_ptr<const Expression>(new Computation(
      op, std::move(operand1), std::move(operand2)));
}

void Computation::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitComputation(*this);
}

Identifier::Identifier(const std::string& name)
    : name_(name) {
}

Identifier::~Identifier() = default;

std::unique_ptr<const Identifier> Identifier::Create(const std::string& name) {
  return std::unique_ptr<const Identifier>(new Identifier(name));
}

void Identifier::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitIdentifier(*this);
}

Literal::Literal(const TypedValue& value)
    : value_(value) {
}

Literal::~Literal() = default;

void Literal::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitLiteral(*this);
}

ExpressionVisitor::~ExpressionVisitor() = default;

void ExpressionVisitor::VisitLiteral(const Literal& expr) {
  DoVisitLiteral(expr);
}

void ExpressionVisitor::VisitIdentifier(const Identifier& expr) {
  DoVisitIdentifier(expr);
}

void ExpressionVisitor::VisitComputation(const Computation& expr) {
  DoVisitComputation(expr);
}
