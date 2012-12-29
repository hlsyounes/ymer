/* -*-C++-*- */
/*
 * Expressions.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Copyright (C) 2012 Google Inc
 *
 * This file is part of Ymer.
 *
 * Ymer is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Ymer is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ymer; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <config.h>
#include "rational.h"
#include <cudd.h>
#include <map>
#include <set>
#include <vector>


/* ====================================================================== */
/* Expression */

struct ValueMap;
struct SubstitutionMap;

/*
 * An abstract expression.
 */
struct Expression {
  /* Register use of the given expression. */
  static void register_use(const Expression* e) {
    if (e != NULL) {
      e->ref_count_++;
    }
  }

  /* Unregister use of the given expression. */
  static void unregister_use(const Expression* e) {
    if (e != NULL) {
      e->ref_count_--;
      if (e->ref_count_ == 0) {
	delete e;
      }
    }
  }

  /* Deletes this expression. */
  virtual ~Expression() {}

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const = 0;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const = 0;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression&
  substitution(const SubstitutionMap& subst) const = 0;

  /* Returns the `current state' MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

protected:
  /* Constructs an expression. */
  Expression() : ref_count_(0) {}

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* Computation */

/*
 * A computation expression.
 */
struct Computation : public Expression {
  /* Deletes this computation. */
  virtual ~Computation();

  /* Returns the first operand for this computation. */
  const Expression& operand1() const { return *operand1_; }

  /* Returns the second operand for this computation. */
  const Expression& operand2() const { return *operand2_; }

protected:
  /* Constructs a computation. */
  Computation(const Expression& operand1, const Expression& operand2);

private:
  /* The first operand for this computation. */
  const Expression* operand1_;
  /* The second operand for this computation. */
  const Expression* operand2_;
};


/* ====================================================================== */
/* Addition */

/*
 * An addition expression.
 */
struct Addition : public Computation {
  /* Returns an addition of the two expressions. */
  static const Expression& make(const Expression& term1,
				const Expression& term2);

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs an addition. */
  Addition(const Expression& term1, const Expression& term2)
    : Computation(term1, term2) {}
};


/* ====================================================================== */
/* Subtraction */

/*
 * A subtraction expression.
 */
struct Subtraction : public Computation {
  /* Returns a subtraction of the two expressions. */
  static const Expression& make(const Expression& term1,
				const Expression& term2);

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a subtraction. */
  Subtraction(const Expression& term1, const Expression& term2)
    : Computation(term1, term2) {}
};


/* ====================================================================== */
/* Multiplication */

/*
 * A multiplication expression.
 */
struct Multiplication : public Computation {
  /* Returns a multiplication of the two expressions. */
  static const Expression& make(const Expression& factor1,
				const Expression& factor2);

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a multiplication. */
  Multiplication(const Expression& factor1, const Expression& factor2)
    : Computation(factor1, factor2) {}
};


/* ====================================================================== */
/* Division */

/*
 * A division expression.
 */
struct Division : public Computation {
  /* Returns a division of the two expressions. */
  static const Expression& make(const Expression& factor1,
				const Expression& factor2);

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a division. */
  Division(const Expression& factor1, const Expression& factor2)
    : Computation(factor1, factor2) {}
};


/* ====================================================================== */
/* Variable */

/*
 * A variable expression.
 */
struct Variable : public Expression {
  /* Constructs a variable. */
  Variable();

  /* Constructs a variable. */
  Variable(int low, int high, int start, int low_bit);

  /* Sets the lower bound for this variable. */
  void set_low(int low);

  /* Sets the upper bound for this variable. */
  void set_high(int high);

  /* Sets the initial value for this variable. */
  void set_start(int start);

  /* Sets the index of the first DD variable used to represent this
     variable. */
  void set_low_bit(int low_bit);

  /* Returns the lower bound for this variable. */
  int low() const { return low_; }

  /* Returns the upper bound for this variable. */
  int high() const { return high_; }

  /* Returns the initial value for this variable. */
  int start() const { return start_; }

  /* Returns the index of the first DD variable used to represent this
     variable. */
  int low_bit() const { return low_bit_; }

  /* Returns the index of the last DD variable used to represent this
     variable. */
  int high_bit() const { return high_bit_; }

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Variable& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Returns the `next state' MTBDD representation for this expression. */
  DdNode* primed_mtbdd(DdManager* dd_man) const;

  /* Returns a BDD representing identity between the `current state'
     and `next state' versions of this variable. */
  DdNode* identity_bdd(DdManager* dd_man) const;

  /* Returns a BDD representing the range for this variable. */
  DdNode* range_bdd(DdManager* dd_man) const;

  /* Releases any cached DDs for this variable. */
  void uncache_dds(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The lower bound for this variable. */
  int low_;
  /* The upper bound for this variable. */
  int high_;
  /* The initial value for this variable. */
  int start_;
  /* Index of the first DD variable used to represent this variable. */
  int low_bit_;
  /* Index of the last DD variable used to represent this variable. */
  int high_bit_;
  /* Cached `current state' MTBDD representation for this variable. */
  mutable DdNode* mtbdd_;
  /* Cached `next state' MTBDD representation for this variable. */
  mutable DdNode* primed_mtbdd_;
  /* Cached BDD representing identity between the `current state' and
     `next state' versions of this variable. */
  mutable DdNode* identity_bdd_;
};


/* ====================================================================== */
/* Value */

/*
 * A value expression.
 */
struct Value : public Expression {
  /* Constructs a value. */
  Value(const Rational& value) : value_(value) {}

  /* Returns the rational value for this value. */
  const Rational& value() const { return value_; }

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Value& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Value& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The value. */
  Rational value_;
};


/* ====================================================================== */
/* VariableList */

/*
 * A list of variables.
 */
struct VariableList : public std::vector<const Variable*> {
};


/* ====================================================================== */
/* VariableSet */

/*
 * A set of variables.
 */
struct VariableSet : public std::set<const Variable*> {
};


/* ====================================================================== */
/* ValueMap */

/*
 * A mapping from variables to values.
 */
struct ValueMap : public std::map<const Variable*, Rational> {
};


/* ====================================================================== */
/* SubstitutionMap */

/*
 * A variable substitution map.
 */
struct SubstitutionMap : public std::map<const Variable*, const Variable*> {
};


#endif /* EXPRESSIONS_H */
