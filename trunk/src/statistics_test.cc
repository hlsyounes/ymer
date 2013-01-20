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

#include "gtest/gtest.h"

namespace {

TEST(LChooseTest, All) {
  EXPECT_EQ(0, lchoose(0, 0));
  EXPECT_EQ(0, lchoose(1, 0));
  EXPECT_EQ(0, lchoose(1, 1));
  EXPECT_EQ(0, lchoose(2, 0));
  EXPECT_DOUBLE_EQ(log(2), lchoose(2, 1));
  EXPECT_EQ(0, lchoose(2, 2));
  EXPECT_DOUBLE_EQ(log(5), lchoose(5, 1));
  EXPECT_DOUBLE_EQ(log(15), lchoose(6, 2));
  EXPECT_DOUBLE_EQ(log(35), lchoose(7, 3));
  EXPECT_DOUBLE_EQ(log(70), lchoose(8, 4));
}

TEST(BinoInvTest, All) {
  EXPECT_EQ(0, binoinv(0, 7, 0.5));
  EXPECT_EQ(0, binoinv(0.007, 7, 0.5));
  EXPECT_EQ(1, binoinv(0.009, 7, 0.5));
  EXPECT_EQ(1, binoinv(0.062, 7, 0.5));
  EXPECT_EQ(2, binoinv(0.064, 7, 0.5));
  EXPECT_EQ(2, binoinv(0.226, 7, 0.5));
  EXPECT_EQ(3, binoinv(0.228, 7, 0.5));
  EXPECT_EQ(3, binoinv(0.499, 7, 0.5));
  EXPECT_EQ(4, binoinv(0.501, 7, 0.5));
  EXPECT_EQ(4, binoinv(0.772, 7, 0.5));
  EXPECT_EQ(5, binoinv(0.774, 7, 0.5));
  EXPECT_EQ(5, binoinv(0.936, 7, 0.5));
  EXPECT_EQ(6, binoinv(0.938, 7, 0.5));
  EXPECT_EQ(6, binoinv(0.991, 7, 0.5));
  EXPECT_EQ(7, binoinv(0.993, 7, 0.5));
  EXPECT_EQ(7, binoinv(1, 7, 0.5));

  EXPECT_EQ(0, binoinv(0, 7, 0));
  EXPECT_EQ(0, binoinv(0.5, 7, 0));
  EXPECT_EQ(0, binoinv(1, 7, 0));

  EXPECT_EQ(0, binoinv(0, 7, 1));
  EXPECT_EQ(7, binoinv(0.5, 7, 1));
  EXPECT_EQ(7, binoinv(1, 7, 1));

  EXPECT_EQ(0, binoinv(0, 5, 0.25));
  EXPECT_EQ(0, binoinv(0.236, 5, 0.25));
  EXPECT_EQ(1, binoinv(0.238, 5, 0.25));
  EXPECT_EQ(1, binoinv(0.632, 5, 0.25));
  EXPECT_EQ(2, binoinv(0.634, 5, 0.25));
  EXPECT_EQ(2, binoinv(0.896, 5, 0.25));
  EXPECT_EQ(3, binoinv(0.898, 5, 0.25));
  EXPECT_EQ(3, binoinv(0.984, 5, 0.25));
  EXPECT_EQ(4, binoinv(0.986, 5, 0.25));
  EXPECT_EQ(4, binoinv(0.999, 5, 0.25));
  EXPECT_EQ(5, binoinv(0.9995, 5, 0.25));
  EXPECT_EQ(5, binoinv(1, 5, 0.25));

  EXPECT_EQ(0, binoinv(0, 4, 0.75));
  EXPECT_EQ(0, binoinv(0.003, 4, 0.75));
  EXPECT_EQ(1, binoinv(0.005, 4, 0.75));
  EXPECT_EQ(1, binoinv(0.050, 4, 0.75));
  EXPECT_EQ(2, binoinv(0.052, 4, 0.75));
  EXPECT_EQ(2, binoinv(0.261, 4, 0.75));
  EXPECT_EQ(3, binoinv(0.263, 4, 0.75));
  EXPECT_EQ(3, binoinv(0.683, 4, 0.75));
  EXPECT_EQ(4, binoinv(0.684, 4, 0.75));
  EXPECT_EQ(4, binoinv(1, 4, 0.75));
}

TEST(SingleSamplingPlanTest, All) {
  const auto ssp1 = SingleSamplingPlan::Create(1, 0, 0.01, 0.02);
  EXPECT_EQ(1, ssp1.n());
  EXPECT_EQ(0, ssp1.c());

  const auto ssp2 = SingleSamplingPlan::Create(0.5, 0, 0.01, 0.02);
  EXPECT_EQ(7, ssp2.n());
  EXPECT_EQ(0, ssp2.c());

  const auto ssp3 = SingleSamplingPlan::Create(1, 0.75, 0.01, 0.02);
  EXPECT_EQ(14, ssp3.n());
  EXPECT_EQ(13, ssp3.c());

  const auto ssp4 = SingleSamplingPlan::Create(0.5, 0.3, 0.2, 0.1);
  EXPECT_EQ(30, ssp4.n());
  EXPECT_EQ(12, ssp4.c());

  const auto ssp5 = SingleSamplingPlan::Create(1, 0.99999, 0.1, 1e-10);
  EXPECT_EQ(2302574, ssp5.n());
  EXPECT_EQ(2302573, ssp5.c());
}

TEST(SampleTest, IntegerObservations) {
  Sample<int> s;

  s.AddObservation(2);
  EXPECT_EQ(2, s.min());
  EXPECT_EQ(2, s.max());
  EXPECT_EQ(1, s.count());
  EXPECT_EQ(2, s.mean());
  EXPECT_EQ(0, s.variance());
  EXPECT_EQ(0, s.stddev());

  s.AddObservation(3);
  EXPECT_EQ(2, s.min());
  EXPECT_EQ(3, s.max());
  EXPECT_EQ(2, s.count());
  EXPECT_EQ(2.5, s.mean());
  EXPECT_EQ(0.5, s.variance());
  EXPECT_EQ(sqrt(0.5), s.stddev());

  s.AddObservation(1);
  EXPECT_EQ(1, s.min());
  EXPECT_EQ(3, s.max());
  EXPECT_EQ(3, s.count());
  EXPECT_EQ(2, s.mean());
  EXPECT_EQ(1, s.variance());
  EXPECT_EQ(1, s.stddev());
}

}  // namespace
