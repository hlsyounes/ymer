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
    : variable_(variable), expr_(expr) {
}

CompiledOutcome::CompiledOutcome(const CompiledDistribution& delay,
                                 const std::vector<CompiledUpdate>& updates,
                                 int first_index)
    : delay_(delay), updates_(updates), first_index_(first_index) {}

CompiledCommand::CompiledCommand(const CompiledExpression& guard,
                                 const std::vector<CompiledOutcome>& outcomes)
    : guard_(guard), outcomes_(outcomes) {
}

CompiledMarkovOutcome::CompiledMarkovOutcome(
    const CompiledExpression& weight,
    const std::vector<CompiledUpdate>& updates)
    : weight_(weight), updates_(updates) {}

CompiledMarkovCommand::CompiledMarkovCommand(
    const CompiledExpression& guard,
    const std::vector<CompiledMarkovOutcome>& outcomes)
    : guard_(guard), outcomes_(outcomes) {}

CompiledGsmpCommand::CompiledGsmpCommand(
    const CompiledExpression& guard, const CompiledDistribution& delay,
    const std::vector<CompiledUpdate>& updates, int first_index)
    : guard_(guard),
      delay_(delay),
      updates_(updates),
      first_index_(first_index) {}

void CompiledModel::AddVariable(
    const std::string& name, int min_value, int max_value, int init_value) {
  const int bit_count = Log2(max_value - min_value) + 1;
  variables_.emplace_back(name, min_value, bit_count);
  init_values_.push_back(init_value);
}

void CompiledModel::AddCommand(const CompiledCommand& command) {
  commands_.push_back(command);
}

void CompiledModel::SetTriggerTimeCount(int trigger_time_count) {
  trigger_time_count_ = trigger_time_count;
}

namespace {

std::pair<int, int> GetDistributionRegisterCounts(
    const CompiledDistribution& dist) {
  int ireg_count = 0;
  int dreg_count = 0;
  for (const CompiledExpression& expr : dist.parameters()) {
    std::pair<int, int> expr_reg_counts = GetExpressionRegisterCounts(expr);
    ireg_count = std::max(ireg_count, expr_reg_counts.first);
    dreg_count = std::max(dreg_count, expr_reg_counts.second);
  }
  return {ireg_count, dreg_count};
}

std::pair<int, int> GetUpdateRegisterCounts(const CompiledUpdate& update) {
  return GetExpressionRegisterCounts(update.expr());
}

std::pair<int, int> GetOutcomeRegisterCounts(const CompiledOutcome& outcome) {
  std::pair<int, int> reg_counts =
      GetDistributionRegisterCounts(outcome.delay());
  for (const CompiledUpdate& update : outcome.updates()) {
    std::pair<int, int> update_reg_counts = GetUpdateRegisterCounts(update);
    reg_counts.first = std::max(reg_counts.first, update_reg_counts.first);
    reg_counts.second = std::max(reg_counts.second, update_reg_counts.second);
  }
  return reg_counts;
}

std::pair<int, int> GetCommandRegisterCounts(const CompiledCommand& command) {
  std::pair<int, int> reg_counts = GetExpressionRegisterCounts(command.guard());
  for (const CompiledOutcome& outcome : command.outcomes()) {
    std::pair<int, int> outcome_reg_counts = GetOutcomeRegisterCounts(outcome);
    reg_counts.first = std::max(reg_counts.first, outcome_reg_counts.first);
    reg_counts.second = std::max(reg_counts.second, outcome_reg_counts.second);
  }
  return reg_counts;
}

}  // namespace

std::pair<int, int> CompiledModel::GetRegisterCounts() const {
  int ireg_count = 0;
  int dreg_count = 0;
  for (const CompiledCommand& command : commands_) {
    std::pair<int, int> command_reg_counts = GetCommandRegisterCounts(command);
    ireg_count = std::max(ireg_count, command_reg_counts.first);
    dreg_count = std::max(dreg_count, command_reg_counts.second);
  }
  return {ireg_count, dreg_count};
}

int CompiledModel::BitCount() const {
  int bit_count = 0;
  for (const auto& v : variables_) {
    bit_count += v.bit_count();
  }
  return bit_count;
}
