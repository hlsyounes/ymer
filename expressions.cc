/*
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
#include "expressions.h"
#include <stdexcept>
#include <typeinfo>


/* ====================================================================== */
/* Expression */

/* Output operator for expressions. */
template<typename T>
std::ostream& operator<<(std::ostream& os, const Expression<T>& e) {
  e.print(os);
  return os;
}


/* Explicit instantiations. */
template std::ostream& operator<<(std::ostream& os, const Expression<int>& e);
template std::ostream& operator<<(std::ostream& os,
                                  const Expression<double>& e);


/* ====================================================================== */
/* Computation */

/* Constructs a computation. */
template<typename T>
Computation<T>::Computation(const Expression<T>& operand1,
			    const Expression<T>& operand2)
  : operand1_(&operand1), operand2_(&operand2) {
  ref(operand1_);
  ref(operand2_);
}


/* Deletes this object. */
template<typename T>
Computation<T>::~Computation() {
  deref(operand1_);
  deref(operand2_);
}


/* Tests if this is a state-invariant expression. */
template<typename T>
bool Computation<T>::state_invariant() const {
  return operand1().state_invariant() && operand2().state_invariant();
}


/* ====================================================================== */
/* Addition */

/* Returns an addition of the two expressions. */
template<typename T>
const Expression<T>& Addition<T>::make(const Expression<T>& term1,
				       const Expression<T>& term2) {
  const Value<T>* v1 = dynamic_cast<const Value<T>*>(&term1);
  if (v1 != 0) {
    const Value<T>* v2 = dynamic_cast<const Value<T>*>(&term2);
    if (v2 != 0) {
      const Value<T>& value = *new Value<T>(v1->value() + v2->value());
      ref(v1);
      ref(v2);
      deref(v1);
      deref(v2);
      return value;
    }
  }
  return *new Addition(term1, term2);
}


/* Returns the value of this expression. */
template<typename T>
T Addition<T>::value(const Values& values) const {
  return this->operand1().value(values) + this->operand2().value(values);
}


/* Returns this expression subject to the given substitutions. */
template<typename T>
const Addition<T>&
Addition<T>::substitution(const Substitutions& subst) const {
  const Expression<T>& e1 = this->operand1().substitution(subst);
  const Expression<T>& e2 = this->operand2().substitution(subst);
  if (&e1 != &this->operand1() || &e2 != &this->operand2()) {
    return *new Addition(e1, e2);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
template<typename T>
void Addition<T>::print(std::ostream& os) const {
  os << this->operand1() << '+' << this->operand2();
}


/* Explicit instantiations. */
template struct Addition<int>;
template struct Addition<double>;


/* ====================================================================== */
/* Subtraction */

/* Returns a subtraction of the two expressions. */
template<typename T>
const Expression<T>& Subtraction<T>::make(const Expression<T>& term1,
					  const Expression<T>& term2) {
  const Value<T>* v1 = dynamic_cast<const Value<T>*>(&term1);
  if (v1 != 0) {
    const Value<T>* v2 = dynamic_cast<const Value<T>*>(&term2);
    if (v2 != 0) {
      const Value<T>& value = *new Value<T>(v1->value() - v2->value());
      ref(v1);
      ref(v2);
      deref(v1);
      deref(v2);
      return value;
    }
  }
  return *new Subtraction(term1, term2);
}


/* Returns the value of this expression. */
template<typename T>
T Subtraction<T>::value(const Values& values) const {
  return this->operand1().value(values) - this->operand2().value(values);
}


/* Returns this expression subject to the given substitutions. */
template<typename T>
const Subtraction<T>&
Subtraction<T>::substitution(const Substitutions& subst) const {
  const Expression<T>& e1 = this->operand1().substitution(subst);
  const Expression<T>& e2 = this->operand2().substitution(subst);
  if (&e1 != &this->operand1() || &e2 != &this->operand2()) {
    return *new Subtraction(e1, e2);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
template<typename T>
void Subtraction<T>::print(std::ostream& os) const {
  os << this->operand1() << '-';
  bool par = (typeid(this->operand2()) == typeid(Addition<T>)
	      || typeid(this->operand2()) == typeid(Subtraction<T>));
  if (par) {
    os << '(';
  }
  os << this->operand2();
  if (par) {
    os << ')';
  }
}


/* Explicit instantiations. */
template struct Subtraction<int>;
template struct Subtraction<double>;


/* ====================================================================== */
/* Multiplication */

/* Returns a multiplication of the two expressions. */
template<typename T>
const Expression<T>& Multiplication<T>::make(const Expression<T>& factor1,
					     const Expression<T>& factor2) {
  const Value<T>* v1 = dynamic_cast<const Value<T>*>(&factor1);
  if (v1 != 0) {
    const Value<T>* v2 = dynamic_cast<const Value<T>*>(&factor2);
    if (v2 != 0) {
      const Value<T>& value = *new Value<T>(v1->value() * v2->value());
      ref(v1);
      ref(v2);
      deref(v1);
      deref(v2);
      return value;
    }
  }
  return *new Multiplication(factor1, factor2);
}


/* Returns the value of this expression. */
template<typename T>
T Multiplication<T>::value(const Values& values) const {
  return this->operand1().value(values) * this->operand2().value(values);
}


/* Returns this expression subject to the given substitutions. */
template<typename T>
const Multiplication<T>&
Multiplication<T>::substitution(const Substitutions& subst) const {
  const Expression<T>& e1 = this->operand1().substitution(subst);
  const Expression<T>& e2 = this->operand2().substitution(subst);
  if (&e1 != &this->operand1() || &e2 != &this->operand2()) {
    return *new Multiplication(e1, e2);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
template<typename T>
void Multiplication<T>::print(std::ostream& os) const {
  bool par = (typeid(this->operand1()) == typeid(Addition<T>)
	      || typeid(this->operand1()) == typeid(Subtraction<T>));
  if (par) {
    os << '(';
  }
  os << this->operand1();
  if (par) {
    os << ')';
  }
  os << '*';
  par = (typeid(this->operand2()) == typeid(Addition<T>)
	 || typeid(this->operand2()) == typeid(Subtraction<T>));
  if (par) {
    os << '(';
  }
  os << this->operand2();
  if (par) {
    os << ')';
  }
}


/* Explicit instantiations. */
template struct Multiplication<int>;
template struct Multiplication<double>;


/* ====================================================================== */
/* Division */

/* Returns a division of the two expressions. */
const Expression<double>& Division::make(const Expression<double>& factor1,
					 const Expression<double>& factor2) {
  const Value<double>* v1 = dynamic_cast<const Value<double>*>(&factor1);
  if (v1 != 0) {
    const Value<double>* v2 = dynamic_cast<const Value<double>*>(&factor2);
    if (v2 != 0) {
      if (v2->value() == 0.0) {
	throw std::invalid_argument("division by zero");
      }
      const Value<double>& value =
	*new Value<double>(v1->value() / v2->value());
      ref(v1);
      ref(v2);
      deref(v1);
      deref(v2);
      return value;
    }
  }
  return *new Division(factor1, factor2);
}


/* Returns the value of this expression. */
double Division::value(const Values& values) const {
  return this->operand1().value(values) / this->operand2().value(values);
}


/* Returns this expression subject to the given substitutions. */
const Division& Division::substitution(const Substitutions& subst) const {
  const Expression<double>& e1 = this->operand1().substitution(subst);
  const Expression<double>& e2 = this->operand2().substitution(subst);
  if (&e1 != &this->operand1() || &e2 != &this->operand2()) {
    return *new Division(e1, e2);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
void Division::print(std::ostream& os) const {
  bool par = (typeid(this->operand1()) == typeid(Addition<double>)
	      || typeid(this->operand1()) == typeid(Subtraction<double>));
  if (par) {
    os << '(';
  }
  os << this->operand1();
  if (par) {
    os << ')';
  }
  os << '/';
  par = (typeid(this->operand2()) == typeid(Addition<double>)
	 || typeid(this->operand2()) == typeid(Subtraction<double>)
	 || typeid(this->operand2()) == typeid(Multiplication<double>)
	 || typeid(this->operand2()) == typeid(Division));
  if (par) {
    os << '(';
  }
  os << this->operand2();
  if (par) {
    os << ')';
  }
}


/* ====================================================================== */
/* Constant */

/* Returns the value of this expression. */
template<typename T>
T Constant<T>::value(const Values& values) const {
  return value();
}


/* Returns this expression subject to the given substitutions. */
template<typename T>
const Constant<T>&
Constant<T>::substitution(const Substitutions& subst) const {
  const Constant<T>* c = subst.find(*this);
  return (c != 0) ? *c : *this;
}


/* Tests if this is a state-invariant expression. */
template<typename T>
bool Constant<T>::state_invariant() const {
  return true;
}


/* Prints this object on the given stream. */
template<typename T>
void Constant<T>::print(std::ostream& os) const {
  os << name();
}


/* Explicit instantiations. */
template struct Constant<int>;
template struct Constant<double>;


/* ====================================================================== */
/* Variable */

/* Constructs a variable. */
template<typename T>
Variable<T>::Variable(const std::string& name, const Expression<T>& low,
		      const Expression<T>& high, const Expression<T>& init)
  : name_(name), low_(&low), high_(&high), init_(&init) {
  ref(low_);
  ref(high_);
  ref(init_);
}


/* Deletes this object. */
template<typename T>
Variable<T>::~Variable() {
  deref(low_);
  deref(high_);
  deref(init_);
}


/* Sets the lower bound for this variable. */
template<typename T>
void Variable<T>::set_low(const Expression<T>& low) {
  if (&low != low_) {
    ref(&low);
    deref(low_);
    low_ = &low;
  }
}


/* Sets the upper bound for this variable. */
template<typename T>
void Variable<T>::set_high(const Expression<T>& high) {
  if (&high != high_) {
    ref(&high);
    deref(high_);
    high_ = &high;
  }
}


/* Sets the initial value for this variable. */
template<typename T>
void Variable<T>::set_init(const Expression<T>& init) {
  if (&init != init_) {
    ref(&init);
    deref(init_);
    init_ = &init;
  }
}


/* Returns the value of this expression. */
template<typename T>
T Variable<T>::value(const Values& values) const {
  T value;
  if (values.find(value, *this)) {
    return value;
  } else {
    throw std::logic_error("unbound variable `" + name() + "'");
  }
}


/* Returns this expression subject to the given substitutions. */
template<typename T>
const Variable<T>&
Variable<T>::substitution(const Substitutions& subst) const {
  const Variable<T>* v = subst.find(*this);
  return (v != 0) ? *v : *this;
}


/* Tests if this is a state-invariant expression. */
template<typename T>
bool Variable<T>::state_invariant() const {
  return false;
}


/* Prints this object on the given stream. */
template<typename T>
void Variable<T>::print(std::ostream& os) const {
  os << name();
}


/* Explicit instantiations. */
template struct Variable<int>;


/* ====================================================================== */
/* Value */

/* Returns the value of this expression. */
template<typename T>
T Value<T>::value(const Values& values) const {
  return value();
}


/* Returns this expression subject to the given substitutions. */
template<typename T>
const Value<T>& Value<T>::substitution(const Substitutions& subst) const {
  return *this;
}


/* Prints this object on the given stream. */
template<typename T>
void Value<T>::print(std::ostream& os) const {
  os << value();
}


/* Tests if this is a state-invariant expression. */
template<typename T>
bool Value<T>::state_invariant() const {
  return true;
}


/* Explicit instantiations. */
template struct Value<int>;
template struct Value<double>;


/* ====================================================================== */
/* TypeCast */

/* Returns type cast of the given expression. */
template<typename From, typename To>
const Expression<To>& TypeCast<From, To>::make(const Expression<From>& expr) {
  const Value<From>* v = dynamic_cast<const Value<From>*>(&expr);
  if (v != 0) {
    const Value<To>& value = *new Value<To>(To(v->value()));
    ref(v);
    deref(v);
    return value;
  }
  return *new TypeCast(expr);
}


/* Constructs a type-cast expression. */
template<typename From, typename To>
TypeCast<From, To>::TypeCast(const Expression<From>& expr)
  : expr_(&expr) {
  ref(expr_);
}


/* Deletes this object. */
template<typename From, typename To>
TypeCast<From, To>::~TypeCast() {
  deref(expr_);
}


/* Returns the value of this expression. */
template<typename From, typename To>
To TypeCast<From, To>::value(const Values& values) const {
  return To(expr().value(values));
}


/* Returns this expression subject to the given substitutions. */
template<typename From, typename To>
const TypeCast<From, To>&
TypeCast<From, To>::substitution(const Substitutions& subst) const {
  const Expression<From>& e = expr().substitution(subst);
  if (&e != &expr()) {
    return *new TypeCast(e);
  } else {
    return *this;
  }
}


/* Tests if this is a state-invariant expression. */
template<typename From, typename To>
bool TypeCast<From, To>::state_invariant() const {
  return expr().state_invariant();
}


/* Prints this object on the given stream. */
template<typename From, typename To>
void TypeCast<From, To>::print(std::ostream& os) const {
  os << expr();
}


/* Explicit instantiations. */
template struct TypeCast<int, double>;


/* ====================================================================== */
/* Values */

/* Inserts a value for an integer-valued variable. */
void Values::insert(const Variable<int>& variable, int value) {
  int_values_.insert(std::make_pair(&variable, value));
}


/* Inserts a value for a double-valued variable. */
void Values::insert(const Variable<double>& variable, double value) {
  double_values_.insert(std::make_pair(&variable, value));
}


/* Sets the value of an integer-valued variable. */
void Values::set(const Variable<int>& variable, int value) {
  int_values_[&variable] = value;
}


/* Sets the value of a double-valued variable. */
void Values::set(const Variable<double>& variable, double value) {
  double_values_[&variable] = value;
}


/* Finds the value for an integer-valued variable. */
bool Values::find(int& value, const Variable<int>& variable) const {
  std::map<const Variable<int>*, int>::const_iterator vi =
    int_values_.find(&variable);
  if (vi != int_values_.end()) {
    value = (*vi).second;
    return true;
  } else {
    return false;
  }
}


/* Finds the value for a double-valued variable. */
bool Values::find(double& value, const Variable<double>& variable) const {
  std::map<const Variable<double>*, double>::const_iterator vi =
    double_values_.find(&variable);
  if (vi != double_values_.end()) {
    value = (*vi).second;
    return true;
  } else {
    return false;
  }
}


/* Output operator for mappings from variables to values. */
std::ostream& operator<<(std::ostream& os, const Values& v) {
  bool first = true;
  for (std::map<const Variable<int>*, int>::const_iterator vi =
	 v.int_values().begin();
       vi != v.int_values().end(); vi++) {
    if (first) {
      first = false;
    } else {
      os << " & ";
    }
    os << *(*vi).first << '=' << (*vi).second;
  }
  for (std::map<const Variable<double>*, double>::const_iterator vi =
	 v.double_values().begin();
       vi != v.double_values().end(); vi++) {
    if (first) {
      first = false;
    } else {
      os << " & ";
    }
    os << *(*vi).first << '=' << (*vi).second;
  }
  return os;
}


/* ====================================================================== */
/* Substitutions */

/* Inserts a substitution for an integer-valued constant. */
void Substitutions::insert(const Constant<int>& c1, const Constant<int>& c2) {
  int_constants_.insert(std::make_pair(&c1, &c2));
}


/* Inserts a substitution for a double-valued constant. */
void Substitutions::insert(const Constant<double>& c1,
			   const Constant<double>& c2) {
  double_constants_.insert(std::make_pair(&c1, &c2));
}


/* Inserts a substitution for an integer-valued variable. */
void Substitutions::insert(const Variable<int>& v1, const Variable<int>& v2) {
  int_variables_.insert(std::make_pair(&v1, &v2));
}


/* Inserts a substitution for a double-valued variable. */
void Substitutions::insert(const Variable<double>& v1,
			   const Variable<double>& v2) {
  double_variables_.insert(std::make_pair(&v1, &v2));
}


/* Removes all substitutions. */
void Substitutions::clear() {
  int_constants_.clear();
  double_constants_.clear();
  int_variables_.clear();
  double_variables_.clear();
}


/* Finds the substitution for an integer-valued constant. */
const Constant<int>* Substitutions::find(const Constant<int>& constant) const {
  std::map<const Constant<int>*, const Constant<int>*>::const_iterator ci =
    int_constants_.find(&constant);
  return (ci != int_constants_.end()) ? (*ci).second : 0;
}


/* Finds the substitution for a double-valued constant. */
const Constant<double>*
Substitutions::find(const Constant<double>& constant) const {
  std::map<const Constant<double>*,
    const Constant<double>*>::const_iterator ci =
    double_constants_.find(&constant);
  return (ci != double_constants_.end()) ? (*ci).second : 0;
}


/* Finds the substitution for an integer-valued variable. */
const Variable<int>* Substitutions::find(const Variable<int>& variable) const {
  std::map<const Variable<int>*, const Variable<int>*>::const_iterator vi =
    int_variables_.find(&variable);
  return (vi != int_variables_.end()) ? (*vi).second : 0;
}


/* Finds the substitution for a double-valued variable. */
const Variable<double>*
Substitutions::find(const Variable<double>& variable) const {
  std::map<const Variable<double>*,
    const Variable<double>*>::const_iterator vi =
    double_variables_.find(&variable);
  return (vi != double_variables_.end()) ? (*vi).second : 0;
}
