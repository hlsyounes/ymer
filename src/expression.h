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

// A literal expression.
class Literal : public Expression {
 public:
  // Constructs a literal that represents the given value.
  explicit Literal(const TypedValue& value);

  virtual ~Literal();

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

  virtual ~Identifier();

  // Factory method for identifiers.
  static std::unique_ptr<const Identifier> New(const std::string& name);

  // Returns the name of this identifier.
  const std::string& name() const { return name_; }

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  std::string name_;
};

// Supported uniary operators.
enum class UnaryOperator {
  NEGATE
};

// Output operator for unary operators.
std::ostream& operator<<(std::ostream& os, UnaryOperator op);

// A unary operation expression.
class UnaryOperation : public Expression {
 public:
  // Constructs a unary operation with the given operator and operand.
  UnaryOperation(UnaryOperator op, std::unique_ptr<const Expression>&& operand);

  virtual ~UnaryOperation();

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
  PLUS, MINUS, MULTIPLY, DIVIDE
};

// Output operator for binary operators.
std::ostream& operator<<(std::ostream& os, BinaryOperator op);

// A binary operation expression.
class BinaryOperation : public Expression {
 public:
  // Constructs a binary operation with the given operator and operands.
  BinaryOperation(BinaryOperator op,
                  std::unique_ptr<const Expression>&& operand1,
                  std::unique_ptr<const Expression>&& operand2);

  virtual ~BinaryOperation();

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

// Abstract base class for expression visitors.
class ExpressionVisitor {
 public:
  void VisitLiteral(const Literal& expr);
  void VisitIdentifier(const Identifier& expr);
  void VisitUnaryOperation(const UnaryOperation& expr);
  void VisitBinaryOperation(const BinaryOperation& expr);

 protected:
  ~ExpressionVisitor();

 private:
  virtual void DoVisitLiteral(const Literal& expr) = 0;
  virtual void DoVisitIdentifier(const Identifier& expr) = 0;
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr) = 0;
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr) = 0;
};

#endif  // EXPRESSION_H_
