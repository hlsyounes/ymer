// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2012 Google Inc
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

#include <map>
#include <ostream>
#include <string>

#include "ddutil.h"
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

  // Increases the reference count for the given expression.
  static void ref(const Expression* e);

  // Decreases the reference count for the given expression and
  // deletes it if the the reference count becomes zero.
  static void destructive_deref(const Expression* e);

  void Accept(ExpressionVisitor* visitor) const;

protected:
  // Constructs an expression.
  Expression();

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const = 0;

  // Reference counter.
  mutable int ref_count_;
};

class VariableProperties {
 public:
  VariableProperties(int min_value, int low_bit, int high_bit);

  int min_value() const { return min_value_; }

  int low_bit() const { return low_bit_; }

  int high_bit() const { return high_bit_; }

 private:
  int min_value_;
  int low_bit_;
  int high_bit_;
};

// Returns the 'current state' MTBDD representation for an expression.
ADD mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Expression& e);

// Returns the 'next state' MTBDD representation for an expression.
ADD primed_mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Expression& e);

// Returns the 'current state' MTBDD representation for a variable.
ADD variable_mtbdd(const DecisionDiagramManager& manager,
                   int low, int low_bit, int high_bit);

// Returns the 'next state' MTBDD representation for a variable.
ADD variable_primed_mtbdd(const DecisionDiagramManager& manager,
                          int low, int low_bit, int high_bit);

// Output operator for expressions.
std::ostream& operator<<(std::ostream& os, const Expression& e);

// A computation expression.
class Computation : public Expression {
 public:
  // Supported computation operators.
  enum Operator {
    PLUS, MINUS, MULTIPLY, DIVIDE
  };

  virtual ~Computation();

  // Factory method for creating computations.
  static const Expression* make(
      Operator op, const Expression& operand1, const Expression& operand2);

  // Returns the operator for this computation.
  const Operator op() const { return op_; }

  // Returns the first operand for this computation.
  const Expression& operand1() const { return *operand1_; }

  // Returns the second operand for this computation.
  const Expression& operand2() const { return *operand2_; }

protected:
  // Constructs a computation.
  Computation(Operator op,
              const Expression& operand1,
              const Expression& operand2);

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  // The operator for this computation.
  Operator op_;
  // The first operand for this computation.
  const Expression* operand1_;
  // The second operand for this computation.
  const Expression* operand2_;
};

// An addition expression.
class Addition : public Computation {
 public:
  virtual ~Addition();

private:
  // Constructs an addition.
  Addition(const Expression& term1, const Expression& term2);

  friend const Expression* Computation::make(
      Operator, const Expression&, const Expression&);
};

// A subtraction expression.
class Subtraction : public Computation {
 public:
  virtual ~Subtraction();

private:
  // Constructs a subtraction.
  Subtraction(const Expression& term1, const Expression& term2);

  friend const Expression* Computation::make(
      Operator, const Expression&, const Expression&);
};

// A multiplication expression.
class Multiplication : public Computation {
 public:
  virtual ~Multiplication();

private:
  // Constructs a multiplication.
  Multiplication(const Expression& factor1, const Expression& factor2);

  friend const Expression* Computation::make(
      Operator, const Expression&, const Expression&);
};

// A division expression.
class Division : public Computation {
 public:
  virtual ~Division();

private:
  // Constructs a division.
  Division(const Expression& factor1, const Expression& factor2);

  friend const Expression* Computation::make(
      Operator, const Expression&, const Expression&);
};

// A variable expression.
class Variable : public Expression {
 public:
  // Constructs an identifier with the given name.
  explicit Variable(const std::string& name);

  virtual ~Variable();

  // Returns the name of this identifier.
  const std::string& name() const { return name_; }

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  // The name of this identifier.
  std::string name_;
};

// A literal expression.
class Literal : public Expression {
 public:
  // Constructs a literal that represents the given value.
  Literal(const TypedValue& value);

  virtual ~Literal();

  // Returns the value of this literal.
  const TypedValue& value() const { return value_; }

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  // The value.
  TypedValue value_;
};

// Abstract base class for expression visitors.
class ExpressionVisitor {
 public:
  void VisitLiteral(const Literal& expr);
  void VisitVariable(const Variable& expr);
  void VisitComputation(const Computation& expr);

 protected:
  ExpressionVisitor();
  ExpressionVisitor(const ExpressionVisitor&);
  ExpressionVisitor& operator=(const ExpressionVisitor&);
  ~ExpressionVisitor();

 private:
  virtual void DoVisitLiteral(const Literal& expr) = 0;
  virtual void DoVisitVariable(const Variable& expr) = 0;
  virtual void DoVisitComputation(const Computation& expr) = 0;
};

#endif  // EXPRESSION_H_
