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
//
// A simulator state.

#ifndef STATES_H_
#define STATES_H_

#include <vector>

#include "src/compiled-model.h"
#include "src/expression.h"
#include "src/rng.h"

// A simulator state.
class State {
 public:
  // Constructs an initial state for the given model.
  explicit State(const CompiledModel& model);

  // Returns the current time of this state.
  double time() const { return time_; }

  // Returns the variable values for this state.
  const std::vector<int>& values() const { return values_; }

  // Returns a sampled successor of this state.
  State Next(const CompiledModel& model, CompiledExpressionEvaluator* evaluator,
             CompiledDistributionSampler<DCEngine>* sampler) const;

private:
  double time_;
  std::vector<int> values_;
  std::vector<double> trigger_times_;
};

#endif  // STATES_H_
