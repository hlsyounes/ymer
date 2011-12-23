// Copyright (C) 2011 Google Inc
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

#include "model.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "expression.h"
#include "process-algebra.h"

namespace {

TEST(RangeTest, ConstructsRange) {
  std::unique_ptr<const ParsedExpression> min(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_min = min.get();
  std::unique_ptr<const ParsedExpression> max(ParsedLiteral::Create(42));
  const ParsedExpression* const raw_max = max.get();
  const Range range(std::move(min), std::move(max));
  EXPECT_EQ(raw_min, &range.min());
  EXPECT_EQ(raw_max, &range.max());
}

TEST(StateRewardTest, ConstructsStateReward) {
  std::unique_ptr<const ParsedExpression> guard(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard = guard.get();
  std::unique_ptr<const ParsedExpression> reward(
      ParsedLiteral::Create(3.14159));
  const ParsedExpression* const raw_reward = reward.get();
  const StateReward state_reward(std::move(guard), std::move(reward));
  EXPECT_EQ(raw_guard, &state_reward.guard());
  EXPECT_EQ(raw_reward, &state_reward.reward());
}

TEST(TransitionRewardTest, ConstructsTransitionReward) {
  std::unique_ptr<const ParsedExpression> guard(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard = guard.get();
  std::unique_ptr<const ParsedExpression> reward(
      ParsedLiteral::Create(3.14159));
  const ParsedExpression* const raw_reward = reward.get();
  const TransitionReward transition_reward(
      "foo", std::move(guard), std::move(reward));
  ASSERT_TRUE(transition_reward.action() ? true : false);
  EXPECT_EQ("foo", transition_reward.action().get());
  EXPECT_EQ(raw_guard, &transition_reward.guard());
  EXPECT_EQ(raw_reward, &transition_reward.reward());
}

TEST(ModelTypeTest, Name) {
  EXPECT_EQ("DTMC", ModelType_Name(ModelType::DTMC));
  EXPECT_EQ("CTMC", ModelType_Name(ModelType::CTMC));
  EXPECT_EQ("MDP", ModelType_Name(ModelType::MDP));
}

Range MakeLiteralRange(int min, int max) {
  return Range(ParsedLiteral::Create(min), ParsedLiteral::Create(max));
}

TEST(ModelTest, InitializesModelType) {
  Model model;
  EXPECT_EQ(ModelType::NONE, model.type());
}

TEST(ModelTest, SetsAndGetsModelType) {
  Model model;
  EXPECT_TRUE(model.SetType(ModelType::CTMC));
  EXPECT_EQ(ModelType::CTMC, model.type());
}

TEST(ModelTest, SetsModelTypeOnce) {
  Model model;
  EXPECT_TRUE(model.SetType(ModelType::DTMC));
  EXPECT_EQ(ModelType::DTMC, model.type());
  EXPECT_FALSE(model.SetType(ModelType::CTMC));
  EXPECT_EQ(ModelType::DTMC, model.type());
}

TEST(ModelTest, AddsConstants) {
  Model model;
  std::unique_ptr<const ParsedExpression> int_init(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_int_init = int_init.get();
  std::unique_ptr<const ParsedExpression> double_init(
      ParsedLiteral::Create(0.5));
  const ParsedExpression* const raw_double_init = double_init.get();
  EXPECT_TRUE(model.AddConstant("c", Type::INT, std::move(int_init)));
  EXPECT_TRUE(model.AddConstant("k", Type::BOOL, nullptr));
  EXPECT_TRUE(model.AddConstant("K", Type::DOUBLE, std::move(double_init)));
  ASSERT_EQ(3, model.num_constants());
  EXPECT_EQ(Type::INT, model.constant_type(0));
  EXPECT_EQ(Type::BOOL, model.constant_type(1));
  EXPECT_EQ(Type::DOUBLE, model.constant_type(2));
  EXPECT_EQ(raw_int_init, model.GetConstantInit(0));
  EXPECT_EQ(nullptr, model.GetConstantInit(1));
  EXPECT_EQ(raw_double_init, model.GetConstantInit(2));
  EXPECT_EQ("c", model.GetConstantName(0));
  EXPECT_EQ("k", model.GetConstantName(1));
  EXPECT_EQ("K", model.GetConstantName(2));
}

TEST(ModelTest, AddsVariables) {
  Model model;
  std::unique_ptr<const ParsedExpression> min0(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_min0 = min0.get();
  std::unique_ptr<const ParsedExpression> max0(ParsedLiteral::Create(42));
  const ParsedExpression* const raw_max0 = max0.get();
  std::unique_ptr<const ParsedExpression> min2(ParsedLiteral::Create(0));
  const ParsedExpression* const raw_min2 = min2.get();
  std::unique_ptr<const ParsedExpression> max2(ParsedLiteral::Create(1));
  const ParsedExpression* const raw_max2 = max2.get();
  std::unique_ptr<const ParsedExpression> int_init(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_int_init = int_init.get();
  std::unique_ptr<const ParsedExpression> bool_init(
      ParsedLiteral::Create(true));
  const ParsedExpression* const raw_bool_init = bool_init.get();
  EXPECT_TRUE(model.AddIntVariable(
      "i", Range(std::move(min0), std::move(max0)), std::move(int_init)));
  EXPECT_TRUE(model.AddBoolVariable("b", std::move(bool_init)));
  EXPECT_TRUE(model.AddIntVariable(
      "j", Range(std::move(min2), std::move(max2)), nullptr));
  ASSERT_EQ(3, model.num_variables());
  EXPECT_EQ(Type::INT, model.variable_type(0));
  EXPECT_EQ(Type::BOOL, model.variable_type(1));
  EXPECT_EQ(Type::INT, model.variable_type(2));
  ASSERT_NE(nullptr, model.GetVariableRange(0));
  EXPECT_EQ(raw_min0, &model.GetVariableRange(0)->min());
  EXPECT_EQ(raw_max0, &model.GetVariableRange(0)->max());
  EXPECT_EQ(nullptr, model.GetVariableRange(1));
  ASSERT_NE(nullptr, model.GetVariableRange(2));
  EXPECT_EQ(raw_min2, &model.GetVariableRange(2)->min());
  EXPECT_EQ(raw_max2, &model.GetVariableRange(2)->max());
  EXPECT_EQ(raw_int_init, model.GetVariableInit(0));
  EXPECT_EQ(raw_bool_init, model.GetVariableInit(1));
  EXPECT_EQ(nullptr, model.GetVariableInit(2));
  EXPECT_EQ("i", model.GetVariableName(0));
  EXPECT_EQ("b", model.GetVariableName(1));
  EXPECT_EQ("j", model.GetVariableName(2));
}

TEST(ModelTest, AddsFormulas) {
  Model model;
  std::unique_ptr<const ParsedExpression> expr0(ParsedLiteral::Create(17));
  const ParsedExpression* const raw_expr0 = expr0.get();
  std::unique_ptr<const ParsedExpression> expr1(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_expr1 = expr1.get();
  std::unique_ptr<const ParsedExpression> expr2(ParsedLiteral::Create(3.14159));
  const ParsedExpression* const raw_expr2 = expr2.get();
  EXPECT_TRUE(model.AddFormula("f", std::move(expr0)));
  EXPECT_TRUE(model.AddFormula("h", std::move(expr1)));
  EXPECT_TRUE(model.AddFormula("g", std::move(expr2)));
  ASSERT_EQ(3, model.num_formulas());
  EXPECT_EQ(raw_expr0, &model.formula_expr(0));
  EXPECT_EQ(raw_expr1, &model.formula_expr(1));
  EXPECT_EQ(raw_expr2, &model.formula_expr(2));
  EXPECT_EQ("f", model.GetFormulaName(0));
  EXPECT_EQ("h", model.GetFormulaName(1));
  EXPECT_EQ("g", model.GetFormulaName(2));
}

TEST(ModelTest, AddsModules) {
  Model model;
  EXPECT_TRUE(model.StartModule("M1"));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M3"));
  model.EndModule();
  EXPECT_EQ(3, model.num_modules());
  EXPECT_EQ("M1", model.GetModuleName(0));
  EXPECT_EQ("M2", model.GetModuleName(1));
  EXPECT_EQ("M3", model.GetModuleName(2));
}

TEST(ModelTest, AddsModuleVariables) {
  Model model;
  EXPECT_TRUE(model.AddBoolVariable("b1", nullptr));
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddBoolVariable("b2", nullptr));
  EXPECT_TRUE(model.AddIntVariable("i", MakeLiteralRange(17, 42), nullptr));
  model.EndModule();
  EXPECT_TRUE(model.AddIntVariable("j", MakeLiteralRange(0, 1), nullptr));
  EXPECT_TRUE(model.StartModule("M3"));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_TRUE(model.AddBoolVariable("b3", nullptr));
  model.EndModule();
  EXPECT_TRUE(model.AddBoolVariable("b4", nullptr));
  ASSERT_EQ(6, model.num_variables());
  EXPECT_EQ(Type::BOOL, model.variable_type(0));
  EXPECT_EQ(Type::BOOL, model.variable_type(1));
  EXPECT_EQ(Type::INT, model.variable_type(2));
  EXPECT_EQ(Type::INT, model.variable_type(3));
  EXPECT_EQ(Type::BOOL, model.variable_type(4));
  EXPECT_EQ(Type::BOOL, model.variable_type(5));
  EXPECT_EQ("b1", model.GetVariableName(0));
  EXPECT_EQ("b2", model.GetVariableName(1));
  EXPECT_EQ("i", model.GetVariableName(2));
  EXPECT_EQ("j", model.GetVariableName(3));
  EXPECT_EQ("b3", model.GetVariableName(4));
  EXPECT_EQ("b4", model.GetVariableName(5));
  ASSERT_EQ(3, model.num_modules());
  EXPECT_EQ(std::set<int>({ 1, 2 }), model.module_variables(0));
  EXPECT_EQ(std::set<int>({}), model.module_variables(1));
  EXPECT_EQ(std::set<int>({ 4 }), model.module_variables(2));
}

TEST(ModelTest, AddsModuleCommands) {
  Model model;
  std::unique_ptr<const ParsedExpression> guard1(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard1 = guard1.get();
  std::unique_ptr<const ParsedExpression> guard2(ParsedLiteral::Create(false));
  const ParsedExpression* const raw_guard2 = guard2.get();
  std::unique_ptr<const ParsedExpression> guard3(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard3 = guard3.get();
  EXPECT_TRUE(model.StartModule("M1"));
  model.AddCommand(ParsedCommand("foo", std::move(guard1), {}));
  model.AddCommand(ParsedCommand(
      Optional<std::string>(), std::move(guard2), {}));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M3"));
  model.AddCommand(ParsedCommand("bar", std::move(guard3), {}));
  model.EndModule();
  ASSERT_EQ(3, model.num_modules());
  ASSERT_EQ(2, model.module_commands(0).size());
  ASSERT_TRUE(model.module_commands(0)[0].action() ? true : false);
  EXPECT_EQ("foo", model.module_commands(0)[0].action().get());
  EXPECT_EQ(raw_guard1, &model.module_commands(0)[0].guard());
  EXPECT_FALSE(model.module_commands(0)[1].action() ? true : false);
  EXPECT_EQ(raw_guard2, &model.module_commands(0)[1].guard());
  EXPECT_EQ(0, model.module_commands(1).size());
  ASSERT_EQ(1, model.module_commands(2).size());
  ASSERT_TRUE(model.module_commands(2)[0].action() ? true : false);
  EXPECT_EQ("bar", model.module_commands(2)[0].action().get());
  EXPECT_EQ(raw_guard3, &model.module_commands(2)[0].guard());
}

TEST(ModelTest, AddsFromModule) {
  Model model;
  EXPECT_TRUE(model.AddConstant("c", Type::INT, ParsedIdentifier::Create("f")));
  EXPECT_TRUE(model.AddConstant("k", Type::INT, ParsedIdentifier::Create("g")));
  EXPECT_TRUE(model.AddFormula("f", ParsedLiteral::Create(17)));
  EXPECT_TRUE(model.AddFormula("g", ParsedLiteral::Create(42)));
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddIntVariable("i",
                                   Range(ParsedLiteral::Create(0),
                                         ParsedIdentifier::Create("c")),
                                   ParsedIdentifier::Create("f")));
  EXPECT_TRUE(model.AddBoolVariable("b1", nullptr));
  std::vector<ParsedUpdate> updates;
  updates.emplace_back("i",
                       ParsedBinaryOperation::Create(
                           BinaryOperator::PLUS,
                           ParsedIdentifier::Create("i"),
                           ParsedLiteral::Create(1)));
  std::vector<ParsedOutcome> outcomes;
  outcomes.emplace_back(
      ParsedBinaryOperation::Create(BinaryOperator::DIVIDE,
                                    ParsedLiteral::Create(1),
                                    ParsedIdentifier::Create("i")),
      std::move(updates));
  EXPECT_TRUE(model.AddCommand(ParsedCommand(
      "a",
      ParsedBinaryOperation::Create(BinaryOperator::LESS,
                                    ParsedIdentifier::Create("i"),
                                    ParsedIdentifier::Create("c")),
      std::move(outcomes))));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_TRUE(model.AddFromModule(
      "M1",
      { { "c", "k" }, { "f", "g" }, { "i", "j" }, { "b1", "b2" },
        { "a", "b" } },
      nullptr));
  model.EndModule();
  ASSERT_EQ(4, model.num_variables());
  EXPECT_EQ(Type::INT, model.variable_type(2));
  EXPECT_EQ(Type::BOOL, model.variable_type(3));
  const Range* const range2 = model.GetVariableRange(2);
  ASSERT_NE(nullptr, range2);
  const ParsedLiteral* const min2 =
      dynamic_cast<const ParsedLiteral*>(&range2->min());
  ASSERT_NE(nullptr, min2);
  EXPECT_EQ(0, min2->value());
  const ParsedIdentifier* const max2 =
      dynamic_cast<const ParsedIdentifier*>(&range2->max());
  ASSERT_NE(nullptr, max2);
  EXPECT_EQ("k", max2->id());
  const ParsedIdentifier* const init2 =
      dynamic_cast<const ParsedIdentifier*>(model.GetVariableInit(2));
  ASSERT_NE(nullptr, init2);
  EXPECT_EQ("g", init2->id());
  EXPECT_EQ(nullptr, model.GetVariableInit(3));
  EXPECT_EQ("j", model.GetVariableName(2));
  EXPECT_EQ("b2", model.GetVariableName(3));
  ASSERT_EQ(2, model.num_modules());
  EXPECT_EQ(std::set<int>({ 2, 3 }), model.module_variables(1));
  ASSERT_EQ(1, model.module_commands(1).size());
  ASSERT_TRUE(model.module_commands(1)[0].action() ? true : false);
  EXPECT_EQ("b", model.module_commands(1)[0].action().get());
  const ParsedBinaryOperation* const guard =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(1)[0].guard());
  ASSERT_NE(nullptr, guard);
  EXPECT_EQ(BinaryOperator::LESS, guard->op());
  const ParsedIdentifier* const guard_operand1 =
      dynamic_cast<const ParsedIdentifier*>(&guard->operand1());
  ASSERT_NE(nullptr, guard_operand1);
  EXPECT_EQ("j", guard_operand1->id());
  const ParsedIdentifier* const guard_operand2 =
      dynamic_cast<const ParsedIdentifier*>(&guard->operand2());
  ASSERT_NE(nullptr, guard_operand2);
  EXPECT_EQ("k", guard_operand2->id());
  ASSERT_EQ(1, model.module_commands(1)[0].outcomes().size());
  const ParsedBinaryOperation* const probability =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(1)[0].outcomes()[0].probability());
  ASSERT_NE(nullptr, probability);
  EXPECT_EQ(BinaryOperator::DIVIDE, probability->op());
  const ParsedLiteral* const probability_operand1 =
      dynamic_cast<const ParsedLiteral*>(&probability->operand1());
  ASSERT_NE(nullptr, probability_operand1);
  EXPECT_EQ(1, probability_operand1->value());
  const ParsedIdentifier* const probability_operand2 =
      dynamic_cast<const ParsedIdentifier*>(&probability->operand2());
  ASSERT_NE(nullptr, probability_operand2);
  EXPECT_EQ("j", probability_operand2->id());
  ASSERT_EQ(1, model.module_commands(1)[0].outcomes()[0].updates().size());
  EXPECT_EQ("j",
            model.module_commands(1)[0].outcomes()[0].updates()[0].variable());
  const ParsedBinaryOperation* const update_expr =
      dynamic_cast<const ParsedBinaryOperation*>(
          &model.module_commands(1)[0].outcomes()[0].updates()[0].expr());
  ASSERT_NE(nullptr, update_expr);
  EXPECT_EQ(BinaryOperator::PLUS, update_expr->op());
  const ParsedIdentifier* const update_expr_operand1 =
      dynamic_cast<const ParsedIdentifier*>(&update_expr->operand1());
  ASSERT_NE(nullptr, update_expr_operand1);
  EXPECT_EQ("j", update_expr_operand1->id());
  const ParsedLiteral* const update_expr_operand2 =
      dynamic_cast<const ParsedLiteral*>(&update_expr->operand2());
  ASSERT_NE(nullptr, update_expr_operand2);
  EXPECT_EQ(1, update_expr_operand2->value());
}

TEST(ModelTest, MissingSourceModule) {
  Model model;
  std::string error;
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_FALSE(model.AddFromModule("M1", {}, &error));
  EXPECT_EQ("undefined module 'M1'", error);
}

TEST(ModelTest, IncompleteSubstitutions) {
  Model model;
  std::string error;
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddBoolVariable("b1", nullptr));
  EXPECT_TRUE(model.AddBoolVariable("b2", nullptr));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_FALSE(model.AddFromModule("M1", { { "b1", "b3" } }, &error));
  EXPECT_EQ("missing substitution for module 'M1' variable 'b2'", error);
}

TEST(ModelTest, DuplicateVariablesFromSubstitution) {
  Model model;
  std::string error;
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddBoolVariable("b1", nullptr));
  EXPECT_TRUE(model.AddBoolVariable("b2", nullptr));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_FALSE(model.AddFromModule(
      "M1", { { "b1", "b3" }, { "b2", "b1" } }, &error));
  EXPECT_EQ("duplicate identifier", error);
}

TEST(ModelTest, RejectsDuplicateIdentifiers) {
  Model model;

  EXPECT_TRUE(model.AddConstant("c", Type::INT, nullptr));
  EXPECT_TRUE(model.AddFormula("f", ParsedLiteral::Create(17)));
  EXPECT_TRUE(model.AddIntVariable("i", MakeLiteralRange(17, 42), nullptr));
  EXPECT_TRUE(model.AddBoolVariable("b", nullptr));
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddCommand(ParsedCommand(
      "a", ParsedLiteral::Create(true), {})));
  model.EndModule();

  EXPECT_FALSE(model.AddConstant("c", Type::INT, nullptr));
  EXPECT_FALSE(model.AddFormula("c", ParsedLiteral::Create(17)));
  EXPECT_FALSE(model.AddIntVariable("c", MakeLiteralRange(17, 42), nullptr));
  EXPECT_FALSE(model.AddBoolVariable("c", nullptr));
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_FALSE(model.AddCommand(ParsedCommand(
      "c", ParsedLiteral::Create(true), {})));
  model.EndModule();

  EXPECT_FALSE(model.AddConstant("i", Type::INT, nullptr));
  EXPECT_FALSE(model.AddFormula("i", ParsedLiteral::Create(17)));
  EXPECT_FALSE(model.AddIntVariable("i", MakeLiteralRange(17, 42), nullptr));
  EXPECT_FALSE(model.AddBoolVariable("i", nullptr));
  EXPECT_TRUE(model.StartModule("M3"));
  EXPECT_FALSE(model.AddCommand(ParsedCommand(
      "i", ParsedLiteral::Create(true), {})));
  model.EndModule();

  EXPECT_FALSE(model.AddConstant("b", Type::INT, nullptr));
  EXPECT_FALSE(model.AddFormula("b", ParsedLiteral::Create(17)));
  EXPECT_FALSE(model.AddIntVariable("b", MakeLiteralRange(17, 42), nullptr));
  EXPECT_FALSE(model.AddBoolVariable("b", nullptr));
  EXPECT_TRUE(model.StartModule("M4"));
  EXPECT_FALSE(model.AddCommand(ParsedCommand(
      "b", ParsedLiteral::Create(true), {})));
  model.EndModule();

  EXPECT_FALSE(model.AddConstant("f", Type::INT, nullptr));
  EXPECT_FALSE(model.AddFormula("f", ParsedLiteral::Create(17)));
  EXPECT_FALSE(model.AddIntVariable("f", MakeLiteralRange(17, 42), nullptr));
  EXPECT_FALSE(model.AddBoolVariable("f", nullptr));
  EXPECT_TRUE(model.StartModule("M5"));
  EXPECT_FALSE(model.AddCommand(ParsedCommand(
      "f", ParsedLiteral::Create(true), {})));
  model.EndModule();

  EXPECT_FALSE(model.AddConstant("a", Type::INT, nullptr));
  EXPECT_FALSE(model.AddFormula("a", ParsedLiteral::Create(17)));
  EXPECT_FALSE(model.AddIntVariable("a", MakeLiteralRange(17, 42), nullptr));
  EXPECT_FALSE(model.AddBoolVariable("a", nullptr));
  EXPECT_TRUE(model.StartModule("M6"));
  // NOTE: It is fine to use the same action label multiple times.
  EXPECT_TRUE(model.AddCommand(ParsedCommand(
      "a", ParsedLiteral::Create(true), {})));
  model.EndModule();
}

TEST(ModelTest, AllowsSameNameForIdentifierAndModule) {
  Model model;
  EXPECT_TRUE(model.AddConstant("x", Type::INT, nullptr));
  EXPECT_TRUE(model.StartModule("x"));
}

TEST(ModelTest, RejectsDuplicateModules) {
  Model model;
  EXPECT_TRUE(model.StartModule("m"));
  model.EndModule();
  EXPECT_FALSE(model.StartModule("m"));
}

TEST(ModelTest, AddsLabel) {
  Model model;
  std::unique_ptr<const ParsedExpression> expr(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_expr = expr.get();
  EXPECT_EQ(nullptr, model.GetLabelExpr("foo"));
  EXPECT_TRUE(model.AddLabel("foo", std::move(expr)));
  EXPECT_EQ(raw_expr, model.GetLabelExpr("foo"));
}

TEST(ModelTest, RelectsDuplicateLabel) {
  Model model;
  EXPECT_TRUE(model.AddLabel("foo", ParsedLiteral::Create(true)));
  EXPECT_FALSE(model.AddLabel("foo", ParsedLiteral::Create(true)));
}

TEST(ModelTest, InitializesInit) {
  Model model;
  EXPECT_EQ(model.init(), nullptr);
}

TEST(ModelTest, SetsAndGetsInit) {
  Model model;
  std::unique_ptr<const ParsedExpression> init(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_init = init.get();
  EXPECT_TRUE(model.SetInit(std::move(init)));
  EXPECT_EQ(raw_init, model.init());
}

TEST(ModelTest, SetsInitOnce) {
  Model model;
  std::unique_ptr<const ParsedExpression> init(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_init = init.get();
  EXPECT_TRUE(model.SetInit(std::move(init)));
  EXPECT_EQ(raw_init, model.init());
  EXPECT_FALSE(model.SetInit(ParsedLiteral::Create(false)));
  EXPECT_EQ(raw_init, model.init());
}

TEST(ModelTest, AddsRewards) {
  Model model;
  std::unique_ptr<const ParsedExpression> guard1(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard1 = guard1.get();
  std::unique_ptr<const ParsedExpression> reward1(ParsedLiteral::Create(1.0));
  const ParsedExpression* const raw_reward1 = reward1.get();
  std::unique_ptr<const ParsedExpression> guard2(ParsedLiteral::Create(false));
  const ParsedExpression* const raw_guard2 = guard2.get();
  std::unique_ptr<const ParsedExpression> reward2(ParsedLiteral::Create(2.0));
  const ParsedExpression* const raw_reward2 = reward2.get();
  std::unique_ptr<const ParsedExpression> guard3(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard3 = guard3.get();
  std::unique_ptr<const ParsedExpression> reward3(ParsedLiteral::Create(1.0));
  const ParsedExpression* const raw_reward3 = reward3.get();
  std::unique_ptr<const ParsedExpression> guard4(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard4 = guard4.get();
  std::unique_ptr<const ParsedExpression> reward4(ParsedLiteral::Create(1.0));
  const ParsedExpression* const raw_reward4 = reward4.get();
  std::unique_ptr<const ParsedExpression> guard5(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard5 = guard5.get();
  std::unique_ptr<const ParsedExpression> reward5(ParsedLiteral::Create(1.0));
  const ParsedExpression* const raw_reward5 = reward5.get();
  std::unique_ptr<const ParsedExpression> guard6(ParsedLiteral::Create(true));
  const ParsedExpression* const raw_guard6 = guard6.get();
  std::unique_ptr<const ParsedExpression> reward6(ParsedLiteral::Create(1.0));
  const ParsedExpression* const raw_reward6 = reward6.get();
  EXPECT_TRUE(model.StartLabeledRewards("r1"));
  model.AddStateReward(StateReward(std::move(guard1), std::move(reward1)));
  model.AddStateReward(StateReward(std::move(guard2), std::move(reward2)));
  model.EndRewards();
  model.StartUnlabeledRewards();
  model.AddTransitionReward(TransitionReward(
      "a3", std::move(guard3), std::move(reward3)));
  model.AddTransitionReward(TransitionReward(
      Optional<std::string>(), std::move(guard4), std::move(reward4)));
  model.EndRewards();
  EXPECT_TRUE(model.StartLabeledRewards("r2"));
  model.AddStateReward(StateReward(std::move(guard5), std::move(reward5)));
  model.AddTransitionReward(TransitionReward(
      "a6", std::move(guard6), std::move(reward6)));
  model.EndRewards();
  ASSERT_EQ(3, model.num_rewards());
  ASSERT_EQ(2, model.state_rewards(0).size());
  EXPECT_EQ(raw_guard1, &model.state_rewards(0)[0].guard());
  EXPECT_EQ(raw_reward1, &model.state_rewards(0)[0].reward());
  EXPECT_EQ(raw_guard2, &model.state_rewards(0)[1].guard());
  EXPECT_EQ(raw_reward2, &model.state_rewards(0)[1].reward());
  EXPECT_EQ(0, model.transition_rewards(0).size());
  EXPECT_EQ(0, model.state_rewards(1).size());
  ASSERT_EQ(2, model.transition_rewards(1).size());
  ASSERT_TRUE(model.transition_rewards(1)[0].action() ? true : false);
  EXPECT_EQ("a3", model.transition_rewards(1)[0].action().get());
  EXPECT_EQ(raw_guard3, &model.transition_rewards(1)[0].guard());
  EXPECT_EQ(raw_reward3, &model.transition_rewards(1)[0].reward());
  EXPECT_FALSE(model.transition_rewards(1)[1].action() ? true : false);
  EXPECT_EQ(raw_guard4, &model.transition_rewards(1)[1].guard());
  EXPECT_EQ(raw_reward4, &model.transition_rewards(1)[1].reward());
  ASSERT_EQ(1, model.state_rewards(2).size());
  EXPECT_EQ(raw_guard5, &model.state_rewards(2)[0].guard());
  EXPECT_EQ(raw_reward5, &model.state_rewards(2)[0].reward());
  ASSERT_EQ(1, model.transition_rewards(2).size());
  ASSERT_TRUE(model.transition_rewards(2)[0].action() ? true : false);
  EXPECT_EQ("a6", model.transition_rewards(2)[0].action().get());
  EXPECT_EQ(raw_guard6, &model.transition_rewards(2)[0].guard());
  EXPECT_EQ(raw_reward6, &model.transition_rewards(2)[0].reward());
  int index = -1;
  EXPECT_TRUE(model.GetLabeledRewardsIndex("r1", &index));
  EXPECT_EQ(0, index);
  EXPECT_TRUE(model.GetLabeledRewardsIndex("r2", &index));
  EXPECT_EQ(2, index);
  EXPECT_FALSE(model.GetLabeledRewardsIndex("foo", &index));
}

TEST(ModelTest, RejectsDuplicateLabeledRewards) {
  Model model;
  EXPECT_TRUE(model.StartLabeledRewards("r"));
  model.EndRewards();
  EXPECT_FALSE(model.StartLabeledRewards("r"));
}

TEST(ModelTest, SetsAndGetsSystem) {
  Model model;
  std::unique_ptr<const ProcessAlgebra> system(ModuleIdentifier::Create("m"));
  const ProcessAlgebra* const raw_system = system.get();
  EXPECT_TRUE(model.SetSystem(std::move(system)));
  EXPECT_EQ(raw_system, model.system());
}

TEST(ModelTest, SetsSystemOnce) {
  Model model;
  std::unique_ptr<const ProcessAlgebra> system(ModuleIdentifier::Create("m"));
  const ProcessAlgebra* const raw_system = system.get();
  EXPECT_TRUE(model.SetSystem(std::move(system)));
  EXPECT_EQ(raw_system, model.system());
  EXPECT_FALSE(model.SetSystem(ModuleIdentifier::Create("foo")));
  EXPECT_EQ(raw_system, model.system());
}

}  // namespace
