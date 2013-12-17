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
 * $Id: modules.cc,v 2.1 2004-01-25 12:37:53 lorens Exp $
 */
#include "modules.h"
#include "distributions.h"
#include "formulas.h"
#include "src/expression.h"

/* ====================================================================== */
/* Update */

/* Constructs a variable update. */
Update::Update(const std::string& variable,
               std::unique_ptr<const Expression>&& expr)
    : variable_(variable), expr_(std::move(expr)) {
}


/* ====================================================================== */
/* Command */

/* Constructs a command. */
Command::Command(size_t synch, const StateFormula* guard,
		 const Distribution* delay)
  : synch_(synch), guard_(guard), delay_(delay) {
}


/* Deletes this command. */
Command::~Command() {
  delete guard_;
  delete delay_;
  for (const Update* update : updates()) {
    delete update;
  }
}


/* Adds an update to this command. */
void Command::add_update(const Update* update) {
  updates_.push_back(update);
}


/* Output operator for commands. */
std::ostream& operator<<(std::ostream& os, const Command& c) {
  os << "[";
  if (c.synch() != 0) {
    os << 's' << c.synch();
  }
  os << "] " << c.guard() << " -> " << c.delay() << " : ";
  auto ui = c.updates().begin();
  if (ui != c.updates().end()) {
    const Update* u = *ui;
    os << u->variable() << "\'=" << u->expr();
    for (ui++; ui != c.updates().end(); ui++) {
      u = *ui;
      os << " & " << u->variable() << "\'=" << u->expr();
    }
  }
  return os;
}


/* ====================================================================== */
/* Module */

/* Constructs a module. */
Module::Module() {
}


/* Deletes this module. */
Module::~Module() {
  for (const Command* command : commands()) {
    delete command;
  }
}


/* Adds a variable to this module. */
void Module::add_variable(const std::string& variable) {
  variables_.push_back(variable);
}


/* Adds a command to this module. */
void Module::add_command(const Command* command) {
  commands_.push_back(command);
}

namespace {

class ExpressionConstantSubstituter : public ExpressionVisitor {
 public:
  explicit ExpressionConstantSubstituter(
      const std::map<std::string, TypedValue>* constant_values);

  std::unique_ptr<const Expression> release_expr() { return std::move(expr_); }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);

  const std::map<std::string, TypedValue>* constant_values_;
  std::unique_ptr<const Expression> expr_;
};

std::unique_ptr<const Expression> SubstituteConstants(
    const Expression& expr,
    const std::map<std::string, TypedValue>& constant_values) {
  ExpressionConstantSubstituter substituter(&constant_values);
  expr.Accept(&substituter);
  return substituter.release_expr();
}

class StateFormulaConstantSubstituter : public StateFormulaVisitor {
 public:
  explicit StateFormulaConstantSubstituter(
      const std::map<std::string, TypedValue>* constant_values);

  ~StateFormulaConstantSubstituter();

  const StateFormula* release_formula();

 private:
  virtual void DoVisitConjunction(const Conjunction& formula);
  virtual void DoVisitDisjunction(const Disjunction& formula);
  virtual void DoVisitNegation(const Negation& formula);
  virtual void DoVisitImplication(const Implication& formula);
  virtual void DoVisitProbabilistic(const Probabilistic& formula);
  virtual void DoVisitComparison(const Comparison& formula);

  const std::map<std::string, TypedValue>* constant_values_;
  const StateFormula* formula_;
};

const StateFormula* SubstituteConstants(
    const StateFormula& formula,
    const std::map<std::string, TypedValue>& constant_values) {
  StateFormulaConstantSubstituter substituter(&constant_values);
  formula.Accept(&substituter);
  return substituter.release_formula();
}

class PathFormulaConstantSubstituter : public PathFormulaVisitor {
 public:
  explicit PathFormulaConstantSubstituter(
      const std::map<std::string, TypedValue>* constant_values);

  ~PathFormulaConstantSubstituter();

  const PathFormula* release_formula();

 private:
  virtual void DoVisitUntil(const Until& formula);

  const std::map<std::string, TypedValue>* constant_values_;
  const PathFormula* formula_;
};

const PathFormula* SubstituteConstants(
    const PathFormula& formula,
    const std::map<std::string, TypedValue>& constant_values) {
  PathFormulaConstantSubstituter substituter(&constant_values);
  formula.Accept(&substituter);
  return substituter.release_formula();
}

StateFormulaConstantSubstituter::StateFormulaConstantSubstituter(
    const std::map<std::string, TypedValue>* constant_values)
    : constant_values_(constant_values), formula_(NULL) {
}

class DistributionConstantSubstituter : public DistributionVisitor {
 public:
  explicit DistributionConstantSubstituter(
      const std::map<std::string, TypedValue>* constant_values);

  ~DistributionConstantSubstituter();

  const Distribution* release_distribution();

 private:
  virtual void DoVisitExponential(const Exponential& distribution);
  virtual void DoVisitWeibull(const Weibull& distribution);
  virtual void DoVisitLognormal(const Lognormal& distribution);
  virtual void DoVisitUniform(const Uniform& distribution);

  const std::map<std::string, TypedValue>* constant_values_;
  const Distribution* distribution_;
};

const Distribution* SubstituteConstants(
    const Distribution& distribution,
    const std::map<std::string, TypedValue>& constant_values) {
  DistributionConstantSubstituter substituter(&constant_values);
  distribution.Accept(&substituter);
  return substituter.release_distribution();
}

const Update* SubstituteConstants(
    const Update& update,
    const std::map<std::string, TypedValue>& constant_values) {
  return new Update(update.variable(),
                    SubstituteConstants(update.expr(), constant_values));
}

const Command* SubstituteConstants(
    const Command& command,
    const std::map<std::string, TypedValue>& constant_values,
    const std::map<std::string, TypedValue>& rate_values) {
  Command* subst_comm = new Command(
      command.synch(),
      SubstituteConstants(command.guard(), constant_values),
      SubstituteConstants(command.delay(), rate_values));
  for (const Update* update: command.updates()) {
    subst_comm->add_update(SubstituteConstants(*update, constant_values));
  }
  return subst_comm;
}

ExpressionConstantSubstituter::ExpressionConstantSubstituter(
    const std::map<std::string, TypedValue>* constant_values)
    : constant_values_(constant_values) {
}

void ExpressionConstantSubstituter::DoVisitLiteral(const Literal& expr) {
  expr_.reset(new Literal(expr.value()));
}

void ExpressionConstantSubstituter::DoVisitIdentifier(const Identifier& expr) {
  std::map<std::string, TypedValue>::const_iterator i =
      constant_values_->find(expr.name());
  if (i == constant_values_->end()) {
    expr_.reset(new Identifier(expr.name()));
  } else {
    expr_.reset(new Literal(i->second));
  }
}

void ExpressionConstantSubstituter::DoVisitFunctionCall(
    const FunctionCall& expr) {
  PointerVector<const Expression> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(release_expr());
  }
  expr_ = FunctionCall::New(expr.function(), std::move(arguments));
}

void ExpressionConstantSubstituter::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  expr.operand().Accept(this);
  expr_ = UnaryOperation::New(expr.op(), release_expr());
}

void ExpressionConstantSubstituter::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  std::unique_ptr<const Expression> operand1 = release_expr();
  expr.operand2().Accept(this);
  expr_ = BinaryOperation::New(expr.op(), std::move(operand1), release_expr());
}

StateFormulaConstantSubstituter::~StateFormulaConstantSubstituter() {
  delete formula_;
}

const StateFormula* StateFormulaConstantSubstituter::release_formula() {
  const StateFormula* formula = formula_;
  formula_ = NULL;
  return formula;
}

void StateFormulaConstantSubstituter::DoVisitConjunction(
    const Conjunction& formula) {
  Conjunction* subst_conj = new Conjunction();
  for (const StateFormula* conjunct : formula.conjuncts()) {
    conjunct->Accept(this);
    subst_conj->add_conjunct(release_formula());
  }
  formula_ = subst_conj;
}

void StateFormulaConstantSubstituter::DoVisitDisjunction(
    const Disjunction& formula) {
  Disjunction* subst_disj = new Disjunction();
  for (const StateFormula* disjunct : formula.disjuncts()) {
    disjunct->Accept(this);
    subst_disj->add_disjunct(release_formula());
  }
  formula_ = subst_disj;
}

void StateFormulaConstantSubstituter::DoVisitNegation(const Negation& formula) {
  formula.negand().Accept(this);
  formula_ = new Negation(release_formula());
}

void StateFormulaConstantSubstituter::DoVisitImplication(
    const Implication& formula) {
  formula.antecedent().Accept(this);
  const StateFormula* antecedent = release_formula();
  formula.consequent().Accept(this);
  formula_ = new Implication(antecedent, release_formula());
}

void StateFormulaConstantSubstituter::DoVisitProbabilistic(
    const Probabilistic& formula) {
  formula_ = new Probabilistic(
      formula.threshold(), formula.strict(),
      SubstituteConstants(formula.formula(), *constant_values_));
}

void StateFormulaConstantSubstituter::DoVisitComparison(
    const Comparison& formula) {
  switch (formula.op()) {
    case Comparison::LESS:
      formula_ = new LessThan(
          SubstituteConstants(formula.expr1(), *constant_values_),
          SubstituteConstants(formula.expr2(), *constant_values_));
      break;
    case Comparison::LESS_EQUAL:
      formula_ = new LessThanOrEqual(
          SubstituteConstants(formula.expr1(), *constant_values_),
          SubstituteConstants(formula.expr2(), *constant_values_));
      break;
    case Comparison::GREATER_EQUAL:
      formula_ = new GreaterThanOrEqual(
          SubstituteConstants(formula.expr1(), *constant_values_),
          SubstituteConstants(formula.expr2(), *constant_values_));
      break;
    case Comparison::GREATER:
      formula_ = new GreaterThan(
          SubstituteConstants(formula.expr1(), *constant_values_),
          SubstituteConstants(formula.expr2(), *constant_values_));
      break;
    case Comparison::EQUAL:
      formula_ = new Equality(
          SubstituteConstants(formula.expr1(), *constant_values_),
          SubstituteConstants(formula.expr2(), *constant_values_));
      break;
    case Comparison::NOT_EQUAL:
      formula_ = new Inequality(
          SubstituteConstants(formula.expr1(), *constant_values_),
          SubstituteConstants(formula.expr2(), *constant_values_));
      break;
  }
}

PathFormulaConstantSubstituter::PathFormulaConstantSubstituter(
    const std::map<std::string, TypedValue>* constant_values)
    : constant_values_(constant_values), formula_(NULL) {
}

PathFormulaConstantSubstituter::~PathFormulaConstantSubstituter() {
  delete formula_;
}

const PathFormula* PathFormulaConstantSubstituter::release_formula() {
  const PathFormula* formula = formula_;
  formula_ = NULL;
  return formula;
}

void PathFormulaConstantSubstituter::DoVisitUntil(const Until& formula) {
  formula_ = new Until(SubstituteConstants(formula.pre(), *constant_values_),
                       SubstituteConstants(formula.post(), *constant_values_),
                       formula.min_time(), formula.max_time());
}

DistributionConstantSubstituter::DistributionConstantSubstituter(
    const std::map<std::string, TypedValue>* constant_values)
    : constant_values_(constant_values), distribution_(NULL) {
}

DistributionConstantSubstituter::~DistributionConstantSubstituter() {
  delete distribution_;
}

const Distribution* DistributionConstantSubstituter::release_distribution() {
  const Distribution* distribution = distribution_;
  distribution_ = NULL;
  return distribution;
}

void DistributionConstantSubstituter::DoVisitExponential(
    const Exponential& distribution) {
  distribution_ = Exponential::make(
      SubstituteConstants(distribution.rate(), *constant_values_));
}

void DistributionConstantSubstituter::DoVisitWeibull(
    const Weibull& distribution) {
  distribution_ = Weibull::make(
      SubstituteConstants(distribution.scale(), *constant_values_),
      SubstituteConstants(distribution.shape(), *constant_values_));
}

void DistributionConstantSubstituter::DoVisitLognormal(
    const Lognormal& distribution) {
  distribution_ = Lognormal::make(
      SubstituteConstants(distribution.scale(), *constant_values_),
      SubstituteConstants(distribution.shape(), *constant_values_));
}

void DistributionConstantSubstituter::DoVisitUniform(
    const Uniform& distribution) {
  distribution_ = Uniform::make(
      SubstituteConstants(distribution.low(), *constant_values_),
      SubstituteConstants(distribution.high(), *constant_values_));
}

}  // namespace

/* Substitutes constants with values. */
void Module::compile(const std::map<std::string, TypedValue>& constant_values,
                     const std::map<std::string, TypedValue>& rate_values) {
  size_t n = commands().size();
  for (size_t i = 0; i < n; i++) {
    const Command* ci = commands_[i];
    const Command* cj = SubstituteConstants(*ci, constant_values, rate_values);
    delete ci;
    commands_[i] = cj;
  }
}
