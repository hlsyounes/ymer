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

#include "src/distribution.h"
#include "src/expression.h"
#include "src/model.h"


/* ====================================================================== */
/* Command */

/*
 * A module command.
 */
struct Command {
  /* Constructs a command. */
  Command(const std::string& action, std::unique_ptr<const Expression>&& guard,
          std::unique_ptr<const Distribution>&& delay,
          std::vector<Update>&& updates);

  /* Adds an update to this command. */
  void add_update(Update&& update);

  // Returns the action label for this command, or an empty string if the
  // command does not have an action label.
  const std::string& action() const { return action_; }

  /* Returns the guard for this command. */
  const Expression& guard() const { return *guard_; }

  /* Returns the delay distribution for this command. */
  const Distribution& delay() const { return *delay_; }

  /* Returns the updates for this command. */
  const std::vector<Update>& updates() const { return updates_; }

private:
  std::string action_;
  std::unique_ptr<const Expression> guard_;
  std::unique_ptr<const Distribution> delay_;
  std::vector<Update> updates_;
};

/* Output operator for commands. */
std::ostream& operator<<(std::ostream& os, const Command& c);

#endif /* MODULES_H */
