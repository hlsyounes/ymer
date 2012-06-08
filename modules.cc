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
Update::Update(const Variable& variable, const Expression& expr)
  : variable_(&variable), expr_(&expr) {
  Expression::ref(variable_);
  Expression::ref(expr_);
}


/* Deletes this variable update. */
Update::~Update() {
  Expression::destructive_deref(variable_);
  Expression::destructive_deref(expr_);
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
  for (UpdateList::const_iterator ui = updates().begin();
       ui != updates().end(); ui++) {
    delete *ui;
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
  UpdateList::const_iterator ui = c.updates().begin();
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
  for (std::vector<const Variable*>::const_iterator vi = variables().begin();
       vi != variables().end(); vi++) {
    Expression::destructive_deref(*vi);
  }
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    delete *ci;
  }
}


/* Adds a variable to this module. */
void Module::add_variable(const Variable& variable) {
  variables_.push_back(&variable);
  Expression::ref(&variable);
}


/* Adds a command to this module. */
void Module::add_command(const Command* command) {
  commands_.push_back(command);
}

namespace {

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
      *SubstituteConstants(distribution.rate(), *constant_values_));
}

void DistributionConstantSubstituter::DoVisitWeibull(
    const Weibull& distribution) {
  distribution_ = Weibull::make(
      *SubstituteConstants(distribution.scale(), *constant_values_),
      *SubstituteConstants(distribution.shape(), *constant_values_));
}

void DistributionConstantSubstituter::DoVisitLognormal(
    const Lognormal& distribution) {
  distribution_ = Lognormal::make(
      *SubstituteConstants(distribution.scale(), *constant_values_),
      *SubstituteConstants(distribution.shape(), *constant_values_));
}

void DistributionConstantSubstituter::DoVisitUniform(
    const Uniform& distribution) {
  distribution_ = Uniform::make(
      *SubstituteConstants(distribution.low(), *constant_values_),
      *SubstituteConstants(distribution.high(), *constant_values_));
}

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
                    *SubstituteConstants(update.expr(), constant_values));
}

const Command* SubstituteConstants(
    const Command& command,
    const std::map<std::string, TypedValue>& constant_values,
    const std::map<std::string, TypedValue>& rate_values) {
  Command* subst_comm = new Command(
      command.synch(),
      command.guard().substitution(constant_values),
      SubstituteConstants(command.delay(), rate_values));
  for (UpdateList::const_iterator ui = command.updates().begin();
       ui != command.updates().end(); ui++) {
    subst_comm->add_update(SubstituteConstants(**ui, constant_values));
  }
  return subst_comm;
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
