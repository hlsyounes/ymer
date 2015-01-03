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
  ResolveConstants(constants, &constant_values, &errors);
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
  ResolveConstants(constants, &constant_values, &errors);
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
  ResolveConstants(constants, &constant_values, &errors);
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
  ResolveConstants(constants, &constant_values, &errors);
  EXPECT_EQ(expected_errors, errors);
}

TEST(ResolveConstantsTest, Identifier) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("c", Type::DOUBLE, nullptr);
  constants.emplace_back("C", Type::DOUBLE, Identifier::New("a"));
  constants.emplace_back("a", Type::INT, Identifier::New("A"));
  constants.emplace_back("A", Type::INT, nullptr);
  constants.emplace_back("b", Type::BOOL, Identifier::New("B"));
  constants.emplace_back("B", Type::BOOL, nullptr);
  std::map<std::string, TypedValue> expected_constant_values = {
      {"a", 17}, {"A", 17}, {"b", true}, {"B", true}, {"c", 0.5}, {"C", 17}};
  std::map<std::string, TypedValue> constant_values = {
      {"A", 17}, {"B", true}, {"c", 0.5}};
  std::vector<std::string> errors;
  ResolveConstants(constants, &constant_values, &errors);
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
  ResolveConstants(constants, &constant_values, &errors);
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
  ResolveConstants(constants, &constant_values, &errors);
  EXPECT_EQ(expected_errors, errors);
}

TEST(ResolveConstantsTest, IdentifierUninitialized) {
  std::vector<ParsedConstant> constants;
  constants.emplace_back("a", Type::INT, nullptr);
  std::vector<std::string> expected_errors = {"uninitialized constant a"};
  std::map<std::string, TypedValue> constant_values;
  std::vector<std::string> errors;
  ResolveConstants(constants, &constant_values, &errors);
  EXPECT_EQ(expected_errors, errors);
}

// TODO(hlsyounes): test function call.

// TODO(hlsyounes): test unary operation.

// TODO(hlsyounes): test binary operation.

// TODO(hlsyounes): test conditional.

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
}

}  // namespace
