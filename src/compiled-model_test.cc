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

#include "compiled-model.h"

#include "gtest/gtest.h"

#include "expression.h"
#include "model.h"
#include "optional.h"

namespace {

TEST(CompiledModelTest, MakeSetsModelType) {
  Model model1;
  ASSERT_TRUE(model1.SetType(ModelType::DTMC));
  Model model2;
  ASSERT_TRUE(model2.SetType(ModelType::CTMC));
  Model model3;
  ASSERT_TRUE(model3.SetType(ModelType::MDP));

  std::string error;
  CompiledModel compiled_model1 = CompiledModel::Make(model1, {}, &error);
  ASSERT_TRUE(compiled_model1.is_valid()) << error;
  EXPECT_EQ(model1.type(), compiled_model1.model_type());
  CompiledModel compiled_model2 = CompiledModel::Make(model2, {}, &error);
  ASSERT_TRUE(compiled_model2.is_valid()) << error;
  EXPECT_EQ(model2.type(), compiled_model2.model_type());
  CompiledModel compiled_model3 = CompiledModel::Make(model3, {}, &error);
  ASSERT_TRUE(compiled_model3.is_valid()) << error;
  EXPECT_EQ(model3.type(), compiled_model3.model_type());
}

TEST(CompiledModelTest, MakeSetsLiteralRangesAndDefaultInitsForVariables) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b", nullptr));
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(17), ParsedLiteral::Create(42)),
      nullptr));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  ASSERT_TRUE(compiled_model.is_valid()) << error;
  ASSERT_EQ(2, compiled_model.num_variables());
  EXPECT_EQ("b", compiled_model.variable_info(0).name);
  EXPECT_EQ(TypedValue(false), compiled_model.variable_info(0).min_value);
  EXPECT_EQ(true, compiled_model.variable_info(0).max_value);
  EXPECT_EQ(TypedValue(false), compiled_model.variable_info(0).init_value);
  EXPECT_EQ("i", compiled_model.variable_info(1).name);
  EXPECT_EQ(17, compiled_model.variable_info(1).min_value);
  EXPECT_EQ(42, compiled_model.variable_info(1).max_value);
  EXPECT_EQ(17, compiled_model.variable_info(1).init_value);
}

TEST(CompiledModelTest, MakeSetsLiteralInitsForVariables) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b1", ParsedLiteral::Create(true)));
  ASSERT_TRUE(model.AddBoolVariable("b2", ParsedLiteral::Create(false)));
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(17), ParsedLiteral::Create(42)),
      ParsedLiteral::Create(21)));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  ASSERT_TRUE(compiled_model.is_valid()) << error;
  ASSERT_EQ(3, compiled_model.num_variables());
  EXPECT_EQ("b1", compiled_model.variable_info(0).name);
  EXPECT_EQ(true, compiled_model.variable_info(0).init_value);
  EXPECT_EQ("b2", compiled_model.variable_info(1).name);
  EXPECT_EQ(TypedValue(false), compiled_model.variable_info(1).init_value);
  EXPECT_EQ("i", compiled_model.variable_info(2).name);
  EXPECT_EQ(21, compiled_model.variable_info(2).init_value);
}

TEST(CompiledModelTest, MakeSetsExpressionRangesAndInitsForVariables) {
  Model model;
  // const bool B = true;
  ASSERT_TRUE(model.AddConstant("B", Type::BOOL, ParsedLiteral::Create(true)));
  // const int I = floor(17/5);
  ASSERT_TRUE(model.AddConstant(
      "I",
      Type::INT,
      ParsedFunctionCall::Create(
          Function::FLOOR,
          ParsedBinaryOperation::Create(
              BinaryOperator::DIVIDE,
              ParsedLiteral::Create(17), ParsedLiteral::Create(5)))));
  // const int J = B ? -I : 0;
  ASSERT_TRUE(model.AddConstant(
      "J",
      Type::INT,
      ParsedConditional::Create(
          ParsedIdentifier::Create("B"),
          ParsedUnaryOperation::Create(UnaryOperator::NEGATE,
                                       ParsedIdentifier::Create("I")),
          ParsedLiteral::Create(0))));
  // formula f = !B ? mod(42, 0) : 2;
  ASSERT_TRUE(model.AddFormula(
      "f",
      ParsedConditional::Create(
          ParsedUnaryOperation::Create(UnaryOperator::NOT,
                                       ParsedIdentifier::Create("B")),
          ParsedFunctionCall::Create(
              Function::MOD,
              ParsedLiteral::Create(42), ParsedLiteral::Create(0)),
          ParsedLiteral::Create(2))));
  ASSERT_TRUE(model.AddBoolVariable("b", ParsedIdentifier::Create("B")));
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedIdentifier::Create("J"), ParsedIdentifier::Create("I")),
      ParsedIdentifier::Create("f")));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  ASSERT_TRUE(compiled_model.is_valid()) << error;
  ASSERT_EQ(2, compiled_model.num_variables());
  EXPECT_EQ("b", compiled_model.variable_info(0).name);
  EXPECT_EQ(true, compiled_model.variable_info(0).init_value);
  EXPECT_EQ("i", compiled_model.variable_info(1).name);
  EXPECT_EQ(-3, compiled_model.variable_info(1).min_value);
  EXPECT_EQ(3, compiled_model.variable_info(1).max_value);
  EXPECT_EQ(2, compiled_model.variable_info(1).init_value);
}

TEST(CompiledModelTest,
     MakeSetsExpressionRangesAndInitsForVariablesWithOverrides) {
  Model model;
  // const bool B = true;
  ASSERT_TRUE(model.AddConstant("B", Type::BOOL, ParsedLiteral::Create(true)));
  // const int I;
  ASSERT_TRUE(model.AddConstant("I", Type::INT, nullptr));
  // const int J = B ? -I : 0;
  ASSERT_TRUE(model.AddConstant(
      "J",
      Type::INT,
      ParsedConditional::Create(
          ParsedIdentifier::Create("B"),
          ParsedUnaryOperation::Create(UnaryOperator::NEGATE,
                                       ParsedIdentifier::Create("I")),
          ParsedLiteral::Create(0))));
  // formula f = !B ? mod(42, 0) : 2;
  ASSERT_TRUE(model.AddFormula(
      "f",
      ParsedConditional::Create(
          ParsedUnaryOperation::Create(UnaryOperator::NOT,
                                       ParsedIdentifier::Create("B")),
          ParsedFunctionCall::Create(
              Function::MOD,
              ParsedLiteral::Create(42), ParsedLiteral::Create(0)),
          ParsedLiteral::Create(2))));
  ASSERT_TRUE(model.AddBoolVariable("b", ParsedIdentifier::Create("B")));
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedIdentifier::Create("J"), ParsedIdentifier::Create("I")),
      ParsedIdentifier::Create("f")));

  std::string error;
  std::unique_ptr<const ParsedExpression> override1(ParsedLiteral::Create(4));
  std::unique_ptr<const ParsedExpression> override2(ParsedLiteral::Create(1));
  CompiledModel compiled_model = CompiledModel::Make(
      model, { { "I", override1.get() }, { "J", override2.get() } }, &error);
  ASSERT_TRUE(compiled_model.is_valid()) << error;
  ASSERT_EQ(2, compiled_model.num_variables());
  EXPECT_EQ("b", compiled_model.variable_info(0).name);
  EXPECT_EQ(true, compiled_model.variable_info(0).init_value);
  EXPECT_EQ("i", compiled_model.variable_info(1).name);
  EXPECT_EQ(1, compiled_model.variable_info(1).min_value);
  EXPECT_EQ(4, compiled_model.variable_info(1).max_value);
  EXPECT_EQ(2, compiled_model.variable_info(1).init_value);
}

TEST(CompiledModelTest, MakeFailsOnInvertedVariableRange) {
  Model model;
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(42), ParsedLiteral::Create(17)),
      nullptr));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("bad range [42..17] for variable 'i'", error);
}

TEST(CompiledModelTest, MakeFailsOnEmptyVariableRange) {
  Model model;
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(0), ParsedLiteral::Create(0)),
      nullptr));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("bad range [0..0] for variable 'i'", error);
}

TEST(CompiledModelTest, MakeFailsOnOutOfRangeInit) {
  Model model;
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(17), ParsedLiteral::Create(42)),
      ParsedLiteral::Create(-21)));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("init value -21 not in range [17..42] for variable 'i'", error);
}

TEST(CompiledModelTest, MakeFailsOnBoolRangeBoundForIntVariable) {
  Model model;
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(0), ParsedLiteral::Create(true)),
      nullptr));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("type mismatch; expecting int, found bool", error);
}

TEST(CompiledModelTest, MakeFailsOnDoubleRangeBoundForIntVariable) {
  Model model;
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(3.14159), ParsedLiteral::Create(4)),
      nullptr));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("type mismatch; expecting int, found double", error);
}

TEST(CompiledModelTest, MakeFailsOnBoolInitForIntVariable) {
  Model model;
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(0), ParsedLiteral::Create(1)),
      ParsedLiteral::Create(true)));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("type mismatch; expecting int, found bool", error);
}

TEST(CompiledModelTest, MakeFailsOnDoubleInitForIntVariable) {
  Model model;
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(0), ParsedLiteral::Create(1)),
      ParsedLiteral::Create(0.5)));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("type mismatch; expecting int, found double", error);
}

TEST(CompiledModelTest, MakeFailsOnIntInitForBoolVariable) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b", ParsedLiteral::Create(1)));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("type mismatch; expecting bool, found int", error);
}

TEST(CompiledModelTest, MakeFailsOnDoubleInitForBoolVariable) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b", ParsedLiteral::Create(0.5)));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("type mismatch; expecting bool, found double", error);
}

TEST(CompiledModelTest, MakeFailsOnNonConstantRangeBound) {
  Model model;
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(0), ParsedLiteral::Create(1)),
      nullptr));
  ASSERT_TRUE(model.AddIntVariable(
      "j",
      Range(ParsedLiteral::Create(0), ParsedIdentifier::Create("i")),
      nullptr));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("expecting constant expression", error);
}

TEST(CompiledModelTest, MakeFailsOnNonConstantVariableInit) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b1", nullptr));
  ASSERT_TRUE(model.AddBoolVariable("b2", ParsedIdentifier::Create("b1")));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("expecting constant expression", error);
}

TEST(CompiledModelTest, MakeFailsOnUndefinedSymbol) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b", ParsedIdentifier::Create("foo")));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("undefined symbol 'foo'", error);
}

TEST(CompiledModelTest, MakeFailsOnCyclicConstantDependencies) {
  Model model;
  ASSERT_TRUE(model.AddConstant("A", Type::INT, ParsedIdentifier::Create("B")));
  ASSERT_TRUE(model.AddConstant("B", Type::INT, ParsedIdentifier::Create("C")));
  ASSERT_TRUE(model.AddConstant("C", Type::INT, ParsedIdentifier::Create("A")));
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(0), ParsedLiteral::Create(1)),
      ParsedIdentifier::Create("A")));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("cyclic evaluation for constant 'A'", error);
}

TEST(CompiledModelTest, MakeFailsOnUninitializedConstant) {
  Model model;
  ASSERT_TRUE(model.AddConstant("I", Type::INT, nullptr));
  ASSERT_TRUE(model.AddIntVariable(
      "i",
      Range(ParsedLiteral::Create(0), ParsedLiteral::Create(1)),
      ParsedIdentifier::Create("I")));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("uninitialized constant 'I'", error);
}

TEST(CompiledModelTest, MakeWithoutInitExpression) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b", nullptr));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  ASSERT_TRUE(compiled_model.is_valid()) << error;
  EXPECT_EQ(nullptr, compiled_model.init());
}

TEST(CompiledModelTest, MakeSetsInitExpression) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b", nullptr));
  ASSERT_TRUE(model.SetInit(ParsedLiteral::Create(true)));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  ASSERT_TRUE(compiled_model.is_valid()) << error;
  ASSERT_NE(nullptr, compiled_model.init());
  EXPECT_EQ(true, compiled_model.init()->ValueInState({}));
}

TEST(CompiledModelTest, MakeFailsOnAmbiguousVariableInit) {
  Model model;
  ASSERT_TRUE(model.AddBoolVariable("b", ParsedLiteral::Create(true)));
  ASSERT_TRUE(model.SetInit(ParsedLiteral::Create(true)));

  std::string error;
  CompiledModel compiled_model = CompiledModel::Make(model, {}, &error);
  EXPECT_FALSE(compiled_model.is_valid());
  EXPECT_EQ("global init not allowed in model with explicit variable inits",
            error);
}

}  // namespace
