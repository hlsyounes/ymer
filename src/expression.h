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
//
// A class hierarchy for representing expressions that supports the visitor
// pattern.

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <memory>
#include <ostream>
#include <string>

#include "typed-value.h"
#include "unique-ptr-vector.h"

class ExpressionVisitor;

// Abstract base class for expressions.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcreteExpressionVisitor : public ExpressionVisitor {
//     ...
//   };
//
//   const Expression* expr = ...;
//   ConcreteExpressionVisitor visitor;
//   expr->Accept(&visitor);
//
class Expression {
 public:
  virtual ~Expression();

  void Accept(ExpressionVisitor* visitor) const;

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const = 0;
};

// Output operator for expressions.
std::ostream& operator<<(std::ostream& os, const Expression& e);

class PathPropertyVisitor;

// Abstract base class for path properties.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcretePathPropertyVisitor : public PathPropertyVisitor {
//     ...
//   };
//
//   const PathProperty* path_property = ...;
//   ConcretePathPropertyVisitor visitor;
//   path_property->Accept(&visitor);
//
class PathProperty {
 public:
  virtual ~PathProperty();

  void Accept(PathPropertyVisitor* visitor) const;

 private:
  virtual void DoAccept(PathPropertyVisitor* visitor) const = 0;
};

// Output operator for path properties.
std::ostream& operator<<(std::ostream& os, const PathProperty& p);

// A literal expression.
class Literal : public Expression {
 public:
  // Constructs a literal that represents the given value.
  explicit Literal(const TypedValue& value);

  // Factory method for literals.
  static std::unique_ptr<const Literal> New(const TypedValue& value);

  // Returns the value of this literal.
  const TypedValue& value() const { return value_; }

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  TypedValue value_;
};

// An identifier expression.
class Identifier : public Expression {
 public:
  // Constructs an identifier with the given name.
  explicit Identifier(const std::string& name);

  // Factory method for identifiers.
  static std::unique_ptr<const Identifier> New(const std::string& name);

  // Returns the name of this identifier.
  const std::string& name() const { return name_; }

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  std::string name_;
};

// Supported functions.
enum class Function {
  UNKNOWN, MIN, MAX, FLOOR, CEIL, POW, LOG, MOD
};

// Output operator for functions.
std::ostream& operator<<(std::ostream& os, Function function);

// A function call expression.
class FunctionCall : public Expression {
 public:
  // Constructs a function call for the given function with the given arguments.
  explicit FunctionCall(Function function,
                        UniquePtrVector<const Expression>&& arguments);

  // Factory method for function calls.
  static std::unique_ptr<const FunctionCall> New(
      Function function, UniquePtrVector<const Expression>&& arguments);

  // Returns the function of this function call.
  Function function() const { return function_; }

  // Returns the arguments of this function call.
  const UniquePtrVector<const Expression>& arguments() const {
    return arguments_;
  }

 private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  Function function_;
  UniquePtrVector<const Expression> arguments_;
};

// Supported uniary operators.
enum class UnaryOperator {
  NEGATE, NOT
};

// Output operator for unary operators.
std::ostream& operator<<(std::ostream& os, UnaryOperator op);

// A unary operation expression.
class UnaryOperation : public Expression {
 public:
  // Constructs a unary operation with the given operator and operand.
  explicit UnaryOperation(UnaryOperator op,
                          std::unique_ptr<const Expression>&& operand);

  // Factory method for unary operations.
  static std::unique_ptr<const UnaryOperation> New(
      UnaryOperator op, std::unique_ptr<const Expression>&& operand);

  // Returns the operator of this unary operation.
  UnaryOperator op() const { return op_; }

  // Returns the operand of this unary operation.
  const Expression& operand() const { return *operand_; }

 private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  UnaryOperator op_;
  std::unique_ptr<const Expression> operand_;
};

// Supported binary operators.
enum class BinaryOperator {
  PLUS, MINUS, MULTIPLY, DIVIDE, AND, OR, IMPLY, IFF,
  LESS, LESS_EQUAL, GREATER_EQUAL, GREATER, EQUAL, NOT_EQUAL
};

// Output operator for binary operators.
std::ostream& operator<<(std::ostream& os, BinaryOperator op);

// A binary operation expression.
class BinaryOperation : public Expression {
 public:
  // Constructs a binary operation with the given operator and operands.
  explicit BinaryOperation(BinaryOperator op,
                           std::unique_ptr<const Expression>&& operand1,
                           std::unique_ptr<const Expression>&& operand2);

  // Factory method for binary operations.
  static std::unique_ptr<const BinaryOperation> New(
      BinaryOperator op,
      std::unique_ptr<const Expression>&& operand1,
      std::unique_ptr<const Expression>&& operand2);

  // Returns the operator for this binary operation.
  const BinaryOperator op() const { return op_; }

  // Returns the first operand for this binary operation.
  const Expression& operand1() const { return *operand1_; }

  // Returns the second operand for this binary operation.
  const Expression& operand2() const { return *operand2_; }

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  BinaryOperator op_;
  std::unique_ptr<const Expression> operand1_;
  std::unique_ptr<const Expression> operand2_;
};

// A conditional expression.
class Conditional : public Expression {
 public:
  // Constructs a conditional with the given condition and branch expressions.
  explicit Conditional(std::unique_ptr<const Expression>&& condition,
                       std::unique_ptr<const Expression>&& if_branch,
                       std::unique_ptr<const Expression>&& else_branch);

  // Factory method for conditional expressions.
  static std::unique_ptr<const Conditional> New(
      std::unique_ptr<const Expression>&& condition,
      std::unique_ptr<const Expression>&& if_branch,
      std::unique_ptr<const Expression>&& else_branch);

  // Returns the condition for this conditional expression.
  const Expression& condition() const { return *condition_; }

  // Returns the if-branch for this conditional expression.
  const Expression& if_branch() const { return *if_branch_; }

  // Returns the else-branch for this conditional expression.
  const Expression& else_branch() const { return *else_branch_; }

 private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  std::unique_ptr<const Expression> condition_;
  std::unique_ptr<const Expression> if_branch_;
  std::unique_ptr<const Expression> else_branch_;
};

// Supported operators for probability threshold operations.
enum class ProbabilityThresholdOperator {
  LESS, LESS_EQUAL, GREATER_EQUAL, GREATER
};

// Output operator for probability threshold operators.
std::ostream& operator<<(std::ostream& os, ProbabilityThresholdOperator op);

// A probability threshold operation.
class ProbabilityThresholdOperation : public Expression {
 public:
  // Constructs a probability threshold expression with the given operator,
  // threshold, and path property.
  explicit ProbabilityThresholdOperation(
      ProbabilityThresholdOperator op, double threshold,
      std::unique_ptr<const PathProperty>&& path_property);

  // Factory method for probability threshold operations.
  static std::unique_ptr<const ProbabilityThresholdOperation> New(
      ProbabilityThresholdOperator op, double threshold,
      std::unique_ptr<const PathProperty>&& path_property);

  // Returns the operator for this probability threshold operation.
  ProbabilityThresholdOperator op() const { return op_; }

  // Returns the threshold for this probability threshold operation.
  double threshold() const { return threshold_; }

  const PathProperty& path_property() const { return *path_property_; }

 private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  ProbabilityThresholdOperator op_;
  double threshold_;
  std::unique_ptr<const PathProperty> path_property_;
};

// An until path property.
class UntilProperty : public PathProperty {
 public:
  // Constructs an until path property with the given time range, pre-, and
  // post-condition expressions.
  explicit UntilProperty(double min_time, double max_time,
                         std::unique_ptr<const Expression>&& pre_expr,
                         std::unique_ptr<const Expression>&& post_expr);

  // Factory method for until path properties.
  static std::unique_ptr<const UntilProperty> New(
      double min_time, double max_time,
      std::unique_ptr<const Expression>&& pre_expr,
      std::unique_ptr<const Expression>&& post_expr);

  // Returns the min time for this until path property.
  double min_time() const { return min_time_; }

  // Returns the max time for this until path property.
  double max_time() const { return max_time_; }

  // Returns the precondition expression for this until path property.
  const Expression& pre_expr() const { return *pre_expr_; }

  // Returns the post-condition expression for this until path property.
  const Expression& post_expr() const { return *post_expr_; }

 private:
  virtual void DoAccept(PathPropertyVisitor* visitor) const;

  double min_time_;
  double max_time_;
  std::unique_ptr<const Expression> pre_expr_;
  std::unique_ptr<const Expression> post_expr_;
};

// Abstract base class for expression visitors.
class ExpressionVisitor {
 public:
  void VisitLiteral(const Literal& expr);
  void VisitIdentifier(const Identifier& expr);
  void VisitFunctionCall(const FunctionCall& expr);
  void VisitUnaryOperation(const UnaryOperation& expr);
  void VisitBinaryOperation(const BinaryOperation& expr);
  void VisitConditional(const Conditional& expr);
  void VisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr);

 protected:
  ~ExpressionVisitor();

 private:
  virtual void DoVisitLiteral(const Literal& expr) = 0;
  virtual void DoVisitIdentifier(const Identifier& expr) = 0;
  virtual void DoVisitFunctionCall(const FunctionCall& expr) = 0;
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr) = 0;
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr) = 0;
  virtual void DoVisitConditional(const Conditional& expr) = 0;
  virtual void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) = 0;
};

// Abstract base class for path property visitors.
class PathPropertyVisitor {
 public:
  void VisitUntilProperty(const UntilProperty& path_property);

 protected:
  ~PathPropertyVisitor();

 private:
  virtual void DoVisitUntilProperty(const UntilProperty& path_property) = 0;
};

#endif  // EXPRESSION_H_
