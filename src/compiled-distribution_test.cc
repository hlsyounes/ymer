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

#include "compiled-expression.h"

#include "gtest/gtest.h"

namespace {

class CompiledDistributionTest : public testing::Test {
 protected:
  CompiledDistributionTest()
      : e1(CompiledExpression({ Operation::MakeDCONST(1, 0) })),
        e2(CompiledExpression({ Operation::MakeDCONST(2, 0) })),
        evaluator_(0, 1) {
  }

  std::vector<double> Evaluate(const std::vector<CompiledExpression>& exprs) {
    std::vector<double> values;
    for (const CompiledExpression& expr : exprs) {
      values.push_back(evaluator_.EvaluateDoubleExpression(expr, {}));
    }
    return values;
  }

  CompiledExpression e1;
  CompiledExpression e2;

 private:
  CompiledExpressionEvaluator evaluator_;
};

TEST_F(CompiledDistributionTest, MakesMemoryless) {
  const CompiledDistribution dist(CompiledDistribution::MakeMemoryless(e1));
  EXPECT_EQ(DistributionType::MEMORYLESS, dist.type());
  EXPECT_EQ(std::vector<double>({ 1 }), Evaluate(dist.parameters()));
}

TEST_F(CompiledDistributionTest, MakesWeibull) {
  const CompiledDistribution dist(CompiledDistribution::MakeWeibull(e1, e2));
  EXPECT_EQ(DistributionType::WEIBULL, dist.type());
  EXPECT_EQ(std::vector<double>({ 1, 2 }), Evaluate(dist.parameters()));
}

TEST_F(CompiledDistributionTest, MakesLognormal) {
  const CompiledDistribution dist(CompiledDistribution::MakeLognormal(e1, e2));
  EXPECT_EQ(DistributionType::LOGNORMAL, dist.type());
  EXPECT_EQ(std::vector<double>({ 1, 2 }), Evaluate(dist.parameters()));
}

TEST_F(CompiledDistributionTest, MakesUniform) {
  const CompiledDistribution dist(CompiledDistribution::MakeUniform(e1, e2));
  EXPECT_EQ(DistributionType::UNIFORM, dist.type());
  EXPECT_EQ(std::vector<double>({ 1, 2 }), Evaluate(dist.parameters()));
}

}  // namespace
