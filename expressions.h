/* -*-C++-*- */
/*
 * Expressions.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: expressions.h,v 1.1 2003-08-10 01:52:30 lorens Exp $
 */
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <config.h>
#include "rational.h"
#include "hashing.h"
#include <vector>

struct DdManager;
struct DdNode;


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

  /* Returns an MTBDD representation for this expression. */
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
  /* Constructs an addition. */
  Addition(const Expression& term1, const Expression& term2);

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Addition& substitution(const SubstitutionMap& subst) const;

  /* Returns an MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Subtraction */

/*
 * A subtraction expression.
 */
struct Subtraction : public Computation {
  /* Constructs a subtraction. */
  Subtraction(const Expression& term1, const Expression& term2);

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Subtraction& substitution(const SubstitutionMap& subst) const;

  /* Returns an MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Multiplication */

/*
 * A multiplication expression.
 */
struct Multiplication : public Computation {
  /* Constructs a multiplication. */
  Multiplication(const Expression& factor1, const Expression& factor2);

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Multiplication&
  substitution(const SubstitutionMap& subst) const;

  /* Returns an MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Division */

/*
 * A division expression.
 */
struct Division : public Computation {
  /* Constructs a division. */
  Division(const Expression& factor1, const Expression& factor2);

  /* Returns the value of this expression. */
  virtual Rational value(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Division& substitution(const SubstitutionMap& subst) const;

  /* Returns an MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
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

  /* Returns an MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Returns an MTBDD representation for the primed version of this
     variable. */
  DdNode* primed_mtbdd(DdManager* dd_man) const;

  /* Returns a BDD representing identity between this variable and the
     primed version of this variable. */
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
  /* Index of the first DD variable used to represent this
     variable. */
  int low_bit_;
  /* Index of the last DD variable used to represent this variable. */
  int high_bit_;
  /* Cached MTBDD representation for this variable. */
  mutable DdNode* mtbdd_;
  /* Cached MTBDD representation for the primed version of this
     variable. */
  mutable DdNode* primed_mtbdd_;
  /* Cached BDD representing identity between this variable and the
     primed version of this variable. */
  mutable DdNode* identity_bdd_;
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
struct VariableSet : public hashing::hash_set<const Variable*> {
};


/* ====================================================================== */
/* ValueMap */

/*
 * A mapping from variables to values.
 */
struct ValueMap : public hashing::hash_map<const Variable*, Rational> {
};


/* ====================================================================== */
/* SubstitutionMap */

/*
 * A variable substitution map.
 */
struct SubstitutionMap
  : public hashing::hash_map<const Variable*, const Variable*> {
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
  virtual const Expression& substitution(const ValueMap& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Value& substitution(const SubstitutionMap& subst) const;

  /* Returns an MTBDD representation for this expression. */
  virtual DdNode* mtbdd(DdManager* dd_man) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The value. */
  Rational value_;
};


#endif /* EXPRESSIONS_H */
