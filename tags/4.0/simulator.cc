/*
 * Discrete-event simulator.
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
#include "simulator.h"
#include "distributions.h"
#include "formulas.h"
#include "verbosity.h"
#include "rng.h"
#include <cmath>


/* A mapping from commands to times. */
struct CommandTimeMap : public std::map<const Command*, double> {
};


/* ====================================================================== */
/* State */

/*
 * An extended simulation state.
 */
struct ExtendedState : public State {
  /* Constructs an extended state. */
  explicit ExtendedState(const Values& values);

  /* Schedules the given commands. */
  void schedule(const CommandList& commands,
		const Command* trigger, double time,
		const CommandTimeMap& lifetimes,
		const CommandTimeMap& trigger_times);

  /* Returns a sampled successor of this state. */
  virtual const State& next(const Command*& trigger, double& time,
			    const Model& model) const;

  /* Returns a copy of this state with resampled trigger times. */
  virtual const State& resampled(const Model& model) const;

private:
  /* Lifetimes for currently enabled commands. */
  CommandTimeMap lifetimes_;
  /* Sampled trigger times for enabled commands. */
  CommandTimeMap trigger_times_;
  /* Command with earliest sampled trigger time. */
  const Command* trigger_;
  /* The earliest sampled trigger time. */
  double trigger_time_;
};


/* Constructs an initial state for the given model. */
State::State(const Model& model) {
  for (VariableList<int>::const_iterator vi = model.int_variables().begin();
       vi != model.int_variables().end(); vi++) {
    values_.insert(**vi, (*vi)->init().value(Values()));
  }
  for (VariableList<double>::const_iterator vi =
	 model.double_variables().begin();
       vi != model.double_variables().end(); vi++) {
    values_.insert(**vi, (*vi)->init().value(Values()));
  }
  for (ModuleList::const_iterator mi = model.modules().begin();
       mi != model.modules().end(); mi++) {
    for (VariableList<int>::const_iterator vi = (*mi)->int_variables().begin();
	 vi != (*mi)->int_variables().end(); vi++) {
      values_.insert(**vi, (*vi)->init().value(Values()));
    }
    for (VariableList<double>::const_iterator vi =
	   (*mi)->double_variables().begin();
	 vi != (*mi)->double_variables().end(); vi++) {
      values_.insert(**vi, (*vi)->init().value(Values()));
    }
  }
}


/* Returns a sampled successor of this state. */
const State& State::next(const Command*& trigger, double& time,
			 const Model& model) const {
  ExtendedState* next_state = new ExtendedState(values());
  next_state->schedule(model.commands(), 0, 0.0,
		       CommandTimeMap(), CommandTimeMap());
  const State& result = next_state->next(trigger, time, model);
  delete next_state;
  return result;
}


/* Returns a copy of this state with resampled trigger times. */
const State& State::resampled(const Model& model) const {
  return *this;
}


/* Output operator for states. */
std::ostream& operator<<(std::ostream& os, const State& s) {
  os << s.values();
  return os;
}


/* Constructs an extended state. */
ExtendedState::ExtendedState(const Values& values)
  : State(values), trigger_(0), trigger_time_(HUGE_VAL) {}


/* Schedules the given commands. */
void ExtendedState::schedule(const CommandList& commands,
			     const Command* trigger, double time,
			     const CommandTimeMap& lifetimes,
			     const CommandTimeMap& trigger_times) {
  int streak = 1;
  double tie_breaker = -1.0;
  for (CommandList::const_iterator ci = commands.begin();
       ci != commands.end(); ci++) {
    const Command* c = *ci;
    if (c->guard().holds(values())) {
      double l, t;
      if (c == trigger) {
	l = 0.0;
	t = c->delay().sample(values());
      } else {
	CommandTimeMap::const_iterator li = lifetimes.find(c);
	l = (li != lifetimes.end()) ? (*li).second + time : 0.0;
	CommandTimeMap::const_iterator ti = trigger_times.find(c);
	t = ((ti != trigger_times.end())
	     ? (*ti).second - time : c->delay().sample(l, values()));
      }
      if (!c->delay().memoryless()) {
	lifetimes_.insert(std::make_pair(c, l));
      }
      if (c->delay().state_invariant()) {
	trigger_times_.insert(std::make_pair(c, t));
      }
      if (verbosity.schedule) {
	std::cout << "  " << *c << " @ " << t << std::endl;
      }
      if (trigger_ != 0 && t == trigger_time_) {
	streak++;
	if (tie_breaker < 0.0) {
	  tie_breaker = Distribution::rand01ex();
	}
      } else if (t < trigger_time_) {
	streak = 1;
      }
      if (t < trigger_time_
	  || (t == trigger_time_ && tie_breaker < 1.0/streak)) {
	trigger_ = c;
	trigger_time_ = t;
      }
    }
  }
}


/* Returns a sampled successor of this state. */
const State& ExtendedState::next(const Command*& trigger, double& time,
				 const Model& model) const {
  trigger = trigger_;
  time = trigger_time_;
  ExtendedState* next_state = new ExtendedState(values());
  if (trigger_ != 0) {
    for (UpdateList<int>::const_iterator ui = trigger_->int_updates().begin();
	 ui != trigger_->int_updates().end(); ui++) {
      next_state->values().set((*ui)->variable(),
			       (*ui)->expr().value(values()));
    }
    for (UpdateList<double>::const_iterator ui =
	   trigger_->double_updates().begin();
	 ui != trigger_->double_updates().end(); ui++) {
      next_state->values().set((*ui)->variable(),
			       (*ui)->expr().value(values()));
    }
    next_state->schedule(model.commands(), trigger_, trigger_time_,
			 lifetimes_, trigger_times_);
  }
  return *next_state;
}


/* Returns a copy of this state with resampled trigger times. */
const State& ExtendedState::resampled(const Model& model) const {
  ExtendedState* new_state = new ExtendedState(values());
  new_state->lifetimes_ = lifetimes_;
  int streak = 1;
  double tie_breaker = -1.0;
  for (CommandTimeMap::const_iterator ti = trigger_times_.begin();
       ti != trigger_times_.end(); ti++) {
    const Command* c = (*ti).first;
    CommandTimeMap::const_iterator li = lifetimes_.find(c);
    double t = ((li != lifetimes_.end())
		? c->delay().sample((*li).second, values())
		: c->delay().sample(values()));
    new_state->trigger_times_.insert(std::make_pair(c, t));
    if (verbosity.schedule) {
      std::cout << "  " << *c << " @ " << t << std::endl;
    }
    if (new_state->trigger_ != 0 && t == new_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = Distribution::rand01ex();
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
