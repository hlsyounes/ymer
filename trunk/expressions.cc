/*
 * Copyright (C) 2003 Carnegie Mellon University
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
 *
 * $Id: expressions.cc,v 1.6 2003-11-12 03:39:27 lorens Exp $
 */
#include "expressions.h"
#include <stdexcept>


/* ====================================================================== */
/* Expression */

/* Output operator for expressions. */
std::ostream& operator<<(std::ostream& os, const Expression& e) {
  e.print(os);
  return os;
}


/* ====================================================================== */
/* Computation */

/* Constructs a computation. */
Computation::Computation(const Expression& operand1,
			 const Expression& operand2)
  : operand1_(&operand1), operand2_(&operand2) {
  register_use(operand1_);
  register_use(operand2_);
}


/* Deletes this computation. */
Computation::~Computation() {
  unregister_use(operand1_);
  unregister_use(operand2_);
}


/* ====================================================================== */
/* Addition */

/* Returns an addition of the two expressions. */
const Expression& Addition::make(const Expression& term1,
				 const Expression& term2) {
  const Value* v1 = dynamic_cast<const Value*>(&term1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&term2);
    if (v2 != NULL) {
      const Value& value = *new Value(v1->value() + v2->value());
      register_use(v1);
      register_use(v2);
      unregister_use(v1);
      unregister_use(v2);
      return value;
    }
  }
  return *new Addition(term1, term2);
}


/* Returns the value of this expression. */
Rational Addition::value(const ValueMap& values) const {
  return operand1().value(values) + operand2().value(values);
}


/* Returns this expression subject to the given substitutions. */
const Expression& Addition::substitution(const ValueMap& values) const {
  const Expression& e1 = operand1().substitution(values);
  const Expression& e2 = operand2().substitution(values);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this expression subject to the given substitutions. */
const Addition& Addition::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = operand1().substitution(subst);
  const Expression& e2 = operand2().substitution(subst);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return *new Addition(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' MTBDD representation for this expression. */
DdNode* Addition::mtbdd(DdManager* dd_man) const {
  DdNode* dd1 = operand1().mtbdd(dd_man);
  DdNode* dd2 = operand2().mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man, Cudd_addPlus, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man, dd1);
  Cudd_RecursiveDeref(dd_man, dd2);
  return ddc;
}


/* Prints this object on the given stream. */
void Addition::print(std::ostream& os) const {
  os << operand1() << '+' << operand2();
}


/* ====================================================================== */
/* Subtraction */

/* Returns a subtraction of the two expressions. */
const Expression& Subtraction::make(const Expression& term1,
				    const Expression& term2) {
  const Value* v1 = dynamic_cast<const Value*>(&term1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&term2);
    if (v2 != NULL) {
      const Value& value = *new Value(v1->value() - v2->value());
      register_use(v1);
      register_use(v2);
      unregister_use(v1);
      unregister_use(v2);
      return value;
    }
  }
  return *new Subtraction(term1, term2);
}


/* Returns the value of this expression. */
Rational Subtraction::value(const ValueMap& values) const {
  return operand1().value(values) - operand2().value(values);
}


/* Returns this expression subject to the given substitutions. */
const Expression& Subtraction::substitution(const ValueMap& values) const {
  const Expression& e1 = operand1().substitution(values);
  const Expression& e2 = operand2().substitution(values);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this expression subject to the given substitutions. */
const Subtraction&
Subtraction::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = operand1().substitution(subst);
  const Expression& e2 = operand2().substitution(subst);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return *new Subtraction(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' MTBDD representation for this expression. */
DdNode* Subtraction::mtbdd(DdManager* dd_man) const {
  DdNode* dd1 = operand1().mtbdd(dd_man);
  DdNode* dd2 = operand2().mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man, dd1);
  Cudd_RecursiveDeref(dd_man, dd2);
  return ddc;
}


/* Prints this object on the given stream. */
void Subtraction::print(std::ostream& os) const {
  os << operand1() << '-';
  bool par = (typeid(operand2()) == typeid(Addition)
	      || typeid(operand2()) == typeid(Subtraction));
  if (par) {
    os << '(';
  }
  os << operand2();
  if (par) {
    os << ')';
  }
}


/* ====================================================================== */
/* Multiplication */

/* Returns a multiplication of the two expressions. */
const Expression& Multiplication::make(const Expression& factor1,
				       const Expression& factor2) {
  const Value* v1 = dynamic_cast<const Value*>(&factor1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&factor2);
    if (v2 != NULL) {
      const Value& value = *new Value(v1->value() * v2->value());
      register_use(v1);
      register_use(v2);
      unregister_use(v1);
      unregister_use(v2);
      return value;
    }
  }
  return *new Multiplication(factor1, factor2);
}


/* Returns the value of this expression. */
Rational Multiplication::value(const ValueMap& values) const {
  return operand1().value(values) * operand2().value(values);
}


/* Returns this expression subject to the given substitutions. */
const Expression& Multiplication::substitution(const ValueMap& values) const {
  const Expression& e1 = operand1().substitution(values);
  const Expression& e2 = operand2().substitution(values);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this expression subject to the given substitutions. */
const Multiplication&
Multiplication::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = operand1().substitution(subst);
  const Expression& e2 = operand2().substitution(subst);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return *new Multiplication(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' MTBDD representation for this expression. */
DdNode* Multiplication::mtbdd(DdManager* dd_man) const {
  DdNode* dd1 = operand1().mtbdd(dd_man);
  DdNode* dd2 = operand2().mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man, Cudd_addTimes, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man, dd1);
  Cudd_RecursiveDeref(dd_man, dd2);
  return ddc;
}


/* Prints this object on the given stream. */
void Multiplication::print(std::ostream& os) const {
  bool par = (typeid(operand1()) == typeid(Addition)
	      || typeid(operand1()) == typeid(Subtraction));
  if (par) {
    os << '(';
  }
  os << operand1();
  if (par) {
    os << ')';
  }
  os << '*';
  par = (typeid(operand2()) == typeid(Addition)
	 || typeid(operand2()) == typeid(Subtraction));
  if (par) {
    os << '(';
  }
  os << operand2();
  if (par) {
    os << ')';
  }
}


/* ====================================================================== */
/* Division */

/* Returns a division of the two expressions. */
const Expression& Division::make(const Expression& factor1,
				 const Expression& factor2) {
  const Value* v1 = dynamic_cast<const Value*>(&factor1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&factor2);
    if (v2 != NULL) {
      if (v2->value() == 0) {
	throw std::invalid_argument("division by zero");
      }
      const Value& value = *new Value(v1->value() / v2->value());
      register_use(v1);
      register_use(v2);
      unregister_use(v1);
      unregister_use(v2);
      return value;
    }
  }
  return *new Division(factor1, factor2);
}


/* Returns the value of this expression. */
Rational Division::value(const ValueMap& values) const {
  return operand1().value(values) / operand2().value(values);
}


/* Returns this expression subject to the given substitutions. */
const Expression& Division::substitution(const ValueMap& values) const {
  const Expression& e1 = operand1().substitution(values);
  const Expression& e2 = operand2().substitution(values);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this expression subject to the given substitutions. */
const Division& Division::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = operand1().substitution(subst);
  const Expression& e2 = operand2().substitution(subst);
  if (&e1 != &operand1() || &e2 != &operand2()) {
    return *new Division(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' MTBDD representation for this expression. */
DdNode* Division::mtbdd(DdManager* dd_man) const {
  DdNode* dd1 = operand1().mtbdd(dd_man);
  DdNode* dd2 = operand2().mtbdd(dd_man);
  DdNode* ddc = Cudd_addApply(dd_man, Cudd_addDivide, dd1, dd2);
  Cudd_Ref(ddc);
  Cudd_RecursiveDeref(dd_man, dd1);
  Cudd_RecursiveDeref(dd_man, dd2);
  return ddc;
}


/* Prints this object on the given stream. */
void Division::print(std::ostream& os) const {
  bool par = (typeid(operand1()) == typeid(Addition)
	      || typeid(operand1()) == typeid(Subtraction));
  if (par) {
    os << '(';
  }
  os << operand1();
  if (par) {
    os << ')';
  }
  os << '/';
  par = (typeid(operand2()) == typeid(Addition)
	 || typeid(operand2()) == typeid(Subtraction)
	 || typeid(operand2()) == typeid(Multiplication)
	 || typeid(operand2()) == typeid(Division));
  if (par) {
    os << '(';
  }
  os << operand2();
  if (par) {
    os << ')';
  }
}


/* ====================================================================== */
/* Variable */

/* Constructs a variable. */
Variable::Variable()
  : mtbdd_(NULL), primed_mtbdd_(NULL), identity_bdd_(NULL) {}


/* Constructs a variable. */
Variable::Variable(int low, int high, int start, int low_bit)
  : low_(low), high_(high), start_(start), mtbdd_(NULL), primed_mtbdd_(NULL),
    identity_bdd_(NULL) {
  set_low_bit(low_bit);
}


/* Sets the lower bound for this variable. */
void Variable::set_low(int low) {
  low_ = low;
}


/* Sets the upper bound for this variable. */
void Variable::set_high(int high) {
  high_ = high;
}


/* Sets the initial value for this variable. */
void Variable::set_start(int start) {
  start_ = start;
}


/* Sets the index of the first DD variable used to represent this
   variable. */
void Variable::set_low_bit(int low_bit) {
  low_bit_ = low_bit;
  int h = high() - low();
  int nbits = 0;
  while (h > 0) {
    h >>= 1;
    nbits++;
  }
  high_bit_ = low_bit_ + nbits - 1;
}


/* Returns the value of this expression. */
Rational Variable::value(const ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(this);
  if (vi != values.end()) {
    return (*vi).second;
  } else {
    throw std::logic_error("unbound variable");
  }
}


/* Returns this expression subject to the given substitutions. */
const Expression& Variable::substitution(const ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(this);
  if (vi != values.end()) {
    return *new Value((*vi).second);
  } else {
    return *this;
  }
}


/* Returns this expression subject to the given substitutions. */
const Variable& Variable::substitution(const SubstitutionMap& subst) const {
  SubstitutionMap::const_iterator si = subst.find(this);
  if (si != subst.end()) {
    return *(*si).second;
  } else {
    return *this;
  }
}


/* Returns the `current state' MTBDD representation for this expression. */
DdNode* Variable::mtbdd(DdManager* dd_man) const {
  if (mtbdd_ == NULL) {
    DdNode* ddv = Cudd_ReadZero(dd_man);
    Cudd_Ref(ddv);
    for (int i = high_bit(); i >= low_bit(); i--) {
      DdNode* ddc = Cudd_addConst(dd_man, 1 << (high_bit() - i));
      Cudd_Ref(ddc);
      DdNode* ddi = Cudd_addIthVar(dd_man, 2*i);
      Cudd_Ref(ddi);
      DdNode* dd_mul = Cudd_addApply(dd_man, Cudd_addTimes, ddc, ddi);
      Cudd_Ref(dd_mul);
      Cudd_RecursiveDeref(dd_man, ddc);
      Cudd_RecursiveDeref(dd_man, ddi);
      ddi = Cudd_addApply(dd_man, Cudd_addPlus, dd_mul, ddv);
      Cudd_Ref(ddi);
      Cudd_RecursiveDeref(dd_man, dd_mul);
      Cudd_RecursiveDeref(dd_man, ddv);
      ddv = ddi;
    }
    if (low() > 0) {
      DdNode* ddl = Cudd_addConst(dd_man, low());
      Cudd_Ref(ddl);
      mtbdd_ = Cudd_addApply(dd_man, Cudd_addPlus, ddl, ddv);
      Cudd_Ref(mtbdd_);
      Cudd_RecursiveDeref(dd_man, ddl);
      Cudd_RecursiveDeref(dd_man, ddv);
    } else {
      mtbdd_ = ddv;
    }
  } else {
    Cudd_Ref(mtbdd_);
  }
  return mtbdd_;
}


/* Returns the `next state' MTBDD representation for this expression. */
DdNode* Variable::primed_mtbdd(DdManager* dd_man) const {
  if (primed_mtbdd_ == NULL) {
    DdNode* ddv = Cudd_ReadZero(dd_man);
    Cudd_Ref(ddv);
    for (int i = high_bit(); i >= low_bit(); i--) {
      DdNode* ddc = Cudd_addConst(dd_man, 1 << (high_bit() - i));
      Cudd_Ref(ddc);
      DdNode* ddi = Cudd_addIthVar(dd_man, 2*i + 1);
      Cudd_Ref(ddi);
      DdNode* dd_mul = Cudd_addApply(dd_man, Cudd_addTimes, ddc, ddi);
      Cudd_Ref(dd_mul);
      Cudd_RecursiveDeref(dd_man, ddc);
      Cudd_RecursiveDeref(dd_man, ddi);
      ddi = Cudd_addApply(dd_man, Cudd_addPlus, dd_mul, ddv);
      Cudd_Ref(ddi);
      Cudd_RecursiveDeref(dd_man, dd_mul);
      Cudd_RecursiveDeref(dd_man, ddv);
      ddv = ddi;
    }
    if (low() > 0) {
      DdNode* ddl = Cudd_addConst(dd_man, low());
      Cudd_Ref(ddl);
      primed_mtbdd_ = Cudd_addApply(dd_man, Cudd_addPlus, ddl, ddv);
      Cudd_Ref(primed_mtbdd_);
      Cudd_RecursiveDeref(dd_man, ddl);
      Cudd_RecursiveDeref(dd_man, ddv);
    } else {
      primed_mtbdd_ = ddv;
    }
  } else {
    Cudd_Ref(primed_mtbdd_);
  }
  return primed_mtbdd_;
}


/* Returns a BDD representing identity between the `current state'
   and `next state' versions of this variable. */
DdNode* Variable::identity_bdd(DdManager* dd_man) const {
  if (identity_bdd_ == NULL) {
    mtbdd(dd_man);
    primed_mtbdd(dd_man);
    DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, mtbdd_, primed_mtbdd_);
    Cudd_Ref(dde);
    Cudd_RecursiveDeref(dd_man, mtbdd_);
    Cudd_RecursiveDeref(dd_man, primed_mtbdd_);
    identity_bdd_ = Cudd_addBddInterval(dd_man, dde, 0, 0);
    Cudd_Ref(identity_bdd_);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    Cudd_Ref(identity_bdd_);
  }
  return identity_bdd_;
}


/* Returns a BDD representing the range for this variable. */
DdNode* Variable::range_bdd(DdManager* dd_man) const {
  DdNode* range;
  if (high() - low() == (1 << (high_bit() - low_bit() + 1)) - 1) {
    range = Cudd_ReadOne(dd_man);
    Cudd_Ref(range);
  } else {
    mtbdd(dd_man);
    DdNode* ddr = Cudd_addBddInterval(dd_man, mtbdd_, low(), high());
    Cudd_Ref(ddr);
    Cudd_RecursiveDeref(dd_man, mtbdd_);
    primed_mtbdd(dd_man);
    DdNode* ddp = Cudd_addBddInterval(dd_man, primed_mtbdd_, low(), high());
    Cudd_Ref(ddp);
    Cudd_RecursiveDeref(dd_man, primed_mtbdd_);
    range = Cudd_bddAnd(dd_man, ddr, ddp);
    Cudd_Ref(range);
    Cudd_RecursiveDeref(dd_man, ddr);
    Cudd_RecursiveDeref(dd_man, ddp);
  }
  return range;
}


/* Releases any cached DDs for this variable. */
void Variable::uncache_dds(DdManager* dd_man) const {
  if (mtbdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man, mtbdd_);
    mtbdd_ = NULL;
  }
  if (primed_mtbdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man, primed_mtbdd_);
    primed_mtbdd_ = NULL;
  }
  if (identity_bdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man, identity_bdd_);
    identity_bdd_ = NULL;
  }
}


/* Prints this object on the given stream. */
void Variable::print(std::ostream& os) const {
  os << 'v' << low_bit();
  if (low_bit() != high_bit()) {
    os << '_' << high_bit();
  }
}


/* ====================================================================== */
/* Value */

/* Returns the value of this expression. */
Rational Value::value(const ValueMap& values) const {
  return value();
}


/* Returns this expression subject to the given substitutions. */
const Value& Value::substitution(const ValueMap& values) const {
  return *this;
}


/* Returns this expression subject to the given substitutions. */
const Value& Value::substitution(const SubstitutionMap& subst) const {
  return *this;
}


/* Returns the `next state' MTBDD representation for this expression. */
DdNode* Value::mtbdd(DdManager* dd_man) const {
  DdNode* ddv = Cudd_addConst(dd_man, value().double_value());
  Cudd_Ref(ddv);
  return ddv;
}


/* Prints this object on the given stream. */
void Value::print(std::ostream& os) const {
  os << value();
}
