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
#include <set>
#include <vector>

#include "ddutil.h"
#include "typed-value.h"

class Variable;

// A mapping from variables to values.
typedef std::map<const Variable*, TypedValue> ValueMap;

// A variable substitution map.
typedef std::map<const Variable*, const Variable*> SubstitutionMap;

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
  virtual TypedValue value(const ValueMap& values) const = 0;

  // Returns this expression subject to the given substitutions.
  virtual const Expression& substitution(const ValueMap& values) const = 0;

  // Returns this expression subject to the given substitutions.
  virtual const Expression& substitution(
      const SubstitutionMap& subst) const = 0;

  // Returns the `current state' MTBDD representation for this expression.
  virtual DdNode* mtbdd(const DecisionDiagramManager& dd_man) const = 0;

  // Returns the `next state' MTBDD representation for this expression.
  virtual DdNode* primed_mtbdd(const DecisionDiagramManager& dd_man) const = 0;

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

// Output operator for expressions.
std::ostream& operator<<(std::ostream& os, const Expression& e);

// A computation expression.
class Computation : public Expression {
 public:
  // Supported computation operators.
  enum Operator {
    MULTIPLY, DIVIDE, PLUS, MINUS
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
  static const Expression& make(const Expression& term1,
                                const Expression& term2);

  // Returns the value of this expression.
  virtual TypedValue value(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Expression& substitution(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Addition& substitution(const SubstitutionMap& subst) const;

  // Returns the `current state' MTBDD representation for this expression.
  virtual DdNode* mtbdd(const DecisionDiagramManager& dd_man) const;

  // Returns the `next state' MTBDD representation for this expression.
  virtual DdNode* primed_mtbdd(const DecisionDiagramManager& dd_man) const;

private:
  // Constructs an addition.
  Addition(const Expression& term1, const Expression& term2);
};

// A subtraction expression.
class Subtraction : public Computation {
 public:
  virtual ~Subtraction();

  // Returns a subtraction of the two expressions.
  static const Expression& make(const Expression& term1,
                                const Expression& term2);

  // Returns the value of this expression.
  virtual TypedValue value(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Expression& substitution(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Subtraction& substitution(const SubstitutionMap& subst) const;

  // Returns the `current state' MTBDD representation for this expression.
  virtual DdNode* mtbdd(const DecisionDiagramManager& dd_man) const;

  // Returns the `next state' MTBDD representation for this expression.
  virtual DdNode* primed_mtbdd(const DecisionDiagramManager& dd_man) const;

private:
  // Constructs a subtraction.
  Subtraction(const Expression& term1, const Expression& term2);
};

// A multiplication expression.
class Multiplication : public Computation {
 public:
  virtual ~Multiplication();

  // Returns a multiplication of the two expressions.
  static const Expression& make(const Expression& factor1,
                                const Expression& factor2);

  // Returns the value of this expression.
  virtual TypedValue value(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Expression& substitution(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Multiplication& substitution(
      const SubstitutionMap& subst) const;

  // Returns the `current state' MTBDD representation for this expression.
  virtual DdNode* mtbdd(const DecisionDiagramManager& dd_man) const;

  // Returns the `next state' MTBDD representation for this expression.
  virtual DdNode* primed_mtbdd(const DecisionDiagramManager& dd_man) const;

private:
  // Constructs a multiplication.
  Multiplication(const Expression& factor1, const Expression& factor2);
};

// A division expression.
class Division : public Computation {
 public:
  virtual ~Division();

  // Returns a division of the two expressions.
  static const Expression& make(const Expression& factor1,
                                const Expression& factor2);

  // Returns the value of this expression.
  virtual TypedValue value(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Expression& substitution(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Division& substitution(const SubstitutionMap& subst) const;

  // Returns the `current state' MTBDD representation for this expression.
  virtual DdNode* mtbdd(const DecisionDiagramManager& dd_man) const;

  // Returns the `next state' MTBDD representation for this expression.
  virtual DdNode* primed_mtbdd(const DecisionDiagramManager& dd_man) const;

private:
  // Constructs a division.
  Division(const Expression& factor1, const Expression& factor2);
};

// A variable expression.
class Variable : public Expression {
 public:
  // Constructs a variable.
  Variable();
  Variable(int low, int high, int start, int low_bit);

  virtual ~Variable();

  // Sets the lower bound for this variable.
  void set_low(int low);

  // Sets the upper bound for this variable.
  void set_high(int high);

  // Sets the initial value for this variable.
  void set_start(int start);

  // Sets the index of the first DD variable used to represent this
  // variable.
  void set_low_bit(int low_bit);

  // Returns the lower bound for this variable.
  int low() const { return low_; }

  // Returns the upper bound for this variable.
  int high() const { return high_; }

  // Returns the initial value for this variable.
  int start() const { return start_; }

  // Returns the index of the first DD variable used to represent this
  // variable.
  int low_bit() const { return low_bit_; }

  // Returns the index of the last DD variable used to represent this
  // variable.
  int high_bit() const { return high_bit_; }

  // Returns the value of this expression.
  virtual TypedValue value(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Expression& substitution(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Variable& substitution(const SubstitutionMap& subst) const;

  // Returns the `current state' MTBDD representation for this expression.
  virtual DdNode* mtbdd(const DecisionDiagramManager& dd_man) const;

  // Returns the `next state' MTBDD representation for this expression.
  DdNode* primed_mtbdd(const DecisionDiagramManager& dd_man) const;

  // Returns a BDD representing identity between the `current state'
  // and `next state' versions of this variable.
  DdNode* identity_bdd(const DecisionDiagramManager& dd_man) const;

  // Returns a BDD representing the range for this variable.
  DdNode* range_bdd(const DecisionDiagramManager& dd_man) const;

  // Releases any cached DDs for this variable.
  void uncache_dds(const DecisionDiagramManager& dd_man) const;

private:
  virtual void DoAccept(ExpressionVisitor* visitor) const;

  // The lower bound for this variable.
  int low_;
  // The upper bound for this variable.
  int high_;
  // The initial value for this variable.
  int start_;
  // Index of the first DD variable used to represent this variable.
  int low_bit_;
  // Index of the last DD variable used to represent this variable.
  int high_bit_;
  // Cached BDD representing identity between the `current state' and
  // `next state' versions of this variable.
  mutable DdNode* identity_bdd_;
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
  virtual TypedValue value(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Literal& substitution(const ValueMap& values) const;

  // Returns this expression subject to the given substitutions.
  virtual const Literal& substitution(const SubstitutionMap& subst) const;

  // Returns the `current state' MTBDD representation for this expression.
  virtual DdNode* mtbdd(const DecisionDiagramManager& dd_man) const;

  // Returns the `next state' MTBDD representation for this expression.
  virtual DdNode* primed_mtbdd(const DecisionDiagramManager& dd_man) const;

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

// A list of variables.
typedef std::vector<const Variable*> VariableList;

// A set of variables.
typedef std::set<const Variable*> VariableSet;

#endif  // EXPRESSION_H_
