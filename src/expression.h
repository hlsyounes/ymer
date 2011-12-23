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
// A class hierarchy for representing expressions that supports the visitor
// pattern.

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"

#include "type.h"

template <typename IdType, typename ValueType> class ExpressionVisitor;

template <typename IdType> class IdentifierTypeInfo;

template <>
class IdentifierTypeInfo<std::string> {
 public:
  using State = std::map<std::string, TypedValue>;

  IdentifierTypeInfo() = delete;
};

template <>
class IdentifierTypeInfo<int> {
 public:
  using State = std::vector<int>;

  IdentifierTypeInfo() = delete;
};

template <typename IdType>
using State = typename IdentifierTypeInfo<IdType>::State;

using ParsedState = State<std::string>;
using CompiledState = State<int>;

// Abstract base class for expressions.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcreteExpressionVisitor : public ParsedExpressionVisitor {
//     ...
//   };
//
//   ParsedExpression* expr = ...;
//   ConcreteExpressionVisitor visitor;
//   expr->Accept(&visitor);
//
template <typename IdType, typename ValueType>
class Expression {
 public:
  // Disallow copy and assign.
  Expression(const Expression<IdType, ValueType>&) = delete;
  Expression& operator=(const Expression<IdType, ValueType>&) = delete;

  virtual ~Expression();

  virtual ValueType ValueInState(const State<IdType>& state) const = 0;

  virtual void Accept(ExpressionVisitor<IdType, ValueType>* visitor) const = 0;

 protected:
  Expression();
};

using ParsedExpression = Expression<std::string, TypedValue>;
template <typename ValueType>
using CompiledExpression = Expression<int, ValueType>;

// A literal.
template <typename IdType, typename ValueType>
class Literal : public Expression<IdType, ValueType> {
 public:
  // Constructs a literal that represents the given value.
  explicit Literal(const ValueType& value);

  virtual ~Literal();

  // Factory method for literal.
  static std::unique_ptr<const Literal<IdType, ValueType> > Create(
      const ValueType& value);

  // Returns the value of this literal.
  const ValueType& value() const { return value_; }

  virtual ValueType ValueInState(const State<IdType>& state) const;

  virtual void Accept(ExpressionVisitor<IdType, ValueType>* visitor) const;

 private:
  // The value of this literal.
  const ValueType value_;
};

using ParsedLiteral = Literal<std::string, TypedValue>;
template <typename ValueType>
using CompiledLiteral = Literal<int, ValueType>;

// An identifier.
template <typename IdType, typename ValueType>
class Identifier : public Expression<IdType, ValueType> {
 public:
  // Constructs an identifier with the given name.
  explicit Identifier(const IdType& id);

  virtual ~Identifier();

  // Factory method for identifier.
  static std::unique_ptr<const Identifier<IdType, ValueType> > Create(
      const IdType& id);

  // Returns the ID of this identifier.
  const IdType& id() const { return id_; }

  virtual ValueType ValueInState(const State<IdType>& state) const;

  virtual void Accept(ExpressionVisitor<IdType, ValueType>* visitor) const;

 private:
  // The ID of this identifier.
  const IdType id_;
};

using ParsedIdentifier = Identifier<std::string, TypedValue>;
template <typename ValueType>
using CompiledIdentifier = Identifier<int, ValueType>;

// An argument list for a function call.
template <typename IdType, typename ValueType>
class ArgumentList {
 public:
  // An iterator for ArgumentList.
  class Iterator : public std::iterator<std::input_iterator_tag,
                                        Expression<IdType, ValueType> > {
   public:
    // Copy and assign for argument list iterator.
    Iterator(const Iterator&) = default;
    Iterator& operator=(const Iterator&) = default;

    // Equality operator for argument list iterator.
    bool operator==(const Iterator& rhs) const { return i_ == rhs.i_; }

    // Inequality operator for argument list iterator.
    bool operator!=(const Iterator& rhs) const { return i_ != rhs.i_; }

    // Dereference operator for argument list iterator.
    const Expression<IdType, ValueType>& operator*() const { return **i_; }

    // Pointer operator for argument list iterator.
    const Expression<IdType, ValueType>* operator->() const {
      return i_->get();
    }

    // Pre-increment operator for argument list iterator.
    Iterator& operator++() { ++i_; return *this; }

    // Post-increment operator for argument list iterator.
    Iterator operator++(int) { Iterator tmp(*this); ++i_; return tmp; }

   private:
    using Elements =
        std::vector<std::unique_ptr<const Expression<IdType, ValueType> > >;

    // Constructs an argument list iterator from the given iterator for the
    // underlying container of an argument list.
    Iterator(typename Elements::const_iterator i)
        : i_(i) {
    }

    // Iterator for the underlying container of an argument list.
    typename Elements::const_iterator i_;

    friend class ArgumentList;
  };

  // Constructs an argument list.
  ArgumentList();
  template <typename... Args>
  ArgumentList(Args&&... args) {
    Init(std::forward<Args>(args)...);
  }
  ArgumentList(ArgumentList<IdType, ValueType>&& argument_list);

  // Disallow copy and assign.
  ArgumentList(const ArgumentList<IdType, ValueType>&) = delete;
  ArgumentList& operator=(const ArgumentList<IdType, ValueType>&) = delete;

  // Adds an argument to the end of the list.
  void push_back(
      std::unique_ptr<const Expression<IdType, ValueType> >&& argument);

  // Returns the size of this argument list.
  int size() const { return arguments_.size(); }

  // Returns an iterator for the beginning of this argument list.
  Iterator begin() const { return arguments_.begin(); }

  // Returns an iterator for the end of this argument list.
  Iterator end() const { return arguments_.end(); }

  // Returns the ith argument.
  const Expression<IdType, ValueType>& operator[](int i) const;

 private:
  // Initializes argument list from variable number of arguments.
  void Init();
  template <typename... Args>
  void Init(std::unique_ptr<const Expression<IdType, ValueType> >&& argument,
            Args&&... args) {
    push_back(std::move(argument));
    Init(std::forward<Args>(args)...);
  }

  // The arguments for this argument list.
  typename Iterator::Elements arguments_;
};

using ParsedArgumentList = ArgumentList<std::string, TypedValue>;
template <typename ValueType>
using CompiledArgumentList = ArgumentList<int, ValueType>;

// Supported functions.
enum class Function { NO_FUNCTION, MIN, MAX, FLOOR, CEIL, POW, MOD, LOG };

std::ostream& operator<<(std::ostream& os, Function function);

// A function call.
template <typename IdType, typename ValueType>
class FunctionCall : public Expression<IdType, ValueType> {
 public:
  // Constructs a function call for the given function with the given arguments.
  template <typename... Args>
  FunctionCall(Function function, Args&&... args)
      : function_(function), arguments_(std::forward<Args>(args)...) {
  }

  virtual ~FunctionCall();

  // Factory method for function call.
  template <typename... Args>
  static std::unique_ptr<const FunctionCall<IdType, ValueType> > Create(
      Function function, Args&&... args) {
    return std::unique_ptr<const FunctionCall<IdType, ValueType> >(
        new FunctionCall<IdType, ValueType>(
            function, std::forward<Args>(args)...));
  }

  // Returns the function for this function call.
  Function function() const { return function_; }

  // Returns the arguments for this function call.
  const ArgumentList<IdType, ValueType>& arguments() const {
    return arguments_;
  }

  virtual ValueType ValueInState(const State<IdType>& state) const;

  virtual void Accept(ExpressionVisitor<IdType, ValueType>* visitor) const;

 private:
  // The function for this function call.
  const Function function_;
  // The argument for this function call.
  const ArgumentList<IdType, ValueType> arguments_;
};

using ParsedFunctionCall = FunctionCall<std::string, TypedValue>;
template <typename ValueType>
using CompiledFunctionCall = FunctionCall<int, ValueType>;

// Supported unary operators.
enum class UnaryOperator { NEGATE, NOT };

std::ostream& operator<<(std::ostream& os, UnaryOperator op);

// A unary operation.
template <typename IdType, typename ValueType>
class UnaryOperation : public Expression<IdType, ValueType> {
 public:
  // Constructs a unary operation with the given operator and operand.
  UnaryOperation(
      UnaryOperator op,
      std::unique_ptr<const Expression<IdType, ValueType> >&& operand);

  virtual ~UnaryOperation();

  // Factory method for unary operation.
  static std::unique_ptr<const UnaryOperation<IdType, ValueType> > Create(
      UnaryOperator op,
      std::unique_ptr<const Expression<IdType, ValueType> >&& operand);

  // Returns the operator for this unary operation.
  UnaryOperator op() const { return op_; }

  // Returns the operand for this unary operation.
  const Expression<IdType, ValueType>& operand() const { return *operand_; }

  virtual ValueType ValueInState(const State<IdType>& state) const;

  virtual void Accept(ExpressionVisitor<IdType, ValueType>* visitor) const;

 private:
  // The operator for this unary operation.
  const UnaryOperator op_;
  // The operand for this unary operation.
  const std::unique_ptr<const Expression<IdType, ValueType> > operand_;
};

using ParsedUnaryOperation = UnaryOperation<std::string, TypedValue>;
template <typename ValueType>
using CompiledUnaryOperation = UnaryOperation<int, ValueType>;

// Supported binary operators.
enum class BinaryOperator {
  MULTIPLY, DIVIDE, PLUS, MINUS,
  LESS, LESS_EQUAL, GREATER_EQUAL, GREATER, EQUAL, NOT_EQUAL,
  AND, OR, IMPLY
};

std::ostream& operator<<(std::ostream& os, BinaryOperator op);

// A binary operation.
template <typename IdType, typename ValueType>
class BinaryOperation : public Expression<IdType, ValueType> {
 public:
  // Constructs a binary operation with the given operator and operands.
  BinaryOperation(
      BinaryOperator op,
      std::unique_ptr<const Expression<IdType, ValueType> >&& operand1,
      std::unique_ptr<const Expression<IdType, ValueType> >&& operand2);

  virtual ~BinaryOperation();

  // Factory method for binary operation.
  static std::unique_ptr<const BinaryOperation<IdType, ValueType> > Create(
      BinaryOperator op,
      std::unique_ptr<const Expression<IdType, ValueType> >&& operand1,
      std::unique_ptr<const Expression<IdType, ValueType> >&& operand2);

  // Returns the operator for this binary operation.
  BinaryOperator op() const { return op_; }

  // Returns the first operand for this binary operation.
  const Expression<IdType, ValueType>& operand1() const { return *operand1_; }

  // Returns the second operand for this binary operation.
  const Expression<IdType, ValueType>& operand2() const { return *operand2_; }

  virtual ValueType ValueInState(const State<IdType>& state) const;

  virtual void Accept(ExpressionVisitor<IdType, ValueType>* visitor) const;

 private:
  // The operator for this binary operation.
  const BinaryOperator op_;
  // The first operand for this binary operation.
  const std::unique_ptr<const Expression<IdType, ValueType> > operand1_;
  // The second operand for this binary operation.
  const std::unique_ptr<const Expression<IdType, ValueType> > operand2_;
};

using ParsedBinaryOperation = BinaryOperation<std::string, TypedValue>;
template <typename ValueType>
using CompiledBinaryOperation = BinaryOperation<int, ValueType>;

// A conditional expression (condition ? expr1 : expr2).
template <typename IdType, typename ValueType>
class Conditional : public Expression<IdType, ValueType> {
 public:
  // Constructs a conditional expression with the given condition and
  // sub-expressions.
  Conditional(
      std::unique_ptr<const Expression<IdType, ValueType> >&& condition,
      std::unique_ptr<const Expression<IdType, ValueType> >&& if_expr,
      std::unique_ptr<const Expression<IdType, ValueType> >&& else_expr);

  virtual ~Conditional();

  // Factory method for conditional.
  static std::unique_ptr<const Conditional<IdType, ValueType> > Create(
      std::unique_ptr<const Expression<IdType, ValueType> >&& condition,
      std::unique_ptr<const Expression<IdType, ValueType> >&& if_expr,
      std::unique_ptr<const Expression<IdType, ValueType> >&& else_expr);

  // Returns the condition for this conditional expression.
  const Expression<IdType, ValueType>& condition() const { return *condition_; }

  // Returns the expression for the if-branch of this conditional expression.
  const Expression<IdType, ValueType>& if_expr() const { return *if_expr_; }

  // Returns the expression for the else-branch of this conditional expression.
  const Expression<IdType, ValueType>& else_expr() const { return *else_expr_; }

  virtual ValueType ValueInState(const State<IdType>& state) const;

  virtual void Accept(ExpressionVisitor<IdType, ValueType>* visitor) const;

 private:
  // The condition for this conditional expression.
  const std::unique_ptr<const Expression<IdType, ValueType> > condition_;
  // The if-branch of this conditional expression.
  const std::unique_ptr<const Expression<IdType, ValueType> > if_expr_;
  // The else-branch of this conditional expression.
  const std::unique_ptr<const Expression<IdType, ValueType> > else_expr_;
};

using ParsedConditional = Conditional<std::string, TypedValue>;
template <typename ValueType>
using CompiledConditional = Conditional<int, ValueType>;

// Abstract base class for expression visitors.
template <typename IdType, typename ValueType>
class ExpressionVisitor {
 public:
  virtual ~ExpressionVisitor();

  // Called by the Accept() method of the respective expression classes.
  virtual void VisitLiteral(const Literal<IdType, ValueType>& expr) = 0;
  virtual void VisitIdentifier(const Identifier<IdType, ValueType>& expr) = 0;
  virtual void VisitFunctionCall(
      const FunctionCall<IdType, ValueType>& expr) = 0;
  virtual void VisitUnaryOperation(
      const UnaryOperation<IdType, ValueType>& expr) = 0;
  virtual void VisitBinaryOperation(
      const BinaryOperation<IdType, ValueType>& expr) = 0;
  virtual void VisitConditional(const Conditional<IdType, ValueType>& expr) = 0;

 protected:
  ExpressionVisitor();
};

using ParsedExpressionVisitor = ExpressionVisitor<std::string, TypedValue>;
template <typename ValueType>
using CompiledExpressionVisitor = ExpressionVisitor<int, ValueType>;

#endif  // EXPRESSION_H_
