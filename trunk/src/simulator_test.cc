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

CompiledExpression MakeGuard(int variable, int min_value, int max_value) {
  return CompiledExpression(
      {Operation::MakeILOAD(variable, 0), Operation::MakeICONST(min_value, 1),
       Operation::MakeIGE(0, 1), Operation::MakeIFFALSE(0, 7),
       Operation::MakeILOAD(variable, 0), Operation::MakeICONST(max_value, 1),
       Operation::MakeILE(0, 1)});
}

CompiledExpression MakeWeight(double weight) {
  return CompiledExpression({Operation::MakeDCONST(weight, 0)});
}

CompiledUpdate MakeUpdate(int variable, int delta) {
  return CompiledUpdate(variable,
                        CompiledExpression({Operation::MakeILOAD(variable, 0),
                                            Operation::MakeICONST(delta, 1),
                                            Operation::MakeIADD(0, 1)}));
}

TEST(NextStateSamplerTest, NoEvents) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", 0, 42, 17);
  CompiledExpressionEvaluator evaluator(0, 0);
  // No random numbers consumed.
  FakeEngine engine(0, 0, {});
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
           MakeGuard(0, 17, 17),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18, 18),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})})});
  CompiledExpressionEvaluator evaluator(2, 1);
  // One random number consumed per state transition.  No choice.
  FakeEngine engine(0, 0, {0, 0});
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

TEST(NextStateSamplerTest, OneEnabledMarkovEventCtmc) {
  CompiledModel model(CompiledModelType::CTMC);
  model.AddVariable("a", 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 17),
           {CompiledMarkovOutcome(MakeWeight(2.0), {MakeUpdate(0, 1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18, 18),
           {CompiledMarkovOutcome(MakeWeight(3.0), {MakeUpdate(0, 1)})})});
  CompiledExpressionEvaluator evaluator(2, 1);
  // Two random numbers consumed per state transition.  No choice.
  FakeEngine engine(0, 3, {0, 1, 0, 2});
  CompiledDistributionSampler<FakeEngine> sampler(&evaluator, &engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(-log(0.75) / 2.0, next_state.time());
  EXPECT_EQ(std::vector<int>({18}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.5) / 3.0, next_state.time());
  EXPECT_EQ(std::vector<int>({19}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({19}), next_state.values());
}

TEST(NextStateSamplerTest, MultipleEnabledMarkovEventsDtmc) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 18),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 17, 19),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18, 19),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -1)})})});
  CompiledExpressionEvaluator evaluator(2, 1);
  // 2 random numbers for the 1st state transition:
  //
  //   choice 1: log(1 - 1/4) = -0.288
  //   choice 2: log(1 - 0/4) = 0       [winner]
  //
  // 3 random numbers for the 2nd state transition:
  //
  //   choice 1: log(1 - 1/4) = -0.288
  //   choice 2: log(1 - 2/4) = -0.693
  //   choice 3: log(1 - 0/4) = 0       [winner]
  //
  // 2 random numbers for the 3rd state transition:
  //
  //   choice 1: log(1 - 0/4) = 0       [winner]
  //   choice 2: log(1 - 1/4) = -0.288
  //
  FakeEngine engine(0, 3, {1, 0, 1, 2, 0, 0, 1});
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
  EXPECT_EQ(std::vector<int>({17}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(3, next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
}

TEST(NextStateSamplerTest, MultipleEnabledMarkovEventsCtmc) {
  CompiledModel model(CompiledModelType::CTMC);
  model.AddVariable("a", 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 18),
           {CompiledMarkovOutcome(MakeWeight(2.0), {MakeUpdate(0, -1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 17, 19),
           {CompiledMarkovOutcome(MakeWeight(3.0), {MakeUpdate(0, 1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18, 19),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -1)})})});
  CompiledExpressionEvaluator evaluator(2, 1);
  // 3 random numbers for the 1st state transition:
  //
  //   choice 1: log(1 - 4/8) / 2 = -0.347
  //   choice 2: log(1 - 4/8) / 3 = -0.231  [winner]
  //   time: -log(1 - 2/8) / 3
  //
  // 4 random numbers for the 2nd state transition:
  //
  //   choice 1: log(1 - 4/8) / 2 = -0.347
  //   choice 2: log(1 - 6/8) / 3 = -0.462
  //   choice 3: log(1 - 1/8) / 1 = -0.134  [winner]
  //   time: -log(1 - 6/8) / 1
  //
  // 3 random numbers for the 3rd state transition:
  //
  //   choice 1: log(1 - 4/8) / 2 = -0.347  [winner]
  //   choice 2: log(1 - 6/8) / 3 = -0.462
  //   time: -log(1 - 4/8) / 2
  //
  FakeEngine engine(0, 7, {4, 4, 2, 4, 6, 1, 6, 4, 6, 4});
  CompiledDistributionSampler<FakeEngine> sampler(&evaluator, &engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(-log(0.75) / 3.0, next_state.time());
  EXPECT_EQ(std::vector<int>({18}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.25) / 1.0, next_state.time());
  EXPECT_EQ(std::vector<int>({17}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.5) / 2.0, next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
}

TEST(NextStateSamplerTest, ComplexMarkovEventsDtmc) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", 0, 42, 17);
  model.AddVariable("b", 0, 4711, 1);
  model.set_single_markov_commands({CompiledMarkovCommand(
      MakeGuard(0, 17, 18),
      {CompiledMarkovOutcome(MakeWeight(0.75), {MakeUpdate(0, -1)}),
       CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, 1)})})});
  model.set_factored_markov_commands(
      {{{CompiledMarkovCommand(
             MakeGuard(0, 17, 19),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19),
             {CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, -1)}),
              CompiledMarkovOutcome(MakeWeight(0.75), {MakeUpdate(0, 2)})})},
        {CompiledMarkovCommand(
            MakeGuard(1, 0, 1),
            {CompiledMarkovOutcome(MakeWeight(0.125), {MakeUpdate(1, 2)}),
             CompiledMarkovOutcome(MakeWeight(0.625), {MakeUpdate(1, 3)}),
             CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(1, -1)})})}},
       {{CompiledMarkovCommand(
             MakeGuard(0, 17, 19),
             {CompiledMarkovOutcome(MakeWeight(0.75), {MakeUpdate(0, 1)}),
              CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -5)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 19, 19),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -2)})})},
        {CompiledMarkovCommand(
             MakeGuard(1, 1, 1),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(1, 1)})}),
         CompiledMarkovCommand(
             MakeGuard(1, 1, 3),
             {CompiledMarkovOutcome(MakeWeight(0.5), {MakeUpdate(1, -2)}),
              CompiledMarkovOutcome(MakeWeight(0.5), {MakeUpdate(1, 1)})})}}});
  CompiledExpressionEvaluator evaluator(2, 1);
  // 7 random numbers for the 1st state transition:
  //
  //   command choice 1: log(1 - 2/8) = -0.288
  //   command choice 2: log(1 - 1/8) = -0.134  [winner]
  //   command choice 3: log(1 - 3/8) = -0.470
  //   command choice 4: log(1 - 4/8) = -0.693
  //   outcome choice 1: log(1 - 2/8) / 0.125 = -2.30  [winner]
  //   outcome choice 2: log(1 - 7/8) / 0.625 = -3.38
  //   outcome choice 3: log(1 - 4/8) / 0.25  = -2.77
  //
  // 5 random numbers for the 2nd state transition:
  //
  //   command choice 1: log(1 - 1/8) = -0.134
  //   command choice 2: log(1 - 2/8) = -0.288
  //   command choice 3: log(1 - 0/8) = 0       [winner]
  //   outcome choice 1: log(1 - 6/8) / 0.5 = -2.77
  //   outcome choice 2: log(1 - 1/8) / 0.5 = -0.267  [winner]
  //
  // 3 random number for the 3rd state transition:
  //
  //   command choice 1: log(1 - 0/8) = 0  [winner]
  //   outcome choice 1: log(1 - 2/8) / 0.75 = -0.384  [winner]
  //   outcome choice 2: log(1 - 1/8) / 0.25 = -0.534
  //
  FakeEngine engine(0, 7, {2, 1, 3, 4, 2, 7, 4, 1, 2, 0, 6, 1, 0, 2, 1});
  CompiledDistributionSampler<FakeEngine> sampler(&evaluator, &engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(1, next_state.time());
  EXPECT_EQ(std::vector<int>({19, 3}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(2, next_state.time());
  EXPECT_EQ(std::vector<int>({17, 4}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(3, next_state.time());
  EXPECT_EQ(std::vector<int>({16, 4}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16, 4}), next_state.values());
}

TEST(NextStateSamplerTest, ComplexMarkovEventsCtmc) {
  CompiledModel model(CompiledModelType::CTMC);
  model.AddVariable("a", 0, 42, 17);
  model.AddVariable("b", 0, 4711, 1);
  model.set_single_markov_commands({CompiledMarkovCommand(
      MakeGuard(0, 17, 18),
      {CompiledMarkovOutcome(MakeWeight(2.0), {MakeUpdate(0, -1)}),
       CompiledMarkovOutcome(MakeWeight(3.0), {MakeUpdate(0, 1)})})});
  model.set_factored_markov_commands(
      {{{CompiledMarkovCommand(
             MakeGuard(0, 17, 19),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19),
             {CompiledMarkovOutcome(MakeWeight(0.5), {MakeUpdate(0, -1)}),
              CompiledMarkovOutcome(MakeWeight(0.75), {MakeUpdate(0, 2)})})},
        {CompiledMarkovCommand(
            MakeGuard(1, 0, 1),
            {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(1, 2)}),
             CompiledMarkovOutcome(MakeWeight(2.75), {MakeUpdate(1, 3)}),
             CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(1, -1)})})}},
       {{CompiledMarkovCommand(
             MakeGuard(0, 17, 19),
             {CompiledMarkovOutcome(MakeWeight(0.75), {MakeUpdate(0, 1)}),
              CompiledMarkovOutcome(MakeWeight(1.25), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -5)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 19, 19),
             {CompiledMarkovOutcome(MakeWeight(1.5), {MakeUpdate(0, -2)})})},
        {CompiledMarkovCommand(
             MakeGuard(1, 1, 1),
             {CompiledMarkovOutcome(MakeWeight(2.0), {MakeUpdate(1, 1)})}),
         CompiledMarkovCommand(
             MakeGuard(1, 1, 3),
             {CompiledMarkovOutcome(MakeWeight(1.25), {MakeUpdate(1, -2)}),
              CompiledMarkovOutcome(MakeWeight(1.75), {MakeUpdate(1, 1)})})}}});
  CompiledExpressionEvaluator evaluator(2, 1);
  // 8 random numbers for the 1st state transition:
  //
  //   command choice 1: log(1 - 2/8) / 5 = -0.0575
  //   command choice 2: log(1 - 1/8) / 4 = -0.0334  [winner]
  //   command choice 3: log(1 - 2/8) / 4 = -0.0719
  //   command choice 4: log(1 - 2/8) / 6 = -0.0479
  //   outcome choice 1: log(1 - 3/8) / 1    = -0.470  [winner]
  //   outcome choice 2: log(1 - 6/8) / 2.75 = -0.504
  //   outcome choice 3: log(1 - 1/8) / 0.25 = -0.534
  //   time: log(1 - 7/8) / 1
  //
  // 6 random numbers for the 2nd state transition:
  //
  //   command choice 1: log(1 - 5/8) / 6   = -0.163
  //   command choice 2: log(1 - 3/8) / 3   = -0.567
  //   command choice 3: log(1 - 4/8) / 4.5 = -0.154  [winner]
  //   outcome choice 1: log(1 - 7/8) / 1.25 = -1.66
  //   outcome choice 2: log(1 - 7/8) / 1.75 = -1.19  [winner]
  //   time: log(1 - 5/8) / 2.625
  //
  // 4 random number for the 3rd state transition:
  //
  //   command choice 1: log(1 - 0/8) / 6 = 0  [winner]
  //   outcome choice 1: log(1 - 4/8) / 2 = -0.347  [winner]
  //   outcome choice 2: log(1 - 6/8) / 3 = -0.462
  //   time: log(1 - 3/8) / 2
  //
  FakeEngine engine(0, 7,
                    {2, 1, 2, 2, 3, 6, 1, 7, 5, 3, 4, 7, 7, 5, 0, 4, 6, 3});
  CompiledDistributionSampler<FakeEngine> sampler(&evaluator, &engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(-log(0.125) / 1.0, next_state.time());
  EXPECT_EQ(std::vector<int>({19, 3}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.375) / 2.625, next_state.time());
  EXPECT_EQ(std::vector<int>({17, 4}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.625) / 2.0, next_state.time());
  EXPECT_EQ(std::vector<int>({16, 4}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16, 4}), next_state.values());
}

TEST(NextStateSamplerTest, BreaksTiesForMarkovCommands) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 18),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -2)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 17, 18),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})})});
  CompiledExpressionEvaluator evaluator(2, 1);
  // 3 random numbers consumed per state transition:
  //
  //   1st transition: 2nd command wins tie because 1/4 * 2 < 1
  //
  //   2nd transition: 1st command wins tie because 2/4 * 2 >= 1
  //
  FakeEngine engine(0, 3, {0, 0, 1, 3, 3, 2});
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
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
}

TEST(NextStateSamplerTest, BreaksTiesForMarkovOutcomes) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", 0, 42, 17);
  model.set_single_markov_commands({CompiledMarkovCommand(
      MakeGuard(0, 17, 19),
      {CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, 2)}),
       CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, 1)}),
       CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, -1)}),
       CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, -3)})})});
  CompiledExpressionEvaluator evaluator(2, 1);
  // 8 random numbers consumed per state transition:
  //
  //   1st transition: 2nd outcome wins tie because
  //     3/8 * 2 < 1, 3/8 * 3 >= 1, 2/8 * 4 >= 1
  //
  //   2nd transition: 3rd outcome wins tie because
  //     2/8 * 2 < 1, 2/8 * 3 < 1, 2/8 * 4 >= 1
  //
  //   3rd transition: 1st outcome wins tie because
  //     4/8 * 2 >= 1, 3/8 * 3 >= 1, 2/8 * 4 >= 1
  //
  //   4th transition: 4th outcome wins tie because
  //     0/8 * 2 < 1, 7/8 * 3 >= 1, 1/8 * 4 < 1
  //
  FakeEngine engine(0, 7, {0, 0, 0, 3, 0, 3, 0, 2, 1, 1, 1, 2, 1, 2, 1, 2, 2, 2,
                           2, 4, 2, 3, 2, 2, 3, 3, 3, 0, 3, 7, 3, 1});
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
  EXPECT_EQ(std::vector<int>({17}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(3, next_state.time());
  EXPECT_EQ(std::vector<int>({19}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(4, next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
}

// TODO(hlsyounes): Add tests for GSMP events.

}  // namespace
