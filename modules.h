/* -*-C++-*- */
/*
 * Modules.
 *
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
 * $Id: modules.h,v 1.1 2003-08-10 01:52:46 lorens Exp $
 */
#ifndef MODULES_H
#define MODULES_H

#include <config.h>
#include "expressions.h"

struct StateFormula;


/* ====================================================================== */
/* Update */

/*
 * A variable update.
 */
struct Update {
  /* Constructs a variable update. */
  Update(const Variable& variable, const Expression& expr);

  /* Deletes this variable update. */
  ~Update();

  /* Returns the variable for this update. */
  const Variable& variable() const { return *variable_; }

  /* Returns the expression for this update. */
  const Expression& expr() const { return *expr_; }

  /* Returns this update subject to the given substitutions. */
  const Update& substitution(const ValueMap& values) const;

  /* Returns this update subject to the given substitutions. */
  const Update& substitution(const SubstitutionMap& subst) const;

  /* Returns a BDD representation of this update. */
  DdNode* bdd(DdManager* dd_man) const;

private:
  /* The variable for this update. */
  const Variable* variable_;
  /* The expression for this update. */
  const Expression* expr_;
};


/* ====================================================================== */
/* UpdateList */

struct UpdateList : public std::vector<const Update*> {
};


/* ====================================================================== */
/* SynchronizationMap */

/*
 * Substitution map for synchronizations.
 */
struct SynchSubstitutionMap : public hashing::hash_map<size_t, size_t> {
};


/* ====================================================================== */
/* Command */

/*
 * A module command.
 */
struct Command {
  /* Constructs a command. */
  Command(size_t synch, const StateFormula& guard, const Expression& rate);

  /* Deletes this command. */
  ~Command();

  /* Adds an update to this command. */
  void add_update(const Update& update);

  /* Returns the synchronization for this command; 0 if this command
     requires no synchronization. */
  size_t synch() const { return synch_; }

  /* Returns the guard for this command. */
  const StateFormula& guard() const { return *guard_; }

  /* Returns the rate for this command. */
  const Expression& rate() const { return *rate_; }

  /* Returns the updates for this command. */
  const UpdateList& updates() const { return updates_; }

  /* Returns this command subject to the given substitutions. */
  const Command& substitution(const ValueMap& constants,
			      const ValueMap& rates) const;

  /* Returns this command subject to the given substitutions. */
  const Command& substitution(const SubstitutionMap& subst,
			      const SynchSubstitutionMap& synchs) const;

  /* Returns a BDD representation of this command and fills the
     provided set with variables updated by this command. */
  DdNode* bdd(VariableSet& updated, DdManager* dd_man) const;

private:
  /* The synchronization for this command; 0 if this command requires
     no synchronization. */
  size_t synch_;
  /* The guard for this command. */
  const StateFormula* guard_;
  /* The rate for this command. */
  const Expression* rate_;
  /* The updates for this command. */
  UpdateList updates_;
};


/* ====================================================================== */
/* CommandList */

/*
 * A list of commands.
 */
struct CommandList : public std::vector<const Command*> {
};


/* ====================================================================== */
/* Module */

/*
 * A module.
 */
struct Module {
  /* Constructs a module. */
  Module();

  /* Deletes this module. */
  ~Module();

  /* Adds a variable to this module. */
  void add_variable(const Variable& variable);

  /* Adds a command to this module. */
  void add_command(const Command& command);

  /* Substitutes constants with values. */
  void compile(const ValueMap& constants, const ValueMap& rates);

  /* Returns the variables for this module. */
  const VariableList& variables() const { return variables_; }

  /* Returns the commands for this module. */
  const CommandList& commands() const { return commands_; }

  /* Returns this module subject to the given substitutions. */
  Module& substitution(const SubstitutionMap& subst,
		       const SynchSubstitutionMap& synchs) const;

  /* Returns a BDD representing the identity between the module
     variables and their primed versions. */
  DdNode* identity_bdd(DdManager* dd_man) const;

  /* Releases any cached DDs for this module. */
  void uncache_dds(DdManager* dd_man) const;

private:
  /* The variables for this module. */
  VariableList variables_;
  /* The commands for this module. */
  CommandList commands_;
  /* Cached BDD representing identity between the module variables and
     their primed versions. */
  mutable DdNode* identity_bdd_;
};


/* ====================================================================== */
/* ModuleList */

/*
 * A list of modules.
 */
struct ModuleList : public std::vector<const Module*> {
};


#endif /* MODULES_H */
