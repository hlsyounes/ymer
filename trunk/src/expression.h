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
#include <vector>

#include "ddutil.h"
#include "typed-value.h"

class Variable;

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
//   ComcreteExpressionVisitor visitor;
//   expr->Accept(&visitor);
//
class Expression {
 public:
  virtual ~Expression();

  // Increases the reference count for the given expression.
  static void ref(const Expression* e);

  // Decreases the reference count for the given expression and
  // deletes it if the the reference count becomes zero.
  static void destructive_deref(const Expression* e);

  void Accept(ExpressionVisitor* visitor) const;

  // Returns the value of this expression.
  virtual TypedValue value(const std::vector<int>& state) const = 0;

protected:
  // Constructs an expression.
  Expression();

private:
  // Disallow copy and assign.
  Expression(const Expression&);
  Expression& operator=(const Expression&);

  virtual void DoAccept(ExpressionVisitor* visitor) const = 0;

  // Reference counter.
  mutable int ref_count_;

  friend std::ostream& operator<<(std::ostream& os, const Expression& e);
};

// Returns the given expression subject to the given substitutions of values
// for constant identifiers.
const Expression* SubstituteConstants(
    const Expression& expr,
    const std::map<std::string, TypedValue>& constant_values);

// Returns the given expression subject to the given identifier substitutions.
const Expression* SubstituteIdentifiers(
    const Expression& expr,
    const std::map<std::string, const Variable*>& substitutions);

// Returns the 'current state' MTBDD representation for an expression.
ADD mtbdd(const DecisionDiagramManager& manager, const Expression& e);

// Returns the 'next state' MTBDD representation for an expression.
ADD primed_mtbdd(const DecisionDiagramManager& manager, const Expression& e);

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

  // Returns an addition of the two expressions.
  static const Expression* make(const Expression& term1,
                                const Expression& term2);

  // Returns the value of this expression.
  virtual TypedValue value(const std::vector<int>& state) const;

private:
  // Constructs an addition.
  Addition(const Expression& term1, const Expression& term2);
};

// A subtraction expression.
class Subtraction : public Computation {
 public:
  virtual ~Subtraction();

  // Returns a subtraction of the two expressions.
  static const Expression* make(const Expression& term1,
                                const Expression& term2);

  // Returns the value of this expression.
  virtual TypedValue value(const std::vector<int>& state) const;

private:
  // Constructs a subtraction.
  Subtraction(const Expression& term1, const Expression& term2);
};

// A multiplication expression.
class Multiplication : public Computation {
 public:
  virtual ~Multiplication();

  // Returns a multiplication of the two expressions.
  static const Expression* make(const Expression& factor1,
                                const Expression& factor2);

  // Returns the value of this expression.
  virtual TypedValue value(const std::vector<int>& state) const;

private:
  // Constructs a multiplication.
  Multiplication(const Expression& factor1, const Expression& factor2);
};

// A division expression.
class Division : public Computation {
 public:
  virtual ~Division();

  // Returns a division of the two expressions.
  static const Expression* make(const Expression& factor1,
                                const Expression& factor2);

  // Returns the value of this expression.
  virtual TypedValue value(const std::vector<int>& state) const;

private:
  // Constructs a division.
  Division(const Expression& factor1, const Expression& factor2);
};

// A variable expression.
class Variable : public Expression {
 public:
  // Constructs an identifier with the given name.
  explicit Variable(const std::string& name);

  virtual ~Variable();

  void SetVariableProperties(
      int low, int high, int start, int index, int low_bit);

  // Returns the name of this identifier.
  const std::string& name() const { return name_; }

  // Returns the lower bound for this variable.
  int low() const { return low_; }

  // Returns the upper bound for this variable.
  int high() const { return high_; }

  // Returns the initial value for this variable.
  int start() const { return start_; }

  // Returns the index of this variable.
  int index() const { return index_; }

  // Returns the index of the first DD variable used to represent this
  // variable.
  int low_bit() const { return low_bit_; }

  // Returns the index of the last DD variable used to represent this
  // variable.
  int high_bit() const { return high_bit_; }

  // Returns the value of this expression.
  virtual TypedValue value(const std::vector<int>& state) const;

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  // The name of this identifier.
  std::string name_;
  // The lower bound for this variable.
  int low_;
  // The upper bound for this variable.
  int high_;
  // The initial value for this variable.
  int start_;
  // The index of this variable.
  int index_;
  // Index of the first DD variable used to represent this variable.
  int low_bit_;
  // Index of the last DD variable used to represent this variable.
  int high_bit_;
};

// A literal expression.
class Literal : public Expression {
 public:
  // Constructs a literal that represents the given value.
  Literal(const TypedValue& value);

  virtual ~Literal();

  // Returns the value of this literal.
  const TypedValue& value() const { return value_; }

  // Returns the value of this expression.
  virtual TypedValue value(const std::vector<int>& state) const;

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
