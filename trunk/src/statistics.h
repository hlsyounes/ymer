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
#include <limits>

#include "gsl/gsl_cdf.h"

// Returns the natural logarithm of the generalized binomial coefficient.
double lchoose(double x, double y);

// Returns the inverse of the binomial cumulative distribution function.
int binoinv(double y, int n, double p);

// A single sampling plan.
class SingleSamplingPlan {
 public:
  static SingleSamplingPlan Create(double p0, double p1,
                                   double alpha, double beta);

  int n() const { return n_; }
  int c() const { return c_; }

 private:
  SingleSamplingPlan(int n, int c);

  int n_;
  int c_;
};

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
  double variance() const { return (count_ > 1) ? m2_ / count_ : 0.0; }
  double sample_variance() const {
    return (count_ > 1) ? m2_ / (count_ - 1) : 0.0;
  }
  double sample_stddev() const { return sqrt(sample_variance()); }

 private:
  T min_;
  T max_;
  int count_;
  double mean_;
  double m2_;
};

// A sequential estimator based on the paper "On the Asymptotic Theory of
// Fixed-Width Sequential Confidence Intervals for the Mean" by Chow & Robbins.
// Stop estimation when state() <= bound().
template <typename T>
class SequentialEstimator {
 public:
  // Constructs a sequential estimator for estimating an interval of width
  // 2*delta with coverage probability 1-alpha.
  SequentialEstimator(double delta, double alpha);

  void AddObservation(T x);

  double delta() const { return delta_; }
  double alpha() const { return alpha_; }
  int count() const { return sample_.count(); }
  // The current estimated value.
  double value() const { return sample_.mean(); }
  // The current state of the estimator.
  double state() const { return state_; }
  // The current bound of the estimator.
  double bound() const { return bound_; }

 private:
  double delta_;
  double alpha_;
  Sample<T> sample_;
  double state_;
  double bound_;
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

template <typename T>
SequentialEstimator<T>::SequentialEstimator(double delta, double alpha)
    : delta_(delta), alpha_(alpha),
      state_(std::numeric_limits<double>::infinity()), bound_(0) {
}

template <typename T>
void SequentialEstimator<T>::AddObservation(T x) {
  sample_.AddObservation(x);
  state_ = 1.0 / sample_.count() + sample_.variance();
  if (sample_.count() > 1) {
    const double a = gsl_cdf_tdist_Pinv(1 - 0.5 * alpha_, sample_.count() - 1);
    bound_ = sample_.count() * delta_ * delta_ / a / a;
  }
}

#endif  // STATISTICS_H_
