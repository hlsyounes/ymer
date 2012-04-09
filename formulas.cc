/*
 * Copyright (C) 2003, 2004 Carnegie Mellon University
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
 * $Id: formulas.cc,v 2.1 2004-01-25 12:23:03 lorens Exp $
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
    destructive_deref(*fi);
  }
}


/* Adds a conjunct to this conjunction. */
void Conjunction::add_conjunct(const StateFormula& conjunct) {
  if (conjunct.probabilistic()) {
    conjuncts_.push_front(&conjunct);
  } else {
    conjuncts_.push_back(&conjunct);
  }
  ref(&conjunct);
}


/* Tests if this state formula contains probabilistic elements. */
bool Conjunction::probabilistic() const {
  return !conjuncts().empty() && conjuncts().front()->probabilistic();
}


/* Tests if this state formula holds in the given state. */
bool Conjunction::holds(const ValueMap& values) const {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    if (!(*fi)->holds(values)) {
      return false;
    }
  }
  return true;
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& Conjunction::substitution(const ValueMap& values) const {
  Conjunction* subst_conj = new Conjunction();
  bool changed = false;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const StateFormula& f = (*fi)->substitution(values);
    if (!changed && &f != *fi) {
      changed = true;
    }
    subst_conj->add_conjunct(f);
  }
  if (changed) {
    return *subst_conj;
  } else {
    delete subst_conj;
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const Conjunction&
Conjunction::substitution(const SubstitutionMap& subst) const {
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
    delete subst_conj;
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* Conjunction::bdd(DdManager* dd_man) const {
  DdNode* dd = Cudd_ReadOne(dd_man);
  Cudd_Ref(dd);
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    DdNode* ddf = (*fi)->bdd(dd_man);
    DdNode* dd_tmp = Cudd_bddAnd(dd_man, ddf, dd);
    Cudd_Ref(dd_tmp);
    Cudd_RecursiveDeref(dd_man, ddf);
    Cudd_RecursiveDeref(dd_man, dd);
    dd = dd_tmp;
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* Conjunction::primed_bdd(DdManager* dd_man) const {
  DdNode* dd = Cudd_ReadOne(dd_man);
  Cudd_Ref(dd);
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    DdNode* ddf = (*fi)->primed_bdd(dd_man);
    DdNode* dd_tmp = Cudd_bddAnd(dd_man, ddf, dd);
    Cudd_Ref(dd_tmp);
    Cudd_RecursiveDeref(dd_man, ddf);
    Cudd_RecursiveDeref(dd_man, dd);
    dd = dd_tmp;
  }
  return dd;
}


/* Prints this object on the given stream. */
void Conjunction::print(std::ostream& os) const {
  if (conjuncts().empty()) {
    os << "true";
  } else if (conjuncts().size() == 1) {
    os << *conjuncts().front();
  } else {
    FormulaList::const_iterator fi = conjuncts().begin();
    bool par = (typeid(**fi) == typeid(Disjunction)
		|| typeid(**fi) == typeid(Implication));
    if (par) {
      os << '(';
    }
    os << **fi;
    if (par) {
      os << ')';
    }
    for (fi++; fi != conjuncts().end(); fi++) {
      os << " & ";
      par = (typeid(**fi) == typeid(Disjunction)
	     || typeid(**fi) == typeid(Implication));
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
    destructive_deref(*fi);
  }
}


/* Adds a disjunct to this disjunction. */
void Disjunction::add_disjunct(const StateFormula& disjunct) {
  if (disjunct.probabilistic()) {
    disjuncts_.push_front(&disjunct);
  } else {
    disjuncts_.push_back(&disjunct);
  }
  ref(&disjunct);
}


/* Tests if this state formula contains probabilistic elements. */
bool Disjunction::probabilistic() const {
  return !disjuncts().empty() && disjuncts().front()->probabilistic();
}


/* Tests if this state formula holds in the given state. */
bool Disjunction::holds(const ValueMap& values) const {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    if ((*fi)->holds(values)) {
      return true;
    }
  }
  return false;
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& Disjunction::substitution(const ValueMap& values) const {
  Disjunction* subst_disj = new Disjunction();
  bool changed = false;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const StateFormula& f = (*fi)->substitution(values);
    if (!changed && &f != *fi) {
      changed = true;
    }
    subst_disj->add_disjunct(f);
  }
  if (changed) {
    return *subst_disj;
  } else {
    delete subst_disj;
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const Disjunction&
Disjunction::substitution(const SubstitutionMap& subst) const {
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
    delete subst_disj;
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* Disjunction::bdd(DdManager* dd_man) const {
  DdNode* dd = Cudd_ReadLogicZero(dd_man);
  Cudd_Ref(dd);
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    DdNode* ddf = (*fi)->bdd(dd_man);
    DdNode* dd_tmp = Cudd_bddOr(dd_man, ddf, dd);
    Cudd_Ref(dd_tmp);
    Cudd_RecursiveDeref(dd_man, ddf);
    Cudd_RecursiveDeref(dd_man, dd);
    dd = dd_tmp;
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* Disjunction::primed_bdd(DdManager* dd_man) const {
  DdNode* dd = Cudd_ReadLogicZero(dd_man);
  Cudd_Ref(dd);
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    DdNode* ddf = (*fi)->primed_bdd(dd_man);
    DdNode* dd_tmp = Cudd_bddOr(dd_man, ddf, dd);
    Cudd_Ref(dd_tmp);
    Cudd_RecursiveDeref(dd_man, ddf);
    Cudd_RecursiveDeref(dd_man, dd);
    dd = dd_tmp;
  }
  return dd;
}


/* Prints this object on the given stream. */
void Disjunction::print(std::ostream& os) const {
  if (disjuncts().empty()) {
    os << "false";
  } else if (disjuncts().size() == 1) {
    os << *disjuncts().front();
  } else {
    FormulaList::const_iterator fi = disjuncts().begin();
    bool par = (typeid(**fi) == typeid(Conjunction)
		|| typeid(**fi) == typeid(Implication));
    if (par) {
      os << '(';
    }
    os << **fi;
    if (par) {
      os << ')';
    }
    for (fi++; fi != disjuncts().end(); fi++) {
      os << " | ";
      par = (typeid(**fi) == typeid(Conjunction)
	     || typeid(**fi) == typeid(Implication));
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
/* Negation */

/* Constructs a negation. */
Negation::Negation(const StateFormula& negand)
  : negand_(&negand) {
  ref(negand_);
}


/* Deletes this negation. */
Negation::~Negation() {
  destructive_deref(negand_);
}


/* Tests if this state formula contains probabilistic elements. */
bool Negation::probabilistic() const {
  return negand().probabilistic();
}


/* Tests if this state formula holds in the given state. */
bool Negation::holds(const ValueMap& values) const {
  return !negand().holds(values);
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& Negation::substitution(const ValueMap& values) const {
  const StateFormula& f = negand().substitution(values);
  if (&f != &negand()) {
    return *new Negation(f);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const Negation& Negation::substitution(const SubstitutionMap& subst) const {
  const StateFormula& f = negand().substitution(subst);
  if (&f != &negand()) {
    return *new Negation(f);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* Negation::bdd(DdManager* dd_man) const {
  DdNode* ddn = negand().bdd(dd_man);
  DdNode* dd = Cudd_Not(ddn);
  Cudd_Ref(dd);
  Cudd_RecursiveDeref(dd_man, ddn);
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* Negation::primed_bdd(DdManager* dd_man) const {
  DdNode* ddn = negand().primed_bdd(dd_man);
  DdNode* dd = Cudd_Not(ddn);
  Cudd_Ref(dd);
  Cudd_RecursiveDeref(dd_man, ddn);
  return dd;
}


/* Prints this object on the given stream. */
void Negation::print(std::ostream& os) const {
  os << '!';
  bool par = (typeid(negand()) == typeid(Conjunction)
	      || typeid(negand()) == typeid(Disjunction)
	      || typeid(negand()) == typeid(Implication));
  if (par) {
    os << '(';
  }
  os << negand();
  if (par) {
    os << ')';
  }
}


/* ====================================================================== */
/* Implication */

/* Constructs an implication. */
Implication::Implication(const StateFormula& antecedent,
			 const StateFormula& consequent)
  : antecedent_(&antecedent), consequent_(&consequent) {
  ref(antecedent_);
  ref(consequent_);
}


/* Deletes this implication. */
Implication::~Implication() {
  destructive_deref(antecedent_);
  destructive_deref(consequent_);
}


/* Tests if this state formula contains probabilistic elements. */
bool Implication::probabilistic() const {
  return antecedent().probabilistic() || consequent().probabilistic();
}


/* Tests if this state formula holds in the given state. */
bool Implication::holds(const ValueMap& values) const {
  return !antecedent().holds(values) || consequent().holds(values);
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& Implication::substitution(const ValueMap& values) const {
  const StateFormula& f1 = antecedent().substitution(values);
  const StateFormula& f2 = consequent().substitution(values);
  if (&f1 != &antecedent() || &f2 != &consequent()) {
    return *new Implication(f1, f2);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const Implication&
Implication::substitution(const SubstitutionMap& subst) const {
  const StateFormula& f1 = antecedent().substitution(subst);
  const StateFormula& f2 = consequent().substitution(subst);
  if (&f1 != &antecedent() || &f2 != &consequent()) {
    return *new Implication(f1, f2);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* Implication::bdd(DdManager* dd_man) const {
  DdNode* dda = antecedent().bdd(dd_man);
  DdNode* ddn = Cudd_Not(dda);
  Cudd_Ref(ddn);
  Cudd_RecursiveDeref(dd_man, dda);
  DdNode* ddc = consequent().bdd(dd_man);
  DdNode* ddi = Cudd_bddOr(dd_man, ddn, ddc);
  Cudd_Ref(ddi);
  Cudd_RecursiveDeref(dd_man, ddn);
  Cudd_RecursiveDeref(dd_man, ddc);
  return ddi;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* Implication::primed_bdd(DdManager* dd_man) const {
  DdNode* dda = antecedent().primed_bdd(dd_man);
  DdNode* ddn = Cudd_Not(dda);
  Cudd_Ref(ddn);
  Cudd_RecursiveDeref(dd_man, dda);
  DdNode* ddc = consequent().primed_bdd(dd_man);
  DdNode* ddi = Cudd_bddOr(dd_man, ddn, ddc);
  Cudd_Ref(ddi);
  Cudd_RecursiveDeref(dd_man, ddn);
  Cudd_RecursiveDeref(dd_man, ddc);
  return ddi;
}


/* Prints this object on the given stream. */
void Implication::print(std::ostream& os) const {
  os << antecedent() << " => ";
  bool par = typeid(consequent()) == typeid(Implication);
  if (par) {
    os << '(';
  }
  os << consequent();
  if (par) {
    os << ')';
  }
}


/* ====================================================================== */
/* Probabilistic */

/* Constructs a probabilistic path quantification. */
Probabilistic::Probabilistic(const Rational& threshold, bool strict,
			     const PathFormula& formula)
  : threshold_(threshold), strict_(strict), formula_(&formula) {
  PathFormula::ref(formula_);
}


/* Deletes this probabilistic path quantification. */
Probabilistic::~Probabilistic() {
  PathFormula::destructive_deref(formula_);
}


/* Tests if this state formula contains probabilistic elements. */
bool Probabilistic::probabilistic() const {
  return true;
}


/* Tests if this state formula holds in the given state. */
bool Probabilistic::holds(const ValueMap& values) const {
  throw std::logic_error("Probabilistic::holds not implemented");
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& Probabilistic::substitution(const ValueMap& values) const {
  const PathFormula& f = formula().substitution(values);
  if (&f != &formula()) {
    return *new Probabilistic(threshold(), strict(), f);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const Probabilistic&
Probabilistic::substitution(const SubstitutionMap& subst) const {
  const PathFormula& f = formula().substitution(subst);
  if (&f != &formula()) {
    return *new Probabilistic(threshold(), strict(), f);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* Probabilistic::bdd(DdManager* dd_man) const {
  throw std::logic_error("Probabilistic::bdd not implemented");
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* Probabilistic::primed_bdd(DdManager* dd_man) const {
  throw std::logic_error("Probabilistic::primed_bdd not implemented");
}


/* Prints this object on the given stream. */
void Probabilistic::print(std::ostream& os) const {
  os << 'P' << (strict() ? ">" : ">=") << threshold()
     << " [ " << formula() << " ]";
}


/* ====================================================================== */
/* Comparison */

/* Constructs a comparison. */
Comparison::Comparison(const Expression& expr1, const Expression& expr2)
  : expr1_(&expr1), expr2_(&expr2) {
  Expression::ref(expr1_);
  Expression::ref(expr2_);
}


/* Deletes this comparison. */
Comparison::~Comparison() {
  Expression::destructive_deref(expr1_);
  Expression::destructive_deref(expr2_);
}


/* Tests if this state formula contains probabilistic elements. */
bool Comparison::probabilistic() const {
  return false;
}


/* ====================================================================== */
/* LessThan */

/* Constructs a less-than comparison. */
LessThan::LessThan(const Expression& expr1, const Expression& expr2)
  : Comparison(expr1, expr2) {}


/* Tests if this state formula holds in the given state. */
bool LessThan::holds(const ValueMap& values) const {
  return expr1().value(values) < expr2().value(values);
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& LessThan::substitution(const ValueMap& values) const {
  const Expression& e1 = expr1().substitution(values);
  const Expression& e2 = expr2().substitution(values);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new LessThan(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const LessThan& LessThan::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = expr1().substitution(subst);
  const Expression& e2 = expr2().substitution(subst);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new LessThan(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* LessThan::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr1());
  if (value != NULL) {
    /* value < expr2  <==>  expr2 > value */
    double threshold = value->value().double_value();
    DdNode* dde = expr2().mtbdd(dd_man);
    dd = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr2());
    if (value != NULL) {
      /* expr1 < value  <==>  !(expr1 >= value) */
      double threshold = value->value().double_value();
      DdNode* dde = expr1().mtbdd(dd_man);
      DdNode* ddf = Cudd_addBddThreshold(dd_man, dde, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
      dd = Cudd_Not(ddf);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, ddf);
    } else {
      /* expr1 < expr2  <==>  expr2 - expr1 > 0 */
      DdNode* dd1 = expr1().mtbdd(dd_man);
      DdNode* dd2 = expr2().mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd2, dd1);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddStrictThreshold(dd_man, dde, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* LessThan::primed_bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr1());
  if (value != NULL) {
    /* value < expr2  <==>  expr2 > value */
    double threshold = value->value().double_value();
    DdNode* dde = expr2().primed_mtbdd(dd_man);
    dd = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr2());
    if (value != NULL) {
      /* expr1 < value  <==>  !(expr1 >= value) */
      double threshold = value->value().double_value();
      DdNode* dde = expr1().primed_mtbdd(dd_man);
      DdNode* ddf = Cudd_addBddThreshold(dd_man, dde, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
      dd = Cudd_Not(ddf);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, ddf);
    } else {
      /* expr1 < expr2  <==>  expr2 - expr1 > 0 */
      DdNode* dd1 = expr1().primed_mtbdd(dd_man);
      DdNode* dd2 = expr2().primed_mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd2, dd1);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddStrictThreshold(dd_man, dde, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Prints this object on the given stream. */
void LessThan::print(std::ostream& os) const {
  os << expr1() << '<' << expr2();
}


/* ====================================================================== */
/* LessThanOrEqual */

/* Constructs a less-than-or-equal comparison. */
LessThanOrEqual::LessThanOrEqual(const Expression& expr1,
				 const Expression& expr2)
  : Comparison(expr1, expr2) {}


/* Tests if this state formula holds in the given state. */
bool LessThanOrEqual::holds(const ValueMap& values) const {
  return expr1().value(values) <= expr2().value(values);
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula&
LessThanOrEqual::substitution(const ValueMap& values) const {
  const Expression& e1 = expr1().substitution(values);
  const Expression& e2 = expr2().substitution(values);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new LessThanOrEqual(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const LessThanOrEqual&
LessThanOrEqual::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = expr1().substitution(subst);
  const Expression& e2 = expr2().substitution(subst);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new LessThanOrEqual(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* LessThanOrEqual::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr1());
  if (value != NULL) {
    /* value <= expr2  <==>  expr2 >= value */
    double threshold = value->value().double_value();
    DdNode* dde = expr2().mtbdd(dd_man);
    dd = Cudd_addBddThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr2());
    if (value != NULL) {
      /* expr1 <= value  <==>  !(expr1 > value) */
      double threshold = value->value().double_value();
      DdNode* dde = expr1().mtbdd(dd_man);
      DdNode* ddf = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
      dd = Cudd_Not(ddf);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, ddf);
    } else {
      /* expr1 <= expr2  <==>  expr2 - expr1 >= 0 */
      DdNode* dd1 = expr1().mtbdd(dd_man);
      DdNode* dd2 = expr2().mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd2, dd1);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddThreshold(dd_man, dde, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* LessThanOrEqual::primed_bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr1());
  if (value != NULL) {
    /* value <= expr2  <==>  expr2 >= value */
    double threshold = value->value().double_value();
    DdNode* dde = expr2().primed_mtbdd(dd_man);
    dd = Cudd_addBddThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr2());
    if (value != NULL) {
      /* expr1 <= value  <==>  !(expr1 > value) */
      double threshold = value->value().double_value();
      DdNode* dde = expr1().primed_mtbdd(dd_man);
      DdNode* ddf = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
      dd = Cudd_Not(ddf);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, ddf);
    } else {
      /* expr1 <= expr2  <==>  expr2 - expr1 >= 0 */
      DdNode* dd1 = expr1().primed_mtbdd(dd_man);
      DdNode* dd2 = expr2().primed_mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd2, dd1);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddThreshold(dd_man, dde, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Prints this object on the given stream. */
void LessThanOrEqual::print(std::ostream& os) const {
  os << expr1() << "<="<< expr2();
}


/* ====================================================================== */
/* GreaterThanOrEqual */

/* Constructs a greater-than-or-equal comparison. */
GreaterThanOrEqual::GreaterThanOrEqual(const Expression& expr1,
				       const Expression& expr2)
  : Comparison(expr1, expr2) {}


/* Tests if this state formula holds in the given state. */
bool GreaterThanOrEqual::holds(const ValueMap& values) const {
  return expr1().value(values) >= expr2().value(values);
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula&
GreaterThanOrEqual::substitution(const ValueMap& values) const {
  const Expression& e1 = expr1().substitution(values);
  const Expression& e2 = expr2().substitution(values);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new GreaterThanOrEqual(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const GreaterThanOrEqual&
GreaterThanOrEqual::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = expr1().substitution(subst);
  const Expression& e2 = expr2().substitution(subst);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new GreaterThanOrEqual(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* GreaterThanOrEqual::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr2());
  if (value != NULL) {
    /* expr1 >= value */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().mtbdd(dd_man);
    dd = Cudd_addBddThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr1());
    if (value != NULL) {
      /* value >= expr2  <==>  !(expr2 > value) */
      double threshold = value->value().double_value();
      DdNode* dde = expr2().mtbdd(dd_man);
      DdNode* ddf = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
      dd = Cudd_Not(ddf);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, ddf);
    } else {
      /* expr1 >= expr2  <==>  expr1 - expr2 >= 0 */
      DdNode* dd1 = expr1().mtbdd(dd_man);
      DdNode* dd2 = expr2().mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddThreshold(dd_man, dde, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* GreaterThanOrEqual::primed_bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr2());
  if (value != NULL) {
    /* expr1 >= value */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().primed_mtbdd(dd_man);
    dd = Cudd_addBddThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr1());
    if (value != NULL) {
      /* value >= expr2  <==>  !(expr2 > value) */
      double threshold = value->value().double_value();
      DdNode* dde = expr2().primed_mtbdd(dd_man);
      DdNode* ddf = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
      dd = Cudd_Not(ddf);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, ddf);
    } else {
      /* expr1 >= expr2  <==>  expr1 - expr2 >= 0 */
      DdNode* dd1 = expr1().primed_mtbdd(dd_man);
      DdNode* dd2 = expr2().primed_mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddThreshold(dd_man, dde, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Prints this object on the given stream. */
void GreaterThanOrEqual::print(std::ostream& os) const {
  os << expr1() << ">=" << expr2();
}


/* ====================================================================== */
/* GreaterThan */

/* Constructs a greater-than comparison. */
GreaterThan::GreaterThan(const Expression& expr1, const Expression& expr2)
  : Comparison(expr1, expr2) {}


/* Tests if this state formula holds in the given state. */
bool GreaterThan::holds(const ValueMap& values) const {
  return expr1().value(values) > expr2().value(values);
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& GreaterThan::substitution(const ValueMap& values) const {
  const Expression& e1 = expr1().substitution(values);
  const Expression& e2 = expr2().substitution(values);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new GreaterThan(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const GreaterThan&
GreaterThan::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = expr1().substitution(subst);
  const Expression& e2 = expr2().substitution(subst);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new GreaterThan(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* GreaterThan::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr2());
  if (value != NULL) {
    /* expr1 > value */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().mtbdd(dd_man);
    dd = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr1());
    if (value != NULL) {
      /* value > expr2  <==>  !(expr2 >= value) */
      double threshold = value->value().double_value();
      DdNode* dde = expr2().mtbdd(dd_man);
      DdNode* ddf = Cudd_addBddThreshold(dd_man, dde, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
      dd = Cudd_Not(ddf);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, ddf);
    } else {
      /* expr1 > expr2  <==>  expr1 - expr2 > 0 */
      DdNode* dd1 = expr1().mtbdd(dd_man);
      DdNode* dd2 = expr2().mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddStrictThreshold(dd_man, dde, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* GreaterThan::primed_bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr2());
  if (value != NULL) {
    /* expr1 > value */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().primed_mtbdd(dd_man);
    dd = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr1());
    if (value != NULL) {
      /* value > expr2  <==>  !(expr2 >= value) */
      double threshold = value->value().double_value();
      DdNode* dde = expr2().primed_mtbdd(dd_man);
      DdNode* ddf = Cudd_addBddThreshold(dd_man, dde, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
      dd = Cudd_Not(ddf);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, ddf);
    } else {
      /* expr1 > expr2  <==>  expr1 - expr2 > 0 */
      DdNode* dd1 = expr1().primed_mtbdd(dd_man);
      DdNode* dd2 = expr2().primed_mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddStrictThreshold(dd_man, dde, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Prints this object on the given stream. */
void GreaterThan::print(std::ostream& os) const {
  os << expr1() << '>' << expr2();
}


/* ====================================================================== */
/* Equality */

/* Constructs an equality comparison. */
Equality::Equality(const Expression& expr1, const Expression& expr2)
  : Comparison(expr1, expr2) {}


/* Tests if this state formula holds in the given state. */
bool Equality::holds(const ValueMap& values) const {
  return expr1().value(values) == expr2().value(values);
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& Equality::substitution(const ValueMap& values) const {
  const Expression& e1 = expr1().substitution(values);
  const Expression& e2 = expr2().substitution(values);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new Equality(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const Equality& Equality::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = expr1().substitution(subst);
  const Expression& e2 = expr2().substitution(subst);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new Equality(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* Equality::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr2());
  if (value != NULL) {
    /* expr1 == value  <==>  expr1 in [value,value] */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().mtbdd(dd_man);
    dd = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr1());
    if (value != NULL) {
      /* value == expr2  <==>  expr2 in [value,value] */
      double threshold = value->value().double_value();
      DdNode* dde = expr2().mtbdd(dd_man);
      dd = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    } else {
      /* expr1 == expr2  <==>  expr1 - expr2 in [0,0] */
      DdNode* dd1 = expr1().mtbdd(dd_man);
      DdNode* dd2 = expr2().mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddInterval(dd_man, dde, 0, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* Equality::primed_bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Literal* value = dynamic_cast<const Literal*>(&expr2());
  if (value != NULL) {
    /* expr1 == value  <==>  expr1 in [value,value] */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().primed_mtbdd(dd_man);
    dd = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr1());
    if (value != NULL) {
      /* value == expr2  <==>  expr2 in [value,value] */
      double threshold = value->value().double_value();
      DdNode* dde = expr2().primed_mtbdd(dd_man);
      dd = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    } else {
      /* expr1 == expr2  <==>  expr1 - expr2 in [0,0] */
      DdNode* dd1 = expr1().primed_mtbdd(dd_man);
      DdNode* dd2 = expr2().primed_mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      dd = Cudd_addBddInterval(dd_man, dde, 0, 0);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  return dd;
}


/* Prints this object on the given stream. */
void Equality::print(std::ostream& os) const {
  os << expr1() << '=' << expr2();
}


/* ====================================================================== */
/* Inequality */

/* Constructs an inequality comparison. */
Inequality::Inequality(const Expression& expr1, const Expression& expr2)
  : Comparison(expr1, expr2) {}


/* Tests if this state formula holds in the given state. */
bool Inequality::holds(const ValueMap& values) const {
  return expr1().value(values) != expr2().value(values);
}


/* Returns this state formula subject to the given substitutions. */
const StateFormula& Inequality::substitution(const ValueMap& values) const {
  const Expression& e1 = expr1().substitution(values);
  const Expression& e2 = expr2().substitution(values);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new Inequality(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this state formula subject to the given substitutions. */
const Inequality&
Inequality::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = expr1().substitution(subst);
  const Expression& e2 = expr2().substitution(subst);
  if (&e1 != &expr1() || &e2 != &expr2()) {
    return *new Inequality(e1, e2);
  } else {
    return *this;
  }
}


/* Returns the `current state' BDD representation for this state formula. */
DdNode* Inequality::bdd(DdManager* dd_man) const {
  const Literal* value = dynamic_cast<const Literal*>(&expr2());
  DdNode* ddf;
  if (value != NULL) {
    /* expr1 != value  <==>  !(expr1 in [value,value]) */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().mtbdd(dd_man);
    ddf = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
    Cudd_Ref(ddf);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr1());
    if (value != NULL) {
      /* value != expr2  <==>  !(expr2 in [value,value]) */
      double threshold = value->value().double_value();
      DdNode* dde = expr2().mtbdd(dd_man);
      ddf = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
    } else {
      /* expr1 != expr2  <==>  !(expr1 - expr2 in [0,0]) */
      DdNode* dd1 = expr1().mtbdd(dd_man);
      DdNode* dd2 = expr2().mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      ddf = Cudd_addBddInterval(dd_man, dde, 0, 0);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  DdNode* dd = Cudd_Not(ddf);
  Cudd_Ref(dd);
  Cudd_RecursiveDeref(dd_man, ddf);
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
DdNode* Inequality::primed_bdd(DdManager* dd_man) const {
  const Literal* value = dynamic_cast<const Literal*>(&expr2());
  DdNode* ddf;
  if (value != NULL) {
    /* expr1 != value  <==>  !(expr1 in [value,value]) */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().primed_mtbdd(dd_man);
    ddf = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
    Cudd_Ref(ddf);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Literal*>(&expr1());
    if (value != NULL) {
      /* value != expr2  <==>  !(expr2 in [value,value]) */
      double threshold = value->value().double_value();
      DdNode* dde = expr2().primed_mtbdd(dd_man);
      ddf = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
    } else {
      /* expr1 != expr2  <==>  !(expr1 - expr2 in [0,0]) */
      DdNode* dd1 = expr1().primed_mtbdd(dd_man);
      DdNode* dd2 = expr2().primed_mtbdd(dd_man);
      DdNode* dde = Cudd_addApply(dd_man, Cudd_addMinus, dd1, dd2);
      Cudd_Ref(dde);
      Cudd_RecursiveDeref(dd_man, dd1);
      Cudd_RecursiveDeref(dd_man, dd2);
      ddf = Cudd_addBddInterval(dd_man, dde, 0, 0);
      Cudd_Ref(ddf);
      Cudd_RecursiveDeref(dd_man, dde);
    }
  }
  DdNode* dd = Cudd_Not(ddf);
  Cudd_Ref(dd);
  Cudd_RecursiveDeref(dd_man, ddf);
  return dd;
}


/* Prints this object on the given stream. */
void Inequality::print(std::ostream& os) const {
  os << expr1() << "!=" << expr2();
}


/* ====================================================================== */
/* Until */

/* Constructs an until formula. */
Until::Until(const StateFormula& pre, const StateFormula& post,
	     const Rational& min_time, const Rational& max_time)
  : pre_(&pre), post_(&post), min_time_(min_time), max_time_(max_time) {
  StateFormula::ref(pre_);
  StateFormula::ref(post_);
}


/* Deletes this until formula. */
Until::~Until() {
  StateFormula::destructive_deref(pre_);
  StateFormula::destructive_deref(post_);
}


/* Tests if this path formula contains probabilistic elements. */
bool Until::probabilistic() const {
  return pre().probabilistic() || post().probabilistic();
}


/* Returns this path formula subject to the given substitutions. */
const PathFormula& Until::substitution(const ValueMap& values) const {
  const StateFormula& f1 = pre().substitution(values);
  const StateFormula& f2 = post().substitution(values);
  if (&f1 != &pre() || &f2 != &post()) {
    return *new Until(f1, f2, min_time(), max_time());
  } else {
    return *this;
  }
}


/* Returns this path formula subject to the given substitutions. */
const Until& Until::substitution(const SubstitutionMap& subst) const {
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
