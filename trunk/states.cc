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

State::State(const CompiledModel& model,
             CompiledDistributionSampler<DCEngine>* sampler)
    : sampler_(sampler),
      time_(0.0),
      values_(model.init_values()) {}

namespace {

const int kNoTrigger = -1;

}  // namespace

State State::Next(const CompiledModel& model,
                  CompiledExpressionEvaluator* evaluator) const {
  State next_state(*this);
  const int num_commands = model.commands().size();
  int trigger = kNoTrigger;
  double trigger_time = std::numeric_limits<double>::infinity();
  int streak = 0;
  for (int i = 0; i < num_commands; ++i) {
    const CompiledCommand& command = model.commands()[i];
    const CompiledOutcome& outcome = command.outcomes()[0];
    if (evaluator->EvaluateIntExpression(command.guard(), values_)) {
      double t;
      if (outcome.delay().type() != DistributionType::MEMORYLESS) {
        auto ti = trigger_times_.find(i);
        if (ti == trigger_times_.end()) {
          t = sampler_->Sample(outcome.delay(), values_) + time_;
          next_state.trigger_times_.insert({ i, t });
        } else {
          t = ti->second;
        }
      } else {
        t = sampler_->Sample(outcome.delay(), values_) + time_;
      }
      if (trigger == kNoTrigger || t < trigger_time) {
        streak = 1;
        trigger = i;
        trigger_time = t;
      } else if (t == trigger_time) {
        ++streak;
        if (sampler_->StandardUniform() * streak < 1.0) {
          trigger = i;
        }
      }
    } else if (outcome.delay().type() != DistributionType::MEMORYLESS) {
      next_state.trigger_times_.erase(i);
    }
  }
  next_state.time_ = trigger_time;
  if (trigger != kNoTrigger) {
    const CompiledCommand& trigger_command = model.commands()[trigger];
    const CompiledOutcome& trigger_outcome = trigger_command.outcomes()[0];
    for (const CompiledUpdate& update : trigger_outcome.updates()) {
      next_state.values_[update.variable()] =
          evaluator->EvaluateIntExpression(update.expr(), values_);
    }
    if (trigger_outcome.delay().type() != DistributionType::MEMORYLESS) {
      next_state.trigger_times_.erase(trigger);
    }
  }
  return next_state;
}
