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
 * $Id: states.cc,v 2.1 2004-01-25 12:43:28 lorens Exp $
 */
#include "states.h"
#include "models.h"
#include "distributions.h"
#include "formulas.h"
#include "rng.h"
#include <cmath>


/* Verbosity level. */
extern int verbosity;


/* ====================================================================== */
/* State */

/*
 * An extended state.
 */
struct ExtendedState : public State {
  /* Constructs an extended state. */
  ExtendedState(const ValueMap& values, double dt);

  /* Returns the time spent in the previous state. */
  virtual double dt() const { return dt_; }

  /* Returns a sampled successor of this state. */
  virtual const State& next(const Model& model) const;

private:
  /* A mapping from commands to lifetimes. */
  struct LifetimeMap : public std::map<const Command*, double> {
  };

  /* Time spent in the previous state. */
  double dt_;
  /* Sampled lifetimes for enabled commands. */
  LifetimeMap lifetimes_;
  /* Command with shortest sampled lifetime. */
  const Command* trigger_;
  /* The shortest sampled lifetime. */
  double trigger_time_;

  friend const State& State::next(const Model& model) const;
};


/* Constructs an initial state for the given model. */
State::State(const Model& model) {
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


/* Returns a sampled successor of this state. */
const State& State::next(const Model& model) const {
  CommandList commands;
  for (CommandList::const_iterator ci = model.commands().begin();
       ci != model.commands().end(); ci++) {
    const Command* c = *ci;
    if (c->guard().holds(values())) {
      commands.push_back(c);
    }
  }
  ExtendedState* next_state = new ExtendedState(values(), 0.0);
  int streak = 1;
  double tie_breaker = -1.0;
  for (CommandList::const_iterator ci = commands.begin();
       ci != commands.end(); ci++) {
    const Command* c = *ci;
    double t = c->delay().sample(values());
    next_state->lifetimes_.insert(std::make_pair(c, t));
    if (next_state->trigger_ != NULL && t == next_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = genrand_real2_id(Distribution::mts);
      }
    } else if (t < next_state->trigger_time_) {
      streak = 1;
    }
    if (t < next_state->trigger_time_
	|| (t == next_state->trigger_time_ && tie_breaker < 1.0/streak)) {
      next_state->trigger_ = c;
      next_state->trigger_time_ = t;
    }
  }
  const State& result = next_state->next(model);
  delete next_state;
  return result;
}


/* Prints this object on the given stream. */
void State::print(std::ostream& os) const {
  ValueMap::const_iterator vi = values().begin();
  os << (*vi).first << '=' << (*vi).second;
  for (vi++; vi != values().end(); vi++) {
    os << " & " << (*vi).first << '=' << (*vi).second;
  }
}


/* Constructs an extended state. */
ExtendedState::ExtendedState(const ValueMap& values, double dt)
  : State(values), dt_(dt), trigger_(NULL), trigger_time_(HUGE_VAL) {}


/* Returns a sampled successor of this state. */
const State& ExtendedState::next(const Model& model) const {
  if (verbosity > 2) {
    for (LifetimeMap::const_iterator li = lifetimes_.begin();
	 li != lifetimes_.end(); li++) {
      std::cout << "  " << *(*li).first << " @ " << (*li).second << std::endl;
    }
  }
  ExtendedState* next_state = new ExtendedState(values(), trigger_time_);
  if (trigger_ != NULL) {
    for (UpdateList::const_iterator ui = trigger_->updates().begin();
	 ui != trigger_->updates().end(); ui++) {
      next_state->values()[&(*ui)->variable()] = (*ui)->expr().value(values());
    }
    CommandList commands;
    for (CommandList::const_iterator ci = model.commands().begin();
	 ci != model.commands().end(); ci++) {
      const Command* c = *ci;
      if (c->guard().holds(next_state->values())) {
	commands.push_back(c);
      }
    }
    int streak = 1;
    double tie_breaker = -1.0;
    for (CommandList::const_iterator ci = commands.begin();
	 ci != commands.end(); ci++) {
      const Command* c = *ci;
      double t;
      if (c == trigger_) {
	t = c->delay().sample(next_state->values());
      } else {
	LifetimeMap::const_iterator ti = lifetimes_.find(c);
	if (ti != lifetimes_.end()) {
	  t = (*ti).second - trigger_time_;
	} else {
	  t = c->delay().sample(next_state->values());
	}
      }
      next_state->lifetimes_.insert(std::make_pair(c, t));
      if (next_state->trigger_ != NULL && t == next_state->trigger_time_) {
	streak++;
	if (tie_breaker < 0.0) {
	  tie_breaker = genrand_real2_id(Distribution::mts);
	}
      } else if (t < next_state->trigger_time_) {
	streak = 1;
      }
      if (t < next_state->trigger_time_
	  || (t == next_state->trigger_time_ && tie_breaker < 1.0/streak)) {
	next_state->trigger_ = c;
	next_state->trigger_time_ = t;
      }
    }
  }
  return *next_state;
}
