/*
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
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
 * $Id: modules.cc,v 1.1 2003-08-10 01:52:41 lorens Exp $
 */
#include "modules.h"
#include "formulas.h"
#include "expressions.h"
#include "util.h"
#include "cudd.h"


/* ====================================================================== */
/* Update */

/* Constructs a variable update. */
Update::Update(const Variable& variable, const Expression& expr)
  : variable_(&variable), expr_(&expr) {
  Expression::register_use(variable_);
  Expression::register_use(expr_);
}


/* Deletes this variable update. */
Update::~Update() {
  Expression::unregister_use(variable_);
  Expression::unregister_use(expr_);
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
  const Value* value = dynamic_cast<const Value*>(&expr());
  DdNode* ddu;
  DdNode* ddv = variable().primed_mtbdd(dd_man);
  if (value != NULL) {
    /* variable' == value  <==>  variable' in [value,value] */
    double threshold = value->value().double_value();
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
		 const Expression& rate)
  : synch_(synch), guard_(&guard), rate_(&rate) {
  StateFormula::register_use(guard_);
  Expression::register_use(rate_);
}


/* Deletes this command. */
Command::~Command() {
  StateFormula::unregister_use(guard_);
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
				    rate().substitution(rates));
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
				    rate().substitution(subst));
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


/* ====================================================================== */
/* Module */

/* Constructs a module. */
Module::Module()
  : identity_bdd_(NULL) {}


/* Deletes this module. */
Module::~Module() {
  for (VariableList::const_iterator vi = variables().begin();
       vi != variables().end(); vi++) {
    Expression::unregister_use(*vi);
  }
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    delete *ci;
  }
}


/* Adds a variable to this module. */
void Module::add_variable(const Variable& variable) {
  variables_.push_back(&variable);
  Expression::register_use(&variable);
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


/* Returns a BDD representing the identity between the module
   variables and their primed versions. */
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
