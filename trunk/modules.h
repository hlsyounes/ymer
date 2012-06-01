/* -*-C++-*- */
/*
 * Modules.
 *
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
 * $Id: modules.h,v 2.1 2004-01-25 12:38:08 lorens Exp $
 */
#ifndef MODULES_H
#define MODULES_H

#include <config.h>
#include "src/expression.h"
#include <set>
#include <vector>

struct StateFormula;
struct Distribution;


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
  const Update& substitution(
      const std::map<std::string, TypedValue>& constant_values) const;

  /* Returns this update subject to the given substitutions. */
  const Update& substitution(
      const std::map<std::string, const Variable*>& substitutions) const;

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
struct SynchSubstitutionMap : public std::map<size_t, size_t> {
};


/* ====================================================================== */
/* Command */

/*
 * A module command.
 */
struct Command {
  /* Constructs a command. */
  Command(size_t synch, const StateFormula& guard, const Distribution& delay);

  /* Deletes this command. */
  ~Command();

  /* Adds an update to this command. */
  void add_update(const Update& update);

  /* Returns the synchronization for this command; 0 if this command
     requires no synchronization. */
  size_t synch() const { return synch_; }

  /* Returns the guard for this command. */
  const StateFormula& guard() const { return *guard_; }

  /* Returns the delay distribution for this command. */
  const Distribution& delay() const { return *delay_; }

  /* Returns the updates for this command. */
  const UpdateList& updates() const { return updates_; }

  /* Returns this command subject to the given substitutions. */
  const Command& substitution(
      const std::map<std::string, TypedValue>& constant_values,
      const std::map<std::string, TypedValue>& rate_values) const;

  /* Returns this command subject to the given substitutions. */
  const Command& substitution(
      const std::map<std::string, const Variable*>& substitutions,
      const SynchSubstitutionMap& synchs) const;

private:
  /* The synchronization for this command; 0 if this command requires
     no synchronization. */
  size_t synch_;
  /* The guard for this command. */
  const StateFormula* guard_;
  /* The rate for this command. */
  const Distribution* delay_;
  /* The updates for this command. */
  UpdateList updates_;
};

/* Output operator for commands. */
std::ostream& operator<<(std::ostream& os, const Command& c);


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
  void compile(const std::map<std::string, TypedValue>& constant_values,
               const std::map<std::string, TypedValue>& rate_values);

  /* Returns the variables for this module. */
  const std::vector<const Variable*>& variables() const { return variables_; }

  /* Returns the commands for this module. */
  const CommandList& commands() const { return commands_; }

  /* Returns this module subject to the given substitutions. */
  Module& substitution(
      const std::map<std::string, const Variable*>& substitutions,
      const SynchSubstitutionMap& synchs) const;

private:
  /* The variables for this module. */
  std::vector<const Variable*> variables_;
  /* The commands for this module. */
  CommandList commands_;
};


/* ====================================================================== */
/* ModuleList */

/*
 * A list of modules.
 */
struct ModuleList : public std::vector<const Module*> {
};


/* ====================================================================== */
/* ModuleSet */

/*
 * A set of modules.
 */
struct ModuleSet : public std::set<const Module*> {
};


#endif /* MODULES_H */
