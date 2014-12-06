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
#include "src/distribution.h"
#include "src/expression.h"

Command::Command(size_t synch, const std::string& action,
                 std::unique_ptr<const Expression>&& guard,
                 std::unique_ptr<const Distribution>&& delay,
                 std::vector<Update>&& updates)
    : synch_(synch),
      action_(action),
      guard_(std::move(guard)),
      delay_(std::move(delay)),
      updates_(std::move(updates)) {}

void Command::add_update(Update&& update) {
  updates_.push_back(std::move(update));
}

std::ostream& operator<<(std::ostream& os, const Command& c) {
  os << "[" << c.action() << "] " << c.guard() << " -> " << c.delay() << " : ";
  auto ui = c.updates().begin();
  if (ui != c.updates().end()) {
    const Update& u0 = *ui;
    os << u0.variable() << "\'=" << u0.expr();
    for (ui++; ui != c.updates().end(); ui++) {
      const Update& u = *ui;
      os << " & " << u.variable() << "\'=" << u.expr();
    }
  }
  return os;
}

/* ====================================================================== */
/* Module */

/* Constructs a module. */
Module::Module() {}

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
