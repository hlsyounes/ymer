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
// A fake random number engine, useful for testing.

#ifndef FAKE_RNG_H_
#define FAKE_RNG_H_

#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

#include "glog/logging.h"

class FakeEngine {
 public:
  using result_type = uint64_t;

  explicit FakeEngine(std::vector<double> values)
      : values_(std::move(values)), next_value_(0) {}

  ~FakeEngine() {
    CHECK_EQ(next_value_, values_.size()) << values_.size() - next_value_
                                          << " unconsumed values";
  }

  static constexpr result_type min() { return 0; }
  static constexpr result_type max() {
    return result_type(1) << std::numeric_limits<double>::digits;
  }

  result_type operator()() {
    CHECK_LT(next_value_, values_.size()) << "insufficient values";
    return max() * values_[next_value_++];
  }

 private:
  std::vector<double> values_;
  std::vector<double>::size_type next_value_;
};

#endif  // FAKE_RNG_H_
