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

  // Factory method for creating literals.
  static std::unique_ptr<const Literal> Create(const TypedValue& value);

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

  // Factory method for creating identifiers.
  static std::unique_ptr<const Identifier> Create(const std::string& name);

  // Returns the name of this identifier.
  const std::string& name() const { return name_; }

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  std::string name_;
};

// A computation expression.
class Computation : public Expression {
 public:
  // Supported computation operators.
  enum Operator {
    PLUS, MINUS, MULTIPLY, DIVIDE
  };

  // Constructs a computation.
  Computation(Operator op,
              std::unique_ptr<const Expression>&& operand1,
              std::unique_ptr<const Expression>&& operand2);

  virtual ~Computation();

  // Factory method for creating computations.
  static std::unique_ptr<const Expression> Create(
      Operator op,
      std::unique_ptr<const Expression>&& operand1,
      std::unique_ptr<const Expression>&& operand2);

  // Returns the operator for this computation.
  const Operator op() const { return op_; }

  // Returns the first operand for this computation.
  const Expression& operand1() const { return *operand1_; }

  // Returns the second operand for this computation.
  const Expression& operand2() const { return *operand2_; }

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  Operator op_;
  std::unique_ptr<const Expression> operand1_;
  std::unique_ptr<const Expression> operand2_;
};

// Abstract base class for expression visitors.
class ExpressionVisitor {
 public:
  void VisitLiteral(const Literal& expr);
  void VisitIdentifier(const Identifier& expr);
  void VisitComputation(const Computation& expr);

 protected:
  ~ExpressionVisitor();

 private:
  virtual void DoVisitLiteral(const Literal& expr) = 0;
  virtual void DoVisitIdentifier(const Identifier& expr) = 0;
  virtual void DoVisitComputation(const Computation& expr) = 0;
};

#endif  // EXPRESSION_H_
