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
#include <limits>

#include "gtest/gtest.h"

namespace {

TEST(BinoInvTest, SmallN) {
  EXPECT_EQ(0, binoinv(0, 7, 0.5));
  EXPECT_EQ(0, binoinv(0.0078125, 7, 0.5));
  EXPECT_EQ(1, binoinv(0.0078126, 7, 0.5));
  EXPECT_EQ(1, binoinv(0.0625, 7, 0.5));
  EXPECT_EQ(2, binoinv(0.0626, 7, 0.5));
  EXPECT_EQ(2, binoinv(0.2265625, 7, 0.5));
  EXPECT_EQ(3, binoinv(0.2265626, 7, 0.5));
  EXPECT_EQ(3, binoinv(0.5, 7, 0.5));
  EXPECT_EQ(4, binoinv(0.51, 7, 0.5));
  EXPECT_EQ(4, binoinv(0.7734375, 7, 0.5));
  EXPECT_EQ(5, binoinv(0.7734376, 7, 0.5));
  EXPECT_EQ(5, binoinv(0.9375, 7, 0.5));
  EXPECT_EQ(6, binoinv(0.9376, 7, 0.5));
  EXPECT_EQ(6, binoinv(0.9921875, 7, 0.5));
  EXPECT_EQ(7, binoinv(0.9921876, 7, 0.5));
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
  EXPECT_EQ(4, binoinv(0.9990234375, 5, 0.25));
  EXPECT_EQ(5, binoinv(0.9990234376, 5, 0.25));
  EXPECT_EQ(5, binoinv(1, 5, 0.25));

  EXPECT_EQ(0, binoinv(0, 4, 0.75));
  EXPECT_EQ(0, binoinv(0.00390625, 4, 0.75));
  EXPECT_EQ(1, binoinv(0.00390626, 4, 0.75));
  EXPECT_EQ(1, binoinv(0.050, 4, 0.75));
  EXPECT_EQ(2, binoinv(0.052, 4, 0.75));
  EXPECT_EQ(2, binoinv(0.261, 4, 0.75));
  EXPECT_EQ(3, binoinv(0.263, 4, 0.75));
  EXPECT_EQ(3, binoinv(0.683, 4, 0.75));
  EXPECT_EQ(4, binoinv(0.684, 4, 0.75));
  EXPECT_EQ(4, binoinv(1, 4, 0.75));
}

TEST(BinoInvTest, LargeN) {
  EXPECT_EQ(0, binoinv(0, 2000, 0.5));
  EXPECT_EQ(1000, binoinv(0.5, 2000, 0.5));
  EXPECT_EQ(2000, binoinv(1, 2000, 0.5));
}

TEST(MaxNestedErrorTest, All) {
  EXPECT_EQ(0.2 / 0.7, MaxNestedError(0.2));
  EXPECT_EQ(0.1 / 0.6, MaxNestedError(0.1));
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

  const auto ssp6 = SingleSamplingPlan::Create(0.25, 0, 0.02, 0.01);
  EXPECT_EQ(14, ssp6.n());
  EXPECT_EQ(0, ssp6.c());
}

TEST(SampleTest, IntegerObservations) {
  Sample<int> s;
  EXPECT_EQ(0, s.count());

  s.AddObservation(2);
  EXPECT_EQ(2, s.min());
  EXPECT_EQ(2, s.max());
  EXPECT_EQ(2, s.sum());
  EXPECT_EQ(1, s.count());
  EXPECT_EQ(2, s.mean());
  EXPECT_EQ(0, s.variance());
  EXPECT_EQ(0, s.sample_variance());
  EXPECT_EQ(0, s.sample_stddev());

  s.AddObservation(3);
  EXPECT_EQ(2, s.min());
  EXPECT_EQ(3, s.max());
  EXPECT_EQ(5, s.sum());
  EXPECT_EQ(2, s.count());
  EXPECT_EQ(2.5, s.mean());
  EXPECT_EQ(0.25, s.variance());
  EXPECT_EQ(0.5, s.sample_variance());
  EXPECT_EQ(sqrt(0.5), s.sample_stddev());

  s.AddObservation(1);
  EXPECT_EQ(1, s.min());
  EXPECT_EQ(3, s.max());
  EXPECT_EQ(6, s.sum());
  EXPECT_EQ(3, s.count());
  EXPECT_EQ(2, s.mean());
  EXPECT_EQ(2.0 / 3.0, s.variance());
  EXPECT_EQ(1, s.sample_variance());
  EXPECT_EQ(1, s.sample_stddev());
  EXPECT_TRUE(s.distribution().empty());
}

TEST(SampleTest, MergeFromIntegerObservations) {
  Sample<int> s1, s2;

  s1.AddObservation(2);
  s1.MergeFrom(s2);
  EXPECT_EQ(2, s1.min());
  EXPECT_EQ(2, s1.max());
  EXPECT_EQ(2, s1.sum());
  EXPECT_EQ(1, s1.count());
  EXPECT_EQ(2, s1.mean());
  EXPECT_EQ(0, s1.variance());
  EXPECT_EQ(0, s1.sample_variance());
  EXPECT_EQ(0, s1.sample_stddev());

  s2.AddObservation(3);
  s2.AddObservation(1);
  s1.MergeFrom(s2);
  EXPECT_EQ(1, s1.min());
  EXPECT_EQ(3, s1.max());
  EXPECT_EQ(6, s1.sum());
  EXPECT_EQ(3, s1.count());
  EXPECT_EQ(2, s1.mean());
  EXPECT_EQ(2.0 / 3.0, s1.variance());
  EXPECT_EQ(1, s1.sample_variance());
  EXPECT_EQ(1, s1.sample_stddev());
  EXPECT_TRUE(s1.distribution().empty());
}

TEST(SampleTest, IntegerObservationsWithDistribution) {
  using Dist = std::map<int, int>;
  Sample<int> s(true);
  EXPECT_EQ(Dist(), s.distribution());

  s.AddObservation(2);
  EXPECT_EQ(Dist({{2, 1}}), s.distribution());

  s.AddObservation(3);
  EXPECT_EQ(Dist({{2, 2}}), s.distribution());

  s.AddObservation(1);
  EXPECT_EQ(Dist({{1, 1}, {2, 2}}), s.distribution());

  s.AddObservation(4);
  EXPECT_EQ(Dist({{1, 1}, {2, 2}, {3, 1}}), s.distribution());

  s.AddObservation(0);
  EXPECT_EQ(Dist({{0, 1}, {1, 1}, {2, 2}, {3, 1}}), s.distribution());
}

TEST(SampleTest, MergeFromIntegerObservationsWithDistribution) {
  using Dist = std::map<int, int>;
  Sample<int> s1(true), s2(true), s3(true);

  s2.AddObservation(2);
  s2.AddObservation(3);
  s1.MergeFrom(s2);
  EXPECT_EQ(Dist({{2, 2}}), s1.distribution());

  s3.AddObservation(1);
  s3.AddObservation(4);
  s3.AddObservation(0);
  s1.MergeFrom(s3);
  EXPECT_EQ(Dist({{0, 1}, {1, 1}, {2, 2}, {3, 1}}), s1.distribution());
}

TEST(SampleTest, BoolObservations) {
  Sample<bool> s;
  EXPECT_EQ(0, s.count());

  s.AddObservation(true);
  EXPECT_TRUE(s.min());
  EXPECT_TRUE(s.max());
  EXPECT_EQ(1, s.sum());
  EXPECT_EQ(1, s.count());
  EXPECT_EQ(1, s.mean());
  EXPECT_EQ(0, s.variance());
  EXPECT_EQ(0, s.sample_variance());
  EXPECT_EQ(0, s.sample_stddev());

  s.AddObservation(false);
  EXPECT_FALSE(s.min());
  EXPECT_TRUE(s.max());
  EXPECT_EQ(1, s.sum());
  EXPECT_EQ(2, s.count());
  EXPECT_EQ(0.5, s.mean());
  EXPECT_EQ(0.25, s.variance());
  EXPECT_EQ(0.5, s.sample_variance());
  EXPECT_EQ(sqrt(0.5), s.sample_stddev());

  s.AddObservation(true);
  EXPECT_FALSE(s.min());
  EXPECT_TRUE(s.max());
  EXPECT_EQ(2, s.sum());
  EXPECT_EQ(3, s.count());
  EXPECT_EQ(2.0 / 3.0, s.mean());
  EXPECT_DOUBLE_EQ(2.0 / 9.0, s.variance());
  EXPECT_DOUBLE_EQ(2.0 / 6.0, s.sample_variance());
  EXPECT_DOUBLE_EQ(sqrt(2.0 / 6.0), s.sample_stddev());
}

TEST(ChowRobbinsTesterTest, IntegerObservations) {
  ChowRobbinsTester<int> tester(0.6, 0.4, 0.01);
  EXPECT_EQ(0.6, tester.theta0());
  EXPECT_EQ(0.4, tester.theta1());
  EXPECT_FALSE(tester.done());

  tester.AddObservation(2);
  EXPECT_FALSE(tester.done());
  EXPECT_EQ(1, tester.sample().count());
  EXPECT_EQ(2, tester.sample().mean());

  tester.AddObservation(3);
  EXPECT_FALSE(tester.done());
  EXPECT_EQ(2, tester.sample().count());
  EXPECT_EQ(2.5, tester.sample().mean());

  tester.AddObservation(1);
  EXPECT_FALSE(tester.done());
  EXPECT_EQ(3, tester.sample().count());
  EXPECT_EQ(2, tester.sample().mean());
}

TEST(FixedSampleSizeTesterTest, AcceptsThenRejects) {
  FixedSampleSizeTester<bool> tester(0.6, 0.4, 3);
  EXPECT_EQ(0.6, tester.theta0());
  EXPECT_EQ(0.4, tester.theta1());
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_TRUE(tester.done());
  EXPECT_TRUE(tester.accept());

  Sample<bool> sample;
  sample.AddObservation(false);
  sample.AddObservation(true);
  tester.SetSample(sample);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_TRUE(tester.done());
  EXPECT_FALSE(tester.accept());
}

TEST(FixedSampleSizeTesterTest, RejectsThenAccepts) {
  FixedSampleSizeTester<bool> tester(0.75, 0.75, 3);
  EXPECT_EQ(0.75, tester.theta0());
  EXPECT_EQ(0.75, tester.theta1());
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_TRUE(tester.done());
  EXPECT_FALSE(tester.accept());

  Sample<bool> sample;
  sample.AddObservation(true);
  sample.AddObservation(true);
  tester.SetSample(sample);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_TRUE(tester.done());
  EXPECT_TRUE(tester.accept());
}

TEST(SingleSamplingBernoulliTesterTest, AcceptsThenRejects) {
  SingleSamplingBernoulliTester tester(0.5, 0.3, 0.2, 0.1);
  EXPECT_EQ(0.5, tester.theta0());
  EXPECT_EQ(0.3, tester.theta1());
  for (int i = 0; i < 13; ++i) {
    EXPECT_FALSE(tester.done());
    tester.AddObservation(true);
  }
  EXPECT_TRUE(tester.done());
  EXPECT_TRUE(tester.accept());

  Sample<bool> sample;
  sample.AddObservation(true);
  for (int i = 0; i < 17; ++i) {
    sample.AddObservation(false);
  }
  sample.AddObservation(true);
  tester.SetSample(sample);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_TRUE(tester.done());
  EXPECT_FALSE(tester.accept());
}

TEST(SingleSamplingBernoulliTesterTest, RejectsThenAccepts) {
  SingleSamplingBernoulliTester tester(0.5, 0.3, 0.2, 0.1);
  EXPECT_EQ(0.5, tester.theta0());
  EXPECT_EQ(0.3, tester.theta1());
  for (int i = 0; i < 18; ++i) {
    EXPECT_FALSE(tester.done());
    tester.AddObservation(false);
  }
  EXPECT_TRUE(tester.done());
  EXPECT_FALSE(tester.accept());

  Sample<bool> sample;
  sample.AddObservation(false);
  for (int i = 0; i < 12; ++i) {
    sample.AddObservation(true);
  }
  sample.AddObservation(false);
  tester.SetSample(sample);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_TRUE(tester.done());
  EXPECT_TRUE(tester.accept());
}

TEST(SprtBernoulliTesterTest, AcceptsThenRejects) {
  SprtBernoulliTester tester(0.5, 0.3, 0.2, 0.1);
  EXPECT_EQ(0.5, tester.theta0());
  EXPECT_EQ(0.3, tester.theta1());
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_TRUE(tester.done());
  EXPECT_TRUE(tester.accept());

  Sample<bool> sample;
  sample.AddObservation(false);
  sample.AddObservation(false);
  sample.AddObservation(false);
  sample.AddObservation(false);
  tester.SetSample(sample);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_TRUE(tester.done());
  EXPECT_FALSE(tester.accept());
}

TEST(SprtBernoulliTesterTest, RejectsThenAccepts) {
  SprtBernoulliTester tester(0.5, 0.3, 0.2, 0.1);
  EXPECT_EQ(0.5, tester.theta0());
  EXPECT_EQ(0.3, tester.theta1());
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_TRUE(tester.done());
  EXPECT_FALSE(tester.accept());

  Sample<bool> sample;
  sample.AddObservation(true);
  sample.AddObservation(false);
  sample.AddObservation(true);
  sample.AddObservation(true);
  sample.AddObservation(false);
  sample.AddObservation(true);
  sample.AddObservation(true);
  tester.SetSample(sample);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_TRUE(tester.done());
  EXPECT_TRUE(tester.accept());
}

TEST(SprtBernoulliTesterTest, AcceptsThenRejectsWithMaxTheta0) {
  SprtBernoulliTester tester(1, 0.75, 0.01, 0.02);
  for (int i = 0; i < 14; ++i) {
    EXPECT_FALSE(tester.done());
    tester.AddObservation(true);
  }
  EXPECT_TRUE(tester.done());
  EXPECT_TRUE(tester.accept());

  Sample<bool> sample;
  sample.AddObservation(true);
  tester.SetSample(sample);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(false);
  EXPECT_TRUE(tester.done());
  EXPECT_FALSE(tester.accept());
}

TEST(SprtBernoulliTesterTest, RejectsThenAcceptsWithMinTheta1) {
  SprtBernoulliTester tester(0.25, 0, 0.02, 0.01);
  for (int i = 0; i < 14; ++i) {
    EXPECT_FALSE(tester.done());
    tester.AddObservation(false);
  }
  EXPECT_TRUE(tester.done());
  EXPECT_FALSE(tester.accept());

  Sample<bool> sample;
  sample.AddObservation(false);
  tester.SetSample(sample);
  EXPECT_FALSE(tester.done());
  tester.AddObservation(true);
  EXPECT_TRUE(tester.done());
  EXPECT_TRUE(tester.accept());
}

}  // namespace
