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
 */

#ifndef MODULES_H
#define MODULES_H

#include <map>
#include <set>
#include <vector>

#include "src/expression.h"

class Distribution;


/* ====================================================================== */
/* Update */

/*
 * A variable update.
 */
struct Update {
  /* Constructs a variable update. */
  Update(const std::string& variable, std::unique_ptr<const Expression>&& expr);

  /* Returns the variable for this update. */
  const std::string& variable() const { return variable_; }

  /* Returns the expression for this update. */
  const Expression& expr() const { return *expr_; }

private:
  /* The variable for this update. */
  std::string variable_;
  /* The expression for this update. */
  std::unique_ptr<const Expression> expr_;
};


/* ====================================================================== */
/* Command */

/*
 * A module command.
 */
struct Command {
  /* Constructs a command. */
  Command(size_t synch, std::unique_ptr<const Expression>&& guard,
          const Distribution* delay);

  /* Deletes this command. */
  ~Command();

  /* Adds an update to this command. */
  void add_update(const Update* update);

  /* Returns the synchronization for this command; 0 if this command
     requires no synchronization. */
  size_t synch() const { return synch_; }

  /* Returns the guard for this command. */
  const Expression& guard() const { return *guard_; }

  /* Returns the delay distribution for this command. */
  const Distribution& delay() const { return *delay_; }

  /* Returns the updates for this command. */
  const std::vector<const Update*>& updates() const { return updates_; }

private:
  // Disallow copy and assign.
  Command(const Command&);
  Command& operator=(const Command&);

  /* The synchronization for this command; 0 if this command requires
     no synchronization. */
  size_t synch_;
  /* The guard for this command. */
  std::unique_ptr<const Expression> guard_;
  /* The rate for this command. */
  const Distribution* delay_;
  /* The updates for this command. */
  std::vector<const Update*> updates_;
};

/* Output operator for commands. */
std::ostream& operator<<(std::ostream& os, const Command& c);


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
  void add_variable(const std::string& variable);

  /* Adds a command to this module. */
  void add_command(const Command* command);

  /* Substitutes constants with values. */
  void compile(const std::map<std::string, TypedValue>& constant_values,
               const std::map<std::string, TypedValue>& rate_values);

  /* Returns the variables for this module. */
  const std::vector<std::string>& variables() const { return variables_; }

  /* Returns the commands for this module. */
  const std::vector<const Command*>& commands() const { return commands_; }

private:
  /* The variables for this module. */
  std::vector<std::string> variables_;
  /* The commands for this module. */
  std::vector<const Command*> commands_;
};

#endif /* MODULES_H */
