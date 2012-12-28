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

#include "formulas.h"
#include "states.h"
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
  ExtendedState(const CompiledModel* model,
                const std::vector<int>& values,
                CompiledExpressionEvaluator* evaluator,
                CompiledDistributionSampler<DCEngine>* sampler,
                double dt);

  /* Returns the time spent in the previous state. */
  virtual double dt() const { return dt_; }

  /* Returns a sampled successor of this state. */
  virtual const State& next() const;

  /* Returns a copy of this state with resampled trigger times. */
  virtual const State& resampled() const;

private:
  /* A mapping from command index to time. */
  struct CommandTimeMap : public std::map<int, double> {
  };

  /* Time spent in the previous state. */
  double dt_;
  /* Lifetimes for currently enabled commands. */
  CommandTimeMap lifetimes_;
  /* Sampled trigger times for enabled commands. */
  CommandTimeMap trigger_times_;
  /* Index of command with earliest sampled trigger time. */
  int trigger_;
  /* The earliest sampled trigger time. */
  double trigger_time_;

  friend const State& State::next() const;
  friend const State& State::resampled() const;
};

namespace {

const int kNoTrigger = -1;

}  // namespace

/* Constructs an initial state for the given model. */
State::State(const CompiledModel* model,
             CompiledExpressionEvaluator* evaluator,
             CompiledDistributionSampler<DCEngine>* sampler)
    : model_(model), evaluator_(evaluator), sampler_(sampler) {
  values_.reserve(model->variables().size());
  for (const CompiledVariable& v : model->variables()) {
    values_.push_back(v.init_value());
  }
}


/* Returns a sampled successor of this state. */
const State& State::next() const {
  ExtendedState* next_state = new ExtendedState(
      model_, values(), evaluator_, sampler(), 0.0);
  int streak = 1;
  double tie_breaker = -1.0;
  const int num_commands = model_->commands().size();
  for (int i = 0; i < num_commands; ++i) {
    const CompiledCommand& command = model_->commands()[i];
    const CompiledOutcome& outcome = command.outcomes()[0];
    if (!evaluator_->EvaluateIntExpression(command.guard(), values_)) {
      continue;
    }
    double t = sampler_->Sample(outcome.delay(), values_);
    if (outcome.delay().type() != DistributionType::MEMORYLESS) {
      next_state->lifetimes_.insert({ i, 0.0 });
      next_state->trigger_times_.insert({ i, t });
    }
    if (next_state->trigger_ != kNoTrigger && t == next_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = sampler_->StandardUniform();
      }
    } else if (t < next_state->trigger_time_) {
      streak = 1;
    }
    if (t < next_state->trigger_time_
	|| (t == next_state->trigger_time_ && tie_breaker < 1.0/streak)) {
      next_state->trigger_ = i;
      next_state->trigger_time_ = t;
    }
  }
  const State& result = next_state->next();
  delete next_state;
  return result;
}


/* Returns a copy of this state with resampled trigger times. */
const State& State::resampled() const {
  ExtendedState* new_state = new ExtendedState(
      model_, values(), evaluator_, sampler(), 0.0);
  int streak = 1;
  double tie_breaker = -1.0;
  const int num_commands = model_->commands().size();
  for (int i = 0; i < num_commands; ++i) {
    const CompiledCommand& command = model_->commands()[i];
    const CompiledOutcome& outcome = command.outcomes()[0];
    if (!evaluator_->EvaluateIntExpression(command.guard(), values_)) {
      continue;
    }
    double t = sampler_->Sample(outcome.delay(), values_);
    if (outcome.delay().type() != DistributionType::MEMORYLESS) {
      new_state->lifetimes_.insert({ i, 0.0 });
      new_state->trigger_times_.insert({ i, t });
    }
    if (new_state->trigger_ != kNoTrigger && t == new_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = sampler_->StandardUniform();
      }
    } else if (t < new_state->trigger_time_) {
      streak = 1;
    }
    if (t < new_state->trigger_time_
	|| (t == new_state->trigger_time_ && tie_breaker < 1.0/streak)) {
      new_state->trigger_ = i;
      new_state->trigger_time_ = t;
    }
  }
  return *new_state;
}


/* Prints this object on the given stream. */
void State::print(std::ostream& os) const {
  os << model_->variables()[0].name() << '=' << values()[0];
  for (size_t i = 1; i < values().size(); ++i) {
    os << " & " << model_->variables()[i].name() << '=' << values()[i];
  }
}


/* Constructs an extended state. */
ExtendedState::ExtendedState(const CompiledModel* model,
                             const std::vector<int>& values,
                             CompiledExpressionEvaluator* evaluator,
                             CompiledDistributionSampler<DCEngine>* sampler,
                             double dt)
    : State(model, values, evaluator, sampler),
      dt_(dt), trigger_(kNoTrigger), trigger_time_(HUGE_VAL) {}


/* Returns a sampled successor of this state. */
const State& ExtendedState::next() const {
  if (verbosity > 2 && StateFormula::formula_level() == 1) {
    for (auto ti = trigger_times_.begin(); ti != trigger_times_.end(); ti++) {
      std::cout << "  " << ti->first << " @ " << ti->second << std::endl;
    }
  }
  ExtendedState* next_state = new ExtendedState(
      model(), values(), evaluator(), sampler(), trigger_time_);
  if (trigger_ != kNoTrigger) {
    const CompiledCommand& trigger_command = model()->commands()[trigger_];
    const CompiledOutcome& trigger_outcome = trigger_command.outcomes()[0];
    for (const CompiledUpdate& update : trigger_outcome.updates()) {
      next_state->values()[update.variable()] =
          evaluator()->EvaluateIntExpression(update.expr(), values());
    }
    int streak = 1;
    double tie_breaker = -1.0;
    const int num_commands = model()->commands().size();
    for (int i = 0; i < num_commands; ++i) {
      const CompiledCommand& command = model()->commands()[i];
      const CompiledOutcome& outcome = command.outcomes()[0];
      if (!evaluator()->EvaluateIntExpression(command.guard(),
                                              next_state->values())) {
        continue;
      }
      double l, t;
      if (i == trigger_) {
	l = 0.0;
	t = sampler()->Sample(outcome.delay(), next_state->values());
      } else {
	auto li = lifetimes_.find(i);
	if (li != lifetimes_.end()) {
	  l = (*li).second + trigger_time_;
	} else {
	  l = 0.0;
	}
	auto ti = trigger_times_.find(i);
	if (ti != trigger_times_.end()) {
	  t = (*ti).second - trigger_time_;
	} else {
	  t = sampler()->Sample(outcome.delay(), next_state->values());
	}
      }
      if (outcome.delay().type() != DistributionType::MEMORYLESS) {
	next_state->lifetimes_.insert({ i, l });
	next_state->trigger_times_.insert({ i, t });
      }
      if (next_state->trigger_ != kNoTrigger && t == next_state->trigger_time_) {
	streak++;
	if (tie_breaker < 0.0) {
	  tie_breaker = sampler()->StandardUniform();
	}
      } else if (t < next_state->trigger_time_) {
	streak = 1;
      }
      if (t < next_state->trigger_time_
	  || (t == next_state->trigger_time_ && tie_breaker < 1.0/streak)) {
	next_state->trigger_ = i;
	next_state->trigger_time_ = t;
      }
    }
  }
  return *next_state;
}


/* Returns a copy of this state with resampled trigger times. */
const State& ExtendedState::resampled() const {
  ExtendedState* new_state = new ExtendedState(
      model(), values(), evaluator(), sampler(), dt());
  new_state->lifetimes_ = lifetimes_;
  int streak = 1;
  double tie_breaker = -1.0;
  for (auto ti = trigger_times_.begin(); ti != trigger_times_.end(); ti++) {
    const int i = ti->first;
    const CompiledCommand& command = model()->commands()[i];
    const CompiledOutcome& outcome = command.outcomes()[0];
    double t;
    auto li = lifetimes_.find(i);
    if (li != lifetimes_.end()) {
      double l = (*li).second;
      do {
	t = sampler()->Sample(outcome.delay(), values()) - l;
      } while (t <= 0.0);
    } else {
      t = sampler()->Sample(outcome.delay(), values());
    }
    new_state->trigger_times_.insert({ i, t });
    if (new_state->trigger_ != kNoTrigger && t == new_state->trigger_time_) {
      streak++;
      if (tie_breaker < 0.0) {
	tie_breaker = sampler()->StandardUniform();
      }
    } else if (t < new_state->trigger_time_) {
      streak = 1;
    }
    if (t < new_state->trigger_time_
	|| (t == new_state->trigger_time_ && tie_breaker < 1.0/streak)) {
      new_state->trigger_ = i;
      new_state->trigger_time_ = t;
    }
  }
  return *new_state;
}
