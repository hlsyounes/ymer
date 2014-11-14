/*
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: states.cc,v 1.2 2003-11-07 04:26:28 lorens Exp $
 */
#include "states.h"
#include "models.h"
#include "formulas.h"
#include <cmath>


/* Generates a random number in the interval [0,1). */
static double rand01ex() {
  return rand()/(RAND_MAX + 1.0);
}


/* Generates a random number in the interval (0,1). */
static double rand0ex1ex() {
  return (rand() + 1.0)/(RAND_MAX + 2.0);
}


/* Generates an exponentially distributed random number. */
static double rand_exp() {
  return -log(rand0ex1ex());
}


/* ====================================================================== */
/* State */

/* Constructs an initial state for the given model. */
State::State(const Model& model)
  : dt_(0.0) {
  for (VariableList::const_iterator vi = model.variables().begin();
       vi != model.variables().end(); vi++) {
    values_.insert(std::make_pair(*vi, (*vi)->start()));
  }
  for (ModuleList::const_iterator mi = model.modules().begin();
       mi != model.modules().end(); mi++) {
    for (VariableList::const_iterator vi = (*mi)->variables().begin();
	 vi != (*mi)->variables().end(); vi++) {
      values_.insert(std::make_pair(*vi, (*vi)->start()));
    }
  }
}


/* Constructs a successor state for the given state. */
State::State(const State& state, const Command* command, double dt)
  : values_(state.values()), dt_(dt) {
  if (command != NULL) {
    for (UpdateList::const_iterator ui = command->updates().begin();
	 ui != command->updates().end(); ui++) {
      values_[&(*ui)->variable()] = (*ui)->expr().value(state.values());
    }
  }
}


/* Returns a sampled successor of this state. */
const State& State::next(const Model& model) const {
  /* The earliest command. */
  const Command* first_command = NULL;
  /* Time of the earliest command. */
  double first_time = HUGE_VAL;
  /* Number of commands with the same trigger time. */
  int streak = 1;
  /* Random number used to break ties (only set if needed). */
  double tie_breaker = -1.0;
  for (CommandList::const_iterator ci = model.commands().begin();
       ci != model.commands().end(); ci++) {
    const Command& command = **ci;
    if (command.guard().holds(values())) {
      double rate = command.rate().value(values()).double_value();
      double t = rand_exp()/rate;
      if (first_command != NULL && t == first_time) {
	streak++;
	if (tie_breaker < 0.0) {
	  tie_breaker = rand01ex();
	}
      } else {
	streak = 1;
      }
      if (t < first_time || (t == first_time && tie_breaker < 1.0/streak)) {
	first_command = &command;
	first_time = t;
      }
    }
  }
  if (first_command != NULL) {
    return *new State(*this, first_command, first_time);
  } else {
    return *new State(*this, NULL, HUGE_VAL);
  }
}


/* Prints this object on the given stream. */
void State::print(std::ostream& os) const {
  ValueMap::const_iterator vi = values().begin();
  (*vi).first->print(os);
  os << '=' << (*vi).second;
  for (vi++; vi != values().end(); vi++) {
    os << " & ";
    (*vi).first->print(os);
    os << '=' << (*vi).second;
  }
}
