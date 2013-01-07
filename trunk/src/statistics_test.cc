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

#include "gtest/gtest.h"

namespace {

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
