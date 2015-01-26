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

#include "compiled-distribution.h"

#include "gtest/gtest.h"

namespace {

TEST(CompiledDistributionTest, MakesWeibull) {
  const CompiledGsmpDistribution dist(
      CompiledGsmpDistribution::MakeWeibull(17.0, 0.5));
  EXPECT_EQ(CompiledGsmpDistributionType::WEIBULL, dist.type());
  EXPECT_EQ(std::vector<double>({17, 0.5}), dist.parameters());
}

TEST(CompiledDistributionTest, MakesLognormal) {
  const CompiledGsmpDistribution dist(
      CompiledGsmpDistribution::MakeLognormal(17.0, 0.5));
  EXPECT_EQ(CompiledGsmpDistributionType::LOGNORMAL, dist.type());
  EXPECT_EQ(std::vector<double>({17, 0.5}), dist.parameters());
}

TEST(CompiledDistributionTest, MakesUniform) {
  const CompiledGsmpDistribution dist(
      CompiledGsmpDistribution::MakeUniform(0.5, 2));
  EXPECT_EQ(CompiledGsmpDistributionType::UNIFORM, dist.type());
  EXPECT_EQ(std::vector<double>({0.5, 2}), dist.parameters());
}

}  // namespace
