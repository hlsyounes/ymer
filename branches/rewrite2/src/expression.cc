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
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"

#include "type.h"

template <typename IdType, typename ValueType>
Expression<IdType, ValueType>::Expression() = default;

template <typename IdType, typename ValueType>
Expression<IdType, ValueType>::~Expression() = default;

template class Expression<std::string, TypedValue>;
template class Expression<int, TypedValue>;
template class Expression<int, int>;
template class Expression<int, double>;

template <typename IdType, typename ValueType>
Literal<IdType, ValueType>::Literal(const ValueType& value)
    : value_(value) {
}

template <typename IdType, typename ValueType>
Literal<IdType, ValueType>::~Literal() = default;

template <typename IdType, typename ValueType>
std::unique_ptr<const Literal<IdType, ValueType> >
Literal<IdType, ValueType>::Create(
    const ValueType& value) {
  return std::unique_ptr<const Literal<IdType, ValueType> >(
      new Literal<IdType, ValueType>(value));
}

template <typename IdType, typename ValueType>
ValueType Literal<IdType, ValueType>::ValueInState(
    const State<IdType>& state) const {
  return value();
}

template <typename IdType, typename ValueType>
void Literal<IdType, ValueType>::Accept(
    ExpressionVisitor<IdType, ValueType>* visitor) const {
  CHECK(visitor);
  visitor->VisitLiteral(*this);
}

template class Literal<std::string, TypedValue>;
template class Literal<int, TypedValue>;
template class Literal<int, int>;
template class Literal<int, double>;

template <typename IdType, typename ValueType>
Identifier<IdType, ValueType>::Identifier(const IdType& id)
    : id_(id) {
}

template <typename IdType, typename ValueType>
Identifier<IdType, ValueType>::~Identifier() = default;

template <typename IdType, typename ValueType>
std::unique_ptr<const Identifier<IdType, ValueType> >
Identifier<IdType, ValueType>::Create(
    const IdType& id) {
  return std::unique_ptr<const Identifier<IdType, ValueType> >(
      new Identifier<IdType, ValueType>(id));
}

template <>
TypedValue Identifier<std::string, TypedValue>::ValueInState(
    const State<std::string>& state) const {
  auto i = state.find(id());
  CHECK(i != state.end());
  return i->second;
}

template <>
TypedValue Identifier<int, TypedValue>::ValueInState(
    const State<int>& state) const {
  return state[id()];
}

template <>
int Identifier<int, int>::ValueInState(const State<int>& state) const {
  return state[id()];
}

template <>
double Identifier<int, double>::ValueInState(const State<int>& state) const {
  return state[id()];
}

template <typename IdType, typename ValueType>
void Identifier<IdType, ValueType>::Accept(
    ExpressionVisitor<IdType, ValueType>* visitor) const {
  CHECK(visitor);
  visitor->VisitIdentifier(*this);
}

template class Identifier<std::string, TypedValue>;
template class Identifier<int, TypedValue>;
template class Identifier<int, int>;
template class Identifier<int, double>;

template <typename IdType, typename ValueType>
ArgumentList<IdType, ValueType>::ArgumentList() {
}

template <typename IdType, typename ValueType>
ArgumentList<IdType, ValueType>::ArgumentList(
    ArgumentList<IdType, ValueType>&& argument_list)
    : arguments_(std::move(argument_list.arguments_)) {
}

template <typename IdType, typename ValueType>
void ArgumentList<IdType, ValueType>::Init() {
}

template <typename IdType, typename ValueType>
void ArgumentList<IdType, ValueType>::push_back(
    std::unique_ptr<const Expression<IdType, ValueType> >&& argument) {
  CHECK(argument);
  arguments_.push_back(std::move(argument));
}

template <typename IdType, typename ValueType>
const Expression<IdType, ValueType>&
ArgumentList<IdType, ValueType>::operator[](int i) const {
  CHECK_GE(i, 0);
  CHECK_LT(i, size());
  return *arguments_[i];
}

template class ArgumentList<std::string, TypedValue>;
template class ArgumentList<int, TypedValue>;
template class ArgumentList<int, int>;
template class ArgumentList<int, double>;

std::ostream& operator<<(std::ostream& os, Function function) {
  switch (function) {
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
    case Function::MOD:
      return os << "mod";
    case Function::LOG:
      return os << "log";
    case Function::NO_FUNCTION:
      break;
  }
  LOG(FATAL) << "bad function";
}

template <typename IdType, typename ValueType>
FunctionCall<IdType, ValueType>::~FunctionCall() = default;

namespace {

template <typename ValueType>
ValueType ApplyUnaryFunction(Function function, ValueType arg) {
  switch (function) {
    case Function::FLOOR:
      return floor(arg);
    case Function::CEIL:
      return ceil(arg);
  }
  LOG(FATAL) << "bad function";
}

TypedValue mod(TypedValue arg1, TypedValue arg2) {
  return arg1 % arg2;
}

int mod(int arg1, int arg2) {
  return arg1 % arg2;
}

double mod(double arg1, double arg2) {
  LOG(FATAL) << "internal error";
}

template <typename ValueType>
ValueType ApplyBinaryFunction(
    Function function, ValueType arg1, ValueType arg2) {
  switch (function) {
    case Function::MIN:
      return std::min(arg1, arg2);
    case Function::MAX:
      return std::max(arg1, arg2);
    case Function::POW:
      return pow(arg1, arg2);
    case Function::LOG:
      return log(arg1) / log(arg2);
    case Function::MOD:
      return mod(arg1, arg2);
  }
  LOG(FATAL) << "bad function";
}

}  // namespace

template <typename IdType, typename ValueType>
ValueType FunctionCall<IdType, ValueType>::ValueInState(
    const State<IdType>& state) const {
  std::vector<ValueType> values;
  for (const Expression<IdType, ValueType>& argument: arguments()) {
    values.push_back(argument.ValueInState(state));
  }
  switch (function()) {
    default:
    case Function::NO_FUNCTION:
      LOG(FATAL) << "bad function";
    case Function::MIN:
    case Function::MAX: {
      CHECK_GE(values.size(), 1);
      ValueType value = values[0];
      for (int i = 1; i < values.size(); ++i) {
        value = ApplyBinaryFunction(function(), value, values[i]);
      }
      return value;
    }
    case Function::FLOOR:
    case Function::CEIL:
      CHECK_EQ(1, values.size());
      return ApplyUnaryFunction(function(), values[0]);
    case Function::POW:
    case Function::LOG:
    case Function::MOD:
      CHECK_EQ(2, values.size());
      return ApplyBinaryFunction(function(), values[0], values[1]);
  }
}

template <typename IdType, typename ValueType>
void FunctionCall<IdType, ValueType>::Accept(
    ExpressionVisitor<IdType, ValueType>* visitor) const {
  CHECK(visitor);
  visitor->VisitFunctionCall(*this);
}

template class FunctionCall<std::string, TypedValue>;
template class FunctionCall<int, TypedValue>;
template class FunctionCall<int, int>;
template class FunctionCall<int, double>;

std::ostream& operator<<(std::ostream& os, UnaryOperator op) {
  switch (op) {
    case UnaryOperator::NEGATE:
      return os << "-";
    case UnaryOperator::NOT:
      return os << "!";
  }
  LOG(FATAL) << "bad unary operator";
}

template <typename IdType, typename ValueType>
UnaryOperation<IdType, ValueType>::UnaryOperation(
    UnaryOperator op,
    std::unique_ptr<const Expression<IdType, ValueType> >&& operand)
    : op_(op), operand_(std::move(operand)) {
  CHECK(operand_);
}

template <typename IdType, typename ValueType>
UnaryOperation<IdType, ValueType>::~UnaryOperation() = default;

template <typename IdType, typename ValueType>
std::unique_ptr<const UnaryOperation<IdType, ValueType> >
UnaryOperation<IdType, ValueType>::Create(
    UnaryOperator op,
    std::unique_ptr<const Expression<IdType, ValueType> >&& operand) {
  return std::unique_ptr<const UnaryOperation<IdType, ValueType> >(
      new UnaryOperation<IdType, ValueType>(op, std::move(operand)));
}

template <typename IdType, typename ValueType>
ValueType UnaryOperation<IdType, ValueType>::ValueInState(
    const State<IdType>& state) const {
  switch (op()) {
    case UnaryOperator::NEGATE:
      return -operand().ValueInState(state);
    case UnaryOperator::NOT:
      return !operand().ValueInState(state);
  }
  LOG(FATAL) << "bad unary operator";
}

template <typename IdType, typename ValueType>
void UnaryOperation<IdType, ValueType>::Accept(
    ExpressionVisitor<IdType, ValueType>* visitor) const {
  CHECK(visitor);
  visitor->VisitUnaryOperation(*this);
}

template class UnaryOperation<std::string, TypedValue>;
template class UnaryOperation<int, TypedValue>;
template class UnaryOperation<int, int>;
template class UnaryOperation<int, double>;

std::ostream& operator<<(std::ostream& os, BinaryOperator op) {
  switch (op) {
    case BinaryOperator::MULTIPLY:
      return os << "*";
    case BinaryOperator::DIVIDE:
      return os << "/";
    case BinaryOperator::PLUS:
      return os << "+";
    case BinaryOperator::MINUS:
      return os << "-";
    case BinaryOperator::LESS:
      return os << "<";
    case BinaryOperator::LESS_EQUAL:
      return os << "<=";
    case BinaryOperator::GREATER_EQUAL:
      return os << ">=";
    case BinaryOperator::GREATER:
      return os << ">";
    case BinaryOperator::EQUAL:
      return os << "=";
    case BinaryOperator::NOT_EQUAL:
      return os << "!=";
    case BinaryOperator::AND:
      return os << "&";
    case BinaryOperator::OR:
      return os << "|";
    case BinaryOperator::IMPLY:
      return os << "=>";
  }
  LOG(FATAL) << "bad binary operator";
}

template <typename IdType, typename ValueType>
BinaryOperation<IdType, ValueType>::BinaryOperation(
    BinaryOperator op,
    std::unique_ptr<const Expression<IdType, ValueType> >&& operand1,
    std::unique_ptr<const Expression<IdType, ValueType> >&& operand2)
    : op_(op), operand1_(std::move(operand1)), operand2_(std::move(operand2)) {
  CHECK(operand1_);
  CHECK(operand2_);
}

template <typename IdType, typename ValueType>
BinaryOperation<IdType, ValueType>::~BinaryOperation() = default;

template <typename IdType, typename ValueType>
std::unique_ptr<const BinaryOperation<IdType, ValueType> >
BinaryOperation<IdType, ValueType>::Create(
    BinaryOperator op,
    std::unique_ptr<const Expression<IdType, ValueType> >&& operand1,
    std::unique_ptr<const Expression<IdType, ValueType> >&& operand2) {
  return std::unique_ptr<const BinaryOperation<IdType, ValueType> >(
      new BinaryOperation<IdType, ValueType>(
          op, std::move(operand1), std::move(operand2)));
}

namespace {

bool BoolValue(const TypedValue& value) {
  return value.value<bool>();
}

bool BoolValue(int value) {
  return value;
}

bool BoolValue(double value) {
  return value;
}

}  // namespace

template <typename IdType, typename ValueType>
ValueType BinaryOperation<IdType, ValueType>::ValueInState(
    const State<IdType>& state) const {
  switch (op()) {
    case BinaryOperator::MULTIPLY:
      return operand1().ValueInState(state) * operand2().ValueInState(state);
    case BinaryOperator::DIVIDE:
      return operand1().ValueInState(state) / operand2().ValueInState(state);
    case BinaryOperator::PLUS:
      return operand1().ValueInState(state) + operand2().ValueInState(state);
    case BinaryOperator::MINUS:
      return operand1().ValueInState(state) - operand2().ValueInState(state);
    case BinaryOperator::LESS:
      return operand1().ValueInState(state) < operand2().ValueInState(state);
    case BinaryOperator::LESS_EQUAL:
      return operand1().ValueInState(state) <= operand2().ValueInState(state);
    case BinaryOperator::GREATER_EQUAL:
      return operand1().ValueInState(state) >= operand2().ValueInState(state);
    case BinaryOperator::GREATER:
      return operand1().ValueInState(state) > operand2().ValueInState(state);
    case BinaryOperator::EQUAL:
      return operand1().ValueInState(state) == operand2().ValueInState(state);
    case BinaryOperator::NOT_EQUAL:
      return operand1().ValueInState(state) != operand2().ValueInState(state);
    case BinaryOperator::AND:
      return BoolValue(operand1().ValueInState(state))
          && BoolValue(operand2().ValueInState(state));
    case BinaryOperator::OR:
      return BoolValue(operand1().ValueInState(state))
          || BoolValue(operand2().ValueInState(state));
    case BinaryOperator::IMPLY:
      return !BoolValue(operand1().ValueInState(state))
          || BoolValue(operand2().ValueInState(state));
  }
  LOG(FATAL) << "bad binary operator";
}

template <typename IdType, typename ValueType>
void BinaryOperation<IdType, ValueType>::Accept(
    ExpressionVisitor<IdType, ValueType>* visitor) const {
  CHECK(visitor);
  visitor->VisitBinaryOperation(*this);
}

template class BinaryOperation<std::string, TypedValue>;
template class BinaryOperation<int, TypedValue>;
template class BinaryOperation<int, int>;
template class BinaryOperation<int, double>;

template <typename IdType, typename ValueType>
Conditional<IdType, ValueType>::Conditional(
    std::unique_ptr<const Expression<IdType, ValueType> >&& condition,
    std::unique_ptr<const Expression<IdType, ValueType> >&& if_expr,
    std::unique_ptr<const Expression<IdType, ValueType> >&& else_expr)
    : condition_(std::move(condition)),
      if_expr_(std::move(if_expr)),
      else_expr_(std::move(else_expr)) {
  CHECK(condition_);
  CHECK(if_expr_);
  CHECK(else_expr_);
}

template <typename IdType, typename ValueType>
Conditional<IdType, ValueType>::~Conditional() = default;

template <typename IdType, typename ValueType>
std::unique_ptr<const Conditional<IdType, ValueType> >
Conditional<IdType, ValueType>::Create(
    std::unique_ptr<const Expression<IdType, ValueType> >&& condition,
    std::unique_ptr<const Expression<IdType, ValueType> >&& if_expr,
    std::unique_ptr<const Expression<IdType, ValueType> >&& else_expr) {
  return std::unique_ptr<const Conditional<IdType, ValueType> >(
      new Conditional<IdType, ValueType>(
          std::move(condition), std::move(if_expr), std::move(else_expr)));
}

template <typename IdType, typename ValueType>
ValueType Conditional<IdType, ValueType>::ValueInState(
    const State<IdType>& state) const {
  const bool cond = BoolValue(condition().ValueInState(state));
  if (cond) {
    return if_expr().ValueInState(state);
  } else {
    return else_expr().ValueInState(state);
  }
}

template <typename IdType, typename ValueType>
void Conditional<IdType, ValueType>::Accept(
    ExpressionVisitor<IdType, ValueType>* visitor) const {
  CHECK(visitor);
  visitor->VisitConditional(*this);
}

template class Conditional<std::string, TypedValue>;
template class Conditional<int, TypedValue>;
template class Conditional<int, int>;
template class Conditional<int, double>;

template <typename IdType, typename ValueType>
ExpressionVisitor<IdType, ValueType>::ExpressionVisitor() = default;

template <typename IdType, typename ValueType>
ExpressionVisitor<IdType, ValueType>::~ExpressionVisitor() = default;

template class ExpressionVisitor<std::string, TypedValue>;
template class ExpressionVisitor<int, TypedValue>;
template class ExpressionVisitor<int, int>;
template class ExpressionVisitor<int, double>;
