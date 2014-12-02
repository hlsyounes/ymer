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
  EXPECT_EQ("a' = 17", StrCat(Update("a", Literal::New(17))));
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

}  // namespace
