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

#include "compiled-model.h"

#include <string>
#include <vector>

#include "compiled-distribution.h"
#include "compiled-expression.h"
#include "ddutil.h"

#include "glog/logging.h"

CompiledUpdate::CompiledUpdate(int variable, const CompiledExpression& expr)
    : variable_(variable), expr_(expr) {}

CompiledMarkovOutcome::CompiledMarkovOutcome(
    const CompiledExpression& probability,
    const std::vector<CompiledUpdate>& updates)
    : probability_(probability), updates_(updates) {}

CompiledMarkovCommand::CompiledMarkovCommand(
    const Optional<int>& module, const CompiledExpression& guard,
    const CompiledExpression& weight,
    const std::vector<CompiledMarkovOutcome>& outcomes)
    : module_(module), guard_(guard), weight_(weight), outcomes_(outcomes) {}

CompiledGsmpCommand::CompiledGsmpCommand(
    const Optional<int>& module, const CompiledExpression& guard,
    const CompiledGsmpDistribution& delay,
    const std::vector<CompiledUpdate>& updates, int first_index)
    : module_(module),
      guard_(guard),
      delay_(delay),
      updates_(updates),
      first_index_(first_index) {}

CompiledModel::CompiledModel(CompiledModelType type,
                             const std::vector<StateVariableInfo>& variables,
                             const std::vector<std::set<int>>& module_variables,
                             const std::vector<int>& init_values)
    : type_(type),
      variables_(variables),
      module_variables_(module_variables),
      init_values_(init_values),
      gsmp_event_count_(0) {}

int CompiledModel::EventCount() const {
  int event_count = gsmp_event_count();
  if (pivot_variable_.has_value()) {
    for (const auto& commands : pivoted_single_markov_commands_) {
      for (const auto& command : commands) {
        event_count += command.outcomes().size();
      }
    }
  }
  for (const auto& command : single_markov_commands_) {
    event_count += command.outcomes().size();
  }
  for (const auto& commands_per_module : factored_markov_commands_) {
    int n = 1;
    for (const auto& commands : commands_per_module) {
      int m = 0;
      for (const auto& command : commands) {
        m += command.outcomes().size();
      }
      n *= m;
    }
    event_count += n;
  }
  return event_count;
}

namespace {

std::pair<int, int> ComponentMax(std::pair<int, int> left,
                                 std::pair<int, int> right) {
  return {std::max(left.first, right.first),
          std::max(left.second, right.second)};
}

std::pair<int, int> GetUpdateRegisterCounts(const CompiledUpdate& update) {
  return GetExpressionRegisterCounts(update.expr());
}

std::pair<int, int> GetMarkovOutcomeRegisterCounts(
    const CompiledMarkovOutcome& outcome) {
  std::pair<int, int> reg_counts =
      GetExpressionRegisterCounts(outcome.probability());
  for (const auto& update : outcome.updates()) {
    reg_counts = ComponentMax(reg_counts, GetUpdateRegisterCounts(update));
  }
  return reg_counts;
}

std::pair<int, int> GetMarkovCommandRegisterCounts(
    const CompiledMarkovCommand& command) {
  std::pair<int, int> reg_counts = GetExpressionRegisterCounts(command.guard());
  for (const auto& outcome : command.outcomes()) {
    reg_counts =
        ComponentMax(reg_counts, GetMarkovOutcomeRegisterCounts(outcome));
  }
  return reg_counts;
}

std::pair<int, int> GetGsmpCommandRegisterCounts(
    const CompiledGsmpCommand& command) {
  std::pair<int, int> reg_counts = GetExpressionRegisterCounts(command.guard());
  for (const auto& update : command.updates()) {
    reg_counts = ComponentMax(reg_counts, GetUpdateRegisterCounts(update));
  }
  return reg_counts;
}

}  // namespace

std::pair<int, int> CompiledModel::GetRegisterCounts() const {
  std::pair<int, int> reg_counts = {0, 0};
  for (const auto& command : single_markov_commands_) {
    reg_counts =
        ComponentMax(reg_counts, GetMarkovCommandRegisterCounts(command));
  }
  for (const auto& modules_by_action : factored_markov_commands_) {
    for (const auto& commands_by_module : modules_by_action) {
      for (const auto& command : commands_by_module) {
        reg_counts =
            ComponentMax(reg_counts, GetMarkovCommandRegisterCounts(command));
      }
    }
  }
  for (const auto& command : single_gsmp_commands_) {
    reg_counts =
        ComponentMax(reg_counts, GetGsmpCommandRegisterCounts(command));
  }
  for (const auto& factors : factored_gsmp_commands_) {
    for (const auto& command : factors.gsmp_commands) {
      reg_counts =
          ComponentMax(reg_counts, GetGsmpCommandRegisterCounts(command));
    }
  }
  return reg_counts;
}
