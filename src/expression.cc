// Copyright (C) 2011 Google Inc
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
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"

Expression::Expression() = default;

Expression::~Expression() = default;

IntLiteral::IntLiteral(int value)
    : value_(value) {
}

IntLiteral::~IntLiteral() = default;

std::unique_ptr<const IntLiteral> IntLiteral::Create(int value) {
  return std::unique_ptr<const IntLiteral>(new IntLiteral(value));
}

void IntLiteral::Accept(ExpressionVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitIntLiteral(*this);
}

DoubleLiteral::DoubleLiteral(double value)
    : value_(value) {
}

DoubleLiteral::~DoubleLiteral() = default;

std::unique_ptr<const DoubleLiteral> DoubleLiteral::Create(double value) {
  return std::unique_ptr<const DoubleLiteral>(new DoubleLiteral(value));
}

void DoubleLiteral::Accept(ExpressionVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitDoubleLiteral(*this);
}

BoolLiteral::BoolLiteral(bool value)
    : value_(value) {
}

BoolLiteral::~BoolLiteral() = default;

std::unique_ptr<const BoolLiteral> BoolLiteral::Create(bool value) {
  return std::unique_ptr<const BoolLiteral>(new BoolLiteral(value));
}

void BoolLiteral::Accept(ExpressionVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitBoolLiteral(*this);
}

Identifier::Identifier(const std::string& name)
    : name_(name) {
}

Identifier::~Identifier() = default;

std::unique_ptr<const Identifier> Identifier::Create(const std::string& name) {
  return std::unique_ptr<const Identifier>(new Identifier(name));
}

void Identifier::Accept(ExpressionVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitIdentifier(*this);
}

ArgumentList::ArgumentList() {
}

ArgumentList::ArgumentList(ArgumentList&& argument_list)
    : arguments_(std::move(argument_list.arguments_)) {
}

void ArgumentList::Init() {
}

void ArgumentList::push_back(std::unique_ptr<const Expression>&& argument) {
  CHECK(argument);
  arguments_.push_back(std::move(argument));
}

const Expression& ArgumentList::operator[](int i) const {
  CHECK_GE(i, 0);
  CHECK_LT(i, size());
  return *arguments_[i];
}

std::string Function_Name(Function function) {
  switch (function) {
    case Function::MIN:
      return "min";
    case Function::MAX:
      return "max";
    case Function::FLOOR:
      return "floor";
    case Function::CEIL:
      return "ceil";
    case Function::POW:
      return "pow";
    case Function::MOD:
      return "mod";
    case Function::LOG:
      return "log";
    case Function::NO_FUNCTION:
      break;
  }
  LOG(FATAL) << "bad function";
}

FunctionCall::FunctionCall(Function function, ArgumentList&& arguments)
    : function_(function), arguments_(std::move(arguments)) {
}

FunctionCall::~FunctionCall() = default;

std::unique_ptr<const FunctionCall> FunctionCall::Create(
    Function function, ArgumentList&& arguments) {
  return std::unique_ptr<const FunctionCall>(new FunctionCall(
      function, std::move(arguments)));
}

void FunctionCall::Accept(ExpressionVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitFunctionCall(*this);
}

UnaryOperation::UnaryOperation(UnaryOperator op,
                               std::unique_ptr<const Expression>&& operand)
    : op_(op), operand_(std::move(operand)) {
  CHECK(operand_);
}

UnaryOperation::~UnaryOperation() = default;

std::unique_ptr<const UnaryOperation> UnaryOperation::Create(
    UnaryOperator op, std::unique_ptr<const Expression>&& operand) {
  return std::unique_ptr<const UnaryOperation>(new UnaryOperation(
      op, std::move(operand)));
}

void UnaryOperation::Accept(ExpressionVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitUnaryOperation(*this);
}

BinaryOperation::BinaryOperation(BinaryOperator op,
                                 std::unique_ptr<const Expression>&& operand1,
                                 std::unique_ptr<const Expression>&& operand2)
    : op_(op), operand1_(std::move(operand1)), operand2_(std::move(operand2)) {
  CHECK(operand1_);
  CHECK(operand2_);
}

BinaryOperation::~BinaryOperation() = default;

std::unique_ptr<const BinaryOperation> BinaryOperation::Create(
    BinaryOperator op,
    std::unique_ptr<const Expression>&& operand1,
    std::unique_ptr<const Expression>&& operand2) {
  return std::unique_ptr<const BinaryOperation>(new BinaryOperation(
      op, std::move(operand1), std::move(operand2)));
}

void BinaryOperation::Accept(ExpressionVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitBinaryOperation(*this);
}

Conditional::Conditional(std::unique_ptr<const Expression>&& condition,
                         std::unique_ptr<const Expression>&& if_expr,
                         std::unique_ptr<const Expression>&& else_expr)
    : condition_(std::move(condition)),
      if_expr_(std::move(if_expr)),
      else_expr_(std::move(else_expr)) {
  CHECK(condition_);
  CHECK(if_expr_);
  CHECK(else_expr_);
}

Conditional::~Conditional() = default;

std::unique_ptr<const Conditional> Conditional::Create(
    std::unique_ptr<const Expression>&& condition,
    std::unique_ptr<const Expression>&& if_expr,
    std::unique_ptr<const Expression>&& else_expr) {
  return std::unique_ptr<const Conditional>(new Conditional(
      std::move(condition), std::move(if_expr), std::move(else_expr)));
}

void Conditional::Accept(ExpressionVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitConditional(*this);
}

ExpressionVisitor::ExpressionVisitor() = default;

ExpressionVisitor::~ExpressionVisitor() = default;
