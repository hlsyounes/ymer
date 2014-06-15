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

#include "typed-value.h"
#include "unique-ptr-vector.h"

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

  std::ostream* os_;
  int parent_precedence_;
};

int GetTernaryOperatorPrecedence() {
  return 0;
}

int GetBinaryOperatorPrecedence(BinaryOperator op) {
  switch (op) {
    case BinaryOperator::IFF:
      return 1;
    case BinaryOperator::IMPLY:
      return 2;
    case BinaryOperator::OR:
      return 3;
    case BinaryOperator::AND:
      return 4;
    case BinaryOperator::EQUAL:
    case BinaryOperator::NOT_EQUAL:
      return 5;
    case BinaryOperator::LESS:
    case BinaryOperator::LESS_EQUAL:
    case BinaryOperator::GREATER_EQUAL:
    case BinaryOperator::GREATER:
      return 6;
    case BinaryOperator::PLUS:
    case BinaryOperator::MINUS:
      return 7;
    case BinaryOperator::MULTIPLY:
    case BinaryOperator::DIVIDE:
      return 8;
  }
  LOG(FATAL) << "bad binary operator";
}

int GetUnaryOperatorPrecedence(UnaryOperator op) {
  switch (op) {
    case UnaryOperator::NEGATE:
    case UnaryOperator::NOT:
      return 9;
  }
  LOG(FATAL) << "bad unary operator";
}

int GetRightPrecedence(BinaryOperator op, int precedence) {
  switch (op) {
    case BinaryOperator::OR:
    case BinaryOperator::AND:
    case BinaryOperator::PLUS:
    case BinaryOperator::MULTIPLY:
      return precedence;
    case BinaryOperator::IFF:
    case BinaryOperator::IMPLY:
    case BinaryOperator::EQUAL:
    case BinaryOperator::NOT_EQUAL:
    case BinaryOperator::LESS:
    case BinaryOperator::LESS_EQUAL:
    case BinaryOperator::GREATER_EQUAL:
    case BinaryOperator::GREATER:
    case BinaryOperator::MINUS:
    case BinaryOperator::DIVIDE:
      return precedence + 1;
  }
  LOG(FATAL) << "bad binary operator";
}

ExpressionPrinter::ExpressionPrinter(std::ostream* os)
    : os_(os), parent_precedence_(GetTernaryOperatorPrecedence()) {
}

void ExpressionPrinter::DoVisitLiteral(const Literal& expr) {
  *os_ << expr.value();
}

void ExpressionPrinter::DoVisitIdentifier(const Identifier& expr) {
  *os_ << expr.name();
}

void ExpressionPrinter::DoVisitFunctionCall(const FunctionCall& expr) {
  int precedence = GetTernaryOperatorPrecedence();
  *os_ << expr.function() << '(';
  std::swap(parent_precedence_, precedence);
  bool first = true;
  for (const Expression& argument : expr.arguments()) {
    if (first) {
      first = false;
    } else {
      *os_ << ", ";
    }
    argument.Accept(this);
  }
  std::swap(parent_precedence_, precedence);
  *os_ << ')';
}

void ExpressionPrinter::DoVisitUnaryOperation(const UnaryOperation& expr) {
  int precedence = GetUnaryOperatorPrecedence(expr.op());
  *os_ << expr.op();
  std::swap(parent_precedence_, precedence);
  expr.operand().Accept(this);
  std::swap(parent_precedence_, precedence);
}

void ExpressionPrinter::DoVisitBinaryOperation(const BinaryOperation& expr) {
  int precedence = GetBinaryOperatorPrecedence(expr.op());
  if (parent_precedence_ > precedence) {
    *os_ << '(';
  }
  std::swap(parent_precedence_, precedence);
  expr.operand1().Accept(this);
  std::swap(parent_precedence_, precedence);
  *os_ << ' ' << expr.op() << ' ';
  int right_precedence = GetRightPrecedence(expr.op(), precedence);
  std::swap(parent_precedence_, right_precedence);
  expr.operand2().Accept(this);
  std::swap(parent_precedence_, right_precedence);
  if (parent_precedence_ > precedence) {
    *os_ << ')';
  }
}

void ExpressionPrinter::DoVisitConditional(const Conditional& expr) {
  int precedence = GetTernaryOperatorPrecedence();
  if (parent_precedence_ > precedence) {
    *os_ << '(';
  }
  std::swap(parent_precedence_, precedence);
  expr.condition().Accept(this);
  *os_ << " ? ";
  expr.if_branch().Accept(this);
  *os_ << " : ";
  expr.else_branch().Accept(this);
  std::swap(parent_precedence_, precedence);
  if (parent_precedence_ > precedence) {
    *os_ << ')';
  }
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
    case UnaryOperator::NOT:
      return os << '!';
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
    case BinaryOperator::AND:
      return os << '&';
    case BinaryOperator::OR:
      return os << '|';
    case BinaryOperator::IMPLY:
      return os << "=>";
    case BinaryOperator::IFF:
      return os << "<=>";
    case BinaryOperator::LESS:
      return os << '<';
    case BinaryOperator::LESS_EQUAL:
      return os << "<=";
    case BinaryOperator::GREATER_EQUAL:
      return os << ">=";
    case BinaryOperator::GREATER:
      return os << '>';
    case BinaryOperator::EQUAL:
      return os << '=';
    case BinaryOperator::NOT_EQUAL:
      return os << "!=";
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
