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
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);

  std::ostream* os_;
  std::set<BinaryOperator> need_parentheses_;
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

void ExpressionPrinter::DoVisitUnaryOperation(const UnaryOperation& expr) {
  std::set<BinaryOperator> need_parentheses;
  std::swap(need_parentheses_, need_parentheses);
  *os_ << expr.op();
  need_parentheses_.insert(BinaryOperator::PLUS);
  need_parentheses_.insert(BinaryOperator::MINUS);
  need_parentheses_.insert(BinaryOperator::MULTIPLY);
  need_parentheses_.insert(BinaryOperator::DIVIDE);
  expr.operand().Accept(this);
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
  expr.operand1().Accept(this);
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

ExpressionVisitor::~ExpressionVisitor() = default;

void ExpressionVisitor::VisitLiteral(const Literal& expr) {
  DoVisitLiteral(expr);
}

void ExpressionVisitor::VisitIdentifier(const Identifier& expr) {
  DoVisitIdentifier(expr);
}

void ExpressionVisitor::VisitUnaryOperation(const UnaryOperation& expr) {
  DoVisitUnaryOperation(expr);
}

void ExpressionVisitor::VisitBinaryOperation(const BinaryOperation& expr) {
  DoVisitBinaryOperation(expr);
}
