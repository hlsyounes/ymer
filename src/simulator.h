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

#include "src/compiled-model.h"

// A simulator state.
class State {
 public:
  // Constructs an intial state for the given model.
  explicit State(const CompiledModel& model)
      : time_(0.0),
        values_(model.init_values()),
        trigger_times_(model.trigger_time_count(),
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

#endif  // SIMULATOR_H_
