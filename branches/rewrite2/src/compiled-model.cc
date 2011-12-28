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

#include "compiled-model.h"

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "glog/logging.h"

#include "expression.h"
#include "optional.h"
#include "model.h"
#include "type.h"

namespace {

struct CompiledVariable {
  int index;
  Type type;
};

// A map from constant identifiers of different types to their values.  Does
// lazy evaluation of constant values.
class ConstantValueMap {
 public:
  // Constructs a constant value map.
  ConstantValueMap();

  // Disallow copy and assign.
  ConstantValueMap(const ConstantValueMap&) = delete;
  ConstantValueMap& operator=(const ConstantValueMap&) = delete;

  // Adds a constant with the given name, type, and init expression to this
  // map.  Requires that a constant with given name has not been added before
  // and that init is non-null.
  void AddConstant(
      const std::string& name, Type type, const ParsedExpression* init);

  // Gets the type of the given constant.  Returns true on success; false if a
  // constant with the given name has not been added to this map.
  bool GetType(const std::string& name, Type* type) const;

  // Reads the value of the given constant from this map.  Returns true on
  // success.  Requires that a constant with the given name has been added to
  // this map.  Returns false if the constant cannot be evaluated.  If error
  // is non-null, then it is populated with an error message in case of
  // failure.
  bool GetValue(
      const std::string& name,
      const std::map<std::string, const ParsedExpression*>& formulas,
      const std::map<std::string, CompiledVariable>& compiled_variables,
      TypedValue* value,
      std::string* error) const;

 private:
  // Internal information about constants stored in this table.
  struct ConstantInfo {
    int index;
    Type type;
    const ParsedExpression* init;
  };

  // Current status of a constant with respect to evaluation.
  enum class ConstantStatus { UNEVALUATED, PENDING, EVALUATED };

  // Finds the internal constant information for the given constant name.
  // Requires that a constant with the given name has been added to this map.
  const ConstantInfo& GetInfo(const std::string& name) const;

  // Returns the evaluation status for the given constant name.
  ConstantStatus GetStatus(int index) const;

  // Map from constant name to internal constant information.
  std::map<std::string, ConstantInfo> constants_;
  // Map from constant index to constant status.
  mutable std::map<int, ConstantStatus> status_;
  // Map from constant index to value.
  mutable std::map<int, TypedValue> values_;
};

class ExpressionCompiler : public ParsedExpressionVisitor {
 public:
  ExpressionCompiler(
      const ConstantValueMap* constants,
      const std::map<std::string, const ParsedExpression*>* formulas,
      const std::map<std::string, CompiledVariable>* compiled_variables);

  virtual ~ExpressionCompiler();

  // Returns true if the last compilation was valid.
  bool is_valid() const { return error_.empty(); }

  // Returns the error for the last compilation.  Empty if the last
  // compilation was valid.
  const std::string& error() const { return error_; }

  // Returns the synthesized type for the last compilation.
  Type type() const { return type_; }

  // Returns true if the last compilation resulted in a constant value.
  bool is_constant() const { return is_constant_; }

  const CompiledExpression<TypedValue>& compiled_expr() const {
    return *CHECK_NOTNULL(compiled_expr_.get());
  }

  std::unique_ptr<const CompiledExpression<TypedValue> >
  release_compiled_expr() {
    return std::move(compiled_expr_);
  }

  virtual void VisitLiteral(const ParsedLiteral& expr);
  virtual void VisitIdentifier(const ParsedIdentifier& expr);
  virtual void VisitFunctionCall(const ParsedFunctionCall& expr);
  virtual void VisitUnaryOperation(const ParsedUnaryOperation& expr);
  virtual void VisitBinaryOperation(const ParsedBinaryOperation& expr);
  virtual void VisitConditional(const ParsedConditional& expr);

 private:
  // ConstantValueMap to use for looking up values of constants.
  const ConstantValueMap* const constants_;
  // Map from name to formula expression.
  const std::map<std::string, const ParsedExpression*>* const formulas_;
  // Map from name to compiled variable.
  const std::map<std::string, CompiledVariable>* const compiled_variables_;
  // An error message describing what went wrong when compiling an expression.
  std::string error_;
  // The synthesized expression type.
  Type type_;
  // Whether the expression is constant.
  bool is_constant_;
  // Compiled expression.
  std::unique_ptr<const CompiledExpression<TypedValue> > compiled_expr_;
};

template <typename ValueType>
class ExpressionOptimizer : public CompiledExpressionVisitor<TypedValue> {
 public:
  ExpressionOptimizer();

  virtual ~ExpressionOptimizer();

  std::unique_ptr<const CompiledExpression<ValueType> >
  release_optimized_expr() {
    return std::move(optimized_expr_);
  }

  virtual void VisitLiteral(const CompiledLiteral<TypedValue>& expr);
  virtual void VisitIdentifier(const CompiledIdentifier<TypedValue>& expr);
  virtual void VisitFunctionCall(const CompiledFunctionCall<TypedValue>& expr);
  virtual void VisitUnaryOperation(
      const CompiledUnaryOperation<TypedValue>& expr);
  virtual void VisitBinaryOperation(
      const CompiledBinaryOperation<TypedValue>& expr);
  virtual void VisitConditional(const CompiledConditional<TypedValue>& expr);

 private:
  // Optimized expression.
  std::unique_ptr<const CompiledExpression<ValueType> > optimized_expr_;
};

bool EvaluateConstantExpression(
    const ParsedExpression& expr,
    Type expected_type,
    const ConstantValueMap& constants,
    const std::map<std::string, const ParsedExpression*>& formulas,
    const std::map<std::string, CompiledVariable>& compiled_variables,
    TypedValue* value,
    std::string* error) {
  CHECK(value);

  ExpressionCompiler compiler(&constants, &formulas, &compiled_variables);
  expr.Accept(&compiler);
  if (!compiler.is_valid()) {
    if (error) {
      *error = compiler.error();
    }
    return false;
  }
  if (!compiler.is_constant()) {
    if (error) {
      *error = "expecting constant expression";
    }
    return false;
  }
  if (!ConvertsToType(compiler.type(), expected_type, error)) {
    return false;
  }
  *value = compiler.compiled_expr().ValueInState({});
  return true;
}

std::unique_ptr<const CompiledExpression<TypedValue> > CompileExpression(
    const ParsedExpression& expr,
    Type expected_type,
    const ConstantValueMap& constants,
    const std::map<std::string, const ParsedExpression*>& formulas,
    const std::map<std::string, CompiledVariable>& compiled_variables,
    std::string* error) {
  ExpressionCompiler compiler(&constants, &formulas, &compiled_variables);
  expr.Accept(&compiler);
  if (!compiler.is_valid()) {
    if (error) {
      *error = compiler.error();
    }
    return nullptr;
  }
  if (!ConvertsToType(compiler.type(), expected_type, error)) {
    return nullptr;
  }
  if (compiler.is_constant()) {
    return CompiledLiteral<TypedValue>::Create(
        compiler.compiled_expr().ValueInState({}));
  } else {
    return compiler.release_compiled_expr();
  }
}

template <typename ValueType>
std::unique_ptr<const CompiledExpression<ValueType> > OptimizeExpression(
    const ParsedExpression& expr,
    Type expected_type,
    const ConstantValueMap& constants,
    const std::map<std::string, const ParsedExpression*>& formulas,
    const std::map<std::string, CompiledVariable>& compiled_variables,
    std::string* error) {
  std::unique_ptr<const CompiledExpression<TypedValue> > compiled_expr =
      CompileExpression(expr, expected_type,
                        constants, formulas, compiled_variables,
                        error);
  if (!compiled_expr) {
    return nullptr;
  }
  ExpressionOptimizer<ValueType> optimizer;
  compiled_expr->Accept(&optimizer);
  return optimizer.release_optimized_expr();
}

ConstantValueMap::ConstantValueMap() {
}

void ConstantValueMap::AddConstant(
    const std::string& name, Type type, const ParsedExpression* init) {
  CHECK(init);
  const int index = constants_.size();
  CHECK(constants_.insert({ name, { index, type, init } }).second);
}

bool ConstantValueMap::GetType(const std::string& name, Type* type) const {
  CHECK(type);
  auto i = constants_.find(name);
  if (i != constants_.end()) {
    *type = i->second.type;
    return true;
  }
  return false;
}

const ConstantValueMap::ConstantInfo& ConstantValueMap::GetInfo(
    const std::string& name) const {
  auto i = constants_.find(name);
  CHECK(i != constants_.end());
  return i->second;
}

ConstantValueMap::ConstantStatus ConstantValueMap::GetStatus(int index) const {
  auto i = status_.find(index);
  return (i != status_.end()) ? i->second : ConstantStatus::UNEVALUATED;
}

void SetCyclicEvaluationError(const std::string& name, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "cyclic evaluation for constant '" << name << "'";
    *error = out.str();
  }
}

bool ConstantValueMap::GetValue(
    const std::string& name,
    const std::map<std::string, const ParsedExpression*>& formulas,
    const std::map<std::string, CompiledVariable>& compiled_variables,
    TypedValue* value,
    std::string* error) const {
  CHECK(value);

  const ConstantInfo& info = GetInfo(name);
  const ConstantStatus status = GetStatus(info.index);
  if (status == ConstantStatus::PENDING) {
    SetCyclicEvaluationError(name, error);
    return false;
  }

  if (status == ConstantStatus::UNEVALUATED) {
    status_[info.index] = ConstantStatus::PENDING;
    if (!EvaluateConstantExpression(
            *info.init, info.type, *this, formulas, compiled_variables,
            value, error)) {
      return false;
    }
    values_[info.index] = *value;
    status_[info.index] = ConstantStatus::EVALUATED;
    return true;
  }

  CHECK(status == ConstantStatus::EVALUATED);
  *value = values_[info.index];
  return true;
}

ExpressionCompiler::ExpressionCompiler(
    const ConstantValueMap* constants,
    const std::map<std::string, const ParsedExpression*>* formulas,
    const std::map<std::string, CompiledVariable>* compiled_variables)
    : constants_(constants),
      formulas_(formulas),
      compiled_variables_(compiled_variables),
      error_("unevaluated") {
  CHECK(constants_);
  CHECK(formulas_);
  CHECK(compiled_variables_);
}

ExpressionCompiler::~ExpressionCompiler() {
}

void ExpressionCompiler::VisitLiteral(const ParsedLiteral& expr) {
  error_.clear();
  type_ = expr.value().type();
  is_constant_ = true;
  compiled_expr_ = CompiledLiteral<TypedValue>::Create(expr.value());
}

void SetUndefinedSymbolError(const std::string& name, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "undefined symbol '" << name << "'";
    *error = out.str();
  }
}

void ExpressionCompiler::VisitIdentifier(const ParsedIdentifier& expr) {
  error_.clear();
  auto i = formulas_->find(expr.id());
  if (i != formulas_->end()) {
    // Compile formula.
    i->second->Accept(this);
  } else if (constants_->GetType(expr.id(), &type_)) {
    // Compile constant.
    TypedValue value;
    if (!constants_->GetValue(
            expr.id(), *formulas_, *compiled_variables_, &value, &error_)) {
      return;
    }
    type_ = value.type();
    is_constant_ = true;
    compiled_expr_ = CompiledLiteral<TypedValue>::Create(value);
  } else {
    // Not formula nor constant, so it had better be a variable.
    auto j = compiled_variables_->find(expr.id());
    if (j == compiled_variables_->end()) {
      SetUndefinedSymbolError(expr.id(), &error_);
      return;
    }
    // Compile variable.
    const CompiledVariable& variable = j->second;
    type_ = variable.type;
    is_constant_ = false;
    compiled_expr_ = CompiledIdentifier<TypedValue>::Create(variable.index);
  }
}

void SetFunctionTypeMismatchError(
    Function function, Type type, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "type mismatch; "<< function << "() applied to " << type;
    *error = out.str();
  }
}

void SetFunctionArityError(
    Function function, const std::string& arity, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << function << "() requires " << arity;
    *error = out.str();
  }
}

void ExpressionCompiler::VisitFunctionCall(const ParsedFunctionCall& expr) {
  std::vector<Type> types;
  bool is_constant = true;
  CompiledArgumentList<TypedValue> compiled_arguments;
  for (const ParsedExpression& argument: expr.arguments()) {
    argument.Accept(this);
    if (!is_valid()) {
      return;
    }
    if (type_ == Type::BOOL) {
      SetFunctionTypeMismatchError(expr.function(), Type::BOOL, &error_);
      return;
    }
    types.push_back(type_);
    is_constant = is_constant && is_constant_;
    compiled_arguments.push_back(release_compiled_expr());
  }
  switch (expr.function()) {
    default:
    case Function::NO_FUNCTION:
      error_ = "bad function";
      return;
    case Function::MIN:
    case Function::MAX:
      if (compiled_arguments.size() < 1) {
        SetFunctionArityError(
            expr.function(), "at least one argument", &error_);
        return;
      }
      type_ = types[0];
      for (int i = 1; i < types.size(); ++i) {
        if (ConvertsToType(type_, types[i], nullptr)) {
          // Synthesized type is the most general type.
          type_ = types[i];
        }
      }
      break;
    case Function::FLOOR:
    case Function::CEIL:
      if (compiled_arguments.size() != 1) {
        SetFunctionArityError(expr.function(), "exactly one argument", &error_);
        return;
      }
      type_ = Type::INT;
      break;
    case Function::POW:
    case Function::LOG:
      if (compiled_arguments.size() != 2) {
        SetFunctionArityError(
            expr.function(), "exactly two arguments", &error_);
        return;
      }
      type_ = Type::DOUBLE;
      break;
    case Function::MOD:
      if (compiled_arguments.size() != 2) {
        SetFunctionArityError(
            expr.function(), "exactly two arguments", &error_);
        return;
      }
      if (types[0] == Type::DOUBLE || types[1] == Type::DOUBLE) {
        SetFunctionTypeMismatchError(expr.function(), Type::DOUBLE, &error_);
        return;
      }
      type_ = Type::INT;
      break;
  }
  is_constant_ = is_constant;
  compiled_expr_ = CompiledFunctionCall<TypedValue>::Create(
      expr.function(), std::move(compiled_arguments));
}

void SetUnaryOperatorTypeMismatchError(
    UnaryOperator op, Type type, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "type mismatch; unary operator " << op << " applied to " << type;
    *error = out.str();
  }
}

void ExpressionCompiler::VisitUnaryOperation(const ParsedUnaryOperation& expr) {
  expr.operand().Accept(this);
  if (!is_valid()) {
    return;
  }
  switch (expr.op()) {
    default:
      error_ = "bad unary operator";
      return;
    case UnaryOperator::NEGATE:
      if (type_ == Type::BOOL) {
        SetUnaryOperatorTypeMismatchError(expr.op(), type_, &error_);
        return;
      }
      break;
    case UnaryOperator::NOT:
      if (type_ != Type::BOOL) {
        SetUnaryOperatorTypeMismatchError(expr.op(), type_, &error_);
        return;
      }
      break;
  }
  compiled_expr_ = CompiledUnaryOperation<TypedValue>::Create(
      expr.op(), release_compiled_expr());
}

void SetBinaryOperatorTypeMismatchError(
    BinaryOperator op, Type type, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "type mismatch; binary operator " << op << " applied to " << type;
    *error = out.str();
  }
}

void SetBinaryOperatorIncompatibleTypesError(
    BinaryOperator op, Type type1, Type type2, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "incompatible types for binary operator " << op << "; "
        << type1 << " and " << type2;
    *error = out.str();
  }
}

void ExpressionCompiler::VisitBinaryOperation(
    const ParsedBinaryOperation& expr) {
  expr.operand1().Accept(this);
  if (!is_valid()) {
    return;
  }
  const Type type1 = type_;
  const bool is_constant1 = is_constant_;
  std::unique_ptr<const CompiledExpression<TypedValue> > compiled_operand1 =
      release_compiled_expr();
  expr.operand2().Accept(this);
  if (!is_valid()) {
    return;
  }
  switch (expr.op()) {
    default:
      error_ = "bad binary operator";
      return;
    case BinaryOperator::MULTIPLY:
    case BinaryOperator::DIVIDE:
    case BinaryOperator::PLUS:
    case BinaryOperator::MINUS:
      if (type1 == Type::BOOL || type_ == Type::BOOL) {
        SetBinaryOperatorTypeMismatchError(expr.op(), Type::BOOL, &error_);
        return;
      }
      if (expr.op() == BinaryOperator::DIVIDE) {
        // Synthesized type for division is always double.
        type_ = Type::DOUBLE;
      } else if (ConvertsToType(type_, type1, nullptr)) {
        // Synthesized type is the most general type.
        type_ = type1;
      }
      break;
    case BinaryOperator::LESS:
    case BinaryOperator::LESS_EQUAL:
    case BinaryOperator::GREATER_EQUAL:
    case BinaryOperator::GREATER:
    case BinaryOperator::EQUAL:
    case BinaryOperator::NOT_EQUAL:
      if (!ConvertsToType(type_, type1, nullptr) &&
          !ConvertsToType(type1, type_, nullptr)) {
        SetBinaryOperatorIncompatibleTypesError(
            expr.op(), type1, type_, &error_);
        return;
      }
      type_ = Type::BOOL;
      break;
    case BinaryOperator::AND:
    case BinaryOperator::OR:
    case BinaryOperator::IMPLY:
      if (type1 != Type::BOOL) {
        SetBinaryOperatorTypeMismatchError(expr.op(), type1, &error_);
        return;
      }
      if (type_ != Type::BOOL) {
        SetBinaryOperatorTypeMismatchError(expr.op(), type_, &error_);
        return;
      }
      break;
  }
  is_constant_ = is_constant1 && is_constant_;
  compiled_expr_ = CompiledBinaryOperation<TypedValue>::Create(
      expr.op(), std::move(compiled_operand1), release_compiled_expr());
}

void SetConditionalIncompatibleTypesError(
    Type if_expr_type, Type else_expr_type, std::string* error) {
  if (error) {
    std::stringstream out;
    out << "incompatible types in conditional; "
        << if_expr_type << " and " << else_expr_type;
    *error = out.str();
  }
}

void ExpressionCompiler::VisitConditional(const ParsedConditional& expr) {
  expr.condition().Accept(this);
  if (!is_valid()) {
    return;
  }
  if (!ConvertsToType(type(), Type::BOOL, &error_)) {
    return;
  }
  const bool condition_is_constant = is_constant();
  std::unique_ptr<const CompiledExpression<TypedValue> > compiled_condition =
      release_compiled_expr();

  expr.if_expr().Accept(this);
  if (!is_valid()) {
    return;
  }
  const Type if_expr_type = type();
  const bool if_expr_is_constant = is_constant();
  std::unique_ptr<const CompiledExpression<TypedValue> > compiled_if_expr =
      release_compiled_expr();

  expr.else_expr().Accept(this);
  if (!is_valid()) {
    return;
  }
  if (ConvertsToType(type(), if_expr_type, nullptr)) {
    // Synthesized type is the most general type.
    type_ = if_expr_type;
  } else if (!ConvertsToType(if_expr_type, type(), nullptr)) {
    SetConditionalIncompatibleTypesError(if_expr_type, type_, &error_);
    return;
  }
  is_constant_ = condition_is_constant && if_expr_is_constant && is_constant();
  compiled_expr_ = CompiledConditional<TypedValue>::Create(
      std::move(compiled_condition),
      std::move(compiled_if_expr),
      release_compiled_expr());
}

template <typename ValueType>
ExpressionOptimizer<ValueType>::ExpressionOptimizer() = default;

template <typename ValueType>
ExpressionOptimizer<ValueType>::~ExpressionOptimizer() = default;

template <typename ValueType>
void ExpressionOptimizer<ValueType>::VisitLiteral(
    const CompiledLiteral<TypedValue>& expr) {
  optimized_expr_ = CompiledLiteral<ValueType>::Create(
      expr.value().template value<ValueType>());
}

template <typename ValueType>
void ExpressionOptimizer<ValueType>::VisitIdentifier(
    const CompiledIdentifier<TypedValue>& expr) {
  optimized_expr_ = CompiledIdentifier<ValueType>::Create(expr.id());
}

template <typename ValueType>
void ExpressionOptimizer<ValueType>::VisitFunctionCall(
    const CompiledFunctionCall<TypedValue>& expr) {
  CompiledArgumentList<ValueType> optimized_arguments;
  for (const CompiledExpression<TypedValue>& argument: expr.arguments()) {
    argument.Accept(this);
    optimized_arguments.push_back(release_optimized_expr());
  }
  optimized_expr_ = CompiledFunctionCall<ValueType>::Create(
      expr.function(), std::move(optimized_arguments));
}

template <typename ValueType>
void ExpressionOptimizer<ValueType>::VisitUnaryOperation(
    const CompiledUnaryOperation<TypedValue>& expr) {
  expr.operand().Accept(this);
  optimized_expr_ = CompiledUnaryOperation<ValueType>::Create(
      expr.op(), release_optimized_expr());
}

template <typename ValueType>
void ExpressionOptimizer<ValueType>::VisitBinaryOperation(
    const CompiledBinaryOperation<TypedValue>& expr) {
  expr.operand1().Accept(this);
  std::unique_ptr<const CompiledExpression<ValueType> > optimized_operand1 =
      release_optimized_expr();
  expr.operand2().Accept(this);
  optimized_expr_ = CompiledBinaryOperation<ValueType>::Create(
      expr.op(), std::move(optimized_operand1), release_optimized_expr());
}

template <typename ValueType>
void ExpressionOptimizer<ValueType>::VisitConditional(
    const CompiledConditional<TypedValue>& expr) {
  expr.condition().Accept(this);
  std::unique_ptr<const CompiledExpression<ValueType> > optimized_condition =
      release_optimized_expr();
  expr.if_expr().Accept(this);
  std::unique_ptr<const CompiledExpression<ValueType> > optimized_if_expr =
      release_optimized_expr();
  expr.else_expr().Accept(this);
  optimized_expr_ = CompiledConditional<ValueType>::Create(
      std::move(optimized_condition),
      std::move(optimized_if_expr),
      release_optimized_expr());
}

void SetUninitializedConstantError(
    const std::string& name, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "uninitialized constant '" << name << "'";
    *error = out.str();
  }
}

bool CollectConstantsAndFormulas(
    const Model& model,
    const std::map<std::string, const ParsedExpression*>& const_overrides,
    ConstantValueMap* constants,
    std::map<std::string, const ParsedExpression*>* formulas,
    std::string* error) {
  CHECK(constants);
  CHECK(formulas);

  for (int i = 0; i < model.num_constants(); ++i) {
    const std::string& name = model.GetConstantName(i);
    auto j = const_overrides.find(name);
    const ParsedExpression* const init = (j != const_overrides.end()) ?
        j->second : model.GetConstantInit(i);
    if (!init) {
      SetUninitializedConstantError(name, error);
      return false;
    }
    constants->AddConstant(name, model.constant_type(i), init);
  }

  for (int i = 0; i < model.num_formulas(); ++i) {
    formulas->insert({ model.GetFormulaName(i), &model.formula_expr(i) });
  }

  return true;
}

void SetBadVariableRangeError(const VariableInfo& info, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "bad range [" << info.min_value << ".." << info.max_value
        << "] for variable '" << info.name << "'";
    *error = out.str();
  }
}

void SetBadVariableInitError(const VariableInfo& info, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "init value " << info.init_value << " not in range ["
        << info.min_value << ".." << info.max_value
        << "] for variable '" << info.name << "'";
    *error = out.str();
  }
}

void SetAmbiguousInitError(std::string* error) {
  if (error) {
    *error = "global init not allowed in model with explicit variable inits";
  }
}

bool CollectVariables(
    const Model& model,
    const ConstantValueMap& constants,
    const std::map<std::string, const ParsedExpression*>& formulas,
    std::map<std::string, CompiledVariable>* compiled_variables,
    std::vector<VariableInfo>* variables,
    std::unique_ptr<const CompiledExpression<double> >* init,
    std::string* error) {
  CHECK(compiled_variables);
  CHECK(variables);
  CHECK(init);

  bool has_explicit_variable_inits = false;
  for (int i = 0; i < model.num_variables(); ++i) {
    VariableInfo info;
    info.name = model.GetVariableName(i);
    CompiledVariable variable;
    variable.index = i;
    variable.type = model.variable_type(i);
    if (variable.type == Type::INT) {
      const Range& range = *CHECK_NOTNULL(model.GetVariableRange(i));
      if (!EvaluateConstantExpression(
              range.min(), Type::INT, constants, formulas, *compiled_variables,
              &info.min_value, error) ||
          !EvaluateConstantExpression(
              range.max(), Type::INT, constants, formulas, *compiled_variables,
              &info.max_value, error)) {
        return false;
      }
      if (info.min_value >= info.max_value) {
        SetBadVariableRangeError(info, error);
        return false;
      }
    } else {
      CHECK(variable.type == Type::BOOL);
      info.min_value = false;
      info.max_value = true;
    }
    const ParsedExpression* const variable_init = model.GetVariableInit(i);
    if (variable_init) {
      has_explicit_variable_inits = true;
      if (!EvaluateConstantExpression(
              *variable_init, variable.type,
              constants, formulas, *compiled_variables,
              &info.init_value, error)) {
        return false;
      }
      if (info.init_value < info.min_value ||
          info.init_value > info.max_value) {
        SetBadVariableInitError(info, error);
        return false;
      }
    } else {
      info.init_value = info.min_value;
    }
    CHECK(compiled_variables->insert({ info.name, variable }).second);
    variables->push_back(info);
  }

  if (model.init()) {
    if (has_explicit_variable_inits) {
      SetAmbiguousInitError(error);
      return false;
    }
    *init = OptimizeExpression<double>(
        *model.init(), Type::BOOL, constants, formulas, *compiled_variables,
        error);
    if (!init) {
      return false;
    }
  }

  return true;
}
Optional<int> CompileAction(
    const Optional<std::string>& action, std::map<std::string, int>* actions) {
  CHECK(actions);
  if (action) {
    auto i = actions->find(action.get());
    if (i != actions->end()) {
      return Optional<int>(i->second);
    } else {
      int index = actions->size();
      CHECK(actions->insert({ action.get(), index }).second);
      return Optional<int>(index);
    }
  } else {
    return Optional<int>();
  }
}
void SetUndefinedVariableInUpdateError(
    const std::string& variable, std::string* error) {
  if (error) {
    std::ostringstream out;
    out << "undefined variable '" << variable << "' in update";
    *error = out.str();
  }
}

bool CompileUpdate(
    const ParsedUpdate& update,
    const ConstantValueMap& constants,
    const std::map<std::string, const ParsedExpression*>& formulas,
    const std::map<std::string, CompiledVariable>& compiled_variables,
    std::vector<CompiledUpdate>* updates,
    std::string* error) {
  CHECK(updates);
  auto i = compiled_variables.find(update.variable());
  if (i == compiled_variables.end()) {
    SetUndefinedVariableInUpdateError(update.variable(), error);
    return false;
  }
  const CompiledVariable& compiled_variable = i->second;
  std::unique_ptr<const CompiledExpression<double> > expr =
      OptimizeExpression<double>(update.expr(), compiled_variable.type,
                                 constants, formulas, compiled_variables,
                                 error);
  if (!expr) {
    return false;
  }
  updates->emplace_back(compiled_variable.index, std::move(expr));
  return true;
}

bool CompileOutcome(
    const ParsedOutcome& outcome,
    const ConstantValueMap& constants,
    const std::map<std::string, const ParsedExpression*>& formulas,
    const std::map<std::string, CompiledVariable>& compiled_variables,
    std::vector<CompiledOutcome>* outcomes,
    std::string* error) {
  CHECK(outcomes);
  std::unique_ptr<const CompiledExpression<double> > probability =
      OptimizeExpression<double>(outcome.probability(), Type::DOUBLE,
                                 constants, formulas, compiled_variables,
                                 error);
  if (!probability) {
    return false;
  }
  std::vector<CompiledUpdate> updates;
  for (const ParsedUpdate& update: outcome.updates()) {
    if (!CompileUpdate(update, constants, formulas, compiled_variables,
                       &updates, error)) {
      return false;
    }
  }
  outcomes->emplace_back(std::move(probability), std::move(updates));
  return true;
}

bool CompileCommands(
    const Model& model,
    const ConstantValueMap& constants,
    const std::map<std::string, const ParsedExpression*>& formulas,
    const std::map<std::string, CompiledVariable>& compiled_variables,
    std::map<std::string, int>* modules,
    std::map<std::string, int>* actions,
    std::vector<CompiledCommand>* commands_without_action,
    std::vector<
      std::vector<std::vector<CompiledCommand> > >* commands_with_action,
    std::string* error) {
  CHECK(modules);
  CHECK(actions);
  CHECK(commands_without_action);
  CHECK(commands_with_action);
  for (int i = 0; i < model.num_modules(); ++i) {
    const std::string& name = model.GetModuleName(i);
    CHECK(modules->insert({ name, i }).second);
    std::set<int> seen_actions;
    for (const ParsedCommand& command: model.module_commands(i)) {
      Optional<int> action = CompileAction(command.action(), actions);
      std::unique_ptr<const CompiledExpression<double> > guard =
          OptimizeExpression<double>(command.guard(), Type::BOOL,
                                  constants, formulas, compiled_variables,
                                  error);
      if (!guard) {
        return false;
      }
      std::vector<CompiledOutcome> outcomes;
      for (const ParsedOutcome& outcome: command.outcomes()) {
        if (!CompileOutcome(outcome, constants, formulas, compiled_variables,
                            &outcomes, error)) {
          return false;
        }
      }
      if (action) {
        commands_with_action->resize(actions->size());
        std::vector<std::vector<CompiledCommand> >& commands_per_module =
            (*commands_with_action)[action.get()];
        if (seen_actions.find(action.get()) == seen_actions.end()) {
          commands_per_module.emplace_back();
          seen_actions.insert(action.get());
        }
        commands_per_module.back().emplace_back(
            action, std::move(guard), std::move(outcomes));
      } else {
        commands_without_action->emplace_back(
            action, std::move(guard), std::move(outcomes));
      }
    }
  }
  commands_with_action->resize(actions->size());
  return true;
}

}  // namespace

CompiledModel::CompiledModel()
    : is_valid_(false) {
}

CompiledModel::CompiledModel(CompiledModel&& compiled_model)
    : model_type_(compiled_model.model_type_),
      variables_(std::move(compiled_model.variables_)),
      action_names_(std::move(compiled_model.action_names_)),
      init_(std::move(compiled_model.init_)),
      commands_without_action_(
          std::move(compiled_model.commands_without_action_)),
      commands_with_action_(
          std::move(compiled_model.commands_with_action_)),
      is_valid_(compiled_model.is_valid_) {
}

CompiledModel::CompiledModel(
    ModelType model_type,
    std::vector<VariableInfo>&& variables,
    std::vector<std::string>&& action_names,
    std::unique_ptr<const CompiledExpression<double> >&& init,
    std::vector<CompiledCommand>&& commands_without_action,
    std::vector<
      std::vector<std::vector<CompiledCommand> > >&& commands_with_action)
    : model_type_(model_type),
      variables_(std::move(variables)),
      action_names_(std::move(action_names)),
      init_(std::move(init)),
      commands_without_action_(std::move(commands_without_action)),
      commands_with_action_(std::move(commands_with_action)),
      is_valid_(true) {
}

CompiledModel CompiledModel::Make(
    const Model& parsed_model,
    const std::map<std::string, const ParsedExpression*>& const_overrides,
    std::string* error) {
  ConstantValueMap constants;
  std::map<std::string, const ParsedExpression*> formulas;
  if (!CollectConstantsAndFormulas(
          parsed_model, const_overrides, &constants, &formulas, error)) {
    return CompiledModel();
  }

  std::map<std::string, CompiledVariable> compiled_variables;
  std::vector<VariableInfo> variables;
  std::unique_ptr<const CompiledExpression<double> > init;
  if (!CollectVariables(parsed_model, constants, formulas,
                        &compiled_variables, &variables, &init, error)) {
    return CompiledModel();
  }

  std::map<std::string, int> modules;
  std::map<std::string, int> actions;
  std::vector<CompiledCommand> commands_without_action;
  std::vector<std::vector<std::vector<CompiledCommand> > > commands_with_action;
  if (!CompileCommands(
          parsed_model, constants, formulas, compiled_variables,
          &modules, &actions, &commands_without_action, &commands_with_action,
          error)) {
    return CompiledModel();
  }

  std::vector<std::string> action_names(actions.size());
  for (const auto& p: actions) {
    action_names[p.second] = p.first;
  }

  // TODO(hlsyounes): compile rewards.

  return CompiledModel(parsed_model.type(),
                       std::move(variables),
                       std::move(action_names),
                       std::move(init),
                       std::move(commands_without_action),
                       std::move(commands_with_action));
}
