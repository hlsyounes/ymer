/* -*-C++-*- */
/*
 * Expressions.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011 Google Inc
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
#include "refcount.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>


/* ====================================================================== */
/* Expression */

struct Values;
struct Substitutions;
template<typename T> struct Expression;

/* Output operator for expressions. */
template<typename T>
std::ostream& operator<<(std::ostream& os, const Expression<T>& e);

/*
 * An abstract expression.
 */
template<typename T>
struct Expression : public RCObject {
  /* Returns the value of this expression. */
  virtual T value(const Values& values) const = 0;

  /* Returns this expression subject to the given substitutions. */
  virtual const Expression<T>&
  substitution(const Substitutions& subst) const = 0;

  /* Tests if this is a state-invariant expression. */
  virtual bool state_invariant() const = 0;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<< <> (std::ostream& os,
				      const Expression<T>& e);
};


/* ====================================================================== */
/* Computation */

/*
 * A computation expression.
 */
template<typename T>
struct Computation : public Expression<T> {
  /* Deletes this object. */
  virtual ~Computation();

  /* Returns the first operand for this computation. */
  const Expression<T>& operand1() const { return *operand1_; }

  /* Returns the second operand for this computation. */
  const Expression<T>& operand2() const { return *operand2_; }

  /* Tests if this is a state-invariant expression. */
  virtual bool state_invariant() const;

protected:
  /* Constructs a computation. */
  Computation(const Expression<T>& operand1, const Expression<T>& operand2);

private:
  /* The first operand for this computation. */
  const Expression<T>* operand1_;
  /* The second operand for this computation. */
  const Expression<T>* operand2_;
};


/* ====================================================================== */
/* Addition */

/*
 * An addition expression.
 */
template<typename T>
struct Addition : public Computation<T> {
  /* Returns an addition of the two expressions. */
  static const Expression<T>& make(const Expression<T>& term1,
				   const Expression<T>& term2);

  /* Returns the value of this expression. */
  virtual T value(const Values& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Addition<T>& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs an addition. */
  Addition(const Expression<T>& term1, const Expression<T>& term2)
    : Computation<T>(term1, term2) {}
};


/* ====================================================================== */
/* Subtraction */

/*
 * A subtraction expression.
 */
template<typename T>
struct Subtraction : public Computation<T> {
  /* Returns a subtraction of the two expressions. */
  static const Expression<T>& make(const Expression<T>& term1,
				   const Expression<T>& term2);

  /* Returns the value of this expression. */
  virtual T value(const Values& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Subtraction<T>& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a subtraction. */
  Subtraction(const Expression<T>& term1, const Expression<T>& term2)
    : Computation<T>(term1, term2) {}
};


/* ====================================================================== */
/* Multiplication */

/*
 * A multiplication expression.
 */
template<typename T>
struct Multiplication : public Computation<T> {
  /* Returns a multiplication of the two expressions. */
  static const Expression<T>& make(const Expression<T>& factor1,
				   const Expression<T>& factor2);

  /* Returns the value of this expression. */
  virtual T value(const Values& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Multiplication<T>&
  substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a multiplication. */
  Multiplication(const Expression<T>& factor1, const Expression<T>& factor2)
    : Computation<T>(factor1, factor2) {}
};


/* ====================================================================== */
/* Division */

/*
 * A division expression.
 */
struct Division : public Computation<double> {
  /* Returns a division of the two expressions. */
  static const Expression<double>& make(const Expression<double>& factor1,
					const Expression<double>& factor2);

  /* Returns the value of this expression. */
  virtual double value(const Values& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Division& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a division. */
  Division(const Expression<double>& factor1,
	   const Expression<double>& factor2)
    : Computation<double>(factor1, factor2) {}
};


/* ====================================================================== */
/* Constant */

/*
 * A constant expression.
 */
template<typename T>
struct Constant : public Expression<T> {
  /* Constructs a constant. */
  Constant(const std::string& name, const T& value)
    : name_(name), value_(value) {}

  /* Returns the name for this constant. */
  const std::string& name() const { return name_; }

  /* Returns the value of this constant. */
  const T& value() const { return value_; }

  /* Returns the value of this expression. */
  virtual T value(const Values& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Constant<T>& substitution(const Substitutions& subst) const;

  /* Tests if this is a state-invariant expression. */
  virtual bool state_invariant() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The name for this constant. */
  std::string name_;
  /* The value of this constant. */
  T value_;
};


/* ====================================================================== */
/* Variable */

/*
 * A variable expression.
 */
template<typename T>
struct Variable : public Expression<T> {
  /* Constructs a variable. */
  Variable(const std::string& name, const Expression<T>& low,
	   const Expression<T>& high, const Expression<T>& init);

  /* Deletes this object. */
  virtual ~Variable();

  /* Sets the lower bound for this variable. */
  void set_low(const Expression<T>& low);

  /* Sets the upper bound for this variable. */
  void set_high(const Expression<T>& high);

  /* Sets the initial value for this variable. */
  void set_init(const Expression<T>& init);

  /* Returns the name for this variable. */
  const std::string& name() const { return name_; }

  /* Returns the lower bound for this variable. */
  const Expression<T>& low() const { return *low_; }

  /* Returns the upper bound for this variable. */
  const Expression<T>& high() const { return *high_; }

  /* Returns the initial value for this variable. */
  const Expression<T>& init() const { return *init_; }

  /* Returns the value of this expression. */
  virtual T value(const Values& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Variable<T>& substitution(const Substitutions& subst) const;

  /* Tests if this is a state-invariant expression. */
  virtual bool state_invariant() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The name for this variable. */
  std::string name_;
  /* The lower bound for this variable. */
  const Expression<T>* low_;
  /* The upper bound for this variable. */
  const Expression<T>* high_;
  /* The initial value for this variable. */
  const Expression<T>* init_;
};


/* ====================================================================== */
/* Value */

/*
 * A value expression.
 */
template<typename T>
struct Value : public Expression<T> {
  /* Constructs a value. */
  explicit Value(const T& value) : value_(value) {}

  /* Returns the value for this value expression. */
  const T& value() const { return value_; }

  /* Returns the value of this expression. */
  virtual T value(const Values& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const Value<T>& substitution(const Substitutions& subst) const;

  /* Tests if this is a state-invariant expression. */
  virtual bool state_invariant() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The value. */
  T value_;
};


/* ====================================================================== */
/* TypeCast */

/*
 * A type-cast expression.
 */
template<typename From, typename To>
struct TypeCast : public Expression<To> {
  /* Returns type cast of the given expression. */
  static const Expression<To>& make(const Expression<From>& expr);

  /* Deletes this object. */
  virtual ~TypeCast();

  /* Returns the original-type expression. */
  const Expression<From>& expr() const { return *expr_; }

  /* Returns the value of this expression. */
  virtual To value(const Values& values) const;

  /* Returns this expression subject to the given substitutions. */
  virtual const TypeCast<From, To>&
  substitution(const Substitutions& subst) const;

  /* Tests if this is a state-invariant expression. */
  virtual bool state_invariant() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The original-type expression. */
  const Expression<From>* expr_;

  /* Constructs a type-cast expression. */
  explicit TypeCast(const Expression<From>& expr);
};


/* ====================================================================== */
/* Values */

/*
 * A mapping from variables to values.
 */
struct Values {
  /* Inserts a value for an integer-valued variable. */
  void insert(const Variable<int>& variable, int value);

  /* Inserts a value for a double-valued variable. */
  void insert(const Variable<double>& variable, double value);

  /* Sets the value of an integer-valued variable. */
  void set(const Variable<int>& variable, int value);

  /* Sets the value of a double-valued variable. */
  void set(const Variable<double>& variable, double value);

  /* Finds the value for an integer-valued variable. */
  bool find(int& value, const Variable<int>& variable) const;

  /* Finds the value for a double-valued variable. */
  bool find(double& value, const Variable<double>& variable) const;

  /* Returns the values for integer-valued variables. */
  const std::map<const Variable<int>*, int>& int_values() const {
    return int_values_;
  }

  /* Returns the values for double-valued variables. */
  const std::map<const Variable<double>*, double>& double_values() const {
    return double_values_;
  }

  /* Returns the total number of variables. */
  int size() const { return int_values().size() + double_values().size(); }

private:
  /* Values for integer-valued variables. */
  std::map<const Variable<int>*, int> int_values_;
  /* Values for double-valued variables. */
  std::map<const Variable<double>*, double> double_values_;
};

/* Less-than operator for mappings from variables to values. */
inline bool operator<(const Values& v1, const Values& v2) {
  std::map<const Variable<int>*, int>::const_iterator vi =
    v1.int_values().begin();
  std::map<const Variable<int>*, int>::const_iterator vj =
    v2.int_values().begin();
  while (vi != v1.int_values().end() && vj != v2.int_values().end()) {
    if ((*vi).first < (*vj).first) {
      return true;
    } else if ((*vj).first < (*vi).first) {
      return false;
    } else if ((*vi).second < (*vj).second) {
      return true;
    } else if ((*vj).second < (*vi).second) {
      return false;
    }
    vi++;
    vj++;
  }
  if (vi != v1.int_values().end()) {
    return true;
  } else if (vj != v2.int_values().end()) {
    return false;
  } else {
    std::map<const Variable<double>*, double>::const_iterator xi =
      v1.double_values().begin();
    std::map<const Variable<double>*, double>::const_iterator xj =
      v2.double_values().begin();
    while (xi != v1.double_values().end() && xj != v2.double_values().end()) {
      if ((*xi).first < (*xj).first) {
	return true;
      } else if ((*xj).first < (*xi).first) {
	return false;
      } else if ((*xi).second < (*xj).second) {
	return true;
      } else if ((*xj).second < (*xi).second) {
	return false;
      }
      xi++;
      xj++;
    }
    return xi != v1.double_values().end();
  }
}

/* Output operator for mappings from variables to values. */
std::ostream& operator<<(std::ostream& os, const Values& v);


/* ====================================================================== */
/* Substitutions */

/*
 * A substitution map for constants and variables.
 */
struct Substitutions {
  /* Inserts a substitution for an integer-valued constant. */
  void insert(const Constant<int>& c1, const Constant<int>& c2);

  /* Inserts a substitution for a double-valued constant. */
  void insert(const Constant<double>& c1, const Constant<double>& c2);

  /* Inserts a substitution for an integer-valued variable. */
  void insert(const Variable<int>& v1, const Variable<int>& v2);

  /* Inserts a substitution for a double-valued variable. */
  void insert(const Variable<double>& v1, const Variable<double>& v2);

  /* Removes all substitutions. */
  void clear();

  /* Finds the substitution for an integer-valued constant. */
  const Constant<int>* find(const Constant<int>& constant) const;

  /* Finds the substitution for a double-valued constant. */
  const Constant<double>* find(const Constant<double>& constant) const;

  /* Finds the substitution for an integer-valued variable. */
  const Variable<int>* find(const Variable<int>& variable) const;

  /* Finds the substitution for a double-valued variable. */
  const Variable<double>* find(const Variable<double>& variable) const;

  /* Returns the substitutions for integer-valued constants. */
  const std::map<const Constant<int>*, const Constant<int>*>&
  int_constants() const {
    return int_constants_;
  }

  /* Returns the substitutions for double-valued constants. */
  const std::map<const Constant<double>*, const Constant<double>*>&
  double_constants() const {
    return double_constants_;
  }

  /* Returns the substitutions for integer-valued variables. */
  const std::map<const Variable<int>*, const Variable<int>*>&
  int_variables() const {
    return int_variables_;
  }

  /* Returns the substitutions for double-valued variables. */
  const std::map<const Variable<double>*, const Variable<double>*>&
  double_variables() const {
    return double_variables_;
  }

private:
  /* Substitutions for integer-valued constants. */
  std::map<const Constant<int>*, const Constant<int>*> int_constants_;
  /* Substitutions for double-valued constants. */
  std::map<const Constant<double>*, const Constant<double>*> double_constants_;
  /* Substitutions for integer-valued variables. */
  std::map<const Variable<int>*, const Variable<int>*> int_variables_;
  /* Substitutions for double-valued variables. */
  std::map<const Variable<double>*, const Variable<double>*> double_variables_;
};


#endif /* EXPRESSIONS_H */
