/*
 * Copyright (C) 2003--2005 Carnegie Mellon University
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
 * $Id: states.cc,v 4.1 2005-02-01 14:18:09 lorens Exp $
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

  /* Returns a copy of this state with resampled trigger times. */
  virtual const State& resampled(const Model& model) const;

private:
  /* A mapping from commands to times. */
  struct CommandTimeMap : public std::map<const Command*, double> {
  };

  /* Time spent in the previous state. */
  double dt_;
  /* Lifetimes for currently enabled commands. */
  CommandTimeMap lifetimes_;
  /* Sampled trigger times for enabled commands. */
  CommandTimeMap trigger_times_;
  /* Command with earliest sampled trigger time. */
  const Command* trigger_;
  /* The earliest sampled trigger time. */
  double trigger_time_;

  friend const State& State::next(const Model& model) const;
  friend const State& State::resampled(const Model& model) const;
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
    if (!c->delay().memoryless()) {
      next_state->lifetimes_.insert(std::make_pair(c, 0.0));
      next_state->trigger_times_.insert(std::make_pair(c, t));
    }
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


/* Returns a copy of this state with resampled trigger times. */
const State& State::resampled(const Model& model) const {
  CommandList commands;
  for (CommandList::const_iterator ci = model.commands().begin();
       ci != model.commands().end(); ci++) {
    const Command* c = *ci;
    if (c->guard().holds(values())) {
      commands.push_back(c);
    }
  }
  ExtendedState* new_state = new ExtendedState(values(), 0.0);
  int streak = 1;
  double tie_breaker = -1.0;
  for (CommandList::const_iterator ci = commands.begin();
       ci != commands.end(); ci++) {
    const Command* c = *ci;
    double t = c->delay().sample(values());
    if (!c->delay().memoryless()) {
      new_state->lifetimes_.insert(std::make_pair(c, 0.0));
      new_state->trigger_times_.insert(std::make_pair(c, t));
    }
    if (new_state->trigger_ != NULL && t == new_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = genrand_real2_id(Distribution::mts);
      }
    } else if (t < new_state->trigger_time_) {
      streak = 1;
    }
    if (t < new_state->trigger_time_
	|| (t == new_state->trigger_time_ && tie_breaker < 1.0/streak)) {
      new_state->trigger_ = c;
      new_state->trigger_time_ = t;
    }
  }
  return *new_state;
}


/* Prints this object on the given stream. */
void State::print(std::ostream& os) const {
  ValueMap::const_iterator vi = values().begin();
  os << *(*vi).first << '=' << (*vi).second;
  for (vi++; vi != values().end(); vi++) {
    os << " & " << *(*vi).first << '=' << (*vi).second;
  }
}


/* Constructs an extended state. */
ExtendedState::ExtendedState(const ValueMap& values, double dt)
  : State(values), dt_(dt), trigger_(NULL), trigger_time_(HUGE_VAL) {}


/* Returns a sampled successor of this state. */
const State& ExtendedState::next(const Model& model) const {
  if (verbosity > 2 && StateFormula::formula_level() == 1) {
    for (CommandTimeMap::const_iterator ti = trigger_times_.begin();
	 ti != trigger_times_.end(); ti++) {
      std::cout << "  " << *(*ti).first << " @ " << (*ti).second << std::endl;
    }
  }
  ExtendedState* next_state = new ExtendedState(values(), trigger_time_);
  if (trigger_ != NULL) {
    for (UpdateList::const_iterator ui = trigger_->updates().begin();
	 ui != trigger_->updates().end(); ui++) {
      ValueMap::iterator vi = next_state->values().find(&(*ui)->variable());
      vi->second = (*ui)->expr().value(values());
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
      double l, t;
      if (c == trigger_) {
	l = 0.0;
	t = c->delay().sample(next_state->values());
      } else {
	CommandTimeMap::const_iterator li = lifetimes_.find(c);
	if (li != lifetimes_.end()) {
	  l = (*li).second + trigger_time_;
	} else {
	  l = 0.0;
	}
	CommandTimeMap::const_iterator ti = trigger_times_.find(c);
	if (ti != trigger_times_.end()) {
	  t = (*ti).second - trigger_time_;
	} else {
	  t = c->delay().sample(next_state->values());
	}
      }
      if (!c->delay().memoryless()) {
	next_state->lifetimes_.insert(std::make_pair(c, l));
	next_state->trigger_times_.insert(std::make_pair(c, t));
      }
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


/* Returns a copy of this state with resampled trigger times. */
const State& ExtendedState::resampled(const Model& model) const {
  ExtendedState* new_state = new ExtendedState(values(), dt());
  new_state->lifetimes_ = lifetimes_;
  int streak = 1;
  double tie_breaker = -1.0;
  for (CommandTimeMap::const_iterator ti = trigger_times_.begin();
       ti != trigger_times_.end(); ti++) {
    const Command* c = (*ti).first;
    double t;
    CommandTimeMap::const_iterator li = lifetimes_.find(c);
    if (li != lifetimes_.end()) {
      double l = (*li).second;
      do {
	t = c->delay().sample(values()) - l;
      } while (t <= 0.0);
    } else {
      t = c->delay().sample(values());
    }
    new_state->trigger_times_.insert(std::make_pair(c, t));
    if (new_state->trigger_ != NULL && t == new_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = genrand_real2_id(Distribution::mts);
      }
    } else if (t < new_state->trigger_time_) {
      streak = 1;
    }
    if (t < new_state->trigger_time_
	|| (t == new_state->trigger_time_ && tie_breaker < 1.0/streak)) {
      new_state->trigger_ = c;
      new_state->trigger_time_ = t;
    }
  }
  return *new_state;
}
