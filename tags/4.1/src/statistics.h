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
#include <string>
#include <type_traits>

#include "strutil.h"

#include "glog/logging.h"
#include "gsl/gsl_cdf.h"

// Returns the natural logarithm of the generalized binomial coefficient.
double lchoose(double x, double y);

// Returns the inverse of the binomial cumulative distribution function.
int binoinv(double y, int n, double p);

// Returns the maximum allowed nested error given an indifference region of
// width 2*delta.
double MaxNestedError(double delta);

// A single sampling plan.
class SingleSamplingPlan {
 public:
  static SingleSamplingPlan Create(double theta0, double theta1, double alpha,
                                   double beta);

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
  using SumType = typename std::decay<decltype(std::declval<const T&>() +
                                               std::declval<const T&>())>::type;

  Sample();
  template <typename U>
  Sample(const Sample<U>& sample);

  void AddObservation(T x);

  T min() const { return min_; }
  T max() const { return max_; }
  SumType sum() const { return sum_; }
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
  SumType sum_;
  int count_;
  double mean_;
  double m2_;
};

// An abstract sequential hypothesis tester.  Tests the hypothesis H0: mu >
// theta0 against the hypothesis H1: mu < theta1, where mu is the expected value
// of the random variable being observed.  Continue to add observations until
// done() is true.  At that point, accept H0 if accept() is true, and accept H1
// if accept() is false.  While done() is false, accept() has no meaning.
//
// NOTE: all implementaitons require that theta0 >= theta1, but some also
// require that theta0 != theta1.
template <typename T>
class SequentialTester {
 public:
  virtual ~SequentialTester();

  void AddObservation(T x);
  void SetSample(const Sample<T>& sample);

  double theta0() const { return theta0_; }
  double theta1() const { return theta1_; }
  bool done() const { return done_; }
  bool accept() const { return accept_; }
  const Sample<T>& sample() const { return sample_; }

  std::string StateToString() const;

 protected:
  SequentialTester(double theta0, double theta1);

  bool done_;
  bool accept_;

 private:
  virtual void UpdateState() = 0;
  virtual std::string StateToStringImpl() const = 0;

  const double theta0_;
  const double theta1_;
  Sample<T> sample_;
};

// A simple SequentialTester that stops when a fixed sample size has been
// reached.  Accepts H0 if mean() > 0.5 * (theta0 + theta1).
//
// This implementation provides no error guarantees.  Error guarantees will
// depend on the provided sample size.  Allows theta0 == theta1.
template <typename T>
class FixedSampleSizeTester : public SequentialTester<T> {
 public:
  FixedSampleSizeTester(double theta0, double theta1, int sample_size);

 private:
  void UpdateState() override;
  std::string StateToStringImpl() const override;

  const int sample_size_;
};

// A SequentialTester that uses the sequential estimator by Chow and Robbins for
// fixed-width confidence intervals:
//
// Chow, Y. S. and Herbert Robbins.  1965.  On the asymptotic theory of
// fixed-width sequential confidence intervals for the mean.  Annals of
// Mathematical Statistics 36, no. 3: 457-462.
//
// Accepts H0 if mean() > 0.5 * (theta0 + theta1).  Does not allow theta0 ==
// theta1.
template <typename T>
class ChowRobbinsTester : public SequentialTester<T> {
 public:
  // Constructs a ChowRobbinsTester that estimates an interval of width
  // (theta0 - theta1) with coverage probability 1 - alpha.
  ChowRobbinsTester(double theta0, double theta1, double alpha);

 private:
  void UpdateState() override;
  std::string StateToStringImpl() const override;

  double State() const;
  double Bound() const;

  double tdist_quantile_;
};

// A SequantialTester based on a single sampling plan for Bernoulli trials with
// a sequential stopping rule.
//
// Respects error bounds alpha and beta.  Does not allow theta0 == theta1.
class SingleSamplingBernoulliTester : public SequentialTester<bool> {
 public:
  SingleSamplingBernoulliTester(double theta0, double theta1, double alpha,
                                double beta);

 private:
  void UpdateState() override;
  std::string StateToStringImpl() const override;

  const SingleSamplingPlan ssp_;
};

// A SequentialTester based on Wald's Sequential Probability Ratio Test for
// Bernoulli trials:
//
// Wald, Abraham.  1945.  Sequential tests of statistical hypotheses.  Annals of
// Mathematical Statistics 16, no. 2: 117-186.
//
// Respects error bounds alpha and beta.  Does not allow theta0 == theta1.
class SprtBernoulliTester : public SequentialTester<bool> {
 public:
  SprtBernoulliTester(double theta0, double theta1, double alpha, double beta);

 private:
  void UpdateState() override;
  std::string StateToStringImpl() const override;

  double State() const;

  const double positive_coefficient_;
  const double negative_coefficient_;
  const double accept_threshold_;
  const double reject_threshold_;
};

// A SequentialTester that implements Lai's sequential test for Bernoulli
// trials:
//
// Lai, Tze Leung.  1988.  Nearly optimal sequential tests of composite
// hypotheses.  The Annals of Statistics 16, no. 2: 856-886.
//
// Takes an observation cost, c, instead of error bounds, and aims to minimize
// overall cost of the test assuming that accepting the wrong hypothesis has
// cost 1.  Allows theta0 == theta1.
class LaiBernoulliTester : public SequentialTester<bool> {
 public:
  LaiBernoulliTester(double theta0, double theta1, double c);

 private:
  void UpdateState() override;
  std::string StateToStringImpl() const override;

  double State() const;
  double Bound() const;

  double c_;
};

template <typename T>
Sample<T>::Sample()
    : sum_(0), count_(0), mean_(0.0), m2_(0.0) {}

template <typename T>
template <typename U>
Sample<T>::Sample(const Sample<U>& sample)
    : min_(sample.min()),
      max_(sample.max()),
      sum_(sample.sum()),
      count_(sample.count()),
      mean_(sample.mean()),
      m2_(sample.variance() * sample.count()) {}

template <typename T>
void Sample<T>::AddObservation(T x) {
  ++count_;
  min_ = (count_ > 1) ? std::min(min_, x) : x;
  max_ = (count_ > 1) ? std::max(max_, x) : x;
  sum_ += x;
  const double delta = x - mean_;
  mean_ += delta / count_;
  m2_ += delta * (x - mean_);
}

template <typename T>
SequentialTester<T>::SequentialTester(double theta0, double theta1)
    : done_(false), theta0_(theta0), theta1_(theta1) {
  CHECK_GE(theta0, theta1);
}

template <typename T>
SequentialTester<T>::~SequentialTester() = default;

template <typename T>
void SequentialTester<T>::AddObservation(T x) {
  sample_.AddObservation(x);
  UpdateState();
}

template <typename T>
void SequentialTester<T>::SetSample(const Sample<T>& sample) {
  sample_ = sample;
  UpdateState();
}

template <typename T>
std::string SequentialTester<T>::StateToString() const {
  return StateToStringImpl();
}

template <typename T>
FixedSampleSizeTester<T>::FixedSampleSizeTester(double theta0, double theta1,
                                                int sample_size)
    : SequentialTester<T>(theta0, theta1), sample_size_(sample_size) {
  CHECK_GT(sample_size_, 0);
}

template <typename T>
void FixedSampleSizeTester<T>::UpdateState() {
  this->done_ = this->sample().count() >= sample_size_;
  if (this->done_) {
    this->accept_ =
        this->sample().mean() > 0.5 * (this->theta0() + this->theta1());
  }
}

template <typename T>
std::string FixedSampleSizeTester<T>::StateToStringImpl() const {
  return StrCat(this->sample().count(), '\t', this->sample().sum());
}

template <typename T>
ChowRobbinsTester<T>::ChowRobbinsTester(double theta0, double theta1,
                                        double alpha)
    : SequentialTester<T>(theta0, theta1), tdist_quantile_(1 - 0.5 * alpha) {
  CHECK_NE(theta0, theta1);
}

template <typename T>
void ChowRobbinsTester<T>::UpdateState() {
  this->done_ = (State() <= Bound());
  if (this->done_) {
    this->accept_ =
        this->sample().mean() > 0.5 * (this->theta0() + this->theta1());
  }
}

template <typename T>
std::string ChowRobbinsTester<T>::StateToStringImpl() const {
  return StrCat(this->sample().count(), '\t', State(), '\t', Bound());
}

template <typename T>
double ChowRobbinsTester<T>::State() const {
  return (this->sample().count() > 0)
             ? 1.0 / this->sample().count() + this->sample().variance()
             : std::numeric_limits<double>::infinity();
}

template <typename T>
double ChowRobbinsTester<T>::Bound() const {
  if (this->sample().count() > 1) {
    const double delta = 0.5 * (this->theta0() - this->theta1());
    const double a =
        gsl_cdf_tdist_Pinv(tdist_quantile_, this->sample().count() - 1);
    return this->sample().count() * delta * delta / a / a;
  }
  return 0;
}

#endif  // STATISTICS_H_
