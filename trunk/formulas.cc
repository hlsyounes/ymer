/*
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
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
 * $Id: formulas.cc,v 1.2 2003-08-10 19:44:34 lorens Exp $
 */
#include "formulas.h"
#include "exceptions.h"


/* ====================================================================== */
/* Conjunction */

/* Deletes this conjunction. */
Conjunction::~Conjunction() {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    unregister_use(*fi);
  }
}


/* Adds a conjunct to this conjunction. */
void Conjunction::add_conjunct(const StateFormula& conjunct) {
  if (conjunct.probabilistic()) {
    conjuncts_.push_front(&conjunct);
  } else {
    conjuncts_.push_back(&conjunct);
  }
  register_use(&conjunct);
}


/* Tests if this state formula contains probabilistic elements. */
bool Conjunction::probabilistic() const {
  return !conjuncts().empty() && conjuncts().front()->probabilistic();
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


/* Returns a BDD representation for this state formula. */
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


/* Prints this object on the given stream. */
void Conjunction::print(std::ostream& os) const {
  if (conjuncts().empty()) {
    os << "true";
  } else if (conjuncts().size() == 1) {
    conjuncts().front()->print(os);
  } else {
    os << '(';
    FormulaList::const_iterator fi = conjuncts().begin();
    (*fi)->print(os);
    for (fi++; fi != conjuncts().end(); fi++) {
      os << " & ";
      (*fi)->print(os);
    }
    os << ')';
  }
}


/* ====================================================================== */
/* Disjunction */

/* Deletes this disjunction. */
Disjunction::~Disjunction() {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    unregister_use(*fi);
  }
}


/* Adds a disjunct to this disjunction. */
void Disjunction::add_disjunct(const StateFormula& disjunct) {
  if (disjunct.probabilistic()) {
    disjuncts_.push_front(&disjunct);
  } else {
    disjuncts_.push_back(&disjunct);
  }
  register_use(&disjunct);
}


/* Tests if this state formula contains probabilistic elements. */
bool Disjunction::probabilistic() const {
  return !disjuncts().empty() && disjuncts().front()->probabilistic();
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


/* Returns a BDD representation for this state formula. */
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


/* Prints this object on the given stream. */
void Disjunction::print(std::ostream& os) const {
  if (disjuncts().empty()) {
    os << "false";
  } else if (disjuncts().size() == 1) {
    disjuncts().front()->print(os);
  } else {
    os << '(';
    FormulaList::const_iterator fi = disjuncts().begin();
    (*fi)->print(os);
    for (fi++; fi != disjuncts().end(); fi++) {
      os << " | ";
      (*fi)->print(os);
    }
    os << ')';
  }
}


/* ====================================================================== */
/* Negation */

/* Constructs a negation. */
Negation::Negation(const StateFormula& negand)
  : negand_(&negand) {
  register_use(negand_);
}


/* Deletes this negation. */
Negation::~Negation() {
  unregister_use(negand_);
}


/* Tests if this state formula contains probabilistic elements. */
bool Negation::probabilistic() const {
  return negand().probabilistic();
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


/* Returns a BDD representation for this state formula. */
DdNode* Negation::bdd(DdManager* dd_man) const {
  DdNode* ddn = negand().bdd(dd_man);
  DdNode* dd = Cudd_Not(ddn);
  Cudd_Ref(dd);
  Cudd_RecursiveDeref(dd_man, ddn);
  return dd;
}


/* Prints this object on the given stream. */
void Negation::print(std::ostream& os) const {
  os << '!';
  negand().print(os);
}


/* ====================================================================== */
/* Implication */

/* Constructs an implication. */
Implication::Implication(const StateFormula& antecedent,
			 const StateFormula& consequent)
  : antecedent_(&antecedent), consequent_(&consequent) {
  register_use(antecedent_);
  register_use(consequent_);
}


/* Deletes this implication. */
Implication::~Implication() {
  unregister_use(antecedent_);
  unregister_use(consequent_);
}


/* Tests if this state formula contains probabilistic elements. */
bool Implication::probabilistic() const {
  return antecedent().probabilistic() || consequent().probabilistic();
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


/* Returns a BDD representation for this state formula. */
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


/* Prints this object on the given stream. */
void Implication::print(std::ostream& os) const {
  os << '(';
  antecedent().print(os);
  os << " => ";
  consequent().print(os);
  os << ')';
}


/* ====================================================================== */
/* Probabilistic */

/* Constructs a probabilistic path quantification. */
Probabilistic::Probabilistic(const Rational& threshold, bool strict,
			     const PathFormula& formula)
  : threshold_(threshold), strict_(strict), formula_(&formula) {
  PathFormula::register_use(formula_);
}


/* Deletes this probabilistic path quantification. */
Probabilistic::~Probabilistic() {
  PathFormula::unregister_use(formula_);
}


/* Tests if this state formula contains probabilistic elements. */
bool Probabilistic::probabilistic() const {
  return true;
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


/* Returns a BDD representation for this state formula. */
DdNode* Probabilistic::bdd(DdManager* dd_man) const {
  throw Exception("Probabilistic::bdd not implemented");
}


/* Prints this object on the given stream. */
void Probabilistic::print(std::ostream& os) const {
  os << 'P' << (strict() ? ">" : ">=") << threshold() << " [ ";
  formula().print(os);
  os << " ]";
}


/* ====================================================================== */
/* Comparison */

/* Constructs a comparison. */
Comparison::Comparison(const Expression& expr1, const Expression& expr2)
  : expr1_(&expr1), expr2_(&expr2) {
  Expression::register_use(expr1_);
  Expression::register_use(expr2_);
}


/* Deletes this comparison. */
Comparison::~Comparison() {
  Expression::unregister_use(expr1_);
  Expression::unregister_use(expr2_);
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


/* Returns a BDD representation for this state formula. */
DdNode* LessThan::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Value* value = dynamic_cast<const Value*>(&expr1());
  if (value != NULL) {
    /* value < expr2  <==>  expr2 > value */
    double threshold = value->value().double_value();
    DdNode* dde = expr2().mtbdd(dd_man);
    dd = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Value*>(&expr2());
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


/* Prints this object on the given stream. */
void LessThan::print(std::ostream& os) const {
  os << '(';
  expr1().print(os);
  os << '<';
  expr2().print(os);
  os << ')';
}


/* ====================================================================== */
/* LessThanOrEqual */

/* Constructs a less-than-or-equal comparison. */
LessThanOrEqual::LessThanOrEqual(const Expression& expr1,
				 const Expression& expr2)
  : Comparison(expr1, expr2) {}


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


/* Returns a BDD representation for this state formula. */
DdNode* LessThanOrEqual::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Value* value = dynamic_cast<const Value*>(&expr1());
  if (value != NULL) {
    /* value <= expr2  <==>  expr2 >= value */
    double threshold = value->value().double_value();
    DdNode* dde = expr2().mtbdd(dd_man);
    dd = Cudd_addBddThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Value*>(&expr2());
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


/* Prints this object on the given stream. */
void LessThanOrEqual::print(std::ostream& os) const {
  os << '(';
  expr1().print(os);
  os << "<=";
  expr2().print(os);
  os << ')';
}


/* ====================================================================== */
/* GreaterThanOrEqual */

/* Constructs a greater-than-or-equal comparison. */
GreaterThanOrEqual::GreaterThanOrEqual(const Expression& expr1,
				       const Expression& expr2)
  : Comparison(expr1, expr2) {}


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


/* Returns a BDD representation for this state formula. */
DdNode* GreaterThanOrEqual::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Value* value = dynamic_cast<const Value*>(&expr2());
  if (value != NULL) {
    /* expr1 >= value */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().mtbdd(dd_man);
    dd = Cudd_addBddThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Value*>(&expr1());
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


/* Prints this object on the given stream. */
void GreaterThanOrEqual::print(std::ostream& os) const {
  os << '(';
  expr1().print(os);
  os << ">=";
  expr2().print(os);
  os << ')';
}


/* ====================================================================== */
/* GreaterThan */

/* Constructs a greater-than comparison. */
GreaterThan::GreaterThan(const Expression& expr1, const Expression& expr2)
  : Comparison(expr1, expr2) {}


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


/* Returns a BDD representation for this state formula. */
DdNode* GreaterThan::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Value* value = dynamic_cast<const Value*>(&expr2());
  if (value != NULL) {
    /* expr1 > value */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().mtbdd(dd_man);
    dd = Cudd_addBddStrictThreshold(dd_man, dde, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Value*>(&expr1());
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


/* Prints this object on the given stream. */
void GreaterThan::print(std::ostream& os) const {
  os << '(';
  expr1().print(os);
  os << '>';
  expr2().print(os);
  os << ')';
}


/* ====================================================================== */
/* Equality */

/* Constructs an equality comparison. */
Equality::Equality(const Expression& expr1, const Expression& expr2)
  : Comparison(expr1, expr2) {}


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


/* Returns a BDD representation for this state formula. */
DdNode* Equality::bdd(DdManager* dd_man) const {
  DdNode* dd;
  const Value* value = dynamic_cast<const Value*>(&expr2());
  if (value != NULL) {
    /* expr1 == value  <==>  expr1 in [value,value] */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().mtbdd(dd_man);
    dd = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
    Cudd_Ref(dd);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Value*>(&expr1());
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


/* Prints this object on the given stream. */
void Equality::print(std::ostream& os) const {
  os << '(';
  expr1().print(os);
  os << '=';
  expr2().print(os);
  os << ')';
}


/* ====================================================================== */
/* Inequality */

/* Constructs an inequality comparison. */
Inequality::Inequality(const Expression& expr1, const Expression& expr2)
  : Comparison(expr1, expr2) {}


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


/* Returns a BDD representation for this state formula. */
DdNode* Inequality::bdd(DdManager* dd_man) const {
  const Value* value = dynamic_cast<const Value*>(&expr2());
  DdNode* ddf;
  if (value != NULL) {
    /* expr1 != value  <==>  !(expr1 in [value,value]) */
    double threshold = value->value().double_value();
    DdNode* dde = expr1().mtbdd(dd_man);
    ddf = Cudd_addBddInterval(dd_man, dde, threshold, threshold);
    Cudd_Ref(ddf);
    Cudd_RecursiveDeref(dd_man, dde);
  } else {
    value = dynamic_cast<const Value*>(&expr1());
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


/* Prints this object on the given stream. */
void Inequality::print(std::ostream& os) const {
  os << '(';
  expr1().print(os);
  os << "!=";
  expr2().print(os);
  os << ')';
}


/* ====================================================================== */
/* Until */

/* Constructs an until formula. */
Until::Until(const StateFormula& pre, const StateFormula& post,
	     const Rational& min_time, const Rational& max_time)
  : pre_(&pre), post_(&post), min_time_(min_time), max_time_(max_time) {
  StateFormula::register_use(pre_);
  StateFormula::register_use(post_);
}


/* Deletes this until formula. */
Until::~Until() {
  StateFormula::unregister_use(pre_);
  StateFormula::unregister_use(post_);
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
  pre().print(os);
  os << " U[" << min_time() << ',' << max_time() << "] ";
  post().print(os);
}
