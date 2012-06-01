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

namespace {

const Variable* SubstituteVariable(
    const Variable* variable,
    const std::map<std::string, const Variable*>& substitutions) {
  std::map<std::string, const Variable*>::const_iterator i =
      substitutions.find(variable->name());
  return (i == substitutions.end()) ? variable : i->second;
}

}  // namespace

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


/* Returns this update subject to the given substitutions. */
const Update& Update::substitution(
    const std::map<std::string, TypedValue>& constant_values) const {
  return *new Update(variable(), *SubstituteConstants(expr(), constant_values));
}

/* Returns this update subject to the given substitutions. */
const Update& Update::substitution(
    const std::map<std::string, const Variable*>& substitutions) const {
  return *new Update(*SubstituteVariable(variable_, substitutions),
                     *SubstituteIdentifiers(expr(), substitutions));
}


/* ====================================================================== */
/* Command */

/* Constructs a command. */
Command::Command(size_t synch, const StateFormula& guard,
		 const Distribution& delay)
  : synch_(synch), guard_(&guard), delay_(&delay) {
  StateFormula::ref(guard_);
}


/* Deletes this command. */
Command::~Command() {
  StateFormula::destructive_deref(guard_);
  delete delay_;
  for (UpdateList::const_iterator ui = updates().begin();
       ui != updates().end(); ui++) {
    delete *ui;
  }
}


/* Adds an update to this command. */
void Command::add_update(const Update& update) {
  updates_.push_back(&update);
}


/* Returns this command subject to the given substitutions. */
const Command& Command::substitution(
    const std::map<std::string, TypedValue>& constant_values,
    const std::map<std::string, TypedValue>& rate_values) const {
  Command* subst_comm = new Command(synch(),
                                    guard().substitution(constant_values),
				    delay().substitution(rate_values));
  for (UpdateList::const_iterator ui = updates().begin();
       ui != updates().end(); ui++) {
    subst_comm->add_update((*ui)->substitution(constant_values));
  }
  return *subst_comm;
}


/* Returns this command subject to the given substitutions. */
const Command& Command::substitution(
    const std::map<std::string, const Variable*>& substitutions,
    const SynchSubstitutionMap& synchs) const {
  size_t s;
  SynchSubstitutionMap::const_iterator si = synchs.find(synch());
  if (si == synchs.end()) {
    s = synch();
  } else {
    s = (*si).second;
  }
  Command* subst_comm = new Command(s, guard().substitution(substitutions),
				    delay().substitution(substitutions));
  for (UpdateList::const_iterator ui = updates().begin();
       ui != updates().end(); ui++) {
    subst_comm->add_update((*ui)->substitution(substitutions));
  }
  return *subst_comm;
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
void Module::add_command(const Command& command) {
  commands_.push_back(&command);
}


/* Substitutes constants with values. */
void Module::compile(const std::map<std::string, TypedValue>& constant_values,
                     const std::map<std::string, TypedValue>& rate_values) {
  size_t n = commands().size();
  for (size_t i = 0; i < n; i++) {
    const Command* ci = commands_[i];
    const Command* cj = &ci->substitution(constant_values, rate_values);
    delete ci;
    commands_[i] = cj;
  }
}


/* Returns this module subject to the given substitutions. */
Module& Module::substitution(
    const std::map<std::string, const Variable*>& substitutions,
    const SynchSubstitutionMap& synchs) const {
  Module* subst_mod = new Module();
  for (std::vector<const Variable*>::const_iterator vi = variables().begin();
       vi != variables().end(); vi++) {
    subst_mod->add_variable(*SubstituteVariable(*vi, substitutions));
  }
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    subst_mod->add_command((*ci)->substitution(substitutions, synchs));
  }
  return *subst_mod;
}
