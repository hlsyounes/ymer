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

SingleSamplingPlan::SingleSamplingPlan(int n, int c)
    : n_(n), c_(c) {
  CHECK_LE(0, c_);
  CHECK_LE(c_, n_);
}

SingleSamplingPlan SingleSamplingPlan::Create(double p0, double p1,
                                              double alpha, double beta) {
  CHECK_LE(0, p1);
  CHECK_LT(p1, p0);
  CHECK_LE(p0, 1);
  CHECK_LT(0, alpha);
  CHECK_LT(alpha, 1);
  CHECK_LT(0, beta);
  CHECK_LT(beta, 1);
  if (p1 == 0 && p0 == 1) {
    return SingleSamplingPlan(1, 0);
  }
  if (p1 == 0) {
    return SingleSamplingPlan(ceil(log(alpha) / log(1 - p0)), 0);
  }
  if (p0 == 1) {
    const int n = ceil(log(beta) / log(p1));
    return SingleSamplingPlan(n, n - 1);
  }
  int n = 0;
  int c0 = -1;
  int c1 = 0;
  while (c0 < c1) {
    ++n;
    c0 = n - binoinv(1 - alpha, n, 1 - p0) - 1;
    c1 = binoinv(1 - beta, n, p1);
    VLOG(1) << n << '\t' << c0 << '\t' << c1;
  }
  return SingleSamplingPlan(n, (c0 + c1) / 2);
}
