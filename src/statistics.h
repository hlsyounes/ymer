// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2013 Google Inc
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
// Statistical functions and classes.

#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <algorithm>
#include <cmath>

// A collection of observations.
template <typename T>
class Sample {
 public:
  Sample();

  void AddObservation(T x);

  T min() const { return min_; }
  T max() const { return max_; }
  int count() const { return count_; }
  double mean() const { return mean_; }
  double variance() const { return (count_ > 1) ? m2_ / (count_ - 1) : 0.0; }
  double stddev() const { return sqrt(variance()); }

 private:
  T min_;
  T max_;
  int count_;
  double mean_;
  double m2_;
};

template <typename T>
Sample<T>::Sample()
    : count_(0), mean_(0.0), m2_(0.0) {
}

template <typename T>
void Sample<T>::AddObservation(T x) {
  ++count_;
  min_ = (count_ > 1) ? std::min(min_, x) : x;
  max_ = (count_ > 1) ? std::max(max_, x) : x;
  const double delta = x - mean_;
  mean_ += delta / count_;
  m2_ += delta * (x - mean_);
}

#endif  // STATISTICS_H_
