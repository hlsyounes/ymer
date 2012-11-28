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

int CompiledModel::NumBits() const {
  int num_bits = 0;
  for (std::vector<CompiledVariable>::const_iterator i = variables_.begin();
       i != variables_.end(); ++i) {
    const CompiledVariable& v = *i;
    num_bits += Log2(v.max_value() - v.min_value()) + 1;
  }
  return num_bits;
}
