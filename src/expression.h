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

#include <memory>
#include <string>
#include <utility>
#include <vector>

class ExpressionVisitor;

// Abstract base class for expressions.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcreteExpressionVisitor : public ExpressionVisitor {
//     ...
//   };
//
//   Expression* expr = ...;
//   ConcreteExpressionVisitor visitor;
//   expr->Accept(&visitor);
//
class Expression {
 public:
  // Disallow copy and assign.
  Expression(const Expression&) = delete;
  Expression& operator=(const Expression&) = delete;

  virtual ~Expression();

  virtual void Accept(ExpressionVisitor* visitor) const = 0;

 protected:
  Expression();
};

// An int literal.
class IntLiteral : public Expression {
 public:
  // Constructs an int literal that represents the given value.
  explicit IntLiteral(int value);

  virtual ~IntLiteral();

  // Factory method for int literal.
  static std::unique_ptr<const IntLiteral> Create(int value);

  // Returns the value of this integer literal.
  int value() const { return value_; }

  virtual void Accept(ExpressionVisitor* visitor) const;

 private:
  // The value of this integer literal.
  const int value_;
};

// A double literal.
class DoubleLiteral : public Expression {
 public:
  // Constructs a double literal that represents the given value.
  explicit DoubleLiteral(double value);

  virtual ~DoubleLiteral();

  // Factory method for double literal.
  static std::unique_ptr<const DoubleLiteral> Create(double value);

  // Returns the value of this double literal.
  double value() const { return value_; }

  virtual void Accept(ExpressionVisitor* visitor) const;

 private:
  // The value of this double literal.
  const double value_;
};

// A bool literal.
class BoolLiteral : public Expression {
 public:
  // Constructs a bool literal that represents the given value.
  explicit BoolLiteral(bool value);

  virtual ~BoolLiteral();

  // Factory method for bool literal.
  static std::unique_ptr<const BoolLiteral> Create(bool value);

  // Returns the value of this bool literal.
  bool value() const { return value_; }

  virtual void Accept(ExpressionVisitor* visitor) const;

 private:
  // The value of this bool literal.
  const bool value_;
};

// An identifier.
class Identifier : public Expression {
 public:
  // Constructs an identifier with the given name.
  explicit Identifier(const std::string& name);

  virtual ~Identifier();

  // Factory method for identifier.
  static std::unique_ptr<const Identifier> Create(const std::string& name);

  // Returns the name of this identifier.
  const std::string& name() const { return name_; }

  virtual void Accept(ExpressionVisitor* visitor) const;

 private:
  // The name of this identifier.
  const std::string name_;
};

// An argument list for a function call.
class ArgumentList {
 public:
  // Constructs an argument list.
  ArgumentList();
  template<typename... Args>
  ArgumentList(Args&&... args) {
    Init(std::forward<Args>(args)...);
  }
  ArgumentList(ArgumentList&& argument_list);

  // Disallow copy and assign.
  ArgumentList(const ArgumentList&) = delete;
  ArgumentList& operator=(const ArgumentList&) = delete;

  // Adds an argument to the end of the list.
  void push_back(std::unique_ptr<const Expression>&& argument);

  // Returns the size of this argument list.
  int size() const { return arguments_.size(); }

  // Returns the ith argument.
  const Expression& operator[](int i) const;

 private:
  // Initializes argument list from variable number of arguments.
  void Init();
  template<typename... Args>
  void Init(std::unique_ptr<const Expression>&& argument, Args&&... args) {
    push_back(std::move(argument));
    Init(std::forward<Args>(args)...);
  }

  // The arguments for this argument list.
  std::vector<std::unique_ptr<const Expression> > arguments_;
};

// Supported functions.
enum class Function { NO_FUNCTION, MIN, MAX, FLOOR, CEIL, POW, MOD, LOG };

// Returns the name of the given function.
std::string Function_Name(Function function);

// A function call.
class FunctionCall : public Expression {
 public:
  // Constructs a function call for the given function with the given arguments.
  FunctionCall(Function function, ArgumentList&& arguments);

  virtual ~FunctionCall();

  // Factory method for function call.
  static std::unique_ptr<const FunctionCall> Create(
      Function function, ArgumentList&& arguments);

  // Returns the function for this function call.
  Function function() const { return function_; }

  // Returns the arguments for this function call.
  const ArgumentList& arguments() const { return arguments_; }

  virtual void Accept(ExpressionVisitor* visitor) const;

 private:
  // The function for this function call.
  const Function function_;
  // The argument for this function call.
  const ArgumentList arguments_;
};

// Supported unary operators.
enum class UnaryOperator { NEGATE, NOT };

// A unary operation.
class UnaryOperation : public Expression {
 public:
  // Constructs a unary operation with the given operator and operand.
  UnaryOperation(UnaryOperator op, std::unique_ptr<const Expression>&& operand);

  virtual ~UnaryOperation();

  // Factory method for unary operation.
  static std::unique_ptr<const UnaryOperation> Create(
      UnaryOperator op, std::unique_ptr<const Expression>&& operand);

  // Returns the operator for this unary operation.
  UnaryOperator op() const { return op_; }

  // Returns the operand for this unary operation.
  const Expression& operand() const { return *operand_; }

  virtual void Accept(ExpressionVisitor* visitor) const;

 private:
  // The operator for this unary operation.
  const UnaryOperator op_;
  // The operand for this unary operation.
  const std::unique_ptr<const Expression> operand_;
};

// Supported binary operators.
enum class BinaryOperator {
  MULTIPLY, DIVIDE, ADD, SUBTRACT,
  LESS, LESS_EQUAL, GREATER_EQUAL, GREATER, EQUAL, NOT_EQUAL,
  AND, OR, IMPLY
};

// A binary operation.
class BinaryOperation : public Expression {
 public:
  // Constructs a binary operation with the given operator and operands.
  BinaryOperation(BinaryOperator op,
                  std::unique_ptr<const Expression>&& operand1,
                  std::unique_ptr<const Expression>&& operand2);

  virtual ~BinaryOperation();

  // Factory method for binary operation.
  static std::unique_ptr<const BinaryOperation> Create(
      BinaryOperator op,
      std::unique_ptr<const Expression>&& operand1,
      std::unique_ptr<const Expression>&& operand2);

  // Returns the operator for this binary operation.
  BinaryOperator op() const { return op_; }

  // Returns the first operand for this binary operation.
  const Expression& operand1() const { return *operand1_; }

  // Returns the second operand for this binary operation.
  const Expression& operand2() const { return *operand2_; }

  virtual void Accept(ExpressionVisitor* visitor) const;

 private:
  // The operator for this binary operation.
  const BinaryOperator op_;
  // The first operand for this binary operation.
  const std::unique_ptr<const Expression> operand1_;
  // The second operand for this binary operation.
  const std::unique_ptr<const Expression> operand2_;
};

// A conditional expression (condition ? expr1 : expr2).
class Conditional : public Expression {
 public:
  // Constructs a conditional expression with the given condition and
  // sub-expressions.
  Conditional(std::unique_ptr<const Expression>&& condition,
              std::unique_ptr<const Expression>&& if_expr,
              std::unique_ptr<const Expression>&& else_expr);

  virtual ~Conditional();

  // Factory method for conditional.
  static std::unique_ptr<const Conditional> Create(
      std::unique_ptr<const Expression>&& condition,
      std::unique_ptr<const Expression>&& if_expr,
      std::unique_ptr<const Expression>&& else_expr);

  // Returns the condition for this conditional expression.
  const Expression& condition() const { return *condition_; }

  // Returns the expression for the if-branch of this conditional expression.
  const Expression& if_expr() const { return *if_expr_; }

  // Returns the expression for the else-branch of this conditional expression.
  const Expression& else_expr() const { return *else_expr_; }

  virtual void Accept(ExpressionVisitor* visitor) const;

 private:
  // The condition for this conditional expression.
  const std::unique_ptr<const Expression> condition_;
  // The if-branch of this conditional expression.
  const std::unique_ptr<const Expression> if_expr_;
  // The else-branch of this conditional expression.
  const std::unique_ptr<const Expression> else_expr_;
};

// Abstract base class for expression visitors.
class ExpressionVisitor {
 public:
  // Disallow copy and assign.
  ExpressionVisitor(const ExpressionVisitor&) = delete;
  ExpressionVisitor& operator=(const ExpressionVisitor&) = delete;

  virtual ~ExpressionVisitor();

  // Called by the Accept() method of the respective expression classes.
  virtual void VisitIntLiteral(const IntLiteral& expr) = 0;
  virtual void VisitDoubleLiteral(const DoubleLiteral& expr) = 0;
  virtual void VisitBoolLiteral(const BoolLiteral& expr) = 0;
  virtual void VisitIdentifier(const Identifier& expr) = 0;
  virtual void VisitFunctionCall(const FunctionCall& expr) = 0;
  virtual void VisitUnaryOperation(const UnaryOperation& expr) = 0;
  virtual void VisitBinaryOperation(const BinaryOperation& expr) = 0;
  virtual void VisitConditional(const Conditional& expr) = 0;

 protected:
  ExpressionVisitor();
};

#endif  // EXPRESSION_H_
