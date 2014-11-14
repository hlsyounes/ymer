/*
 * Formulas.
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
#include "formulas.h"
#include <stdexcept>
#include <typeinfo>


/* ====================================================================== */
/* StateFormula */

/* Output operator for state formulas. */
std::ostream& operator<<(std::ostream& os, const StateFormula& f) {
  f.print(os);
  return os;
}


/* ====================================================================== */
/* PathFormula */

/* Output operator for path formulas. */
std::ostream& operator<<(std::ostream& os, const PathFormula& f) {
  f.print(os);
  return os;
}


/* ====================================================================== */
/* Conjunction */

/* Deletes this conjunction. */
Conjunction::~Conjunction() {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    deref(*fi);
  }
}


/* Adds a conjunct to this conjunction. */
void Conjunction::add_conjunct(const StateFormula& conjunct) {
  conjuncts_.push_back(&conjunct);
  ref(&conjunct);
}


/* Tests if this state formula contains probabilistic elements. */
bool Conjunction::probabilistic() const {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    if ((*fi)->probabilistic()) {
      return true;
    }
  }
  return false;
}


/* Tests if this state formula holds in the given state. */
bool Conjunction::holds(const Values& values) const {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    if (!(*fi)->holds(values)) {
      return false;
    }
  }
  return true;
}


/* Returns this state formula subject to the given substitutions. */
const Conjunction&
Conjunction::substitution(const Substitutions& subst) const {
  Conjunction* subst_conj = new Conjunction();
  bool changed = false;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const StateFormula& f = (*fi)->substitution(subst);
    if (!changed && &f != *fi) {
      changed = true;
    }
    subst_conj->add_conjunct(f);
  }
  if (changed) {
    return *subst_conj;
  } else {
    ref(subst_conj);
    deref(subst_conj);
    return *this;
  }
}


/* Prints this object on the given stream. */
void Conjunction::print(std::ostream& os) const {
  if (conjuncts().empty()) {
    os << "true";
  } else if (conjuncts().size() == 1) {
    os << *conjuncts().front();
  } else {
    FormulaList::const_iterator fi = conjuncts().begin();
    bool par = typeid(**fi) == typeid(Disjunction);
    if (par) {
      os << '(';
    }
    os << **fi;
    if (par) {
      os << ')';
    }
    for (fi++; fi != conjuncts().end(); fi++) {
      os << " & ";
      par = typeid(**fi) == typeid(Disjunction);
      if (par) {
	os << '(';
      }
      os << **fi;
      if (par) {
	os << ')';
      }
    }
  }
}


/* ====================================================================== */
/* Disjunction */

/* Deletes this disjunction. */
Disjunction::~Disjunction() {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    deref(*fi);
  }
}


/* Adds a disjunct to this disjunction. */
void Disjunction::add_disjunct(const StateFormula& disjunct) {
  disjuncts_.push_back(&disjunct);
  ref(&disjunct);
}


/* Tests if this state formula contains probabilistic elements. */
bool Disjunction::probabilistic() const {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    if ((*fi)->probabilistic()) {
      return true;
    }
  }
  return false;
}


/* Tests if this state formula holds in the given state. */
bool Disjunction::holds(const Values& values) const {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    if ((*fi)->holds(values)) {
      return true;
    }
  }
  return false;
}


/* Returns this state formula subject to the given substitutions. */
const Disjunction&
Disjunction::substitution(const Substitutions& subst) const {
  Disjunction* subst_disj = new Disjunction();
  bool changed = false;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const StateFormula& f = (*fi)->substitution(subst);
    if (!changed && &f != *fi) {
      changed = true;
    }
    subst_disj->add_disjunct(f);
  }
  if (changed) {
    return *subst_disj;
  } else {
    ref(subst_disj);
    deref(subst_disj);
    return *this;
  }
}


/* Prints this object on the given stream. */
void Disjunction::print(std::ostream& os) const {
  if (disjuncts().empty()) {
    os << "false";
  } else if (disjuncts().size() == 1) {
    os << *disjuncts().front();
  } else {
    FormulaList::const_iterator fi = disjuncts().begin();
    bool par = typeid(**fi) == typeid(Conjunction);
    if (par) {
      os << '(';
    }
    os << **fi;
    if (par) {
      os << ')';
    }
    for (fi++; fi != disjuncts().end(); fi++) {
      os << " | ";
      par = typeid(**fi) == typeid(Conjunction);
      if (par) {
	os << '(';
      }
      os << **fi;
      if (par) {
	os << ')';
      }
    }
  }
}


/* ====================================================================== */
/* Probabilistic */

/* Constructs a probabilistic path quantification. */
Probabilistic::Probabilistic(bool negated, double threshold, bool strict,
			     const PathFormula& formula)
  : negated_(negated), threshold_(threshold), strict_(strict),
    formula_(&formula) {
  ref(formula_);
}


/* Deletes this probabilistic path quantification. */
Probabilistic::~Probabilistic() {
  deref(formula_);
}


/* Tests if this state formula contains probabilistic elements. */
bool Probabilistic::probabilistic() const {
  return true;
}


/* Tests if this state formula holds in the given state. */
bool Probabilistic::holds(const Values& values) const {
  throw std::logic_error("Probabilistic::holds not implemented");
}


/* Returns this state formula subject to the given substitutions. */
const Probabilistic&
Probabilistic::substitution(const Substitutions& subst) const {
  const PathFormula& f = formula().substitution(subst);
  if (&f != &formula()) {
    return *new Probabilistic(negated(), threshold(), strict(), f);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
void Probabilistic::print(std::ostream& os) const {
  os << 'P';
  if (negated()) {
    os << (strict() ? "<=" : "<");
  } else {
    os << (strict() ? ">" : ">=");
  }
  os << threshold() << " [ " << formula() << " ]";
}


/* ====================================================================== */
/* Comparison */

/* Constructs a comparison. */
template<typename T>
Comparison<T>::Comparison(const Expression<T>& expr1,
			  const Expression<T>& expr2, bool negated)
  : expr1_(&expr1), expr2_(&expr2), negated_(negated) {
  ref(expr1_);
  ref(expr2_);
}


/* Deletes this comparison. */
template<typename T>
Comparison<T>::~Comparison() {
  deref(expr1_);
  deref(expr2_);
}


/* Tests if this state formula contains probabilistic elements. */
template<typename T>
bool Comparison<T>::probabilistic() const {
  return false;
}


/* ====================================================================== */
/* LessThan */

/* Constructs a less-than comparison. */
template<typename T>
LessThan<T>::LessThan(const Expression<T>& expr1, const Expression<T>& expr2,
		      bool negated)
  : Comparison<T>(expr1, expr2, negated) {}


/* Tests if this state formula holds in the given state. */
template<typename T>
bool LessThan<T>::holds(const Values& values) const {
  bool result = this->expr1().value(values) < this->expr2().value(values);
  return this->negated() ? !result : result;
}


/* Returns this state formula subject to the given substitutions. */
template<typename T>
const LessThan<T>&
LessThan<T>::substitution(const Substitutions& subst) const {
  const Expression<T>& e1 = this->expr1().substitution(subst);
  const Expression<T>& e2 = this->expr2().substitution(subst);
  if (&e1 != &this->expr1() || &e2 != &this->expr2()) {
    return *new LessThan(e1, e2, this->negated());
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
template<typename T>
void LessThan<T>::print(std::ostream& os) const {
  if (this->negated()) {
    os << this->expr1() << ">=" << this->expr2();
  } else {
    os << this->expr1() << "<" << this->expr2();
  }
}


/* Explicit instantiations. */
template struct LessThan<int>;


/* ====================================================================== */
/* GreaterThan */

/* Constructs a greater-than comparison. */
template<typename T>
GreaterThan<T>::GreaterThan(const Expression<T>& expr1,
			    const Expression<T>& expr2, bool negated)
  : Comparison<T>(expr1, expr2, negated) {}


/* Tests if this state formula holds in the given state. */
template<typename T>
bool GreaterThan<T>::holds(const Values& values) const {
  bool result = this->expr1().value(values) > this->expr2().value(values);
  return this->negated() ? !result : result;
}


/* Returns this state formula subject to the given substitutions. */
template<typename T>
const GreaterThan<T>&
GreaterThan<T>::substitution(const Substitutions& subst) const {
  const Expression<T>& e1 = this->expr1().substitution(subst);
  const Expression<T>& e2 = this->expr2().substitution(subst);
  if (&e1 != &this->expr1() || &e2 != &this->expr2()) {
    return *new GreaterThan(e1, e2, this->negated());
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
template<typename T>
void GreaterThan<T>::print(std::ostream& os) const {
  if (this->negated()) {
    os << this->expr1() << "<=" << this->expr2();
  } else {
    os << this->expr1() << ">" << this->expr2();
  }
}


/* Explicit instantiations. */
template struct GreaterThan<int>;


/* ====================================================================== */
/* Equality */

/* Constructs an equality comparison. */
template<typename T>
Equality<T>::Equality(const Expression<T>& expr1, const Expression<T>& expr2,
		      bool negated)
  : Comparison<T>(expr1, expr2, negated) {}


/* Tests if this state formula holds in the given state. */
template<typename T>
bool Equality<T>::holds(const Values& values) const {
  bool result = this->expr1().value(values) == this->expr2().value(values);
  return this->negated() ? !result : result;
}


/* Returns this state formula subject to the given substitutions. */
template<typename T>
const Equality<T>&
Equality<T>::substitution(const Substitutions& subst) const {
  const Expression<T>& e1 = this->expr1().substitution(subst);
  const Expression<T>& e2 = this->expr2().substitution(subst);
  if (&e1 != &this->expr1() || &e2 != &this->expr2()) {
    return *new Equality(e1, e2, this->negated());
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
template<typename T>
void Equality<T>::print(std::ostream& os) const {
  if (this->negated()) {
    os << this->expr1() << "!=" << this->expr2();
  } else {
    os << this->expr1() << '=' << this->expr2();
  }
}


/* Explicit instantiations. */
template struct Equality<int>;


/* ====================================================================== */
/* Until */

/* Constructs an until formula. */
Until::Until(const StateFormula& pre, const StateFormula& post,
	     double min_time, double max_time)
  : pre_(&pre), post_(&post), min_time_(min_time), max_time_(max_time) {
  ref(pre_);
  ref(post_);
}


/* Deletes this until formula. */
Until::~Until() {
  deref(pre_);
  deref(post_);
}


/* Tests if this path formula contains probabilistic elements. */
bool Until::probabilistic() const {
  return pre().probabilistic() || post().probabilistic();
}


/* Returns this path formula subject to the given substitutions. */
const Until& Until::substitution(const Substitutions& subst) const {
  const StateFormula& f1 = pre().substitution(subst);
  const StateFormula& f2 = post().substitution(subst);
  if (&f1 != &pre() || &f2 != &post()) {
    return *new Until(f1, f2, min_time(), max_time());
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
void Until::print(std::ostream& os) const {
  os << pre() << " U[" << min_time() << ',' << max_time() << "] " << post();
}
