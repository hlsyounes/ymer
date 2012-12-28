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

CompiledVariable::CompiledVariable(
    const std::string& name, int min_value, int max_value, int init_value)
    : name_(name),
      min_value_(min_value), max_value_(max_value), init_value_(init_value) {
  CHECK_LE(min_value, max_value);
  CHECK_LE(min_value, init_value);
  CHECK_LE(init_value, max_value);
}

CompiledUpdate::CompiledUpdate(int variable, const CompiledExpression& expr)
    : variable_(variable), expr_(expr) {
}

CompiledOutcome::CompiledOutcome(const CompiledDistribution& delay,
                                 const std::vector<CompiledUpdate>& updates)
    : delay_(delay), updates_(updates) {
}

CompiledCommand::CompiledCommand(const CompiledExpression& guard,
                                 const std::vector<CompiledOutcome>& outcomes)
    : guard_(guard), outcomes_(outcomes) {
}

void CompiledModel::AddVariable(
    const std::string& name, int min_value, int max_value, int init_value) {
  variables_.push_back(
      CompiledVariable(name, min_value, max_value, init_value));
}

void CompiledModel::AddCommand(const CompiledCommand& command) {
  commands_.push_back(command);
}

namespace {

std::pair<int, int> GetNumDistributionRegisters(
    const CompiledDistribution& dist) {
  int num_iregs = 0;
  int num_dregs = 0;
  for (const CompiledExpression& expr : dist.parameters()) {
    std::pair<int, int> num_regs_expr = GetNumRegisters(expr);
    num_iregs = std::max(num_iregs, num_regs_expr.first);
    num_dregs = std::max(num_dregs, num_regs_expr.second);
  }
  return { num_iregs, num_dregs };
}

std::pair<int, int> GetNumUpdateRegisters(const CompiledUpdate& update) {
  return GetNumRegisters(update.expr());
}

std::pair<int, int> GetNumOutcomeRegisters(const CompiledOutcome& outcome) {
  std::pair<int, int> num_regs = GetNumDistributionRegisters(outcome.delay());
  for (const CompiledUpdate& update : outcome.updates()) {
    std::pair<int, int> num_regs_update = GetNumUpdateRegisters(update);
    num_regs.first = std::max(num_regs.first, num_regs_update.first);
    num_regs.second = std::max(num_regs.second, num_regs_update.second);
  }
  return num_regs;
}

std::pair<int, int> GetNumCommandRegisters(const CompiledCommand& command) {
  std::pair<int, int> num_regs = GetNumRegisters(command.guard());
  for (const CompiledOutcome& outcome : command.outcomes()) {
    std::pair<int, int> num_regs_outcome = GetNumOutcomeRegisters(outcome);
    num_regs.first = std::max(num_regs.first, num_regs_outcome.first);
    num_regs.second = std::max(num_regs.second, num_regs_outcome.second);
  }
  return num_regs;
}

}  // namespace

std::pair<int, int> CompiledModel::GetNumRegisters() const {
  int num_iregs = 0;
  int num_dregs = 0;
  for (const CompiledCommand& command : commands_) {
    std::pair<int, int> num_regs = GetNumCommandRegisters(command);
    num_iregs = std::max(num_iregs, num_regs.first);
    num_dregs = std::max(num_dregs, num_regs.second);
  }
  return { num_iregs, num_dregs };
}

int CompiledModel::NumBits() const {
  int num_bits = 0;
  for (std::vector<CompiledVariable>::const_iterator i = variables_.begin();
       i != variables_.end(); ++i) {
    const CompiledVariable& v = *i;
    num_bits += Log2(v.max_value() - v.min_value()) + 1;
  }
  return num_bits;
}
