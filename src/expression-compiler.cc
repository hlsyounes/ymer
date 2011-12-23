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

#include "expression-compiler.h"

#include <initializer_list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"

#include "ddutil.h"
#include "expression.h"
#include "type.h"

ConstantValueMap::ConstantValueMap() = default;

ConstantValueMap::ConstantValueMap(
    std::initializer_list<
        std::pair<std::string, std::pair<Type, const Expression*> > > list) {
  for (const std::pair<std::string, std::pair<Type, const Expression*> >& e:
           list) {
    AddConstant(e.first, e.second.first, e.second.second);
  }
}

void ConstantValueMap::AddConstant(
    const std::string& name, Type type, const Expression* init) {
  CHECK(init);
  const int index = constants_.size();
  CHECK(constants_.insert({ name, { index, type, init } }).second);
}

bool ConstantValueMap::HasConstant(const std::string& name) const {
  return constants_.find(name) != constants_.end();
}

Type ConstantValueMap::GetType(const std::string& name) const {
  return GetInfo(name).type;
}

const ConstantValueMap::ConstantInfo& ConstantValueMap::GetInfo(
    const std::string& name) const {
  auto i = constants_.find(name);
  CHECK(i != constants_.cend());
  return i->second;
}

ConstantValueMap::ConstantStatus ConstantValueMap::GetStatus(int index) const {
  auto i = status_.find(index);
  return (i != status_.cend()) ? i->second : ConstantStatus::UNEVALUATED;
}

namespace {


// A type-synthesizing expression compiler.  Synthesizes type for an
// expression and compiles the expression into a BDD/ADD (dependening on
// type).
class TypeSynthesizingExpressionCompiler : public ExpressionVisitor {
 public:
  // Constructs a type-synthesizing expression compiler with the given
  // constant and formula maps.
  TypeSynthesizingExpressionCompiler(
      const ConstantValueMap* constants,
      const std::map<std::string, const Expression*>* formulas,
      const std::map<std::string, VariableInfo>* variables,
      const DecisionDiagramManager* dd_manager);

  // Returns true if the last compilation was valid.
  bool is_valid() const { return error_.empty(); }

  // Returns the synthesized type for the last compilation.
  Type type() const { return type_; }

  // Returns the decision diagram for the last compilation, provided that the
  // expression was of type int of double.
  const ADD& numeric_dd() const { return numeric_dd_; }

  // Returns the decision diagram for the last compilation, provided that the
  // expression was of type bool.
  const BDD& bool_dd() const { return bool_dd_; }

  // Returns the error for the last compilation.  Empty if the last
  // compilation was valid.
  const std::string& error() const { return error_; }

  virtual void VisitIntLiteral(const IntLiteral& expr);
  virtual void VisitDoubleLiteral(const DoubleLiteral& expr);
  virtual void VisitBoolLiteral(const BoolLiteral& expr);
  virtual void VisitIdentifier(const Identifier& expr);
  virtual void VisitFunctionCall(const FunctionCall& expr);
  virtual void VisitUnaryOperation(const UnaryOperation& expr);
  virtual void VisitBinaryOperation(const BinaryOperation& expr);
  virtual void VisitConditional(const Conditional& expr);

 private:
  // ConstantValueMap to use for looking up values of constants.
  const ConstantValueMap* const constants_;
  // Map from name to formula expression.
  const std::map<std::string, const Expression*>* const formulas_;
  // Map from name to variable information.
  const std::map<std::string, VariableInfo>* const variables_;
  // Decision-diagram manager.
  const DecisionDiagramManager* const dd_manager_;
  // The synthesized expression type.
  Type type_;
  // For numeric expressions, holds the compiled expression represented as a
  // decision diagram.
  ADD numeric_dd_;
  // For Booelan expressions, holds the compiled expression represented as a
  // decision diagram.
  BDD bool_dd_;
  // An error message describing what went wrong when compiling an expression.
  std::string error_;
};

TypeSynthesizingExpressionCompiler::TypeSynthesizingExpressionCompiler(
    const ConstantValueMap* constants,
    const std::map<std::string, const Expression*>* formulas,
    const std::map<std::string, VariableInfo>* variables,
    const DecisionDiagramManager* dd_manager)
    : constants_(constants),
      formulas_(formulas),
      variables_(variables),
      dd_manager_(CHECK_NOTNULL(dd_manager)),
      numeric_dd_(dd_manager->GetConstant(0)),
      bool_dd_(dd_manager->GetConstant(true)),
      error_("unevaluated") {
  CHECK(constants_);
  CHECK(formulas_);
  CHECK(variables_);
}

void TypeSynthesizingExpressionCompiler::VisitIntLiteral(
    const IntLiteral& expr) {
  error_.clear();
  type_ = Type::INT;
  numeric_dd_ = dd_manager_->GetConstant(expr.value());
}

void TypeSynthesizingExpressionCompiler::VisitDoubleLiteral(
    const DoubleLiteral& expr) {
  error_.clear();
  type_ = Type::DOUBLE;
  numeric_dd_ = dd_manager_->GetConstant(expr.value());
}

void TypeSynthesizingExpressionCompiler::VisitBoolLiteral(
    const BoolLiteral& expr) {
  error_.clear();
  type_ = Type::BOOL;
  bool_dd_ = dd_manager_->GetConstant(expr.value());
}

void TypeSynthesizingExpressionCompiler::VisitIdentifier(
    const Identifier& expr) {
  error_.clear();
  auto i = formulas_->find(expr.name());
  if (i != formulas_->cend()) {
    // Compile formula.
    i->second->Accept(this);
  } else if (constants_->HasConstant(expr.name())) {
    // Compile constant.
    type_ = constants_->GetType(expr.name());
    switch (type_) {
      case Type::INT: {
        int value;
        if (!constants_->GetValue(
                expr.name(), *formulas_, *variables_, *dd_manager_,
                &value, &error_)) {
          return;
        }
        numeric_dd_ = dd_manager_->GetConstant(value);
        break;
      }
      case Type::DOUBLE: {
        double value;
        if (!constants_->GetValue(
                expr.name(), *formulas_, *variables_, *dd_manager_,
                &value, &error_)) {
          return;
        }
        numeric_dd_ = dd_manager_->GetConstant(value);
        break;
      }
      case Type::BOOL: {
        bool value;
        if (!constants_->GetValue(
                expr.name(), *formulas_, *variables_, *dd_manager_,
                &value, &error_)) {
          return;
        }
        bool_dd_ = dd_manager_->GetConstant(value);
        break;
      }
    }
  } else {
    // Not a formula or constant, so it had better be a variable.
    auto j = variables_->find(expr.name());
    if (j == variables_->end()) {
      error_ = "undefined symbol '";
      error_ += expr.name();
      error_ += "'";
      return;
    }
    // Compile variable.
    const VariableInfo& info = j->second;
    type_ = info.type;
    CompileVariable(info, false /* primed */, *dd_manager_,
                    &numeric_dd_, &bool_dd_);
  }
}

ADD ApplyUnaryFunction(Function function, const ADD& dd) {
  switch (function) {
    case Function::FLOOR:
      return floor(dd);
    case Function::CEIL:
      return ceil(dd);
  }
  LOG(FATAL) << "internal error";
}

ADD ApplyBinaryFunction(Function function, const ADD& dd1, const ADD& dd2) {
  switch (function) {
    case Function::MIN:
      return min(dd1, dd2);
    case Function::MAX:
      return max(dd1, dd2);
    case Function::POW:
      return pow(dd1, dd2);
    case Function::LOG:
      return log(dd1) / log(dd2);
  }
  LOG(FATAL) << "internal error";
}

std::string FunctionTypeMismatchError(Function function, Type type) {
  std::string error = "type mismatch; ";
  error += Function_Name(function);
  error += "() applied to ";
  error += Type_Name(type);
  return error;
}

std::string FunctionArityError(Function function, const std::string& arity) {
  std::string error = Function_Name(function);
  error += "() requires ";
  error += arity;
  return error;
}

void TypeSynthesizingExpressionCompiler::VisitFunctionCall(
    const FunctionCall& expr) {
  error_.clear();
  const int num_arguments = expr.arguments().size();
  std::vector<Type> type(num_arguments);
  std::vector<ADD> numeric_dd(num_arguments, dd_manager_->GetConstant(0));
  for (int i = 0; i < num_arguments; ++i) {
    expr.arguments()[i].Accept(this);
    if (!is_valid()) {
      return;
    }
    if (type_ == Type::BOOL) {
      error_ = FunctionTypeMismatchError(expr.function(), Type::BOOL);
      return;
    }
    type[i] = type_;
    numeric_dd[i] = numeric_dd_;
  }
  switch (expr.function()) {
    case Function::NO_FUNCTION:
      error_ = "invalid function call";
      return;
    case Function::MIN:
    case Function::MAX:
      if (num_arguments < 1) {
        error_ = FunctionArityError(expr.function(), "at least one argument");
        return;
      }
      for (int i = 0; i < num_arguments; ++i) {
        if (i == 0) {
          type_ = type[0];
          numeric_dd_ = numeric_dd[0];
        } else {
          if (ConvertsToType(type_, type[i], nullptr)) {
            // Synthesized type is the most general type.
            type_ = type[i];
          }
          numeric_dd_ =
              ApplyBinaryFunction(expr.function(), numeric_dd_, numeric_dd[i]);
        }
      }
      break;
    case Function::FLOOR:
    case Function::CEIL:
      if (num_arguments != 1) {
        error_ = FunctionArityError(expr.function(), "exactly one argument");
        return;
      }
      type_ = Type::INT;
      numeric_dd_ = ApplyUnaryFunction(expr.function(), numeric_dd[0]);
      break;
    case Function::POW:
    case Function::LOG:
      if (num_arguments != 2) {
        error_ = FunctionArityError(expr.function(), "exactly two arguments");
        return;
      }
      type_ = Type::DOUBLE;
      numeric_dd_ =
          ApplyBinaryFunction(expr.function(), numeric_dd[0], numeric_dd[1]);
      break;
    case Function::MOD:
      if (num_arguments != 2) {
        error_ = FunctionArityError(expr.function(), "exactly two arguments");
        return;
      }
      if (type[0] == Type::DOUBLE || type[1] == Type::DOUBLE) {
        error_ = FunctionTypeMismatchError(expr.function(), Type::DOUBLE);
        return;
      }
      type_ = Type::INT;
      numeric_dd_ = numeric_dd[0] % numeric_dd[1];
      break;
  }
}

void TypeSynthesizingExpressionCompiler::VisitUnaryOperation(
    const UnaryOperation& expr) {
  error_.clear();
  expr.operand().Accept(this);
  if (!is_valid()) {
    return;
  }
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      if (type_ == Type::BOOL) {
        error_ = "type mismatch; unary operator - applied to bool";
        return;
      }
      numeric_dd_ = -numeric_dd_;
      break;
    case UnaryOperator::NOT:
      if (type_ != Type::BOOL) {
        error_ = "type mismatch; unary operator ! applied to ";
        error_ += Type_Name(type_);
        return;
      }
      bool_dd_ = !bool_dd_;
      break;
  }
}

ADD ApplyNumericBinaryOperator(
    BinaryOperator op, const ADD& dd1, const ADD& dd2) {
  switch (op) {
    case BinaryOperator::MULTIPLY:
      return dd1 * dd2;
    case BinaryOperator::DIVIDE:
      return dd1 / dd2;
    case BinaryOperator::PLUS:
      return dd1 + dd2;
    case BinaryOperator::MINUS:
      return dd1 - dd2;
  }
  LOG(FATAL) << "internal error";
}

BDD ApplyBooleanBinaryOperator(
    BinaryOperator op, const BDD& dd1, const BDD& dd2) {
  switch (op) {
    case BinaryOperator::AND:
      return dd1 && dd2;
    case BinaryOperator::OR:
      return dd1 || dd2;
    case BinaryOperator::IMPLY:
      return !dd1 || dd2;
  }
  LOG(FATAL) << "internal error";
}

template <typename DD>
BDD ApplyBinaryPredicate(BinaryOperator op, const DD& dd1, const DD& dd2) {
  switch (op) {
    case BinaryOperator::LESS:
      return dd1 < dd2;
    case BinaryOperator::LESS_EQUAL:
      return dd1 <= dd2;
    case BinaryOperator::GREATER_EQUAL:
      return dd1 >= dd2;
    case BinaryOperator::GREATER:
      return dd1 > dd2;
    case BinaryOperator::EQUAL:
      return dd1 == dd2;
    case BinaryOperator::NOT_EQUAL:
      return dd1 != dd2;
  }
  LOG(FATAL) << "internal error";
}

std::string BinaryOperatorTypeMismatchError(BinaryOperator op, Type type) {
  std::string error = "type mismatch; binary operator ";
  error += BinaryOperator_Name(op);
  error += " applied to ";
  error += Type_Name(type);
  return error;
}

void TypeSynthesizingExpressionCompiler::VisitBinaryOperation(
    const BinaryOperation& expr) {
  error_.clear();
  expr.operand1().Accept(this);
  if (!is_valid()) {
    return;
  }
  Type type1 = type_;
  ADD numeric_dd1 = numeric_dd_;
  BDD bool_dd1 = bool_dd_;
  expr.operand2().Accept(this);
  if (!is_valid()) {
    return;
  }
  switch (expr.op()) {
    case BinaryOperator::MULTIPLY:
    case BinaryOperator::DIVIDE:
    case BinaryOperator::PLUS:
    case BinaryOperator::MINUS:
      if (type1 == Type::BOOL || type_ == Type::BOOL) {
        error_ = BinaryOperatorTypeMismatchError(expr.op(), Type::BOOL);
        return;
      }
      if (expr.op() == BinaryOperator::DIVIDE) {
        // Synthesized type for division is always double.
        type_ = Type::DOUBLE;
      } else if (ConvertsToType(type_, type1, nullptr)) {
        // Synthesized type is the most general type.
        type_ = type1;
      }
      numeric_dd_ =
          ApplyNumericBinaryOperator(expr.op(), numeric_dd1, numeric_dd_);
      break;
    case BinaryOperator::LESS:
    case BinaryOperator::LESS_EQUAL:
    case BinaryOperator::GREATER_EQUAL:
    case BinaryOperator::GREATER:
    case BinaryOperator::EQUAL:
    case BinaryOperator::NOT_EQUAL:
      if (!ConvertsToType(type_, type1, nullptr) &&
          !ConvertsToType(type1, type_, nullptr)) {
        error_ = "incompatible types for binary operator ";
        error_ += BinaryOperator_Name(expr.op());
        error_ += "; ";
        error_ += Type_Name(type1);
        error_ += " and ";
        error_ += Type_Name(type_);
        return;
      }
      type_ = Type::BOOL;
      switch (type1) {
        case Type::INT:
        case Type::DOUBLE:
          bool_dd_ = ApplyBinaryPredicate(expr.op(), numeric_dd1, numeric_dd_);
          break;
        case Type::BOOL:
          bool_dd_ = ApplyBinaryPredicate(expr.op(), bool_dd1, bool_dd_);
          break;
      }
      break;
    case BinaryOperator::AND:
    case BinaryOperator::OR:
    case BinaryOperator::IMPLY:
      if (type1 != Type::BOOL) {
        error_ = BinaryOperatorTypeMismatchError(expr.op(), type1);
        return;
      }
      if (type_ != Type::BOOL) {
        error_ = BinaryOperatorTypeMismatchError(expr.op(), type_);
        return;
      }
      bool_dd_ = ApplyBooleanBinaryOperator(expr.op(), bool_dd1, bool_dd_);
      break;
  }
}

void TypeSynthesizingExpressionCompiler::VisitConditional(
    const Conditional& expr) {
  error_.clear();
  BDD condition_dd = dd_manager_->GetConstant(false);
  if (!CompileExpression<bool>(
          expr.condition(), *constants_, *formulas_, *variables_, *dd_manager_,
          &condition_dd, &error_)) {
    return;
  }
  expr.if_expr().Accept(this);
  if (!is_valid()) {
    return;
  }
  const Type if_type = type_;
  const ADD if_numeric_dd = numeric_dd_;
  const BDD if_bool_dd = bool_dd_;
  expr.else_expr().Accept(this);
  if (!is_valid()) {
    return;
  }
  if (ConvertsToType(type_, if_type, nullptr)) {
    // Synthesized type is the most general type.
    type_ = if_type;
  } else if (!ConvertsToType(if_type, type_, nullptr)) {
    error_ = "incompatible types in conditional; ";
    error_ += Type_Name(if_type);
    error_ += " and ";
    error_ += Type_Name(type_);
    return;
  }
  switch (type_) {
    case Type::INT:
    case Type::DOUBLE:
      numeric_dd_ = Ite(condition_dd, if_numeric_dd, numeric_dd_);
      break;
    case Type::BOOL:
      bool_dd_ = Ite(condition_dd, if_bool_dd, bool_dd_);
      break;
  }
}

}  // namespace

bool CompileExpressionAndSynthesizeType(
    const Expression& expression,
    const ConstantValueMap& constants,
    const std::map<std::string, const Expression*>& formulas,
    const std::map<std::string, VariableInfo>& variables,
    const DecisionDiagramManager& dd_manager,
    Type* type, ADD* numeric_dd, BDD* bool_dd, std::string* error) {
  CHECK(type);
  CHECK(numeric_dd);
  CHECK(bool_dd);

  TypeSynthesizingExpressionCompiler expression_compiler(
      &constants, &formulas, &variables, &dd_manager);
  expression.Accept(&expression_compiler);
  if (!expression_compiler.is_valid()) {
    if (error) {
      *error = expression_compiler.error();
    }
    return false;
  }
  *type = expression_compiler.type();
  *numeric_dd = expression_compiler.numeric_dd();
  *bool_dd = expression_compiler.bool_dd();
  return true;
}

void CompileVariable(const VariableInfo& info,
                     bool primed,
                     const DecisionDiagramManager& dd_manager,
                     ADD* numeric_dd, BDD* bool_dd) {
  CHECK(numeric_dd);
  CHECK(bool_dd);
  const int offset = primed ? 1 : 0;
  switch (info.type) {
    case Type::INT: {
      ADD dd = dd_manager.GetConstant(0);
      for (int i = info.high_bit; i >= info.low_bit; --i) {
        dd = dd +
            (dd_manager.GetAddVariable(2 * i + offset) *
             dd_manager.GetConstant(1 << (info.high_bit - i)));
      }
      *numeric_dd = dd + dd_manager.GetConstant(info.min_value);
      break;
    }
    case Type::DOUBLE:
      LOG(FATAL) << "internal error";
    case Type::BOOL:
      *bool_dd = dd_manager.GetBddVariable(2 * info.low_bit + offset);
      break;
  }
}
