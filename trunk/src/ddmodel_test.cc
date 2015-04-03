// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2015 Google Inc
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

#include "ddmodel.h"

#include "gtest/gtest.h"

namespace {

template <typename T>
CompiledExpression MakeGuard(
    const std::string& variable, T value,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager) {
  const auto result = CompileExpression(
      BinaryOperation(BinaryOperator::EQUAL, Identifier::New("b"),
                      Literal::New(value)),
      Type::BOOL, {}, identifiers_by_name, dd_manager);
  CHECK(result.errors.empty());
  return result.expr;
}

CompiledExpression MakeWeight(
    double value, const Optional<DecisionDiagramManager>& dd_manager) {
  const auto result =
      CompileExpression(Literal(value), Type::DOUBLE, {}, {}, dd_manager);
  CHECK(result.errors.empty());
  return result.expr;
}

CompiledUpdate MakeUpdate(int variable, bool value,
                          const Optional<DecisionDiagramManager>& dd_manager) {
  const auto result =
      CompileExpression(Literal(value), Type::BOOL, {}, {}, dd_manager);
  CHECK(result.errors.empty());
  return CompiledUpdate(variable, result.expr);
}

TEST(DecisionDiagramModelTest, MakeSimple) {
  Optional<DecisionDiagramManager> manager = DecisionDiagramManager(2);
  const std::vector<StateVariableInfo> variables = {{"b", 0, 1}};
  const std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"b", IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, 0)}};
  CompiledModel model(CompiledModelType::CTMC, variables, {}, {0}, {});
  model.set_single_markov_commands(
      {CompiledMarkovCommand(
           {}, MakeGuard("b", false, identifiers_by_name, manager),
           MakeWeight(2, manager),
           {CompiledMarkovOutcome(MakeWeight(1, manager),
                                  {MakeUpdate(0, true, manager)})}),
       CompiledMarkovCommand(
           {}, MakeGuard("b", true, identifiers_by_name, manager),
           MakeWeight(3, manager),
           {CompiledMarkovOutcome(MakeWeight(1, manager),
                                  {MakeUpdate(0, false, manager)})})});
  const auto dd_model = DecisionDiagramModel::Make(&manager.value(), model, 0,
                                                   identifiers_by_name);
  EXPECT_EQ(&manager.value(), &dd_model.manager());
  EXPECT_TRUE(dd_model.initial_states().ValueInState({0}, variables));
  EXPECT_FALSE(dd_model.initial_states().ValueInState({1}, variables));
  EXPECT_EQ(0, dd_model.rate_matrix().ValueInState({false, false}));
  EXPECT_EQ(2, dd_model.rate_matrix().ValueInState({false, true}));
  EXPECT_EQ(3, dd_model.rate_matrix().ValueInState({true, false}));
  EXPECT_EQ(0, dd_model.rate_matrix().ValueInState({true, true}));
  EXPECT_TRUE(dd_model.reachable_states().Value());
  EXPECT_EQ(0, dd_model.initial_state_index().value());
}

}  // namespace
