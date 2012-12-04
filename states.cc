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
 */

#include "states.h"
#include "models.h"
#include "distributions.h"
#include "formulas.h"
#include "src/rng.h"
#include <cmath>
#include <iostream>

/* Verbosity level. */
extern int verbosity;


/* ====================================================================== */
/* State */

/*
 * An extended state.
 */
struct ExtendedState : public State {
  /* Constructs an extended state. */
  ExtendedState(const Model* model, const std::vector<int>& values,
                DCEngine* engine, double dt);

  /* Returns the time spent in the previous state. */
  virtual double dt() const { return dt_; }

  /* Returns a sampled successor of this state. */
  virtual const State& next() const;

  /* Returns a copy of this state with resampled trigger times. */
  virtual const State& resampled() const;

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

  friend const State& State::next() const;
  friend const State& State::resampled() const;
};


/* Constructs an initial state for the given model. */
State::State(const Model* model, const CompiledModel& compiled_model,
             DCEngine* engine)
    : model_(model), engine_(engine) {
  values_.reserve(compiled_model.variables().size());
  for (std::vector<CompiledVariable>::const_iterator vi =
           compiled_model.variables().begin();
       vi != compiled_model.variables().end(); vi++) {
    values_.push_back(vi->init_value());
  }
}


/* Returns a sampled successor of this state. */
const State& State::next() const {
  std::vector<const Command*> commands;
  for (const Command* command : model_->commands()) {
    if (command->guard().holds(values())) {
      commands.push_back(command);
    }
  }
  ExtendedState* next_state =
      new ExtendedState(model_, values(), engine(), 0.0);
  int streak = 1;
  double tie_breaker = -1.0;
  for (const Command* command : commands) {
    double t = command->delay().sample(values(), engine());
    if (!command->delay().memoryless()) {
      next_state->lifetimes_.insert(std::make_pair(command, 0.0));
      next_state->trigger_times_.insert(std::make_pair(command, t));
    }
    if (next_state->trigger_ != NULL && t == next_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = StandardUniform(*engine());
      }
    } else if (t < next_state->trigger_time_) {
      streak = 1;
    }
    if (t < next_state->trigger_time_
	|| (t == next_state->trigger_time_ && tie_breaker < 1.0/streak)) {
      next_state->trigger_ = command;
      next_state->trigger_time_ = t;
    }
  }
  const State& result = next_state->next();
  delete next_state;
  return result;
}


/* Returns a copy of this state with resampled trigger times. */
const State& State::resampled() const {
  std::vector<const Command*> commands;
  for (const Command* command : model_->commands()) {
    if (command->guard().holds(values())) {
      commands.push_back(command);
    }
  }
  ExtendedState* new_state = new ExtendedState(model_, values(), engine(), 0.0);
  int streak = 1;
  double tie_breaker = -1.0;
  for (const Command* command : commands) {
    double t = command->delay().sample(values(), engine());
    if (!command->delay().memoryless()) {
      new_state->lifetimes_.insert(std::make_pair(command, 0.0));
      new_state->trigger_times_.insert(std::make_pair(command, t));
    }
    if (new_state->trigger_ != NULL && t == new_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = StandardUniform(*engine());
      }
    } else if (t < new_state->trigger_time_) {
      streak = 1;
    }
    if (t < new_state->trigger_time_
	|| (t == new_state->trigger_time_ && tie_breaker < 1.0/streak)) {
      new_state->trigger_ = command;
      new_state->trigger_time_ = t;
    }
  }
  return *new_state;
}


/* Prints this object on the given stream. */
void State::print(std::ostream& os) const {
  os << model_->variable_name(0) << '=' << values()[0];
  for (size_t i = 1; i < values().size(); ++i) {
    os << " & " << model_->variable_name(i) << '=' << values()[i];
  }
}


/* Constructs an extended state. */
ExtendedState::ExtendedState(const Model* model,
                             const std::vector<int>& values, DCEngine* engine,
                             double dt)
    : State(model, values, engine), dt_(dt), trigger_(NULL),
      trigger_time_(HUGE_VAL) {}


/* Returns a sampled successor of this state. */
const State& ExtendedState::next() const {
  if (verbosity > 2 && StateFormula::formula_level() == 1) {
    for (CommandTimeMap::const_iterator ti = trigger_times_.begin();
	 ti != trigger_times_.end(); ti++) {
      std::cout << "  " << *(*ti).first << " @ " << (*ti).second << std::endl;
    }
  }
  ExtendedState* next_state =
      new ExtendedState(model(), values(), engine(), trigger_time_);
  if (trigger_ != NULL) {
    for (const Update* update : trigger_->updates()) {
      next_state->values().at(update->variable().index()) =
          update->expr().value(values()).value<int>();
    }
    std::vector<const Command*> commands;
    for (const Command* command : model()->commands()) {
      if (command->guard().holds(next_state->values())) {
	commands.push_back(command);
      }
    }
    int streak = 1;
    double tie_breaker = -1.0;
    for (const Command* command : commands) {
      double l, t;
      if (command == trigger_) {
	l = 0.0;
	t = command->delay().sample(next_state->values(), engine());
      } else {
	CommandTimeMap::const_iterator li = lifetimes_.find(command);
	if (li != lifetimes_.end()) {
	  l = (*li).second + trigger_time_;
	} else {
	  l = 0.0;
	}
	CommandTimeMap::const_iterator ti = trigger_times_.find(command);
	if (ti != trigger_times_.end()) {
	  t = (*ti).second - trigger_time_;
	} else {
	  t = command->delay().sample(next_state->values(), engine());
	}
      }
      if (!command->delay().memoryless()) {
	next_state->lifetimes_.insert(std::make_pair(command, l));
	next_state->trigger_times_.insert(std::make_pair(command, t));
      }
      if (next_state->trigger_ != NULL && t == next_state->trigger_time_) {
	streak++;
	if (tie_breaker < 0.0) {
	  tie_breaker = StandardUniform(*engine());
	}
      } else if (t < next_state->trigger_time_) {
	streak = 1;
      }
      if (t < next_state->trigger_time_
	  || (t == next_state->trigger_time_ && tie_breaker < 1.0/streak)) {
	next_state->trigger_ = command;
	next_state->trigger_time_ = t;
      }
    }
  }
  return *next_state;
}


/* Returns a copy of this state with resampled trigger times. */
const State& ExtendedState::resampled() const {
  ExtendedState* new_state =
      new ExtendedState(model(), values(), engine(), dt());
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
	t = c->delay().sample(values(), engine()) - l;
      } while (t <= 0.0);
    } else {
      t = c->delay().sample(values(), engine());
    }
    new_state->trigger_times_.insert(std::make_pair(c, t));
    if (new_state->trigger_ != NULL && t == new_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = StandardUniform(*engine());
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
