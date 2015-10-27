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

  FakeEngine(result_type min, result_type max, std::vector<result_type> values)
      : min_(min),
        max_(max),
        values_(std::move(values)),
        next_value_(0),
        // Transform values so that std::generate_canonical will consume exactly
        // one value per call.
        multiplier_(1L << (std::numeric_limits<double>::digits - 1 +
                           static_cast<size_t>(std::log2(max_ - min_ + 1.0)))) {
  }

  ~FakeEngine() {
    CHECK_EQ(next_value_, values_.size()) << values_.size() - next_value_
                                          << " unconsumed values";
  }

  result_type min() const { return min_; }
  result_type max() const { return multiplier_ * max_ - 1 + multiplier_; }

  result_type operator()() {
    CHECK_LT(next_value_, values_.size()) << "insufficient values";
    return multiplier_ * values_[next_value_++];
  }

 private:
  result_type min_;
  result_type max_;
  std::vector<result_type> values_;
  std::vector<result_type>::size_type next_value_;
  result_type multiplier_;
};

#endif  // FAKE_RNG_H_
