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

void StateFormula::Accept(StateFormulaVisitor* visitor) const {
  DoAccept(visitor);
}

/* Output operator for state formulas. */
std::ostream& operator<<(std::ostream& os, const StateFormula& f) {
  f.print(os);
  return os;
}


/* ====================================================================== */
/* PathFormula */

void PathFormula::Accept(PathFormulaVisitor* visitor) const {
  DoAccept(visitor);
}

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
    delete *fi;
  }
}

void Conjunction::DoAccept(StateFormulaVisitor* visitor) const {
  visitor->VisitConjunction(*this);
}

/* Adds a conjunct to this conjunction. */
void Conjunction::add_conjunct(const StateFormula* conjunct) {
  if (conjunct->probabilistic()) {
    conjuncts_.push_front(conjunct);
  } else {
    conjuncts_.push_back(conjunct);
  }
}


/* Tests if this state formula contains probabilistic elements. */
bool Conjunction::probabilistic() const {
  return !conjuncts().empty() && conjuncts().front()->probabilistic();
}


/* Tests if this state formula holds in the given state. */
bool Conjunction::holds(const std::vector<int>& state) const {
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    if (!(*fi)->holds(state)) {
      return false;
    }
  }
  return true;
}


/* Returns the `current state' BDD representation for this state formula. */
BDD Conjunction::bdd(const DecisionDiagramManager& dd_man) const {
  BDD dd = dd_man.GetConstant(true);
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); ++fi) {
    dd = (*fi)->bdd(dd_man) && dd;
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
BDD Conjunction::primed_bdd(const DecisionDiagramManager& dd_man) const {
  BDD dd = dd_man.GetConstant(true);
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); ++fi) {
    dd = (*fi)->primed_bdd(dd_man) && dd;
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
    delete *fi;
  }
}

void Disjunction::DoAccept(StateFormulaVisitor* visitor) const {
  visitor->VisitDisjunction(*this);
}

/* Adds a disjunct to this disjunction. */
void Disjunction::add_disjunct(const StateFormula* disjunct) {
  if (disjunct->probabilistic()) {
    disjuncts_.push_front(disjunct);
  } else {
    disjuncts_.push_back(disjunct);
  }
}


/* Tests if this state formula contains probabilistic elements. */
bool Disjunction::probabilistic() const {
  return !disjuncts().empty() && disjuncts().front()->probabilistic();
}


/* Tests if this state formula holds in the given state. */
bool Disjunction::holds(const std::vector<int>& state) const {
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    if ((*fi)->holds(state)) {
      return true;
    }
  }
  return false;
}


/* Returns the `current state' BDD representation for this state formula. */
BDD Disjunction::bdd(const DecisionDiagramManager& dd_man) const {
  BDD dd = dd_man.GetConstant(false);
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); ++fi) {
    dd = (*fi)->bdd(dd_man) || dd;
  }
  return dd;
}


/* Returns the `next state' BDD representation for this state formula. */
BDD Disjunction::primed_bdd(const DecisionDiagramManager& dd_man) const {
  BDD dd = dd_man.GetConstant(false);
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); ++fi) {
    dd = (*fi)->primed_bdd(dd_man) || dd;
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
Negation::Negation(const StateFormula* negand)
  : negand_(negand) {
}


/* Deletes this negation. */
Negation::~Negation() {
  delete negand_;
}

void Negation::DoAccept(StateFormulaVisitor* visitor) const {
  visitor->VisitNegation(*this);
}

/* Tests if this state formula contains probabilistic elements. */
bool Negation::probabilistic() const {
  return negand().probabilistic();
}


/* Tests if this state formula holds in the given state. */
bool Negation::holds(const std::vector<int>& state) const {
  return !negand().holds(state);
}


/* Returns the `current state' BDD representation for this state formula. */
BDD Negation::bdd(const DecisionDiagramManager& dd_man) const {
  return !negand().bdd(dd_man);
}


/* Returns the `next state' BDD representation for this state formula. */
BDD Negation::primed_bdd(const DecisionDiagramManager& dd_man) const {
  return !negand().primed_bdd(dd_man);
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
Implication::Implication(const StateFormula* antecedent,
			 const StateFormula* consequent)
  : antecedent_(antecedent), consequent_(consequent) {
}


/* Deletes this implication. */
Implication::~Implication() {
  delete antecedent_;
  delete consequent_;
}

void Implication::DoAccept(StateFormulaVisitor* visitor) const {
  visitor->VisitImplication(*this);
}

/* Tests if this state formula contains probabilistic elements. */
bool Implication::probabilistic() const {
  return antecedent().probabilistic() || consequent().probabilistic();
}


/* Tests if this state formula holds in the given state. */
bool Implication::holds(const std::vector<int>& state) const {
  return !antecedent().holds(state) || consequent().holds(state);
}


/* Returns the `current state' BDD representation for this state formula. */
BDD Implication::bdd(const DecisionDiagramManager& dd_man) const {
  return !antecedent().bdd(dd_man) || consequent().bdd(dd_man);
}


/* Returns the `next state' BDD representation for this state formula. */
BDD Implication::primed_bdd(const DecisionDiagramManager& dd_man) const {
  return !antecedent().primed_bdd(dd_man) || consequent().primed_bdd(dd_man);
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
Probabilistic::Probabilistic(const TypedValue& threshold, bool strict,
			     const PathFormula* formula)
  : threshold_(threshold), strict_(strict), formula_(formula) {
}


/* Deletes this probabilistic path quantification. */
Probabilistic::~Probabilistic() {
  delete formula_;
}

void Probabilistic::DoAccept(StateFormulaVisitor* visitor) const {
  visitor->VisitProbabilistic(*this);
}

/* Tests if this state formula contains probabilistic elements. */
bool Probabilistic::probabilistic() const {
  return true;
}


/* Tests if this state formula holds in the given state. */
bool Probabilistic::holds(const std::vector<int>& state) const {
  throw std::logic_error("Probabilistic::holds not implemented");
}


/* Returns the `current state' BDD representation for this state formula. */
BDD Probabilistic::bdd(const DecisionDiagramManager& dd_man) const {
  throw std::logic_error("Probabilistic::bdd not implemented");
}


/* Returns the `next state' BDD representation for this state formula. */
BDD Probabilistic::primed_bdd(const DecisionDiagramManager& dd_man) const {
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
Comparison::Comparison(Operator op,
                       const Expression& expr1, const Expression& expr2)
    : op_(op), expr1_(&expr1), expr2_(&expr2) {
  Expression::ref(expr1_);
  Expression::ref(expr2_);
}


/* Deletes this comparison. */
Comparison::~Comparison() {
  Expression::destructive_deref(expr1_);
  Expression::destructive_deref(expr2_);
}

void Comparison::DoAccept(StateFormulaVisitor* visitor) const {
  visitor->VisitComparison(*this);
}

/* Tests if this state formula contains probabilistic elements. */
bool Comparison::probabilistic() const {
  return false;
}


/* ====================================================================== */
/* LessThan */

/* Constructs a less-than comparison. */
LessThan::LessThan(const Expression& expr1, const Expression& expr2)
    : Comparison(LESS, expr1, expr2) {
}


/* Tests if this state formula holds in the given state. */
bool LessThan::holds(const std::vector<int>& state) const {
  return expr1().value(state) < expr2().value(state);
}


/* Returns the `current state' BDD representation for this state formula. */
BDD LessThan::bdd(const DecisionDiagramManager& dd_man) const {
  return mtbdd(dd_man, expr1()) < mtbdd(dd_man, expr2());
}


/* Returns the `next state' BDD representation for this state formula. */
BDD LessThan::primed_bdd(const DecisionDiagramManager& dd_man) const {
  return primed_mtbdd(dd_man, expr1()) < primed_mtbdd(dd_man, expr2());
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
    : Comparison(LESS_EQUAL, expr1, expr2) {
}


/* Tests if this state formula holds in the given state. */
bool LessThanOrEqual::holds(const std::vector<int>& state) const {
  return expr1().value(state) <= expr2().value(state);
}


/* Returns the `current state' BDD representation for this state formula. */
BDD LessThanOrEqual::bdd(const DecisionDiagramManager& dd_man) const {
  return mtbdd(dd_man, expr1()) <= mtbdd(dd_man, expr2());
}


/* Returns the `next state' BDD representation for this state formula. */
BDD LessThanOrEqual::primed_bdd(const DecisionDiagramManager& dd_man) const {
  return primed_mtbdd(dd_man, expr1()) <= primed_mtbdd(dd_man, expr2());
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
    : Comparison(GREATER_EQUAL, expr1, expr2) {
}


/* Tests if this state formula holds in the given state. */
bool GreaterThanOrEqual::holds(const std::vector<int>& state) const {
  return expr1().value(state) >= expr2().value(state);
}


/* Returns the `current state' BDD representation for this state formula. */
BDD GreaterThanOrEqual::bdd(const DecisionDiagramManager& dd_man) const {
  return mtbdd(dd_man, expr1()) >= mtbdd(dd_man, expr2());
}


/* Returns the `next state' BDD representation for this state formula. */
BDD GreaterThanOrEqual::primed_bdd(const DecisionDiagramManager& dd_man) const {
  return primed_mtbdd(dd_man, expr1()) >= primed_mtbdd(dd_man, expr2());
}


/* Prints this object on the given stream. */
void GreaterThanOrEqual::print(std::ostream& os) const {
  os << expr1() << ">=" << expr2();
}


/* ====================================================================== */
/* GreaterThan */

/* Constructs a greater-than comparison. */
GreaterThan::GreaterThan(const Expression& expr1, const Expression& expr2)
    : Comparison(GREATER, expr1, expr2) {
}


/* Tests if this state formula holds in the given state. */
bool GreaterThan::holds(const std::vector<int>& state) const {
  return expr1().value(state) > expr2().value(state);
}


/* Returns the `current state' BDD representation for this state formula. */
BDD GreaterThan::bdd(const DecisionDiagramManager& dd_man) const {
  return mtbdd(dd_man, expr1()) > mtbdd(dd_man, expr2());
}


/* Returns the `next state' BDD representation for this state formula. */
BDD GreaterThan::primed_bdd(const DecisionDiagramManager& dd_man) const {
  return primed_mtbdd(dd_man, expr1()) > primed_mtbdd(dd_man, expr2());
}


/* Prints this object on the given stream. */
void GreaterThan::print(std::ostream& os) const {
  os << expr1() << '>' << expr2();
}


/* ====================================================================== */
/* Equality */

/* Constructs an equality comparison. */
Equality::Equality(const Expression& expr1, const Expression& expr2)
    : Comparison(EQUAL, expr1, expr2) {
}


/* Tests if this state formula holds in the given state. */
bool Equality::holds(const std::vector<int>& state) const {
  return expr1().value(state) == expr2().value(state);
}


/* Returns the `current state' BDD representation for this state formula. */
BDD Equality::bdd(const DecisionDiagramManager& dd_man) const {
  return mtbdd(dd_man, expr1()) == mtbdd(dd_man, expr2());
}


/* Returns the `next state' BDD representation for this state formula. */
BDD Equality::primed_bdd(const DecisionDiagramManager& dd_man) const {
  return primed_mtbdd(dd_man, expr1()) == primed_mtbdd(dd_man, expr2());
}


/* Prints this object on the given stream. */
void Equality::print(std::ostream& os) const {
  os << expr1() << '=' << expr2();
}


/* ====================================================================== */
/* Inequality */

/* Constructs an inequality comparison. */
Inequality::Inequality(const Expression& expr1, const Expression& expr2)
    : Comparison(NOT_EQUAL, expr1, expr2) {
}


/* Tests if this state formula holds in the given state. */
bool Inequality::holds(const std::vector<int>& state) const {
  return expr1().value(state) != expr2().value(state);
}


/* Returns the `current state' BDD representation for this state formula. */
BDD Inequality::bdd(const DecisionDiagramManager& dd_man) const {
  return mtbdd(dd_man, expr1()) != mtbdd(dd_man, expr2());
}


/* Returns the `next state' BDD representation for this state formula. */
BDD Inequality::primed_bdd(const DecisionDiagramManager& dd_man) const {
  return primed_mtbdd(dd_man, expr1()) != primed_mtbdd(dd_man, expr2());
}


/* Prints this object on the given stream. */
void Inequality::print(std::ostream& os) const {
  os << expr1() << "!=" << expr2();
}


/* ====================================================================== */
/* Until */

/* Constructs an until formula. */
Until::Until(const StateFormula* pre, const StateFormula* post,
	     const TypedValue& min_time, const TypedValue& max_time)
  : pre_(pre), post_(post), min_time_(min_time), max_time_(max_time) {
}


/* Deletes this until formula. */
Until::~Until() {
  delete pre_;
  delete post_;
}

void Until::DoAccept(PathFormulaVisitor* visitor) const {
  visitor->VisitUntil(*this);
}

/* Tests if this path formula contains probabilistic elements. */
bool Until::probabilistic() const {
  return pre().probabilistic() || post().probabilistic();
}


/* Prints this object on the given stream. */
void Until::print(std::ostream& os) const {
  os << pre() << " U[" << min_time() << ',' << max_time() << "] " << post();
}

StateFormulaVisitor::StateFormulaVisitor() {
}

StateFormulaVisitor::StateFormulaVisitor(const StateFormulaVisitor&) {
}

StateFormulaVisitor& StateFormulaVisitor::operator=(
    const StateFormulaVisitor&) {
  return *this;
}

StateFormulaVisitor::~StateFormulaVisitor() {
}

void StateFormulaVisitor::VisitConjunction(const Conjunction& formula) {
  DoVisitConjunction(formula);
}

void StateFormulaVisitor::VisitDisjunction(const Disjunction& formula) {
  DoVisitDisjunction(formula);
}

void StateFormulaVisitor::VisitNegation(const Negation& formula) {
  DoVisitNegation(formula);
}

void StateFormulaVisitor::VisitImplication(const Implication& formula) {
  DoVisitImplication(formula);
}

void StateFormulaVisitor::VisitProbabilistic(const Probabilistic& formula) {
  DoVisitProbabilistic(formula);
}

void StateFormulaVisitor::VisitComparison(const Comparison& formula) {
  DoVisitComparison(formula);
}

PathFormulaVisitor::PathFormulaVisitor() {
}

PathFormulaVisitor::PathFormulaVisitor(const PathFormulaVisitor&) {
}

PathFormulaVisitor& PathFormulaVisitor::operator=(const PathFormulaVisitor&) {
  return *this;
}

PathFormulaVisitor::~PathFormulaVisitor() {
}

void PathFormulaVisitor::VisitUntil(const Until& formula) {
  DoVisitUntil(formula);
}
