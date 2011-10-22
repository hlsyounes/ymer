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
//
// Functions and classes for compiling expressions.

#ifndef EXPRESSION_COMPILER_H_
#define EXPRESSION_COMPILER_H_

#include <initializer_list>
#include <map>
#include <string>
#include <utility>

#include "glog/logging.h"

#include "ddutil.h"
#include "type.h"

class Expression;

// Information about a variable needed for compilation.
struct VariableInfo {
  Type type;
  int min_value;
  int low_bit;
  int high_bit;
};

// A map from constant identifiers of different types to their values.  Does
// lazy evaluation of constant values.
class ConstantValueMap {
 public:
  // Constructs a constant value map.
  ConstantValueMap();
  ConstantValueMap(
      std::initializer_list<
          std::pair<std::string, std::pair<Type, const Expression*> > > list);

  // Disallow copy and assign.
  ConstantValueMap(const ConstantValueMap&) = delete;
  ConstantValueMap& operator=(const ConstantValueMap&) = delete;

  // Adds a constant with the given name, type, and init expression to this
  // map.  Requires that a constant with given name has not been added before
  // and that init is non-null.
  void AddConstant(const std::string& name, Type type, const Expression* init);

  // Returns true if the given constant name has been added to this map.
  bool HasConstant(const std::string& name) const;

  // Returns the type of the given constant.
  Type GetType(const std::string& name) const;

  // Reads the value of the given constant from this map.  Returns true on
  // success.  Requires that a constant with the given name has been added to
  // this map.  Returns false if the constant is of the wrong, type or its
  // value cannot be evaluated.  If error is non-null, then it is populated
  // with an error message in case of failure.
  template <typename ValueType>
  bool GetValue(const std::string& name,
                const std::map<std::string, const Expression*>& formulas,
                const std::map<std::string, VariableInfo>& variables,
                const DecisionDiagramManager& dd_manager,
                ValueType* value, std::string* error) const;

 private:
  // Internal information about constants stored in this table.
  struct ConstantInfo {
    int index;
    Type type;
    const Expression* init;
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
  // Map from constant index to value for constants of type int.
  mutable std::map<int, int> int_values_;
  // Map from constant index to value for constants of type double.
  mutable std::map<int, double> double_values_;
  // Map from constant index to value for constants of type bool.
  mutable std::map<int, bool> bool_values_;
};

// Compiles an expression given constant, formula, and variable maps.  Returns
// true on success and sets type to the synthesized type and either numeric_dd
// (if type is int or double) or bool_dd (if type is bool) to the compiled
// expression.  Returns false if compilation failed.  If error is non-null,
// then it is populated with an error message in case of failure.
bool CompileExpressionAndSynthesizeType(
    const Expression& expression,
    const ConstantValueMap& constants,
    const std::map<std::string, const Expression*>& formulas,
    const std::map<std::string, VariableInfo>& variables,
    const DecisionDiagramManager& dd_manager,
    Type* type, ADD* numeric_dd, BDD* bool_dd, std::string* error);

// Class for mapping native type to compiled expression type.
template <typename NativeType> class CompiledTypeInfo;

// Compiles an expression given constant, formula, and variable maps.  Returns
// true if the type of the expression is convertable to ExpectedType and sets
// dd to the compiled expression.  Returns false if compilation failed or the
// type is wrong.  If error is non-null, then it is populated with an error
// message in case of failure.
template <typename ExpectedType>
bool CompileExpression(
    const Expression& expression,
    const ConstantValueMap& constants,
    const std::map<std::string, const Expression*>& formulas,
    const std::map<std::string, VariableInfo>& variables,
    const DecisionDiagramManager& dd_manager,
    typename CompiledTypeInfo<ExpectedType>::dd_type* dd, std::string* error) {
  CHECK(dd);

  Type type;
  ADD numeric_dd = dd_manager.GetConstant(0);
  BDD bool_dd = dd_manager.GetConstant(false);
  if (!CompileExpressionAndSynthesizeType(
          expression, constants, formulas, variables, dd_manager,
          &type, &numeric_dd, &bool_dd, error)) {
    return false;
  }
  if (!ConvertsToType(type, TypeInfo<ExpectedType>::type(), error)) {
    return false;
  }
  switch (type) {
    case Type::INT:
    case Type::DOUBLE:
      *dd = typename CompiledTypeInfo<ExpectedType>::dd_type(numeric_dd);
      break;
    case Type::BOOL:
      *dd = typename CompiledTypeInfo<ExpectedType>::dd_type(bool_dd);
      break;
  }
  return true;
}

// Compiles a variable.
void CompileVariable(const VariableInfo& info,
                     bool primed,
                     const DecisionDiagramManager& dd_manager,
                     ADD* numeric_dd, BDD* bool_dd);

template <typename ValueType>
bool ConstantValueMap::GetValue(
    const std::string& name,
    const std::map<std::string, const Expression*>& formulas,
    const std::map<std::string, VariableInfo>& variables,
    const DecisionDiagramManager& dd_manager,
    ValueType* value, std::string* error) const {
  CHECK(value);

  const ConstantInfo& info = GetInfo(name);
  if (!ConvertsToType(info.type, TypeInfo<ValueType>::type(), error)) {
    return false;
  }

  const ConstantStatus status = GetStatus(info.index);
  if (status == ConstantStatus::PENDING) {
    if (error) {
      *error = "cyclic evaluation for constant '";
      *error += name;
      *error += "'";
    }
    return false;
  }

  if (status == ConstantStatus::UNEVALUATED) {
    status_[info.index] = ConstantStatus::PENDING;
    typename CompiledTypeInfo<ValueType>::dd_type dd =
        dd_manager.GetConstant(ValueType());
    if (!CompileExpression<ValueType>(
            *info.init, *this, formulas, variables, dd_manager, &dd, error)) {
      return false;
    }
    if (!dd.IsConstant()) {
      if (error) {
        *error = "incomplete evaluation for constant '";
        *error += name;
        *error += "'";
      }
      return false;
    }
    switch(info.type) {
      case Type::INT:
        int_values_[info.index] = dd.Value();
        break;
      case Type::DOUBLE:
        double_values_[info.index] = dd.Value();
        break;
      case Type::BOOL:
        bool_values_[info.index] = dd.Value();
        break;
    }
    *value = dd.Value();
    status_[info.index] = ConstantStatus::EVALUATED;
    return true;
  }

  CHECK(status == ConstantStatus::EVALUATED);
  switch (info.type) {
    case Type::INT:
      *value = int_values_[info.index];
      break;
    case Type::DOUBLE:
      *value = double_values_[info.index];
      break;
    case Type::BOOL:
      *value = bool_values_[info.index];
      break;
  }
  return true;
}

template <typename NativeType>
class CompiledTypeInfo {
 public:
  typedef ADD dd_type;

  CompiledTypeInfo() = delete;
};

template <>
class CompiledTypeInfo<bool> {
 public:
  typedef BDD dd_type;

  CompiledTypeInfo() = delete;
};

#endif  // EXPRESSION_COMPILER_H_
