// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2013 Google Inc
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

#include <memory>
#include <ostream>
#include <set>
#include <string>

#include "unique-ptr-vector.h"
#include "typed-value.h"

#include "glog/logging.h"

Expression::~Expression() = default;

void Expression::Accept(ExpressionVisitor* visitor) const {
  DoAccept(visitor);
}

namespace {

// An expression visitor that prints an expression to an output stream.
class ExpressionPrinter : public ExpressionVisitor {
 public:
  explicit ExpressionPrinter(std::ostream* os);

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);

  void CallAccept(const Expression& expr);

  std::ostream* os_;
  std::set<BinaryOperator> need_parentheses_;
  int level_;
};

ExpressionPrinter::ExpressionPrinter(std::ostream* os)
    : os_(os), level_(0) {
}

void ExpressionPrinter::DoVisitLiteral(const Literal& expr) {
  *os_ << expr.value();
}

void ExpressionPrinter::DoVisitIdentifier(const Identifier& expr) {
  *os_ << expr.name();
}

void ExpressionPrinter::DoVisitFunctionCall(const FunctionCall& expr) {
  std::set<BinaryOperator> need_parentheses;
  std::swap(need_parentheses_, need_parentheses);
  *os_ << expr.function() << '(';
  bool first = true;
  for (const Expression& argument : expr.arguments()) {
    if (first) {
      first = false;
    } else {
      *os_ << ", ";
    }
    CallAccept(argument);
  }
  *os_ << ')';
  std::swap(need_parentheses_, need_parentheses);
}

void ExpressionPrinter::DoVisitUnaryOperation(const UnaryOperation& expr) {
  std::set<BinaryOperator> need_parentheses;
  std::swap(need_parentheses_, need_parentheses);
  *os_ << expr.op();
  need_parentheses_.insert(BinaryOperator::PLUS);
  need_parentheses_.insert(BinaryOperator::MINUS);
  need_parentheses_.insert(BinaryOperator::MULTIPLY);
  need_parentheses_.insert(BinaryOperator::DIVIDE);
  CallAccept(expr.operand());
  std::swap(need_parentheses_, need_parentheses);
}

void ExpressionPrinter::DoVisitBinaryOperation(const BinaryOperation& expr) {
  std::set<BinaryOperator> need_parentheses;
  std::swap(need_parentheses_, need_parentheses);
  const bool outer = need_parentheses.find(expr.op()) != need_parentheses.end();
  if (outer) {
    *os_ << '(';
  }
  if (expr.op() == BinaryOperator::MULTIPLY
      || expr.op() == BinaryOperator::DIVIDE) {
    need_parentheses_.insert(BinaryOperator::PLUS);
    need_parentheses_.insert(BinaryOperator::MINUS);
  }
  CallAccept(expr.operand1());
  *os_ << expr.op();
  need_parentheses_.clear();
  if (expr.op() != BinaryOperator::PLUS) {
    need_parentheses_.insert(BinaryOperator::PLUS);
    need_parentheses_.insert(BinaryOperator::MINUS);
  }
  if (expr.op() == BinaryOperator::DIVIDE) {
    need_parentheses_.insert(BinaryOperator::MULTIPLY);
    need_parentheses_.insert(BinaryOperator::DIVIDE);
  }
  CallAccept(expr.operand2());
  if (outer) {
    *os_ << ')';
  }
  std::swap(need_parentheses_, need_parentheses);
}

void ExpressionPrinter::DoVisitConditional(const Conditional& expr) {
  std::set<BinaryOperator> need_parentheses;
  std::swap(need_parentheses_, need_parentheses);
  if (level_ > 0) {
    *os_ << '(';
  }
  CallAccept(expr.condition());
  *os_ << " ? ";
  CallAccept(expr.if_branch());
  *os_ << " : ";
  CallAccept(expr.else_branch());
  if (level_ > 0) {
    *os_ << ')';
  }
  std::swap(need_parentheses_, need_parentheses);
}

void ExpressionPrinter::CallAccept(const Expression& expr) {
  ++level_;
  expr.Accept(this);
  --level_;
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const Expression& e) {
  ExpressionPrinter printer(&os);
  e.Accept(&printer);
  return os;
}

Literal::Literal(const TypedValue& value)
    : value_(value) {
}

Literal::~Literal() = default;

std::unique_ptr<const Literal> Literal::New(const TypedValue& value) {
  return std::unique_ptr<const Literal>(new Literal(value));
}

void Literal::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitLiteral(*this);
}

Identifier::Identifier(const std::string& name)
    : name_(name) {
}

Identifier::~Identifier() = default;

std::unique_ptr<const Identifier> Identifier::New(const std::string& name) {
  return std::unique_ptr<const Identifier>(new Identifier(name));
}

void Identifier::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitIdentifier(*this);
}

std::ostream& operator<<(std::ostream& os, Function function) {
  switch (function) {
    case Function::UNKNOWN:
      return os << "<<unknown function>>";
    case Function::MIN:
      return os << "min";
    case Function::MAX:
      return os << "max";
    case Function::FLOOR:
      return os << "floor";
    case Function::CEIL:
      return os << "ceil";
    case Function::POW:
      return os << "pow";
    case Function::LOG:
      return os << "log";
    case Function::MOD:
      return os << "mod";
  }
  LOG(FATAL) << "bad function";
}

FunctionCall::FunctionCall(Function function,
                           UniquePtrVector<const Expression>&& arguments)
    : function_(function), arguments_(std::move(arguments)) {
}

FunctionCall::~FunctionCall() = default;

std::unique_ptr<const FunctionCall> FunctionCall::New(
    Function function, UniquePtrVector<const Expression>&& arguments) {
  return std::unique_ptr<const FunctionCall>(new FunctionCall(
      function, std::move(arguments)));
}

void FunctionCall::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitFunctionCall(*this);
}

std::ostream& operator<<(std::ostream& os, UnaryOperator op) {
  switch (op) {
    case UnaryOperator::NEGATE:
      return os << '-';
  }
  LOG(FATAL) << "bad unary operator";
}

UnaryOperation::UnaryOperation(UnaryOperator op,
                               std::unique_ptr<const Expression>&& operand)
    : op_(op), operand_(std::move(operand)) {
}

UnaryOperation::~UnaryOperation() = default;

std::unique_ptr<const UnaryOperation> UnaryOperation::New(
    UnaryOperator op, std::unique_ptr<const Expression>&& operand) {
  return std::unique_ptr<const UnaryOperation>(new UnaryOperation(
      op, std::move(operand)));
}

void UnaryOperation::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitUnaryOperation(*this);
}

std::ostream& operator<<(std::ostream& os, BinaryOperator op) {
  switch (op) {
    case BinaryOperator::PLUS:
      return os << '+';
    case BinaryOperator::MINUS:
      return os << '-';
    case BinaryOperator::MULTIPLY:
      return os << '*';
    case BinaryOperator::DIVIDE:
      return os << '/';
  }
  LOG(FATAL) << "bad binary operator";
}

BinaryOperation::BinaryOperation(BinaryOperator op,
                                 std::unique_ptr<const Expression>&& operand1,
                                 std::unique_ptr<const Expression>&& operand2)
    : op_(op), operand1_(std::move(operand1)), operand2_(std::move(operand2)) {
}

BinaryOperation::~BinaryOperation() = default;

std::unique_ptr<const BinaryOperation> BinaryOperation::New(
    BinaryOperator op,
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return std::unique_ptr<const BinaryOperation>(new BinaryOperation(
      op, std::move(operand1), std::move(operand2)));
}

void BinaryOperation::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitBinaryOperation(*this);
}

Conditional::Conditional(std::unique_ptr<const Expression>&& condition,
                         std::unique_ptr<const Expression>&& if_branch,
                         std::unique_ptr<const Expression>&& else_branch)
    : condition_(std::move(condition)),
      if_branch_(std::move(if_branch)), else_branch_(std::move(else_branch)) {
}

Conditional::~Conditional() = default;

std::unique_ptr<const Conditional> Conditional::New(
    std::unique_ptr<const Expression>&& condition,
    std::unique_ptr<const Expression>&& if_branch,
    std::unique_ptr<const Expression>&& else_branch) {
  return std::unique_ptr<const Conditional>(new Conditional(
      std::move(condition), std::move(if_branch), std::move(else_branch)));
}

void Conditional::DoAccept(ExpressionVisitor* visitor) const {
  visitor->VisitConditional(*this);
}

ExpressionVisitor::~ExpressionVisitor() = default;

void ExpressionVisitor::VisitLiteral(const Literal& expr) {
  DoVisitLiteral(expr);
}

void ExpressionVisitor::VisitIdentifier(const Identifier& expr) {
  DoVisitIdentifier(expr);
}

void ExpressionVisitor::VisitFunctionCall(const FunctionCall& expr) {
  DoVisitFunctionCall(expr);
}

void ExpressionVisitor::VisitUnaryOperation(const UnaryOperation& expr) {
  DoVisitUnaryOperation(expr);
}

void ExpressionVisitor::VisitBinaryOperation(const BinaryOperation& expr) {
  DoVisitBinaryOperation(expr);
}

void ExpressionVisitor::VisitConditional(const Conditional& expr) {
  DoVisitConditional(expr);
}
