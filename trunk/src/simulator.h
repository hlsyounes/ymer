// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2014 Google Inc
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
//
// Discrete-event simulator for PRISM models.

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <limits>
#include <vector>

#include "compiled-distribution.h"
#include "compiled-expression.h"
#include "compiled-model.h"

// A simulator state.
class State {
 public:
  // Constructs an intial state for the given model.
  explicit State(const CompiledModel& model)
      : time_(0.0),
        values_(model.init_values()),
        trigger_times_(model.gsmp_event_count(),
                       std::numeric_limits<double>::infinity()) {}

  // Sets the current time for this state.
  void set_time(double time) { time_ = time; }

  // Sets the variable values for this state.
  void set_values(const std::vector<int>& values) { values_ = values; }

  // Sets the variable value for the given variable index.
  void set_value(int index, int value) { values_[index] = value; }

  // Resets the trigger times for GSMP events.
  void reset_trigger_times() {
    fill(trigger_times_.begin(), trigger_times_.end(),
         std::numeric_limits<double>::infinity());
  }

  // Sets the trigger time for the GSMP event with the given index.
  void set_trigger_time(int index, double trigger_time) {
    trigger_times_[index] = trigger_time;
  }

  // Swaps with the given state.
  void swap(State& state) {
    std::swap(time_, state.time_);
    values_.swap(state.values_);
    trigger_times_.swap(state.trigger_times_);
  }

  // Returns the current time for this state.
  double time() const { return time_; }

  // Returns the variable values for this state.
  const std::vector<int>& values() const { return values_; }

  // Returns the trigger times for GSMP events.
  const std::vector<double>& trigger_times() const { return trigger_times_; }

 private:
  double time_;
  std::vector<int> values_;
  std::vector<double> trigger_times_;
};

template <typename Engine>
class NextStateSampler {
 public:
  explicit NextStateSampler(const CompiledModel* model,
                            CompiledExpressionEvaluator* evaluator,
                            CompiledDistributionSampler<Engine>* sampler);

  void NextState(const State& state, State* next_state);

 private:
  double GetMarkovCommandWeight(const State& state,
                                const CompiledMarkovCommand& command,
                                double factor) const;

  double SampleMarkovTriggerTime() const;

  void SampleMarkovCommands(const State& state);

  void SampleFactoredMarkovCommands(
      const State& state, const std::vector<std::vector<CompiledMarkovCommand>>&
                              commands_per_module,
      size_t module, double factor);

  void ConsiderCandidateMarkovCommands(double weight);

  void SampleMarkovOutcomes(const State& state);

  void SampleGsmpCommands(const State& state, State* next_state);

  void SampleFactoredGsmpCommands(
      const State& state, const std::vector<int>& offsets_per_module,
      const std::vector<std::vector<CompiledMarkovCommand>>&
          commands_per_module,
      size_t module, int index, State* next_state);

  void ConsiderCandidateGsmpCommands(const State& state, int index,
                                     State* next_state);

  const CompiledModel* const model_;
  CompiledExpressionEvaluator* const evaluator_;
  CompiledDistributionSampler<Engine>* const sampler_;
  std::vector<const CompiledMarkovCommand*> candidate_markov_commands_;
  std::vector<const CompiledMarkovCommand*> selected_markov_commands_;
  double selected_markov_commands_key_;
  int markov_ties_;
  double markov_weight_sum_;
  std::vector<const CompiledMarkovOutcome*> candidate_markov_outcomes_;
  std::vector<const CompiledMarkovOutcome*> selected_markov_outcomes_;
  const CompiledGsmpCommand* candidate_gsmp_command_;
  const CompiledGsmpCommand* selected_gsmp_command_;
  int selected_gsmp_index_;
  int gsmp_ties_;
};

template <typename Engine>
NextStateSampler<Engine>::NextStateSampler(
    const CompiledModel* model, CompiledExpressionEvaluator* evaluator,
    CompiledDistributionSampler<Engine>* sampler)
    : model_(model), evaluator_(evaluator), sampler_(sampler) {}

template <typename Engine>
void NextStateSampler<Engine>::NextState(const State& state,
                                         State* next_state) {
  selected_markov_commands_.clear();
  selected_markov_commands_key_ = -std::numeric_limits<double>::infinity();
  markov_ties_ = 1;
  markov_weight_sum_ = 0.0;
  selected_markov_outcomes_.clear();
  SampleMarkovCommands(state);
  next_state->set_time(std::numeric_limits<double>::infinity());
  next_state->set_values(state.values());
  if (!selected_markov_commands_.empty()) {
    SampleMarkovOutcomes(state);
    next_state->set_time(state.time() + SampleMarkovTriggerTime());
  }
  if (model_->model_type() == CompiledModelType::GSMP) {
    next_state->reset_trigger_times();
    selected_markov_commands_.clear();
    selected_gsmp_command_ = nullptr;
    gsmp_ties_ = 1;
    SampleGsmpCommands(state, next_state);
    if (selected_gsmp_command_ != nullptr) {
      for (const auto& update : selected_gsmp_command_->updates()) {
        next_state->set_value(
            update.variable(),
            evaluator_->EvaluateIntExpression(update.expr(), state.values()));
      }
      next_state->set_trigger_time(selected_gsmp_index_,
                                   std::numeric_limits<double>::infinity());
    }
  }
  for (const auto* outcome : selected_markov_outcomes_) {
    for (const auto& update : outcome->updates()) {
      next_state->set_value(
          update.variable(),
          evaluator_->EvaluateIntExpression(update.expr(), state.values()));
    }
  }
}

template <typename Engine>
double NextStateSampler<Engine>::GetMarkovCommandWeight(
    const State& state, const CompiledMarkovCommand& command,
    double factor) const {
  switch (model_->model_type()) {
    case CompiledModelType::DTMC:
      return 1.0;
    case CompiledModelType::CTMC:
    case CompiledModelType::GSMP: {
      double weight = 0.0;
      for (const auto& outcome : command.outcomes()) {
        weight += factor *
                  evaluator_->EvaluateDoubleExpression(outcome.weight(),
                                                       state.values());
      }
      return weight;
    }
  }
  LOG(FATAL) << "bad model type";
}

template <typename Engine>
double NextStateSampler<Engine>::SampleMarkovTriggerTime() const {
  switch (model_->model_type()) {
    case CompiledModelType::DTMC:
      return 1;
    case CompiledModelType::CTMC:
    case CompiledModelType::GSMP: {
      return sampler_->Exponential(markov_weight_sum_);
    }
  }
  LOG(FATAL) << "bad model type";
}

template <typename Engine>
void NextStateSampler<Engine>::SampleMarkovCommands(const State& state) {
  for (const auto& command : model_->single_markov_commands()) {
    if (evaluator_->EvaluateIntExpression(command.guard(), state.values())) {
      candidate_markov_commands_.push_back(&command);
      ConsiderCandidateMarkovCommands(
          GetMarkovCommandWeight(state, command, 1.0));
      candidate_markov_commands_.pop_back();
    }
  }
  for (const auto& commands_per_module : model_->factored_markov_commands()) {
    SampleFactoredMarkovCommands(state, commands_per_module, 0, 1.0);
  }
}

template <typename Engine>
void NextStateSampler<Engine>::SampleFactoredMarkovCommands(
    const State& state,
    const std::vector<std::vector<CompiledMarkovCommand>>& commands_per_module,
    size_t module, double factor) {
  if (module == commands_per_module.size()) {
    ConsiderCandidateMarkovCommands(factor);
    return;
  }
  for (const auto& command : commands_per_module[module]) {
    if (evaluator_->EvaluateIntExpression(command.guard(), state.values())) {
      candidate_markov_commands_.push_back(&command);
      SampleFactoredMarkovCommands(
          state, commands_per_module, module + 1,
          GetMarkovCommandWeight(state, command, factor));
      candidate_markov_commands_.pop_back();
    }
  }
}

template <typename Engine>
void NextStateSampler<Engine>::ConsiderCandidateMarkovCommands(double weight) {
  const double key = log(1 - sampler_->StandardUniform()) / weight;
  if (key > selected_markov_commands_key_) {
    markov_ties_ = 1;
    selected_markov_commands_key_ = key;
    selected_markov_commands_ = candidate_markov_commands_;
  } else if (key == selected_markov_commands_key_) {
    ++markov_ties_;
    if (sampler_->StandardUniform() * markov_ties_ < 1.0) {
      selected_markov_commands_ = candidate_markov_commands_;
    }
  }
  markov_weight_sum_ += weight;
}

template <typename Engine>
void NextStateSampler<Engine>::SampleMarkovOutcomes(const State& state) {
  selected_markov_outcomes_.resize(selected_markov_commands_.size());
  for (size_t i = 0; i < selected_markov_commands_.size(); ++i) {
    const auto& command = *selected_markov_commands_[i];
    if (command.outcomes().size() == 1) {
      const auto& outcome = command.outcomes()[0];
      selected_markov_outcomes_[i] = &outcome;
    } else {
      double selected_key = -std::numeric_limits<double>::infinity();
      int ties = 1;
      for (const auto& outcome : command.outcomes()) {
        const double weight = evaluator_->EvaluateDoubleExpression(
            outcome.weight(), state.values());
        const double key = log(1.0 - sampler_->StandardUniform()) / weight;
        if (key > selected_key) {
          ties = 1;
          selected_key = key;
          selected_markov_outcomes_[i] = &outcome;
        } else if (key == selected_key) {
          ++ties;
          if (sampler_->StandardUniform() * ties < 1.0) {
            selected_markov_outcomes_[i] = &outcome;
          }
        }
      }
    }
  }
}

template <typename Engine>
void NextStateSampler<Engine>::SampleGsmpCommands(const State& state,
                                                  State* next_state) {
  for (const auto& command : model_->single_gsmp_commands()) {
    if (evaluator_->EvaluateIntExpression(command.guard(), state.values())) {
      candidate_gsmp_command_ = &command;
      ConsiderCandidateGsmpCommands(state, command.first_index(), next_state);
    }
  }
  for (size_t i = 0; i < model_->factored_gsmp_commands().size(); ++i) {
    const auto& offsets = model_->factored_gsmp_commands()[i].offsets;
    for (const auto& command :
         model_->factored_gsmp_commands()[i].gsmp_commands) {
      candidate_gsmp_command_ = &command;
      SampleFactoredGsmpCommands(state, offsets,
                                 model_->factored_markov_commands()[i], 1,
                                 command.first_index(), next_state);
    }
  }
}

template <typename Engine>
void NextStateSampler<Engine>::SampleFactoredGsmpCommands(
    const State& state, const std::vector<int>& offsets_per_module,
    const std::vector<std::vector<CompiledMarkovCommand>>& commands_per_module,
    size_t module, int index, State* next_state) {
  if (module == commands_per_module.size()) {
    ConsiderCandidateGsmpCommands(state, index, next_state);
  }
  int offset = offsets_per_module[module - 1];
  int i = 0;
  for (const auto& command : commands_per_module[module]) {
    if (evaluator_->EvaluateIntExpression(command.guard(), state.values())) {
      for (const auto& outcome : command.outcomes()) {
        candidate_markov_outcomes_.push_back(&outcome);
        SampleFactoredGsmpCommands(state, offsets_per_module,
                                   commands_per_module, module + 1,
                                   index + offset * i, next_state);
        candidate_markov_outcomes_.pop_back();
        ++i;
      }
    } else {
      i += command.outcomes().size();
    }
  }
}

template <typename Engine>
void NextStateSampler<Engine>::ConsiderCandidateGsmpCommands(
    const State& state, int index, State* next_state) {
  double t = state.trigger_times()[index];
  if (t == std::numeric_limits<double>::infinity()) {
    t = state.time() +
        sampler_->Sample(candidate_gsmp_command_->delay(), state.values());
  }
  next_state->set_trigger_time(index, t);
  bool selected = false;
  if (t < next_state->time()) {
    gsmp_ties_ = 1;
    next_state->set_time(t);
    selected = true;
  } else if (t == next_state->time()) {
    ++gsmp_ties_;
    if (sampler_->StandardUniform() * gsmp_ties_ < 1.0) {
      selected = true;
    }
  }
  if (selected) {
    selected_gsmp_command_ = candidate_gsmp_command_;
    selected_markov_outcomes_ = candidate_markov_outcomes_;
    selected_gsmp_index_ = index;
  }
}

#endif  // SIMULATOR_H_
