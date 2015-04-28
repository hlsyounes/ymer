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

#include "model.h"

#include "strutil.h"

#include "gtest/gtest.h"

namespace {

std::vector<Outcome> NewSimpleOutcomes() {
  std::vector<Outcome> outcomes;
  outcomes.emplace_back(Memoryless::New(Literal::New(1)),
                        std::vector<Update>());
  return std::move(outcomes);
}

TEST(UpdateTest, Output) {
  EXPECT_EQ("(a' = 17)", StrCat(Update("a", Literal::New(17))));
}

TEST(OutcomeTest, Output) {
  EXPECT_EQ("1 : true",
            StrCat(Outcome(Memoryless::New(Literal::New(1)), {})));
  std::vector<Update> updates1;
  updates1.emplace_back("a", Literal::New(17));
  updates1.emplace_back(
      "b", UnaryOperation::New(UnaryOperator::NOT, Identifier::New("b")));
  EXPECT_EQ("0.75 : (a' = 17) & (b' = !b)",
            StrCat(Outcome(Memoryless::New(Literal::New(0.75)),
                           std::move(updates1))));
  std::vector<Update> updates2;
  updates2.emplace_back("a", Literal::New(17));
  EXPECT_EQ("W(2.5, 0.5) : (a' = 17)",
            StrCat(Outcome(Weibull::New(Literal::New(2.5), Literal::New(0.5)),
                           std::move(updates2))));
}

TEST(CommandTest, Output) {
  std::vector<Outcome> outcomes1;
  std::vector<Update> updates1;
  updates1.emplace_back("a", Literal::New(42));
  outcomes1.emplace_back(Weibull::New(Literal::New(2.5), Literal::New(0.5)),
                         std::move(updates1));
  EXPECT_EQ("[act] b -> W(2.5, 0.5) : (a' = 42)",
            StrCat(Command("act", Identifier::New("b"), std::move(outcomes1))));
  std::vector<Outcome> outcomes2;
  std::vector<Update> updates2;
  updates2.emplace_back("a", Literal::New(17));
  updates2.emplace_back(
      "b", UnaryOperation::New(UnaryOperator::NOT, Identifier::New("b")));
  outcomes2.emplace_back(Memoryless::New(Literal::New(0.75)),
                         std::move(updates2));
  std::vector<Update> updates3;
  updates3.emplace_back("a", Literal::New(17));
  outcomes2.emplace_back(Memoryless::New(Literal::New(0.25)),
                         std::move(updates3));
  EXPECT_EQ("[] a = 42 -> 0.75 : (a' = 17) & (b' = !b) + 0.25 : (a' = 17)",
            StrCat(Command("", BinaryOperation::New(BinaryOperator::EQUAL,
                                                    Identifier::New("a"),
                                                    Literal::New(42)),
                           std::move(outcomes2))));
}

TEST(ModelTypeTest, Output) {
  EXPECT_EQ("mdp", StrCat(ModelType::DEFAULT));
  EXPECT_EQ("mdp", StrCat(ModelType::MDP));
  EXPECT_EQ("dtmc", StrCat(ModelType::DTMC));
  EXPECT_EQ("ctmc", StrCat(ModelType::CTMC));
  EXPECT_EQ("gsmp", StrCat(ModelType::GSMP));
}

TEST(ResolveConstantsTest, Literal) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("a", Type::INT, Literal::New(17));
  constants.emplace_back("b", Type::BOOL, Literal::New(true));
  constants.emplace_back("c", Type::DOUBLE, Literal::New(0.5));
  std::map<std::string, TypedValue> expected_constant_values = {
      {"a", 17}, {"b", true}, {"c", 0.5}};
  std::map<std::string, TypedValue> constant_values;
  std::vector<std::string> errors;
  ResolveConstants(constants, {}, &constant_values, &errors);
  EXPECT_EQ(expected_constant_values, constant_values);
  EXPECT_TRUE(errors.empty());
}

TEST(ResolveConstantsTest, LiteralTypeMismatch) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("a", Type::INT, Literal::New(0.5));
  constants.emplace_back("b", Type::BOOL, Literal::New(17));
  constants.emplace_back("c", Type::DOUBLE, Literal::New(true));
  std::vector<std::string> expected_errors = {
      "type mismatch for constant a; expecting value of type int; found double",
      "type mismatch for constant b; expecting value of type bool; found int",
      "type mismatch for constant c; expecting value of type double; found "
      "bool"};
  std::map<std::string, TypedValue> constant_values;
  std::vector<std::string> errors;
  ResolveConstants(constants, {}, &constant_values, &errors);
  EXPECT_EQ(expected_errors, errors);
}

TEST(ResolveConstantsTest, Overrides) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("a", Type::INT, nullptr);
  constants.emplace_back("b", Type::BOOL, Literal::New(false));
  constants.emplace_back("c", Type::DOUBLE, Literal::New(2.5));
  std::map<std::string, TypedValue> expected_constant_values = {
      {"a", 17}, {"b", true}, {"c", 0.5}};
  std::map<std::string, TypedValue> constant_values = {
      {"a", 17}, {"b", true}, {"c", 0.5}};
  std::vector<std::string> errors;
  ResolveConstants(constants, {}, &constant_values, &errors);
  EXPECT_EQ(expected_constant_values, constant_values);
  EXPECT_TRUE(errors.empty());
}

TEST(ResolveConstantsTest, OverridesTypeMismatch) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("a", Type::INT, nullptr);
  constants.emplace_back("b", Type::BOOL, Literal::New(false));
  constants.emplace_back("c", Type::DOUBLE, Literal::New(2.5));
  std::vector<std::string> expected_errors = {
      "type mismatch for constant a; expecting value of type int; found double",
      "type mismatch for constant b; expecting value of type bool; found int",
      "type mismatch for constant c; expecting value of type double; found "
      "bool"};
  std::map<std::string, TypedValue> constant_values = {
      {"a", 0.5}, {"b", 17}, {"c", true}};
  std::vector<std::string> errors;
  ResolveConstants(constants, {}, &constant_values, &errors);
  EXPECT_EQ(expected_errors, errors);
}

TEST(ResolveConstantsTest, Identifier) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("c", Type::DOUBLE, nullptr);
  constants.emplace_back("C", Type::DOUBLE, Identifier::New("a"));
  constants.emplace_back("a", Type::INT, Identifier::New("f"));
  constants.emplace_back("A", Type::INT, nullptr);
  constants.emplace_back("b", Type::BOOL, Identifier::New("B"));
  constants.emplace_back("B", Type::BOOL, nullptr);
  const Identifier formula("A");
  std::map<std::string, TypedValue> expected_constant_values = {
      {"a", 17}, {"A", 17}, {"b", true}, {"B", true}, {"c", 0.5}, {"C", 17}};
  std::map<std::string, TypedValue> constant_values = {
      {"A", 17}, {"B", true}, {"c", 0.5}};
  std::vector<std::string> errors;
  ResolveConstants(constants, {{"f", &formula}}, &constant_values, &errors);
  EXPECT_EQ(expected_constant_values, constant_values);
  EXPECT_TRUE(errors.empty()) << errors[0];
}

TEST(ResolveConstantsTest, IdentifierTypeMismatch) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("a", Type::INT, nullptr);
  constants.emplace_back("b", Type::BOOL, nullptr);
  constants.emplace_back("c", Type::DOUBLE, nullptr);
  constants.emplace_back("A", Type::INT, Identifier::New("c"));
  constants.emplace_back("B", Type::BOOL, Identifier::New("a"));
  constants.emplace_back("C", Type::DOUBLE, Identifier::New("b"));
  std::vector<std::string> expected_errors = {
      "type mismatch for constant A; expecting value of type int; found double",
      "type mismatch for constant B; expecting value of type bool; found int",
      "type mismatch for constant C; expecting value of type double; found "
      "bool"};
  std::map<std::string, TypedValue> constant_values = {
      {"a", 17}, {"b", true}, {"c", 0.5}};
  std::vector<std::string> errors;
  ResolveConstants(constants, {}, &constant_values, &errors);
  EXPECT_EQ(expected_errors, errors);
}

TEST(ResolveConstantsTest, IdentifierCyclic) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("a", Type::INT, Identifier::New("A"));
  constants.emplace_back("A", Type::INT, Identifier::New("a"));
  std::vector<std::string> expected_errors = {
      "cyclic evaluation for constant a"};
  std::map<std::string, TypedValue> constant_values;
  std::vector<std::string> errors;
  ResolveConstants(constants, {}, &constant_values, &errors);
  EXPECT_EQ(expected_errors, errors);
}

TEST(ResolveConstantsTest, IdentifierUninitialized) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("a", Type::INT, nullptr);
  std::vector<std::string> expected_errors = {"uninitialized constant a"};
  std::map<std::string, TypedValue> constant_values;
  std::vector<std::string> errors;
  ResolveConstants(constants, {}, &constant_values, &errors);
  EXPECT_EQ(expected_errors, errors);
}

// TODO(hlsyounes): test function call.

// TODO(hlsyounes): test unary operation.

// TODO(hlsyounes): test binary operation.

// TODO(hlsyounes): test conditional.

TEST(ParsedStateRewardTest, Output) {
  EXPECT_EQ("true : 1",
            StrCat(ParsedStateReward(Literal::New(true), Literal::New(1))));
}

TEST(ParsedTransitionRewardTest, Output) {
  EXPECT_EQ("[] true : 1", StrCat(ParsedTransitionReward("", Literal::New(true),
                                                         Literal::New(1))));
  EXPECT_EQ("[a] x : 2 * y",
            StrCat(ParsedTransitionReward(
                "a", Identifier::New("x"),
                BinaryOperation::New(BinaryOperator::MULTIPLY, Literal::New(2),
                                     Identifier::New("y")))));
}

TEST(ParsedRewardsStructureTest, Output) {
  ParsedRewardsStructure r1("");
  r1.add_state_reward(ParsedStateReward(Literal::New(true), Literal::New(1)));
  r1.add_transition_reward(ParsedTransitionReward(
      "a", Identifier::New("x"),
      BinaryOperation::New(BinaryOperator::MULTIPLY, Literal::New(2),
                           Identifier::New("y"))));
  EXPECT_EQ(
      "rewards\n"
      "  true : 1;\n"
      "  [a] x : 2 * y;\n"
      "endrewards",
      StrCat(r1));
  ParsedRewardsStructure r2("\"foo\"");
  EXPECT_EQ(
      "rewards \"foo\"\n"
      "endrewards",
      StrCat(r2));
}

TEST(ModelTest, InitializesModelType) {
  Model model;
  EXPECT_EQ(ModelType::DEFAULT, model.type());
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
  std::vector<std::string> errors;
  EXPECT_TRUE(model.AddConstant("c", Type::INT, Literal::New(17), &errors));
  EXPECT_TRUE(model.AddConstant("k", Type::BOOL, nullptr, &errors));
  EXPECT_TRUE(model.AddConstant("K", Type::DOUBLE, Literal::New(0.5), &errors));
  ASSERT_EQ(3, model.constants().size());
  EXPECT_EQ("c", model.constants()[0].name());
  EXPECT_EQ(Type::INT, model.constants()[0].type());
  EXPECT_EQ("17", StrCat(*model.constants()[0].init()));
  EXPECT_EQ("k", model.constants()[1].name());
  EXPECT_EQ(Type::BOOL, model.constants()[1].type());
  EXPECT_EQ(nullptr, model.constants()[1].init());
  EXPECT_TRUE(errors.empty());
}

TEST(ModelTest, AddsVariables) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.AddIntVariable("i", Literal::New(17), Literal::New(42),
                                   Literal::New(18), &errors));
  EXPECT_TRUE(model.AddBoolVariable("b", Literal::New(true), &errors));
  EXPECT_TRUE(model.AddIntVariable("j", Literal::New(1), Literal::New(2),
                                   nullptr, &errors));
  ASSERT_EQ(3, model.variables().size());
  EXPECT_EQ("i", model.variables()[0].name());
  EXPECT_EQ(Type::INT, model.variables()[0].type());
  EXPECT_EQ("17", StrCat(model.variables()[0].min()));
  EXPECT_EQ("42", StrCat(model.variables()[0].max()));
  EXPECT_TRUE(model.variables()[0].has_explicit_init());
  EXPECT_EQ("18", StrCat(model.variables()[0].init()));
  EXPECT_EQ("b", model.variables()[1].name());
  EXPECT_EQ(Type::BOOL, model.variables()[1].type());
  EXPECT_EQ("false", StrCat(model.variables()[1].min()));
  EXPECT_EQ("true", StrCat(model.variables()[1].max()));
  EXPECT_TRUE(model.variables()[1].has_explicit_init());
  EXPECT_EQ("true", StrCat(model.variables()[1].init()));
  EXPECT_EQ("j", model.variables()[2].name());
  EXPECT_EQ(Type::INT, model.variables()[2].type());
  EXPECT_EQ("1", StrCat(model.variables()[2].min()));
  EXPECT_EQ("2", StrCat(model.variables()[2].max()));
  EXPECT_FALSE(model.variables()[2].has_explicit_init());
  EXPECT_EQ("1", StrCat(model.variables()[2].init()));
  EXPECT_TRUE(errors.empty());
}

TEST(ModelTest, AddsFormulas) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.AddFormula("f", Literal::New(17), &errors));
  EXPECT_TRUE(model.AddFormula("g", Identifier::New("x"), &errors));
  ASSERT_EQ(2, model.formulas().size());
  EXPECT_EQ("f", model.formulas()[0].name());
  EXPECT_EQ("17", StrCat(model.formulas()[0].expr()));
  EXPECT_EQ("g", model.formulas()[1].name());
  EXPECT_EQ("x", StrCat(model.formulas()[1].expr()));
}

TEST(ModelTest, AddsLabels) {
  Model model;
  EXPECT_TRUE(model.AddLabel("a", Literal::New(true)));
  EXPECT_TRUE(model.AddLabel("b", Identifier::New("x")));
  EXPECT_TRUE(model.AddLabel("c", Literal::New(false)));
  EXPECT_FALSE(model.AddLabel("b", Identifier::New("y")));
}

TEST(ModelTest, AddsModules) {
  Model model;
  EXPECT_TRUE(model.StartModule("M1"));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M3"));
  model.EndModule();
  ASSERT_EQ(3, model.modules().size());
  EXPECT_EQ("M1", model.modules()[0].name());
  EXPECT_TRUE(model.modules()[0].variables().empty());
  EXPECT_TRUE(model.modules()[0].commands().empty());
  EXPECT_EQ("M2", model.modules()[1].name());
  EXPECT_TRUE(model.modules()[1].variables().empty());
  EXPECT_TRUE(model.modules()[1].commands().empty());
  EXPECT_EQ("M3", model.modules()[2].name());
  EXPECT_TRUE(model.modules()[2].variables().empty());
  EXPECT_TRUE(model.modules()[2].commands().empty());
}

TEST(ModelTest, AddsModuleVariables) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.AddBoolVariable("b1", nullptr, &errors));
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddBoolVariable("b2", nullptr, &errors));
  EXPECT_TRUE(model.AddIntVariable("i", Literal::New(17), Literal::New(42),
                                   nullptr, &errors));
  model.EndModule();
  EXPECT_TRUE(model.AddIntVariable("j", Literal::New(0), Literal::New(1),
                                   nullptr, &errors));
  EXPECT_TRUE(model.StartModule("M3"));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_TRUE(model.AddBoolVariable("b3", nullptr, &errors));
  model.EndModule();
  EXPECT_TRUE(model.AddBoolVariable("b4", nullptr, &errors));
  ASSERT_EQ(6, model.variables().size());
  EXPECT_EQ("b1", model.variables()[0].name());
  EXPECT_EQ(Type::BOOL, model.variables()[0].type());
  EXPECT_EQ("b2", model.variables()[1].name());
  EXPECT_EQ(Type::BOOL, model.variables()[1].type());
  EXPECT_EQ("i", model.variables()[2].name());
  EXPECT_EQ(Type::INT, model.variables()[2].type());
  EXPECT_EQ("j", model.variables()[3].name());
  EXPECT_EQ(Type::INT, model.variables()[3].type());
  EXPECT_EQ("b3", model.variables()[4].name());
  EXPECT_EQ(Type::BOOL, model.variables()[4].type());
  EXPECT_EQ("b4", model.variables()[5].name());
  EXPECT_EQ(Type::BOOL, model.variables()[5].type());
  ASSERT_EQ(3, model.modules().size());
  EXPECT_EQ(std::set<int>({1, 2}), model.modules()[0].variables());
  EXPECT_EQ(std::set<int>({}), model.modules()[1].variables());
  EXPECT_EQ(std::set<int>({4}), model.modules()[2].variables());
  EXPECT_TRUE(errors.empty());
}
TEST(ModelTest, AddsModuleCommands) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.StartModule("M1"));
  model.AddCommand(Command("foo", Literal::New(true), NewSimpleOutcomes()),
                   &errors);
  model.AddCommand(Command("", Literal::New(false), NewSimpleOutcomes()),
                   &errors);
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M3"));
  model.AddCommand(Command("bar", Literal::New(true), NewSimpleOutcomes()),
                   &errors);
  model.EndModule();
  ASSERT_EQ(3, model.modules().size());
  ASSERT_EQ(2, model.modules()[0].commands().size());
  EXPECT_EQ("[foo] true -> 1 : true", StrCat(model.modules()[0].commands()[0]));
  EXPECT_EQ("[] false -> 1 : true", StrCat(model.modules()[0].commands()[1]));
  EXPECT_EQ(0, model.modules()[1].commands().size());
  ASSERT_EQ(1, model.modules()[2].commands().size());
  EXPECT_EQ("[bar] true -> 1 : true", StrCat(model.modules()[2].commands()[0]));
  EXPECT_TRUE(errors.empty());
}

TEST(ModelTest, AddsFromModule) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.AddConstant("c", Type::INT, Identifier::New("f"), &errors));
  EXPECT_TRUE(model.AddConstant("k", Type::INT, Identifier::New("g"), &errors));
  EXPECT_TRUE(model.AddConstant("f", Type::INT, Literal::New(17), &errors));
  EXPECT_TRUE(model.AddConstant("g", Type::INT, Literal::New(42), &errors));
  EXPECT_TRUE(model.AddFormula(
      "guard", BinaryOperation::New(BinaryOperator::LESS, Identifier::New("i"),
                                    Identifier::New("c")),
      &errors));

  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddIntVariable("i", Literal::New(0), Identifier::New("c"),
                                   Identifier::New("f"), &errors));
  EXPECT_TRUE(model.AddBoolVariable("b1", nullptr, &errors));
  std::vector<Update> updates;
  updates.emplace_back(
      "i", BinaryOperation::New(BinaryOperator::PLUS, Identifier::New("i"),
                                Literal::New(1)));
  std::vector<Outcome> outcomes;
  outcomes.emplace_back(
      Memoryless::New(BinaryOperation::New(
          BinaryOperator::DIVIDE, Literal::New(1), Identifier::New("i"))),
      std::move(updates));
  EXPECT_TRUE(model.AddCommand(
      Command("a", Identifier::New("guard"), std::move(outcomes)), &errors));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_TRUE(model.AddFromModule(
      "M1", {{"c", "k"}, {"f", "g"}, {"i", "j"}, {"b1", "b2"}, {"a", "b"}},
      &errors));
  model.EndModule();
  ASSERT_EQ(4, model.variables().size());
  EXPECT_EQ("j", model.variables()[2].name());
  EXPECT_EQ(Type::INT, model.variables()[2].type());
  EXPECT_EQ("0", StrCat(model.variables()[2].min()));
  EXPECT_EQ("k", StrCat(model.variables()[2].max()));
  EXPECT_TRUE(model.variables()[2].has_explicit_init());
  EXPECT_EQ("g", StrCat(model.variables()[2].init()));
  EXPECT_EQ("b2", model.variables()[3].name());
  EXPECT_EQ(Type::BOOL, model.variables()[3].type());
  EXPECT_FALSE(model.variables()[3].has_explicit_init());
  ASSERT_EQ(2, model.modules().size());
  EXPECT_EQ(std::set<int>({2, 3}), model.modules()[1].variables());
  ASSERT_EQ(1, model.modules()[1].commands().size());
  EXPECT_EQ("[b] j < k -> 1 / j : (j' = j + 1)",
            StrCat(model.modules()[1].commands()[0]));
  EXPECT_TRUE(errors.empty());
}

TEST(ModelTest, MissingSourceModule) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_FALSE(model.AddFromModule("M1", {}, &errors));
  EXPECT_EQ(std::vector<std::string>({"undefined module M1"}), errors);
}

TEST(ModelTest, IncompleteSubstitutions) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddBoolVariable("b1", nullptr, &errors));
  EXPECT_TRUE(model.AddBoolVariable("b2", nullptr, &errors));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_FALSE(model.AddFromModule("M1", {{"b1", "b3"}}, &errors));
  EXPECT_EQ(std::vector<std::string>({"missing substitution for variable b2"}),
            errors);
}

TEST(ModelTest, DuplicateVariablesFromSubstitution) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddBoolVariable("b1", nullptr, &errors));
  EXPECT_TRUE(model.AddBoolVariable("b2", nullptr, &errors));
  model.EndModule();
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_FALSE(
      model.AddFromModule("M1", {{"b1", "b3"}, {"b2", "b1"}}, &errors));
  EXPECT_EQ(std::vector<std::string>({"duplicate variable b1"}), errors);
}

TEST(ModelTest, RejectsDuplicateIdentifiers) {
  Model model;
  std::vector<std::string> errors;

  EXPECT_TRUE(model.AddConstant("c", Type::INT, nullptr, &errors));
  EXPECT_TRUE(model.AddIntVariable("i", Literal::New(17), Literal::New(42),
                                   nullptr, &errors));
  EXPECT_TRUE(model.AddBoolVariable("b", nullptr, &errors));
  EXPECT_TRUE(model.AddFormula("f", Literal::New(0.5), &errors));
  EXPECT_TRUE(model.StartModule("M1"));
  EXPECT_TRUE(model.AddCommand(
      Command("a", Literal::New(true), NewSimpleOutcomes()), &errors));
  model.EndModule();
  ASSERT_TRUE(errors.empty());

  EXPECT_FALSE(model.AddConstant("c", Type::INT, nullptr, &errors));
  EXPECT_FALSE(model.AddIntVariable("c", Literal::New(17), Literal::New(42),
                                    nullptr, &errors));
  EXPECT_FALSE(model.AddBoolVariable("c", nullptr, &errors));
  EXPECT_FALSE(model.AddFormula("c", Literal::New(0.5), &errors));
  EXPECT_TRUE(model.StartModule("M2"));
  EXPECT_FALSE(model.AddCommand(
      Command("c", Literal::New(true), NewSimpleOutcomes()), &errors));
  model.EndModule();
  EXPECT_EQ(
      std::vector<std::string>({"duplicate constant c",
                                "variable c previously defined as constant",
                                "variable c previously defined as constant",
                                "formula c previously defined as constant",
                                "action c previously defined as constant"}),
      errors);
  errors.clear();

  EXPECT_FALSE(model.AddConstant("i", Type::INT, nullptr, &errors));
  EXPECT_FALSE(model.AddIntVariable("i", Literal::New(17), Literal::New(42),
                                    nullptr, &errors));
  EXPECT_FALSE(model.AddBoolVariable("i", nullptr, &errors));
  EXPECT_FALSE(model.AddFormula("i", Literal::New(0.5), &errors));
  EXPECT_TRUE(model.StartModule("M3"));
  EXPECT_FALSE(model.AddCommand(
      Command("i", Literal::New(true), NewSimpleOutcomes()), &errors));
  model.EndModule();
  EXPECT_EQ(
      std::vector<std::string>({"constant i previously defined as variable",
                                "duplicate variable i", "duplicate variable i",
                                "formula i previously defined as variable",
                                "action i previously defined as variable"}),
      errors);
  errors.clear();

  EXPECT_FALSE(model.AddConstant("b", Type::INT, nullptr, &errors));
  EXPECT_FALSE(model.AddIntVariable("b", Literal::New(17), Literal::New(42),
                                    nullptr, &errors));
  EXPECT_FALSE(model.AddBoolVariable("b", nullptr, &errors));
  EXPECT_FALSE(model.AddFormula("b", Literal::New(0.5), &errors));
  EXPECT_TRUE(model.StartModule("M4"));
  EXPECT_FALSE(model.AddCommand(
      Command("b", Literal::New(true), NewSimpleOutcomes()), &errors));
  model.EndModule();
  EXPECT_EQ(
      std::vector<std::string>({"constant b previously defined as variable",
                                "duplicate variable b", "duplicate variable b",
                                "formula b previously defined as variable",
                                "action b previously defined as variable"}),
      errors);
  errors.clear();

  EXPECT_FALSE(model.AddConstant("f", Type::INT, nullptr, &errors));
  EXPECT_FALSE(model.AddIntVariable("f", Literal::New(17), Literal::New(42),
                                    nullptr, &errors));
  EXPECT_FALSE(model.AddBoolVariable("f", nullptr, &errors));
  EXPECT_FALSE(model.AddFormula("f", Literal::New(0.5), &errors));
  EXPECT_TRUE(model.StartModule("M5"));
  EXPECT_FALSE(model.AddCommand(
      Command("f", Literal::New(true), NewSimpleOutcomes()), &errors));
  model.EndModule();
  EXPECT_EQ(
      std::vector<std::string>({"constant f previously defined as formula",
                                "variable f previously defined as formula",
                                "variable f previously defined as formula",
                                "duplicate formula f",
                                "action f previously defined as formula"}),
      errors);
  errors.clear();

  EXPECT_FALSE(model.AddConstant("a", Type::INT, nullptr, &errors));
  EXPECT_FALSE(model.AddIntVariable("a", Literal::New(17), Literal::New(42),
                                    nullptr, &errors));
  EXPECT_FALSE(model.AddBoolVariable("a", nullptr, &errors));
  EXPECT_FALSE(model.AddFormula("a", Literal::New(0.5), &errors));
  EXPECT_TRUE(model.StartModule("M6"));
  // NOTE: It is fine to use the same action label multiple times.
  EXPECT_TRUE(model.AddCommand(
      Command("a", Literal::New(true), NewSimpleOutcomes()), &errors));
  model.EndModule();
  EXPECT_EQ(
      std::vector<std::string>({"constant a previously defined as action",
                                "variable a previously defined as action",
                                "variable a previously defined as action",
                                "formula a previously defined as action"}),
      errors);
  errors.clear();
}

TEST(ModelTest, AllowsSameNameForIdentifierAndModule) {
  Model model;
  std::vector<std::string> errors;
  EXPECT_TRUE(model.AddConstant("x", Type::INT, nullptr, &errors));
  EXPECT_TRUE(model.StartModule("x"));
  model.EndModule();
  EXPECT_TRUE(errors.empty());
}

TEST(ModelTest, RejectsDuplicateModules) {
  Model model;
  EXPECT_TRUE(model.StartModule("m"));
  model.EndModule();
  EXPECT_FALSE(model.StartModule("m"));
}

TEST(ModelTest, DefaultsToNullInit) {
  Model model;
  EXPECT_EQ(nullptr, model.init());
}

TEST(ModelTest, SetsInitOnce) {
  Model model;
  EXPECT_TRUE(model.SetInit(Literal::New(true)));
  EXPECT_EQ("true", StrCat(*model.init()));
  EXPECT_FALSE(model.SetInit(Literal::New(false)));
  EXPECT_EQ("true", StrCat(*model.init()));
}

}  // namespace
