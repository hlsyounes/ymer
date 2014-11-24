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

#include "glog/logging.h"

class FakeEngine {
 public:
  typedef uint32_t result_type;

  FakeEngine(result_type min, result_type max, std::vector<result_type> values)
      : min_(min), max_(max), values_(values), last_value_(-1) {
  }

  ~FakeEngine() {
    CHECK_EQ(last_value_ + 1, values_.size());
  }

  result_type min() { return min_; }
  result_type max() { return max_; }

  result_type operator()() {
    CHECK_LT(last_value_ + 1, values_.size());
    return values_[++last_value_];
  }

 private:
  result_type min_;
  result_type max_;
  std::vector<result_type> values_;
  std::vector<result_type>::size_type last_value_;
};

#endif  // FAKE_RNG_H_
