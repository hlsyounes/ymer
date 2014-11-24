// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2014 Google Inc
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

#include "simulator.h"

#include "compiled-distribution.h"
#include "compiled-expression.h"
#include "compiled-model.h"
#include "fake_rng.h"

#include "gtest/gtest.h"

namespace {

CompiledExpression MakeGuard(int variable, int value) {
  return CompiledExpression({Operation::MakeILOAD(0, 0),
                             Operation::MakeICONST(value, 1),
                             Operation::MakeIEQ(0, 1)});
}

CompiledExpression MakeWeight(double weight) {
  return CompiledExpression({Operation::MakeDCONST(weight, 0)});
}

CompiledUpdate MakeUpdate(int variable, int value) {
  return CompiledUpdate(variable,
                        CompiledExpression({Operation::MakeICONST(value, 0)}));
}

TEST(NextStateSamplerTest, NoEvents) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", 0, 42, 17);
  CompiledExpressionEvaluator evaluator(0, 0);
  FakeEngine engine(0, 0xffffffff, {});
  CompiledDistributionSampler<FakeEngine> sampler(&evaluator, &engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(state.values(), next_state.values());
}

TEST(NextStateSamplerTest, OneEnabledMarkovEventDtmc) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 18)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 19)})})});
  CompiledExpressionEvaluator evaluator(2, 1);
  FakeEngine engine(0, 0xffffffff, {0, 0});
  CompiledDistributionSampler<FakeEngine> sampler(&evaluator, &engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(1, next_state.time());
  EXPECT_EQ(std::vector<int>({18}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(2, next_state.time());
  EXPECT_EQ(std::vector<int>({19}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({19}), next_state.values());
}

}  // namespace
