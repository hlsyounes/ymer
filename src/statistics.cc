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

#include "statistics.h"

#include <algorithm>
#include <cmath>

int binoinv(double y, int n, double p) {
  CHECK_LE(0, y);
  CHECK_LE(y, 1);
  CHECK_LT(0, n);
  CHECK_LE(0, p);
  CHECK_LE(p, 1);
  if (y == 0 || p == 0) {
    return 0;
  }
  if (y == 1 || p == 1) {
    return n;
  }
  const double q = 1 - p;
  const double r = p / q;
  // Let q = 1 - p and r = p / q.  With C(n, i) being the binomial coefficient,
  // the probability mass function for a binomial distribution with parameters n
  // and p can be written as:
  //
  //   f(i; n, p) = C(n, i) * p^i * q^{n - i}
  //
  // We can derive the following recursive definition:
  //
  //   f(i + 1; n, p) = C(n, i + 1) * p^{i + 1} * q^{n - i - 1}
  //                  = C(n, i + 1) * p^i * p^{n - i} * r
  //                  = C(n, i) * p^i * p^{n - i} * r * (n - i) / (i + 1)
  //                  = f(i; n, p) * r * (n - i) / (i + 1)
  //
  // The base case for the recursion is:
  //
  //   f(0; n, p) = q^n
  //
  // The cumulative distribution function can be given the following recursive
  // definition:
  //
  //   F(i + 1; n, p) = F(i; n, p) + f(i + 1; n, p)
  //   F(0; n, p) = f(0; n, p)
  //
  // To compute the inverse, Finv(y; n, p), we compute F(i; n, p) starting from
  // 0 and incrementing i until F(i; n, p) >= y.
  //
  // For large n, we use log f(i; n, p) in the computation to avoid underflow.
  int i = 0;
  const double lqn = n * std::log2(q);
  if (-lqn < 1000) {
    double f_i = std::pow(q, n);
    double sum = f_i;
    while (sum < y && i < n) {
      f_i *= r * (n - i) / (i + 1);
      sum += f_i;
      ++i;
    }
  } else {
    const double lr = std::log2(r);
    double lf_i = lqn;
    double sum = std::pow(2.0, lf_i);
    while (sum < y && i < n) {
      lf_i += lr + std::log2(n - i) - std::log2(i + 1);
      sum += std::pow(2.0, lf_i);
      ++i;
    }
  }
  return i;
}

double MaxNestedError(double delta) { return delta / (0.5 + delta); }

SingleSamplingPlan::SingleSamplingPlan(int n, int c) : n_(n), c_(c) {
  CHECK_LE(0, c_);
  CHECK_LE(c_, n_);
}

SingleSamplingPlan SingleSamplingPlan::Create(double theta0, double theta1,
                                              double alpha, double beta) {
  CHECK_LE(0, theta1);
  CHECK_LT(theta1, theta0);
  CHECK_LE(theta0, 1);
  CHECK_LT(0, alpha);
  CHECK_LT(alpha, 1);
  CHECK_LT(0, beta);
  CHECK_LT(beta, 1);
  if (theta1 == 0 && theta0 == 1) {
    return SingleSamplingPlan(1, 0);
  }
  if (theta1 == 0) {
    return SingleSamplingPlan(ceil(log(alpha) / log(1 - theta0)), 0);
  }
  if (theta0 == 1) {
    const int n = ceil(log(beta) / log(theta1));
    return SingleSamplingPlan(n, n - 1);
  }
  int n = 0;
  int c0 = -1;
  int c1 = 0;
  while (c0 < c1) {
    ++n;
    c0 = n - binoinv(1 - alpha, n, 1 - theta0) - 1;
    c1 = binoinv(1 - beta, n, theta1);
    VLOG(1) << n << '\t' << c0 << '\t' << c1;
  }
  return SingleSamplingPlan(n, (c0 + c1) / 2);
}

SingleSamplingBernoulliTester::SingleSamplingBernoulliTester(double theta0,
                                                             double theta1,
                                                             double alpha,
                                                             double beta)
    : SequentialTester<bool>(theta0, theta1),
      ssp_(SingleSamplingPlan::Create(theta0, theta1, alpha, beta)) {
  CHECK_NE(theta0, theta1);
}

namespace {

int SingleSamplingAcceptThreshold(const SingleSamplingPlan& ssp) {
  return ssp.c() + 1;
}

int SingleSamplingRejectThreshold(const Sample<bool>& sample,
                                  const SingleSamplingPlan& ssp) {
  return sample.count() + ssp.c() - ssp.n();
}

}  // namesapce

void SingleSamplingBernoulliTester::UpdateState() {
  const int state = sample().sum();
  if (state >= SingleSamplingAcceptThreshold(ssp_)) {
    done_ = true;
    accept_ = true;
  } else if (state <= SingleSamplingRejectThreshold(sample(), ssp_)) {
    done_ = true;
    accept_ = false;
  } else {
    done_ = false;
  }
}

std::string SingleSamplingBernoulliTester::StateToStringImpl() const {
  const int state = sample().sum();
  return StrCat(sample().count(), '\t', state, '\t',
                SingleSamplingAcceptThreshold(ssp_), '\t',
                SingleSamplingRejectThreshold(sample(), ssp_));
}

namespace {

// NOTE: When theta0 is 1 or theta1 is 0, then the SPRT simply becomes a
// curtailed single sampling plan.  When theta0 is 1, we reject on the first
// negative observation.  When theta1 is 0, we reject on the first positive
// observation.  The parameters here are set to handle these two degenerate
// cases.

double SprtPositiveCoefficient(double theta0, double theta1) {
  return (theta1 > 0) ? log(theta1) - log(theta0)
                      : -std::numeric_limits<double>::infinity();
}

double SprtNegativeCoefficient(double theta0, double theta1) {
  return (theta0 < 1) ? log(1 - theta1) - log(1 - theta0)
                      : std::numeric_limits<double>::infinity();
}

double SprtAcceptThreshold(double theta0, double alpha, double beta) {
  double result = log(beta);
  if (theta0 < 1) {
    result -= log(1 - alpha);
  }
  return result;
}

double SprtRejectThreshold(double theta1, double alpha, double beta) {
  double result = -log(alpha);
  if (theta1 > 0) {
    result += log(1 - beta);
  }
  return result;
}

}  // namesapce

SprtBernoulliTester::SprtBernoulliTester(double theta0, double theta1,
                                         double alpha, double beta)
    : SequentialTester<bool>(theta0, theta1),
      positive_coefficient_(SprtPositiveCoefficient(theta0, theta1)),
      negative_coefficient_(SprtNegativeCoefficient(theta0, theta1)),
      accept_threshold_(SprtAcceptThreshold(theta0, alpha, beta)),
      reject_threshold_(SprtRejectThreshold(theta1, alpha, beta)) {
  CHECK_NE(theta0, theta1);
}

void SprtBernoulliTester::UpdateState() {
  const double state = State();
  if (state <= accept_threshold_) {
    done_ = true;
    accept_ = true;
  } else if (state >= reject_threshold_) {
    done_ = true;
    accept_ = false;
  } else {
    done_ = false;
  }
}

std::string SprtBernoulliTester::StateToStringImpl() const {
  return StrCat(sample().count(), '\t', State(), '\t', accept_threshold_, '\t',
                reject_threshold_);
}

double SprtBernoulliTester::State() const {
  double state = 0;
  if (sample().sum() > 0) {
    state += sample().sum() * positive_coefficient_;
  }
  if (sample().sum() < sample().count()) {
    state += (sample().count() - sample().sum()) * negative_coefficient_;
  }
  return state;
}

namespace {

double LaiIFunctionBernoulli(double theta_tilde, double theta) {
  double result = 0.0;
  if (theta_tilde > 0.0) {
    result += theta_tilde * log(theta_tilde / theta);
  }
  if (theta_tilde < 1.0) {
    result += (1.0 - theta_tilde) * log((1.0 - theta_tilde) / (1.0 - theta));
  }
  return result;
}

double LaiHFunction(double gamma, double t) {
  if (gamma == 0.0) {
    if (t >= 0.8) {
      return 0.25 * sqrt(2.0 / M_PI) *
             (1.0 / sqrt(t) - 5.0 / pow(t, 2.5) / 48.0 / M_PI);
    } else if (t >= 0.1) {
      return exp(-0.69 * t - 1.0);
    } else if (t >= 0.01) {
      return 0.39 - 0.015 / sqrt(t);
    } else {
      return sqrt(t * (2.0 * log(1.0 / t) + log(log(1.0 / t)) -
                       log(4.0 * M_PI) - 3.0 * exp(-0.016 / sqrt(t))));
    }
  } else if (gamma <= 20.0) {
    if (t >= 1.0) {
      return LaiHFunction(0.0, t) * exp(-0.5 * gamma * gamma * t);
    } else {
      return LaiHFunction(0.0, t) * exp(-0.5 * gamma * gamma * pow(t, 1.125));
    }
  } else {
    return (sqrt(t) *
                sqrt(2.0 * log(1.0 / t) + log(log(1.0 / t)) - log(4.0 * M_PI)) -
            gamma * t);
  }
}

double LaiGFunction(double gamma, double t) {
  const double g = LaiHFunction(gamma, t) + gamma * t;
  return 0.5 * g * g / t;
}

}  // namespace

LaiBernoulliTester::LaiBernoulliTester(double theta0, double theta1, double c)
    : SequentialTester<bool>(theta0, theta1), c_(c) {}

void LaiBernoulliTester::UpdateState() {
  done_ = (State() >= Bound());
  if (done_) {
    accept_ = this->sample().mean() > 0.5 * (theta0() + theta1());
  }
}

std::string LaiBernoulliTester::StateToStringImpl() const {
  return StrCat(sample().count(), '\t', State(), '\t', Bound());
}

double LaiBernoulliTester::State() const {
  double state = LaiIFunctionBernoulli(sample().mean(), theta0());
  if (theta0() != theta1()) {
    state = std::max(state, LaiIFunctionBernoulli(sample().mean(), theta1()));
  }
  return state;
}

double LaiBernoulliTester::Bound() const {
  const auto n = sample().count();
  return LaiGFunction(0.5 * (theta0() - theta1()) / sqrt(c_), c_ * n) / n;
}
