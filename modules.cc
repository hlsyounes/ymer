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
#include "expressions.h"


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
const Update& Update::substitution(const ValueMap& values) const {
  return *new Update(variable(), expr().substitution(values));
}


/* Returns this update subject to the given substitutions. */
const Update& Update::substitution(const SubstitutionMap& subst) const {
  return *new Update(variable().substitution(subst),
		     expr().substitution(subst));
}


/* Returns a BDD representation of this update. */
DdNode* Update::bdd(DdManager* dd_man) const {
  DdNode* ddu;
  DdNode* ddv = variable().primed_mtbdd(dd_man);
  const Literal* value = dynamic_cast<const Literal*>(&expr());
  if (value != NULL) {
    /* variable' == value  <==>  variable' in [value,value] */
    double threshold = value->value().value<double>();
    ddu = Cudd_addBddInterval(dd_man, ddv, threshold, threshold);
    Cudd_Ref(ddu);
    Cudd_RecursiveDeref(dd_man, ddv);
  } else {
    /* variable' == expr  <==>  variable' - expr in [0,0] */
    DdNode* dde = expr().mtbdd(dd_man);
    DdNode* ddm = Cudd_addApply(dd_man, Cudd_addMinus, ddv, dde);
    Cudd_Ref(ddm);
    Cudd_RecursiveDeref(dd_man, ddv);
    Cudd_RecursiveDeref(dd_man, dde);
    ddu = Cudd_addBddInterval(dd_man, ddm, 0, 0);
    Cudd_Ref(ddu);
    Cudd_RecursiveDeref(dd_man, ddm);
  }
  return ddu;
}


/* ====================================================================== */
/* Command */

/* Constructs a command. */
Command::Command(size_t synch, const StateFormula& guard,
		 const Distribution& delay)
  : synch_(synch), guard_(&guard), delay_(&delay) {
  StateFormula::ref(guard_);
  Distribution::ref(delay_);
}


/* Deletes this command. */
Command::~Command() {
  StateFormula::destructive_deref(guard_);
  Distribution::destructive_deref(delay_);
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
const Command& Command::substitution(const ValueMap& constants,
				     const ValueMap& rates) const {
  Command* subst_comm = new Command(synch(), guard().substitution(constants),
				    delay().substitution(rates));
  for (UpdateList::const_iterator ui = updates().begin();
       ui != updates().end(); ui++) {
    subst_comm->add_update((*ui)->substitution(constants));
  }
  return *subst_comm;
}


/* Returns this command subject to the given substitutions. */
const Command&
Command::substitution(const SubstitutionMap& subst,
		      const SynchSubstitutionMap& synchs) const {
  size_t s;
  SynchSubstitutionMap::const_iterator si = synchs.find(synch());
  if (si == synchs.end()) {
    s = synch();
  } else {
    s = (*si).second;
  }
  Command* subst_comm = new Command(s, guard().substitution(subst),
				    delay().substitution(subst));
  for (UpdateList::const_iterator ui = updates().begin();
       ui != updates().end(); ui++) {
    subst_comm->add_update((*ui)->substitution(subst));
  }
  return *subst_comm;
}


/* Returns a BDD representation of this command and fills the
   provided set with variables updated by this command. */
DdNode* Command::bdd(VariableSet& updated, DdManager* dd_man) const {
  /*
   * Conjunction of BDDs for all updates.
   */
  DdNode* ddu = Cudd_ReadOne(dd_man);
  Cudd_Ref(ddu);
  for (UpdateList::const_iterator ui = updates().begin();
       ui != updates().end(); ui++) {
    const Update& update = **ui;
    DdNode* ddi = update.bdd(dd_man);
    DdNode* dda = Cudd_bddAnd(dd_man, ddi, ddu);
    Cudd_Ref(dda);
    Cudd_RecursiveDeref(dd_man, ddi);
    Cudd_RecursiveDeref(dd_man, ddu);
    ddu = dda;
    updated.insert(&update.variable());
  }
  /*
   * Conjunction with BDD for guard.
   */
  DdNode* ddg = guard().bdd(dd_man);
  DdNode* dda = Cudd_bddAnd(dd_man, ddg, ddu);
  Cudd_Ref(dda);
  Cudd_RecursiveDeref(dd_man, ddg);
  Cudd_RecursiveDeref(dd_man, ddu);
  return dda;
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
Module::Module()
  : identity_bdd_(NULL) {}


/* Deletes this module. */
Module::~Module() {
  for (VariableList::const_iterator vi = variables().begin();
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
void Module::compile(const ValueMap& constants, const ValueMap& rates) {
  size_t n = commands().size();
  for (size_t i = 0; i < n; i++) {
    const Command* ci = commands_[i];
    const Command* cj = &ci->substitution(constants, rates);
    delete ci;
    commands_[i] = cj;
  }
}


/* Returns this module subject to the given substitutions. */
Module& Module::substitution(const SubstitutionMap& subst,
			     const SynchSubstitutionMap& synchs) const {
  Module* subst_mod = new Module();
  for (VariableList::const_iterator vi = variables().begin();
       vi != variables().end(); vi++) {
    subst_mod->add_variable((*vi)->substitution(subst));
  }
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    subst_mod->add_command((*ci)->substitution(subst, synchs));
  }
  return *subst_mod;
}


/* Returns a BDD representing the identity between the `current
   state' and `next state' variables of this module. */
DdNode* Module::identity_bdd(DdManager* dd_man) const {
  if (identity_bdd_ == NULL) {
    DdNode* dd = Cudd_ReadOne(dd_man);
    Cudd_Ref(dd);
    for (VariableList::const_reverse_iterator vi = variables().rbegin();
	 vi != variables().rend(); vi++) {
      DdNode* ddv = (*vi)->identity_bdd(dd_man);
      DdNode* ddi = Cudd_bddAnd(dd_man, ddv, dd);
      Cudd_Ref(ddi);
      Cudd_RecursiveDeref(dd_man, ddv);
      Cudd_RecursiveDeref(dd_man, dd);
      dd = ddi;
    }
    identity_bdd_ = dd;
  } else {
    Cudd_Ref(identity_bdd_);
  }
  return identity_bdd_;
}


/* Releases any cached DDs for this module. */
void Module::uncache_dds(DdManager* dd_man) const {
  if (identity_bdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man, identity_bdd_);
    identity_bdd_ = NULL;
  }
}
