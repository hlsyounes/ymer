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
  model.AddVariable("a", Type::INT, 0, 42, 17);
  EXPECT_EQ(0, model.EventCount());
  CompiledExpressionEvaluator evaluator(0, 0);
  // No random numbers consumed.
  FakeEngine engine(0, 0, {});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(state.values(), next_state.values());
}

TEST(NextStateSamplerTest, OneEnabledMarkovEventDtmc) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 17), MakeWeight(1.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18, 18), MakeWeight(1.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})})});
  EXPECT_EQ(2, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // No random numbers consumed.  No choice.
  FakeEngine engine(0, 0, {});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
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
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 17), MakeWeight(2.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18, 18), MakeWeight(3.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})})});
  EXPECT_EQ(2, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // One random number consumed per state transition.  No choice.
  FakeEngine engine(0, 3, {1, 2});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
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
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 18), MakeWeight(1.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -2)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 17, 19), MakeWeight(1.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18, 19), MakeWeight(1.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -1)})})});
  EXPECT_EQ(3, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // 1 random number for the 1st state transition:
  //
  //   2nd command wins tie because 1/4 * 2 < 1
  //
  // 2 random numbers for the 2nd state transition:
  //
  //   3rd command wins tie because 0/4 * 3 < 1
  //
  // 1 random number for the 3rd state transition:
  //
  //   1st command winst tie because 2/4 * 2 >= 1
  //
  FakeEngine engine(0, 3, {1, 1, 0, 2});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
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
  EXPECT_EQ(std::vector<int>({15}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({15}), next_state.values());
}

TEST(NextStateSamplerTest, MultipleEnabledMarkovEventsCtmc) {
  CompiledModel model(CompiledModelType::CTMC);
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 18), MakeWeight(2.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -2)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 17, 19), MakeWeight(3.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 18, 19), MakeWeight(1.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -1)})})});
  EXPECT_EQ(3, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // 2 random numbers for the 1st state transition:
  //
  //   choice 1: -log(1 - 4/8) / 2 = 0.347
  //   choice 2: -log(1 - 4/8) / 3 = 0.231  [winner]
  //
  // 3 random numbers for the 2nd state transition:
  //
  //   choice 1: -log(1 - 4/8) / 2 = 0.347
  //   choice 2: -log(1 - 6/8) / 3 = 0.462
  //   choice 3: -log(1 - 1/8) / 1 = 0.134  [winner]
  //
  // 2 random numbers for the 3rd state transition:
  //
  //   choice 1: -log(1 - 4/8) / 2 = 0.347  [winner]
  //   choice 2: -log(1 - 6/8) / 3 = 0.462
  //
  FakeEngine engine(0, 7, {4, 4, 4, 6, 1, 4, 6});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(-log(0.5) / 3.0, next_state.time());
  EXPECT_EQ(std::vector<int>({18}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.875), next_state.time());
  EXPECT_EQ(std::vector<int>({17}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.5) / 2.0, next_state.time());
  EXPECT_EQ(std::vector<int>({15}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({15}), next_state.values());
}

TEST(NextStateSamplerTest, ComplexMarkovEventsDtmc) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.AddVariable("b", Type::INT, 0, 4711, 1);
  model.set_single_markov_commands({CompiledMarkovCommand(
      MakeGuard(0, 17, 18), MakeWeight(1.0),
      {CompiledMarkovOutcome(MakeWeight(0.75), {MakeUpdate(0, -1)}),
       CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, 1)})})});
  model.set_factored_markov_commands(
      {{{CompiledMarkovCommand(
             MakeGuard(0, 17, 19), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, -1)}),
              CompiledMarkovOutcome(MakeWeight(0.75), {MakeUpdate(0, 2)})})},
        {CompiledMarkovCommand(
            MakeGuard(1, 0, 1), MakeWeight(1.0),
            {CompiledMarkovOutcome(MakeWeight(0.125), {MakeUpdate(1, 2)}),
             CompiledMarkovOutcome(MakeWeight(0.625), {MakeUpdate(1, 3)}),
             CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(1, -1)})})}},
       {{CompiledMarkovCommand(
             MakeGuard(0, 17, 19), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(0.75), {MakeUpdate(0, 1)}),
              CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -5)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 19, 19), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -2)})})},
        {CompiledMarkovCommand(
             MakeGuard(1, 1, 1), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(1, 1)})}),
         CompiledMarkovCommand(
             MakeGuard(1, 1, 3), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(0.5), {MakeUpdate(1, -2)}),
              CompiledMarkovOutcome(MakeWeight(0.5), {MakeUpdate(1, 1)})})}}});
  EXPECT_EQ(2 + 3 * 3 + 4 * 3, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // 4 random numbers for the 1st state transition:
  //
  //   2nd command wins because 3/8 * 2 < 1, 3/8 * 3 >= 1, 2/8 * 4 >=1
  //   1st outcome wins because 0/8 < 0.125
  //
  // 3 random numbers for the 2nd state transition:
  //
  //   3rd command wins because 2/8 * 3 < 1
  //   2nd outcome wins because 4/8 >= 0.5
  //
  // 1 random number for the 3rd state transition:
  //
  //   1st outcome wins because 5/8 < 0.75
  //
  FakeEngine engine(0, 7, {3, 3, 2, 0, 7, 2, 4, 5});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
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
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.AddVariable("b", Type::INT, 0, 4711, 1);
  model.set_single_markov_commands({CompiledMarkovCommand(
      MakeGuard(0, 17, 18), MakeWeight(5.0),
      {CompiledMarkovOutcome(MakeWeight(0.4), {MakeUpdate(0, -1)}),
       CompiledMarkovOutcome(MakeWeight(0.6), {MakeUpdate(0, 1)})})});
  model.set_factored_markov_commands(
      {{{CompiledMarkovCommand(
             MakeGuard(0, 17, 19), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19), MakeWeight(1.25),
             {CompiledMarkovOutcome(MakeWeight(0.4), {MakeUpdate(0, -1)}),
              CompiledMarkovOutcome(MakeWeight(0.6), {MakeUpdate(0, 2)})})},
        {CompiledMarkovCommand(
            MakeGuard(1, 0, 1), MakeWeight(4.0),
            {CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(1, 2)}),
             CompiledMarkovOutcome(MakeWeight(0.6875), {MakeUpdate(1, 3)}),
             CompiledMarkovOutcome(MakeWeight(0.0625), {MakeUpdate(1, -1)})})}},
       {{CompiledMarkovCommand(
             MakeGuard(0, 17, 19), MakeWeight(2.0),
             {CompiledMarkovOutcome(MakeWeight(0.375), {MakeUpdate(0, 1)}),
              CompiledMarkovOutcome(MakeWeight(0.625), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -5)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 19, 19), MakeWeight(1.5),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -2)})})},
        {CompiledMarkovCommand(
             MakeGuard(1, 1, 1), MakeWeight(2.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(1, 1)})}),
         CompiledMarkovCommand(MakeGuard(1, 1, 3), MakeWeight(3.0),
                               {CompiledMarkovOutcome(MakeWeight(1.25 / 3.0),
                                                      {MakeUpdate(1, -2)}),
                                CompiledMarkovOutcome(MakeWeight(1.75 / 3.0),
                                                      {MakeUpdate(1, 1)})})}}});
  EXPECT_EQ(2 + 3 * 3 + 4 * 3, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // 5 random numbers for the 1st state transition:
  //
  //   command choice 1: -log(1 - 2/8) / 5 = 0.0575
  //   command choice 2: -log(1 - 1/8) / 4 = 0.0334  [winner]
  //   command choice 3: -log(1 - 2/8) / 4 = 0.0719
  //   command choice 4: -log(1 - 2/8) / 6 = 0.0479
  //   1st outcome wins because 1/8 < 0.25
  //
  // 4 random numbers for the 2nd state transition:
  //
  //   command choice 1: -log(1 - 5/8) / 6    = 0.163
  //   command choice 2: -log(1 - 3/8) / 3    = 0.567
  //   command choice 3: -log(1 - 4/8) / 4.5  = 0.154  [winner]
  //   2nd outcome wins because 4/8 >= 1.25 / 3
  //
  // 2 random number for the 3rd state transition:
  //
  //   command choice 1: -log(1 - 6/8) / 5 = 0.277  [winner]
  //   1st outcome wins because 3/8 < 0.4
  //
  FakeEngine engine(0, 7, {2, 1, 2, 2, 1, 5, 3, 4, 4, 6, 3});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(-log(0.875) / 4.0, next_state.time());
  EXPECT_EQ(std::vector<int>({19, 3}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.5) / 4.5, next_state.time());
  EXPECT_EQ(std::vector<int>({17, 4}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(state.time() - log(0.25) / 5.0, next_state.time());
  EXPECT_EQ(std::vector<int>({16, 4}), next_state.values());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16, 4}), next_state.values());
}

TEST(NextStateSamplerTest, BreaksTiesForMarkovCommands) {
  CompiledModel model(CompiledModelType::DTMC);
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           MakeGuard(0, 17, 18), MakeWeight(1.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, -2)})}),
       CompiledMarkovCommand(
           MakeGuard(0, 17, 18), MakeWeight(1.0),
           {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 1)})})});
  EXPECT_EQ(2, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // 1 random number consumed per state transition:
  //
  //   1st transition: 2nd command wins tie because 1/4 * 2 < 1
  //
  //   2nd transition: 1st command wins tie because 2/4 * 2 >= 1
  //
  FakeEngine engine(0, 3, {1, 2});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
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

TEST(NextStateSamplerTest, OneEnabledGsmpEvent) {
  CompiledModel model(CompiledModelType::GSMP);
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.set_single_gsmp_commands(
      {CompiledGsmpCommand(MakeGuard(0, 17, 17),
                           CompiledGsmpDistribution::MakeUniform(3.0, 5.0),
                           {MakeUpdate(0, 1)}, 0),
       CompiledGsmpCommand(MakeGuard(0, 18, 18),
                           CompiledGsmpDistribution::MakeUniform(7.0, 11.0),
                           {MakeUpdate(0, 1)}, 1)});
  EXPECT_EQ(2, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // One random number consumed per state transition.  No choice.
  FakeEngine engine(0, 3, {1, 3});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(3.5, next_state.time());
  EXPECT_EQ(std::vector<int>({18}), next_state.values());
  EXPECT_EQ(std::vector<double>({std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(13.5, next_state.time());
  EXPECT_EQ(std::vector<int>({19}), next_state.values());
  EXPECT_EQ(std::vector<double>({std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({19}), next_state.values());
  EXPECT_EQ(std::vector<double>({std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
}

TEST(NextStateSamplerTest, MultipleEnabledGsmpEvents) {
  CompiledModel model(CompiledModelType::GSMP);
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.set_single_gsmp_commands(
      {CompiledGsmpCommand(MakeGuard(0, 17, 18),
                           CompiledGsmpDistribution::MakeUniform(7.0, 11.0),
                           {MakeUpdate(0, -2)}, 0),
       CompiledGsmpCommand(MakeGuard(0, 17, 19),
                           CompiledGsmpDistribution::MakeUniform(3.0, 5.0),
                           {MakeUpdate(0, 1)}, 1),
       CompiledGsmpCommand(MakeGuard(0, 18, 19),
                           CompiledGsmpDistribution::MakeUniform(1.0, 2.0),
                           {MakeUpdate(0, -1)}, 2)});
  EXPECT_EQ(3, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // 2 random numbers for the 1st state transition:
  //
  //   GSMP trigger time 1: (11 - 7) * 0/4 + 7 = 7
  //   GSMP trigger time 2: (5 - 3) * 2/4 + 3 = 4   [winner]
  //
  // 2 random number for the 2nd state transition:
  //
  //   GSMP trigger time 1: 7 (saved)
  //   GSMP trigger time 2: 4 + (5 - 3) * 3/4 + 3 = 8.5
  //   GSMP trigger time 3: 4 + (2 - 1) * 1/4 + 1 = 5.25  [winner]
  //
  // 0 random numbers for the 3rd state transition:
  //
  //   GSMP trigger time 1: 7 (saved)    [winner]
  //   GSMP trigger time 2: 8.5 (saved)
  //
  FakeEngine engine(0, 3, {0, 2, 3, 1});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(4, next_state.time());
  EXPECT_EQ(std::vector<int>({18}), next_state.values());
  EXPECT_EQ(std::vector<double>({7, std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(5.25, next_state.time());
  EXPECT_EQ(std::vector<int>({17}), next_state.values());
  EXPECT_EQ(
      std::vector<double>({7, 8.5, std::numeric_limits<double>::infinity()}),
      next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(7, next_state.time());
  EXPECT_EQ(std::vector<int>({15}), next_state.values());
  EXPECT_EQ(std::vector<double>({std::numeric_limits<double>::infinity(), 8.5,
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({15}), next_state.values());
  EXPECT_EQ(std::vector<double>({std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
}

TEST(NextStateSamplerTest, ComplexGsmpEvents) {
  CompiledModel model(CompiledModelType::GSMP);
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.AddVariable("b", Type::INT, 0, 4711, 1);
  model.AddVariable("c", Type::INT, 0, 3, 1);
  model.set_single_markov_commands({CompiledMarkovCommand(
      MakeGuard(0, 17, 18), MakeWeight(5.0),
      {CompiledMarkovOutcome(MakeWeight(0.4), {MakeUpdate(0, -1)}),
       CompiledMarkovOutcome(MakeWeight(0.6), {MakeUpdate(0, 1)})})});
  model.set_factored_markov_commands(
      {{{CompiledMarkovCommand(
             MakeGuard(0, 17, 19), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(0, 2)})}),
         CompiledMarkovCommand(
             MakeGuard(0, 18, 19), MakeWeight(1.25),
             {CompiledMarkovOutcome(MakeWeight(0.4), {MakeUpdate(0, -1)}),
              CompiledMarkovOutcome(MakeWeight(0.6), {MakeUpdate(0, 2)})})},
        {CompiledMarkovCommand(
            MakeGuard(1, 0, 1), MakeWeight(4.0),
            {CompiledMarkovOutcome(MakeWeight(0.25), {MakeUpdate(1, 2)}),
             CompiledMarkovOutcome(MakeWeight(0.6875), {MakeUpdate(1, 3)}),
             CompiledMarkovOutcome(MakeWeight(0.0625), {MakeUpdate(1, -1)})})}},
       {{},
        {CompiledMarkovCommand(
             MakeGuard(1, 1, 1), MakeWeight(1.0),
             {CompiledMarkovOutcome(MakeWeight(1.0), {MakeUpdate(1, 1)})}),
         CompiledMarkovCommand(MakeGuard(1, 1, 3), MakeWeight(1.0),
                               {CompiledMarkovOutcome(MakeWeight(1.25 / 3.0),
                                                      {MakeUpdate(1, -2)}),
                                CompiledMarkovOutcome(MakeWeight(1.75 / 3.0),
                                                      {MakeUpdate(1, 1)})})},
        {CompiledMarkovCommand(
            MakeGuard(2, 1, 2), MakeWeight(1.0),
            {CompiledMarkovOutcome(MakeWeight(0.5), {MakeUpdate(2, 1)}),
             CompiledMarkovOutcome(MakeWeight(0.5), {MakeUpdate(2, 2)})})}}});
  model.set_single_gsmp_commands({CompiledGsmpCommand(
      MakeGuard(0, 17, 18), CompiledGsmpDistribution::MakeUniform(100.0, 200.0),
      {MakeUpdate(0, 1)}, 0)});
  model.set_factored_gsmp_commands(
      {{},
       {{CompiledGsmpCommand(MakeGuard(0, 17, 19),
                             CompiledGsmpDistribution::MakeUniform(0.0, 2.0),
                             {MakeUpdate(0, -2)}, 1),
         CompiledGsmpCommand(MakeGuard(0, 18, 19),
                             CompiledGsmpDistribution::MakeUniform(1.0, 5.0),
                             {MakeUpdate(0, -5)}, 3),
         CompiledGsmpCommand(MakeGuard(0, 19, 19),
                             CompiledGsmpDistribution::MakeUniform(2.0, 3.0),
                             {MakeUpdate(0, -1)}, 5)},
        {2, 1}}});
  EXPECT_EQ(2 + 3 * 3 + 1 + 3 * 2, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // 6 random numbers for the 1st state transition:
  //
  //   command choice 1: -log(1 - 2/8) / 5 = 0.0575
  //   command choice 2: -log(1 - 1/8) / 4 = 0.0334  [winner]
  //   command choice 3: (200 - 100) * 6/8 + 100 = 175
  //   command choice 4: (2 - 0) * 3/8 + 0 = 0.75
  //   command choice 5: (2 - 0) * 4/8 + 0 = 1
  //   1st outcome wins because 1/8 < 0.25
  //
  // 4 random numbers for the 2nd state transition:
  //
  //   command choice 1: 1 (saved)                 [winner]
  //   command choice 2: (5 - 1) * 1/8 + 1 = 1.5
  //   command choice 3: (3 - 2) * 6/8 + 2 = 2.75
  //   2nd outcome wins because 4/8 >= 1.25 / 3
  //   2st outcome wins 3/8 < 0.5
  //
  // 3 random numbers for the 3rd state transition:
  //
  //   command choice 1: -log(1 - 7/8) / 5 = 0.416      [winner]
  //   command choice 2: (200 - 100) * 0/8 + 100 = 100
  //   1st outcome wins because 3/8 < 0.4
  //
  FakeEngine engine(0, 7, {2, 1, 6, 3, 4, 1, 1, 6, 4, 3, 7, 0, 3});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(-log(0.875) / 4.0, next_state.time());
  EXPECT_EQ(std::vector<int>({19, 3, 1}), next_state.values());
  EXPECT_EQ(std::vector<double>({175, 0.75, 1,
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(1, next_state.time());
  EXPECT_EQ(std::vector<int>({17, 4, 2}), next_state.values());
  EXPECT_EQ(std::vector<double>(
                {std::numeric_limits<double>::infinity(),
                 std::numeric_limits<double>::infinity(),
                 std::numeric_limits<double>::infinity(),
                 std::numeric_limits<double>::infinity(), state.time() + 1.5,
                 std::numeric_limits<double>::infinity(), state.time() + 2.75}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(1 - log(0.125) / 5.0, next_state.time());
  EXPECT_EQ(std::vector<int>({16, 4, 2}), next_state.values());
  EXPECT_EQ(std::vector<double>({state.time() + 100,
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16, 4, 2}), next_state.values());
  EXPECT_EQ(std::vector<double>({std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
}

TEST(NextStateSamplerTest, BreaksTiesForGsmpCommands) {
  CompiledModel model(CompiledModelType::GSMP);
  model.AddVariable("a", Type::INT, 0, 42, 17);
  model.set_single_gsmp_commands(
      {CompiledGsmpCommand(MakeGuard(0, 17, 18),
                           CompiledGsmpDistribution::MakeUniform(1.0, 3.0),
                           {MakeUpdate(0, -2)}, 0),
       CompiledGsmpCommand(MakeGuard(0, 17, 18),
                           CompiledGsmpDistribution::MakeUniform(0.0, 4.0),
                           {MakeUpdate(0, 1)}, 1)});
  EXPECT_EQ(2, model.EventCount());
  CompiledExpressionEvaluator evaluator(2, 1);
  // 3 random numbers for the 1st state transition:
  //
  //   GSMP trigger time 1: (3 - 1) * 6/8 + 1 = 2.5
  //   GSMP trigger time 2: (4 - 0) * 5/8 + 0 = 2.5
  //   2nd command wins tie because 3/8 * 2 < 1
  //
  // 2 random numbers for the 2nd state transition:
  //
  //   GSMP trigger time 1: 2.5 (saved)
  //   GSMP trigger time 2: 2.5 + (4 - 0) * 0/8 + 0 = 2.5
  //   1st command wins tie because 4/8 * 2 >= 1
  //
  FakeEngine engine(0, 7, {6, 5, 3, 0, 4});
  CompiledDistributionSampler<FakeEngine> sampler(&engine);
  NextStateSampler<FakeEngine> simulator(&model, &evaluator, &sampler);
  State state(model);
  State next_state(model);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(2.5, next_state.time());
  EXPECT_EQ(std::vector<int>({18}), next_state.values());
  EXPECT_EQ(std::vector<double>({2.5, std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(2.5, next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
  EXPECT_EQ(std::vector<double>({std::numeric_limits<double>::infinity(), 2.5}),
            next_state.trigger_times());
  state.swap(next_state);
  simulator.NextState(state, &next_state);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), next_state.time());
  EXPECT_EQ(std::vector<int>({16}), next_state.values());
  EXPECT_EQ(std::vector<double>({std::numeric_limits<double>::infinity(),
                                 std::numeric_limits<double>::infinity()}),
            next_state.trigger_times());
}

}  // namespace
