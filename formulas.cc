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

#include <typeinfo>

#include "glog/logging.h"

ModelCheckingParams::ModelCheckingParams()
    : alpha(1e-2),
      beta(1e-2),
      delta(1e-2),
      epsilon(1e-6),
      algorithm(SPRT),
      fixed_sample_size(0),
      max_path_length(std::numeric_limits<size_t>::max()),
      nested_error(-1),
      memoization(false) {
}

void StateFormula::Accept(StateFormulaVisitor* visitor) const {
  DoAccept(visitor);
}

namespace {

class StateFormulaCompiler : public StateFormulaVisitor {
 public:
  StateFormulaCompiler(
      const DecisionDiagramManager* manager,
      const std::map<std::string, VariableProperties>* variable_properties,
      bool primed);

  BDD bdd() const { return bdd_; }

 private:
  virtual void DoVisitConjunction(const Conjunction& formula);
  virtual void DoVisitDisjunction(const Disjunction& formula);
  virtual void DoVisitNegation(const Negation& formula);
  virtual void DoVisitImplication(const Implication& formula);
  virtual void DoVisitProbabilistic(const Probabilistic& formula);
  virtual void DoVisitComparison(const Comparison& formula);

  const DecisionDiagramManager* manager_;
  const std::map<std::string, VariableProperties>* variable_properties_;
  bool primed_;
  BDD bdd_;
};

StateFormulaCompiler::StateFormulaCompiler(
    const DecisionDiagramManager* manager,
    const std::map<std::string, VariableProperties>* variable_properties,
    bool primed)
    : manager_(manager), variable_properties_(variable_properties),
      primed_(primed), bdd_(manager->GetConstant(false)) {
}

void StateFormulaCompiler::DoVisitConjunction(const Conjunction& formula) {
  BDD result = manager_->GetConstant(true);
  for (const StateFormula* conjunct : formula.conjuncts()) {
    conjunct->Accept(this);
    result = bdd_ && result;
  }
  bdd_ = result;
}

void StateFormulaCompiler::DoVisitDisjunction(const Disjunction& formula) {
  BDD result = manager_->GetConstant(false);
  for (const StateFormula* disjunct : formula.disjuncts()) {
    disjunct->Accept(this);
    result = bdd_ || result;
  }
  bdd_ = result;
}

void StateFormulaCompiler::DoVisitNegation(const Negation& formula) {
  formula.negand().Accept(this);
  bdd_ = !bdd_;
}

void StateFormulaCompiler::DoVisitImplication(const Implication& formula) {
  formula.antecedent().Accept(this);
  BDD antecedent = bdd_;
  formula.consequent().Accept(this);
  bdd_ = !antecedent || bdd_;
}

void StateFormulaCompiler::DoVisitProbabilistic(const Probabilistic& formula) {
  LOG(FATAL) << "not implemented";
}

void StateFormulaCompiler::DoVisitComparison(const Comparison& formula) {
  ADD expr1 = primed_
      ? primed_mtbdd(*manager_, *variable_properties_, formula.expr1())
      : mtbdd(*manager_, *variable_properties_, formula.expr1());
  ADD expr2 = primed_
      ? primed_mtbdd(*manager_, *variable_properties_, formula.expr2())
      : mtbdd(*manager_, *variable_properties_, formula.expr2());
  switch (formula.op()) {
    case Comparison::LESS:
      bdd_ = expr1 < expr2;
      break;
    case Comparison::LESS_EQUAL:
      bdd_ = expr1 <= expr2;
      break;
    case Comparison::GREATER_EQUAL:
      bdd_ = expr1 >= expr2;
      break;
    case Comparison::GREATER:
      bdd_ = expr1 > expr2;
      break;
    case Comparison::EQUAL:
      bdd_ = expr1 == expr2;
      break;
    case Comparison::NOT_EQUAL:
      bdd_ = expr1 != expr2;
      break;
  }
}

}  // namespace

BDD bdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const StateFormula& f) {
  StateFormulaCompiler compiler(&manager, &variable_properties,
                                false /* primed */);
  f.Accept(&compiler);
  return compiler.bdd();
}

BDD primed_bdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const StateFormula& f) {
  StateFormulaCompiler compiler(&manager, &variable_properties,
                                true /* primed */);
  f.Accept(&compiler);
  return compiler.bdd();
}

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
  for (const StateFormula* conjunct : conjuncts()) {
    delete conjunct;
  }
}

void Conjunction::DoAccept(StateFormulaVisitor* visitor) const {
  visitor->VisitConjunction(*this);
}

/* Adds a conjunct to this conjunction. */
void Conjunction::add_conjunct(const StateFormula* conjunct) {
  if (conjunct->probabilistic()) {
    conjuncts_.insert(conjuncts_.begin(), conjunct);
  } else {
    conjuncts_.push_back(conjunct);
  }
}


/* Tests if this state formula contains probabilistic elements. */
bool Conjunction::probabilistic() const {
  return !conjuncts().empty() && conjuncts().front()->probabilistic();
}


/* Prints this object on the given stream. */
void Conjunction::print(std::ostream& os) const {
  if (conjuncts().empty()) {
    os << "true";
  } else if (conjuncts().size() == 1) {
    os << *conjuncts().front();
  } else {
    auto fi = conjuncts().begin();
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
  for (const StateFormula* disjunct : disjuncts()) {
    delete disjunct;
  }
}

void Disjunction::DoAccept(StateFormulaVisitor* visitor) const {
  visitor->VisitDisjunction(*this);
}

/* Adds a disjunct to this disjunction. */
void Disjunction::add_disjunct(const StateFormula* disjunct) {
  if (disjunct->probabilistic()) {
    disjuncts_.insert(disjuncts_.begin(), disjunct);
  } else {
    disjuncts_.push_back(disjunct);
  }
}


/* Tests if this state formula contains probabilistic elements. */
bool Disjunction::probabilistic() const {
  return !disjuncts().empty() && disjuncts().front()->probabilistic();
}


/* Prints this object on the given stream. */
void Disjunction::print(std::ostream& os) const {
  if (disjuncts().empty()) {
    os << "false";
  } else if (disjuncts().size() == 1) {
    os << *disjuncts().front();
  } else {
    auto fi = disjuncts().begin();
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
