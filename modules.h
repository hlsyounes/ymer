/* -*-C++-*- */
/*
 * Modules.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011 Google Inc
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
 */
#ifndef MODULES_H
#define MODULES_H

#include <config.h>
#include "distributions.h"
#include "formulas.h"
#include "expressions.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>


/* ====================================================================== */
/* Update */

/*
 * A variable update.
 */
template<typename T>
struct Update {
  /* Constructs a variable update. */
  Update(const Variable<T>& variable, const Expression<T>& expr);

  /* Deletes this variable update. */
  ~Update();

  /* Returns the variable for this update. */
  const Variable<T>& variable() const { return *variable_; }

  /* Returns the expression for this update. */
  const Expression<T>& expr() const { return *expr_; }

  /* Returns this update subject to the given substitutions. */
  const Update<T>& substitution(const Substitutions& subst) const;

private:
  /* The variable for this update. */
  const Variable<T>* variable_;
  /* The expression for this update. */
  const Expression<T>* expr_;
};


/* ====================================================================== */
/* UpdateList */

template<typename T>
struct UpdateList : public std::vector<const Update<T>*> {
};


/* ====================================================================== */
/* SynchSubstitutions */

/*
 * Substitution map for synchronizations.
 */
struct SynchSubstitutions : public std::map<int, int> {
};


/* ====================================================================== */
/* Command */

/*
 * A module command.
 */
struct Command {
  /* Constructs a command. */
  Command(int synch, const StateFormula& guard, const Distribution& delay);

  /* Deletes this command. */
  ~Command();

  /* Adds an integer-valued update to this command. */
  void add_update(const Update<int>& update);

  /* Adds a double-valued update to this command. */
  void add_update(const Update<double>& update);

  /* Returns the synchronization for this command; 0 if this command
     requires no synchronization. */
  int synch() const { return synch_; }

  /* Returns the guard for this command. */
  const StateFormula& guard() const { return *guard_; }

  /* Returns the delay distribution for this command. */
  const Distribution& delay() const { return *delay_; }

  /* Returns the integer-valued updates for this command. */
  const UpdateList<int>& int_updates() const { return int_updates_; }

  /* Returns the double-valued updates for this command. */
  const UpdateList<double>& double_updates() const { return double_updates_; }

  /* Returns this command subject to the given substitutions. */
  const Command& substitution(const Substitutions& subst,
			      const SynchSubstitutions& synchs) const;

private:
  /* The synchronization for this command; 0 if this command requires
     no synchronization. */
  int synch_;
  /* The guard for this command. */
  const StateFormula* guard_;
  /* The rate for this command. */
  const Distribution* delay_;
  /* The integer-valued updates for this command. */
  UpdateList<int> int_updates_;
  /* The double-valued updates for this command. */
  UpdateList<double> double_updates_;
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
/* VariableList */

template<typename T>
struct VariableList : public std::vector<const Variable<T>*> {
};


/* ====================================================================== */
/* Module */

/*
 * A module.
 */
struct Module {
  /* Constructs a module with the given name. */
  Module(const std::string& name) : name_(name) {}

  /* Deletes this module. */
  ~Module();

  /* Adds an integer-valued variable to this module. */
  void add_variable(const Variable<int>& variable);

  /* Adds a double-valued variable to this module. */
  void add_variable(const Variable<double>& variable);

  /* Adds a command to this module. */
  void add_command(const Command& command);

  /* Returns the name for this module. */
  const std::string& name() const { return name_; }

  /* Returns the integer-valued variables for this module. */
  const VariableList<int>& int_variables() const { return int_variables_; }

  /* Returns the double-valued variables for this module. */
  const VariableList<double>& double_variables() const {
    return double_variables_;
  }

  /* Returns the commands for this module. */
  const CommandList& commands() const { return commands_; }

  /* Returns this module subject to the given substitutions. */
  Module& substitution(const std::string& new_name,
		       const Substitutions& subst,
		       const SynchSubstitutions& synchs) const;

private:
  /* Module name. */
  std::string name_;
  /* The integer-valued variables for this module. */
  VariableList<int> int_variables_;
  /* The double-valued variables for this module. */
  VariableList<double> double_variables_;
  /* The commands for this module. */
  CommandList commands_;
};

/* Output operator for modules. */
std::ostream& operator<<(std::ostream& os, const Module& m);


/* ====================================================================== */
/* ModuleList */

/*
 * A list of modules.
 */
struct ModuleList : public std::vector<const Module*> {
};


#endif /* MODULES_H */
