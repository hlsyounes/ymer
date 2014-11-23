// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2012 Google Inc
//
// This file is part of Ymer.
//
// Ymer is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Ymer is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ymer; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#include "states.h"

#include <limits>

namespace {

const int kNoTrigger = -1;

}  // namespace

State Next(const CompiledModel& model, const State& state,
           CompiledExpressionEvaluator* evaluator,
           CompiledDistributionSampler<DCEngine>* sampler) {
  State next_state(state);
  const int num_commands = model.commands().size();
  int trigger = kNoTrigger;
  double trigger_time = std::numeric_limits<double>::infinity();
  int streak = 0;
  for (int i = 0; i < num_commands; ++i) {
    const CompiledCommand& command = model.commands()[i];
    const CompiledOutcome& outcome = command.outcomes()[0];
    if (evaluator->EvaluateIntExpression(command.guard(), state.values())) {
      double t;
      if (outcome.delay().type() != DistributionType::MEMORYLESS) {
        t = state.trigger_times()[outcome.first_index()];
        if (t == std::numeric_limits<double>::infinity()) {
          t = sampler->Sample(outcome.delay(), state.values()) + state.time();
          next_state.set_trigger_time(outcome.first_index(), t);
        }
      } else {
        t = sampler->Sample(outcome.delay(), state.values()) + state.time();
      }
      if (trigger == kNoTrigger || t < trigger_time) {
        streak = 1;
        trigger = i;
        trigger_time = t;
      } else if (t == trigger_time) {
        ++streak;
        if (sampler->StandardUniform() * streak < 1.0) {
          trigger = i;
        }
      }
    } else if (outcome.delay().type() != DistributionType::MEMORYLESS) {
      next_state.set_trigger_time(outcome.first_index(),
                                  std::numeric_limits<double>::infinity());
    }
  }
  next_state.set_time(trigger_time);
  if (trigger != kNoTrigger) {
    const CompiledCommand& trigger_command = model.commands()[trigger];
    const CompiledOutcome& trigger_outcome = trigger_command.outcomes()[0];
    for (const CompiledUpdate& update : trigger_outcome.updates()) {
      next_state.set_value(
          update.variable(),
          evaluator->EvaluateIntExpression(update.expr(), state.values()));
    }
    if (trigger_outcome.delay().type() != DistributionType::MEMORYLESS) {
      next_state.set_trigger_time(trigger_outcome.first_index(),
                                  std::numeric_limits<double>::infinity());
    }
  }
  return next_state;
}
