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

#include <cmath>

#include "glog/logging.h"

#include "strutil.h"

double lchoose(double x, double y) {
  return lgamma(x + 1) - lgamma(y + 1) - lgamma(x - y + 1);
}

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
  const double lp = log(p);
  const double lq = log(q);
  double sum = pow(q, n);
  int x = 0;
  while (sum < y && x < n) {
    ++x;
    sum += exp(lchoose(n, x) + x * lp + (n - x) * lq);
  }
  return x;
}

double MaxNestedError(double delta) {
  return delta / (0.5 + delta);
}

SingleSamplingPlan::SingleSamplingPlan(int n, int c)
    : n_(n), c_(c) {
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

BernoulliTester::BernoulliTester(double theta0, double theta1)
    : done_(false), theta0_(theta0), theta1_(theta1) {
  CHECK_GE(theta0_, theta1_);
}

BernoulliTester::~BernoulliTester() = default;

void BernoulliTester::AddObservation(bool x) {
  sample_.AddObservation(x ? 1 : 0);
  UpdateState();
}

void BernoulliTester::SetSample(const Sample<int>& sample) {
  sample_ = sample;
  UpdateState();
}

std::string BernoulliTester::StateToString() const {
  return StateToStringImpl();
}

FixedBernoulliTester::FixedBernoulliTester(double theta0, double theta1,
                                           int sample_size)
    : BernoulliTester(theta0, theta1), sample_size_(sample_size) {
  CHECK_GT(sample_size_, 0);
}

FixedBernoulliTester::~FixedBernoulliTester() = default;

void FixedBernoulliTester::UpdateState() {
  done_ = sample().count() >= sample_size_;
  if (done_) {
    accept_ = sample().mean() > 0.5 * (theta0() + theta1());
  }
}

std::string FixedBernoulliTester::StateToStringImpl() const {
  return StrCat(sample().count(), '\t', sample().sum());
}

SingleSamplingBernoulliTester::SingleSamplingBernoulliTester(
    double theta0, double theta1, double alpha, double beta)
    : BernoulliTester(theta0, theta1),
      ssp_(SingleSamplingPlan::Create(theta0, theta1, alpha, beta)) {
  CHECK_NE(theta0, theta1);
}

SingleSamplingBernoulliTester::~SingleSamplingBernoulliTester() = default;

namespace {

int SingleSamplingAcceptThreshold(const SingleSamplingPlan& ssp) {
  return ssp.c() + 1;
}

int SingleSamplingRejectThreshold(const Sample<int>& sample,
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
    : BernoulliTester(theta0, theta1),
      positive_coefficient_(SprtPositiveCoefficient(theta0, theta1)),
      negative_coefficient_(SprtNegativeCoefficient(theta0, theta1)),
      accept_threshold_(SprtAcceptThreshold(theta0, alpha, beta)),
      reject_threshold_(SprtRejectThreshold(theta1, alpha, beta)) {
  CHECK_NE(theta0, theta1);
}

SprtBernoulliTester::~SprtBernoulliTester() = default;

namespace {

double SprtState(const Sample<int>& sample, double positive_coefficient,
                 double negative_coefficient) {
  double state = 0;
  if (sample.sum() > 0) {
    state += sample.sum() * positive_coefficient;
  }
  if (sample.sum() < sample.count()) {
    state += (sample.count() - sample.sum()) * negative_coefficient;
  }
  return state;
}

}  // namespace

void SprtBernoulliTester::UpdateState() {
  const double state =
      SprtState(sample(), positive_coefficient_, negative_coefficient_);
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
  const double state =
      SprtState(sample(), positive_coefficient_, negative_coefficient_);
  return StrCat(sample().count(), '\t', state, '\t',
                accept_threshold_, '\t', reject_threshold_);
}
