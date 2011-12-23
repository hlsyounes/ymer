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

#include "expression-compiler.h"

#include <map>
#include <string>

#include "gtest/gtest.h"

#include "ddutil.h"
#include "expression.h"
#include "type.h"

namespace {

class CompileExpressionTest : public testing::Test {
 protected:
  CompileExpressionTest()
      : dd_manager_(4),
        bdd_(dd_manager_.GetConstant(false)),
        add_(dd_manager_.GetConstant(0)) {
  }

  int GetIntValue(const std::vector<bool>& bits) {
    return GetValue(dd_manager_, add_, bits);
  }

  double GetDoubleValue(const std::vector<bool>& bits) {
    return GetValue(dd_manager_, add_, bits);
  }

  bool GetBoolValue(const std::vector<bool>& bits) {
    return GetValue(dd_manager_, bdd_, bits);
  }

  DecisionDiagramManager dd_manager_;
  BDD bdd_;
  ADD add_;
  std::string error_;
};

TEST_F(CompileExpressionTest, ChecksIntLiteral) {
  const IntLiteral expr(17);
  EXPECT_TRUE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(17, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIntLiteral) {
  const IntLiteral expr(17);
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(17, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksDoubleLiteral) {
  const DoubleLiteral expr(3.14159);
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(3.14159, add_.Value());
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesDoubleLiteral) {
  const DoubleLiteral expr(3.14159);
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(3.14159, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBoolLiteral) {
  const BoolLiteral expr(true);
  EXPECT_TRUE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolLiteral) {
  const BoolLiteral expr(true);
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
}

TEST_F(CompileExpressionTest, ChecksIntConstant) {
  const Identifier i("i");
  const IntLiteral i_init(17);
  const ConstantValueMap constants = { { "i", { Type::INT, &i_init } } };
  EXPECT_TRUE(CompileExpression<int>(
      i, constants, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(17, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      i, constants, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      i, constants, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIntConstant) {
  const Identifier i("i");
  const IntLiteral i_init(17);
  const ConstantValueMap constants = { { "i", { Type::INT, &i_init } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      i, constants, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(17, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksDoubleConstant) {
  const Identifier x("x");
  const DoubleLiteral x_init(3.14159);
  const ConstantValueMap constants = { { "x", { Type::DOUBLE, &x_init } } };
  EXPECT_TRUE(CompileExpression<double>(
      x, constants, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(3.14159, add_.Value());
  EXPECT_FALSE(CompileExpression<int>(
      x, constants, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      x, constants, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesDoubleConstant) {
  const Identifier x("x");
  const DoubleLiteral x_init(3.14159);
  const ConstantValueMap constants = { { "x", { Type::DOUBLE, &x_init } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      x, constants, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(3.14159, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBoolConstant) {
  const Identifier b("b");
  const BoolLiteral b_init(true);
  const ConstantValueMap constants = { { "b", { Type::BOOL, &b_init } } };
  EXPECT_TRUE(CompileExpression<bool>(
      b, constants, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
  EXPECT_FALSE(CompileExpression<int>(
      b, constants, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      b, constants, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolConstant) {
  const Identifier b("b");
  const BoolLiteral b_init(true);
  const ConstantValueMap constants = { { "b", { Type::BOOL, &b_init } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      b, constants, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
}

TEST_F(CompileExpressionTest, ChecksIntFormula) {
  const Identifier i("i");
  const IntLiteral i_init(17);
  const std::map<std::string, const Expression*> formulas =
      { { "i", &i_init } };
  EXPECT_TRUE(CompileExpression<int>(
      i, {}, formulas, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(17, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      i, {}, formulas, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      i, {}, formulas, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIntFormula) {
  const Identifier i("i");
  const IntLiteral i_init(17);
  const std::map<std::string, const Expression*> formulas =
      { { "i", &i_init } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      i, {}, formulas, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(17, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksDoubleFormula) {
  const Identifier x("x");
  const DoubleLiteral x_init(3.14159);
  const std::map<std::string, const Expression*> formulas =
      { { "x", &x_init } };
  EXPECT_TRUE(CompileExpression<double>(
      x, {}, formulas, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(3.14159, add_.Value());
  EXPECT_FALSE(CompileExpression<int>(
      x, {}, formulas, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      x, {}, formulas, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesDoubleFormula) {
  const Identifier x("x");
  const DoubleLiteral x_init(3.14159);
  const std::map<std::string, const Expression*> formulas =
      { { "x", &x_init } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      x, {}, formulas, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(3.14159, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBoolFormula) {
  const Identifier b("b");
  const BoolLiteral b_init(true);
  const std::map<std::string, const Expression*> formulas =
      { { "b", &b_init } };
  EXPECT_TRUE(CompileExpression<bool>(
      b, {}, formulas, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
  EXPECT_FALSE(CompileExpression<int>(
      b, {}, formulas, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      b, {}, formulas, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolFormula) {
  const Identifier b("b");
  const BoolLiteral b_init(true);
  const std::map<std::string, const Expression*> formulas =
      { { "b", &b_init } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      b, {}, formulas, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
}

TEST_F(CompileExpressionTest, ChecksIntVariable) {
  const Identifier i("i");
  const std::map<std::string, VariableInfo> variables =
      { { "i", { Type::INT, 17, 0, 1 } } };
  EXPECT_TRUE(CompileExpression<int>(
      i, {}, {}, variables, dd_manager_, &add_, nullptr));
  EXPECT_EQ(17, GetIntValue({ false, false, false, false }));
  EXPECT_EQ(17, GetIntValue({ false, false, false, true }));
  EXPECT_EQ(18, GetIntValue({ false, false, true, false }));
  EXPECT_EQ(18, GetIntValue({ false, false, true, true }));
  EXPECT_EQ(17, GetIntValue({ false, true, false, false }));
  EXPECT_EQ(17, GetIntValue({ false, true, false, true }));
  EXPECT_EQ(18, GetIntValue({ false, true, true, false }));
  EXPECT_EQ(18, GetIntValue({ false, true, true, true }));
  EXPECT_EQ(19, GetIntValue({ true, false, false, false }));
  EXPECT_EQ(19, GetIntValue({ true, false, false, true }));
  EXPECT_EQ(20, GetIntValue({ true, false, true, false }));
  EXPECT_EQ(20, GetIntValue({ true, false, true, true }));
  EXPECT_EQ(19, GetIntValue({ true, true, false, false }));
  EXPECT_EQ(19, GetIntValue({ true, true, false, true }));
  EXPECT_EQ(20, GetIntValue({ true, true, true, false }));
  EXPECT_EQ(20, GetIntValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<bool>(
      i, {}, {}, variables, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIntVariable) {
  const Identifier i("i");
  const std::map<std::string, VariableInfo> variables =
      { { "i", { Type::INT, 17, 0, 1 } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      i, {}, {}, variables, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(17, GetIntValue({ false, false, false, false }));
  EXPECT_EQ(17, GetIntValue({ false, false, false, true }));
  EXPECT_EQ(18, GetIntValue({ false, false, true, false }));
  EXPECT_EQ(18, GetIntValue({ false, false, true, true }));
  EXPECT_EQ(17, GetIntValue({ false, true, false, false }));
  EXPECT_EQ(17, GetIntValue({ false, true, false, true }));
  EXPECT_EQ(18, GetIntValue({ false, true, true, false }));
  EXPECT_EQ(18, GetIntValue({ false, true, true, true }));
  EXPECT_EQ(19, GetIntValue({ true, false, false, false }));
  EXPECT_EQ(19, GetIntValue({ true, false, false, true }));
  EXPECT_EQ(20, GetIntValue({ true, false, true, false }));
  EXPECT_EQ(20, GetIntValue({ true, false, true, true }));
  EXPECT_EQ(19, GetIntValue({ true, true, false, false }));
  EXPECT_EQ(19, GetIntValue({ true, true, false, true }));
  EXPECT_EQ(20, GetIntValue({ true, true, true, false }));
  EXPECT_EQ(20, GetIntValue({ true, true, true, true }));
}

TEST_F(CompileExpressionTest, ChecksBoolVariable) {
  const Identifier b("b");
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 1, 1 } } };
  EXPECT_TRUE(CompileExpression<bool>(
      b, {}, {}, variables, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(GetBoolValue({ false, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ false, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ false, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ false, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ false, true, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<int>(
      b, {}, {}, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolVariable) {
  const Identifier b("b");
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 1, 1 } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      b, {}, {}, variables, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_FALSE(GetBoolValue({ false, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ false, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ false, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ false, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ false, true, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, true }));
}

TEST_F(CompileExpressionTest, ChecksUndefinedSymbol) {
  const Identifier foo("foo");
  const Identifier bar("bar");
  EXPECT_FALSE(CompileExpression<int>(
      foo, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("undefined symbol 'foo'", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      bar, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("undefined symbol 'bar'", error_);
}

TEST_F(CompileExpressionTest, SynthesizesUndefinedSymbol) {
  const Identifier foo("foo");
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      foo, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("undefined symbol 'foo'", error_);
}

TEST_F(CompileExpressionTest, ChecksChainedConstants) {
  const Identifier i("i");
  const Identifier j("j");
  const IntLiteral j_init(17);
  const ConstantValueMap constants =
      { { "i", { Type::INT, &j } }, { "j", { Type::INT, &j_init } } };
  EXPECT_TRUE(CompileExpression<int>(
      i, constants, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(17, add_.Value());
}

TEST_F(CompileExpressionTest, SynthesizesChainedConstants) {
  const Identifier i("i");
  const Identifier j("j");
  const IntLiteral j_init(17);
  const ConstantValueMap constants =
      { { "i", { Type::INT, &j } }, { "j", { Type::INT, &j_init } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      i, constants, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(17, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksCyclicConstants) {
  const Identifier i("i");
  const Identifier j("j");
  const Identifier k("k");
  const ConstantValueMap constants = {
    { "i", { Type::INT, &j } },
    { "j", { Type::INT, &k } },
    { "k", {  Type::INT, &i } }
  };
  EXPECT_FALSE(CompileExpression<int>(
      i, constants, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("cyclic evaluation for constant 'i'", error_);
}

TEST_F(CompileExpressionTest, SynthesizesCyclicConstants) {
  const Identifier i("i");
  const Identifier j("j");
  const Identifier k("k");
  const ConstantValueMap constants = {
    { "i", { Type::INT, &j } },
    { "j", { Type::INT, &k } },
    { "k", {  Type::INT, &i } }
  };
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      i, constants, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("cyclic evaluation for constant 'i'", error_);
}

TEST_F(CompileExpressionTest, ChecksIncompleteConstants) {
  const Identifier i("i");
  const Identifier j("j");
  const ConstantValueMap constants = { { "i", { Type::INT, &j } } };
  const std::map<std::string, VariableInfo> variables =
      { { "j", { Type::INT, 17, 0, 1 } } };
  EXPECT_FALSE(CompileExpression<int>(
      i, constants, {}, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("incomplete evaluation for constant 'i'", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIncompleteConstants) {
  const Identifier i("i");
  const Identifier j("j");
  const ConstantValueMap constants = { { "i", { Type::INT, &j } } };
  const std::map<std::string, VariableInfo> variables =
      { { "j", { Type::INT, 17, 0, 1 } } };
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      i, constants, {}, variables, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("incomplete evaluation for constant 'i'", error_);
}

TEST_F(CompileExpressionTest, ChecksIntConditional) {
  const Conditional expr(Identifier::Create("b"),
                         Identifier::Create("i"), IntLiteral::Create(42));
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } }, { "i", { Type::INT, 17, 1, 1 } } };
  EXPECT_TRUE(CompileExpression<int>(
      expr, {}, {}, variables, dd_manager_, &add_, nullptr));
  EXPECT_EQ(42, GetIntValue({ false, false, false, false }));
  EXPECT_EQ(42, GetIntValue({ false, false, false, true }));
  EXPECT_EQ(42, GetIntValue({ false, false, true, false }));
  EXPECT_EQ(42, GetIntValue({ false, false, true, true }));
  EXPECT_EQ(42, GetIntValue({ false, true, false, false }));
  EXPECT_EQ(42, GetIntValue({ false, true, false, true }));
  EXPECT_EQ(42, GetIntValue({ false, true, true, false }));
  EXPECT_EQ(42, GetIntValue({ false, true, true, true }));
  EXPECT_EQ(17, GetIntValue({ true, false, false, false }));
  EXPECT_EQ(17, GetIntValue({ true, false, false, true }));
  EXPECT_EQ(18, GetIntValue({ true, false, true, false }));
  EXPECT_EQ(18, GetIntValue({ true, false, true, true }));
  EXPECT_EQ(17, GetIntValue({ true, true, false, false }));
  EXPECT_EQ(17, GetIntValue({ true, true, false, true }));
  EXPECT_EQ(18, GetIntValue({ true, true, true, false }));
  EXPECT_EQ(18, GetIntValue({ true, true, true, true }));
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, {}, variables, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, false, false, false }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, false, false, true }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, false, true, false }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, false, true, true }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, true, false, false }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, true, false, true }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, true, true, false }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, true, true, true }));
  EXPECT_DOUBLE_EQ(17.0, GetDoubleValue({ true, false, false, false }));
  EXPECT_DOUBLE_EQ(17.0, GetDoubleValue({ true, false, false, true }));
  EXPECT_DOUBLE_EQ(18.0, GetDoubleValue({ true, false, true, false }));
  EXPECT_DOUBLE_EQ(18.0, GetDoubleValue({ true, false, true, true }));
  EXPECT_DOUBLE_EQ(17.0, GetDoubleValue({ true, true, false, false }));
  EXPECT_DOUBLE_EQ(17.0, GetDoubleValue({ true, true, false, true }));
  EXPECT_DOUBLE_EQ(18.0, GetDoubleValue({ true, true, true, false }));
  EXPECT_DOUBLE_EQ(18.0, GetDoubleValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, variables, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIntConditional) {
  const Conditional expr(Identifier::Create("b"),
                         Identifier::Create("i"), IntLiteral::Create(42));
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } }, { "i", { Type::INT, 17, 1, 1 } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, variables, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(42, GetIntValue({ false, false, false, false }));
  EXPECT_EQ(42, GetIntValue({ false, false, false, true }));
  EXPECT_EQ(42, GetIntValue({ false, false, true, false }));
  EXPECT_EQ(42, GetIntValue({ false, false, true, true }));
  EXPECT_EQ(42, GetIntValue({ false, true, false, false }));
  EXPECT_EQ(42, GetIntValue({ false, true, false, true }));
  EXPECT_EQ(42, GetIntValue({ false, true, true, false }));
  EXPECT_EQ(42, GetIntValue({ false, true, true, true }));
  EXPECT_EQ(17, GetIntValue({ true, false, false, false }));
  EXPECT_EQ(17, GetIntValue({ true, false, false, true }));
  EXPECT_EQ(18, GetIntValue({ true, false, true, false }));
  EXPECT_EQ(18, GetIntValue({ true, false, true, true }));
  EXPECT_EQ(17, GetIntValue({ true, true, false, false }));
  EXPECT_EQ(17, GetIntValue({ true, true, false, true }));
  EXPECT_EQ(18, GetIntValue({ true, true, true, false }));
  EXPECT_EQ(18, GetIntValue({ true, true, true, true }));
}

TEST_F(CompileExpressionTest, ChecksMixedTypeConditional) {
  const Conditional expr1(BoolLiteral::Create(true),
                          IntLiteral::Create(17),
                          DoubleLiteral::Create(3.14159));
  const Conditional expr2(BoolLiteral::Create(false),
                          DoubleLiteral::Create(3.14159),
                          IntLiteral::Create(42));
  const Conditional expr3(BoolLiteral::Create(true),
                          BoolLiteral::Create(true), IntLiteral::Create(17));
  const Conditional expr4(BoolLiteral::Create(true),
                          DoubleLiteral::Create(3.14159),
                          BoolLiteral::Create(true));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_TRUE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_TRUE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_DOUBLE_EQ(42.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types in conditional; bool and int", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types in conditional; bool and int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("incompatible types in conditional; bool and int", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types in conditional; double and bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types in conditional; double and bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr4, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("incompatible types in conditional; double and bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesMixedTypeConditional) {
  const Conditional expr1(BoolLiteral::Create(true),
                          IntLiteral::Create(17),
                          DoubleLiteral::Create(3.14159));
  const Conditional expr2(BoolLiteral::Create(false),
                          DoubleLiteral::Create(3.14159),
                          IntLiteral::Create(42));
  const Conditional expr3(BoolLiteral::Create(true),
                          BoolLiteral::Create(true), IntLiteral::Create(17));
  const Conditional expr4(BoolLiteral::Create(true),
                          DoubleLiteral::Create(3.14159),
                          BoolLiteral::Create(true));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(42.0, add_.Value());
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("incompatible types in conditional; bool and int", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr4, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("incompatible types in conditional; double and bool", error_);
}

TEST_F(CompileExpressionTest, ChecksDoubleConditional) {
  const Conditional expr(Identifier::Create("b"),
                         Identifier::Create("x"),
                         DoubleLiteral::Create(3.14159));
  DoubleLiteral x_init(0.5);
  const std::map<std::string, const Expression*> formulas =
      { { "x", &x_init } };
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } } };
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, formulas, variables, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(3.14159, GetDoubleValue({ false, false, false, false }));
  EXPECT_DOUBLE_EQ(3.14159, GetDoubleValue({ false, false, false, true }));
  EXPECT_DOUBLE_EQ(3.14159, GetDoubleValue({ false, false, true, false }));
  EXPECT_DOUBLE_EQ(3.14159, GetDoubleValue({ false, false, true, true }));
  EXPECT_DOUBLE_EQ(3.14159, GetDoubleValue({ false, true, false, false }));
  EXPECT_DOUBLE_EQ(3.14159, GetDoubleValue({ false, true, false, true }));
  EXPECT_DOUBLE_EQ(3.14159, GetDoubleValue({ false, true, true, false }));
  EXPECT_DOUBLE_EQ(3.14159, GetDoubleValue({ false, true, true, true }));
  EXPECT_DOUBLE_EQ(0.5, GetDoubleValue({ true, false, false, false }));
  EXPECT_DOUBLE_EQ(0.5, GetDoubleValue({ true, false, false, true }));
  EXPECT_DOUBLE_EQ(0.5, GetDoubleValue({ true, false, true, false }));
  EXPECT_DOUBLE_EQ(0.5, GetDoubleValue({ true, false, true, true }));
  EXPECT_DOUBLE_EQ(0.5, GetDoubleValue({ true, true, false, false }));
  EXPECT_DOUBLE_EQ(0.5, GetDoubleValue({ true, true, false, true }));
  EXPECT_DOUBLE_EQ(0.5, GetDoubleValue({ true, true, true, false }));
  EXPECT_DOUBLE_EQ(0.5, GetDoubleValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, formulas, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, formulas, variables, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesDoubleConditional) {
  const Conditional expr(BoolLiteral::Create(true),
                         DoubleLiteral::Create(0.5),
                         DoubleLiteral::Create(3.14159));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(0.5, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBoolConditional) {
  const Conditional expr(Identifier::Create("b"),
                         BoolLiteral::Create(false), BoolLiteral::Create(true));
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } } };
  EXPECT_TRUE(CompileExpression<bool>(
      expr, {}, {}, variables, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(GetBoolValue({ false, false, false, false }));
  EXPECT_TRUE(GetBoolValue({ false, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ false, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ false, false, true, true }));
  EXPECT_TRUE(GetBoolValue({ false, true, false, false }));
  EXPECT_TRUE(GetBoolValue({ false, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ false, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ false, true, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, true }));
  EXPECT_FALSE(GetBoolValue({ true, false, true, false }));
  EXPECT_FALSE(GetBoolValue({ true, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, true }));
  EXPECT_FALSE(GetBoolValue({ true, true, true, false }));
  EXPECT_FALSE(GetBoolValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr, {}, {}, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolConditional) {
  const Conditional expr(Identifier::Create("b"),
                         BoolLiteral::Create(false), BoolLiteral::Create(true));
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, variables, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(GetBoolValue({ false, false, false, false }));
  EXPECT_TRUE(GetBoolValue({ false, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ false, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ false, false, true, true }));
  EXPECT_TRUE(GetBoolValue({ false, true, false, false }));
  EXPECT_TRUE(GetBoolValue({ false, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ false, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ false, true, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, true }));
  EXPECT_FALSE(GetBoolValue({ true, false, true, false }));
  EXPECT_FALSE(GetBoolValue({ true, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, true }));
  EXPECT_FALSE(GetBoolValue({ true, true, true, false }));
  EXPECT_FALSE(GetBoolValue({ true, true, true, true }));
}

TEST_F(CompileExpressionTest, ChecksConditionalWithNonBoolCondition) {
  const Conditional expr(DoubleLiteral::Create(3.14159),
                         IntLiteral::Create(17), IntLiteral::Create(42));
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesConditionalWithNonBoolCondition) {
  const Conditional expr(DoubleLiteral::Create(3.14159),
                         IntLiteral::Create(17), IntLiteral::Create(42));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, ChecksIntNegate) {
  const UnaryOperation expr(
      UnaryOperator::NEGATE,
      Conditional::Create(Identifier::Create("b"),
                          Identifier::Create("i"), IntLiteral::Create(-42)));
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } }, { "i", { Type::INT, 17, 1, 1 } } };
  EXPECT_TRUE(CompileExpression<int>(
      expr, {}, {}, variables, dd_manager_, &add_, nullptr));
  EXPECT_EQ(42, GetIntValue({ false, false, false, false }));
  EXPECT_EQ(42, GetIntValue({ false, false, false, true }));
  EXPECT_EQ(42, GetIntValue({ false, false, true, false }));
  EXPECT_EQ(42, GetIntValue({ false, false, true, true }));
  EXPECT_EQ(42, GetIntValue({ false, true, false, false }));
  EXPECT_EQ(42, GetIntValue({ false, true, false, true }));
  EXPECT_EQ(42, GetIntValue({ false, true, true, false }));
  EXPECT_EQ(42, GetIntValue({ false, true, true, true }));
  EXPECT_EQ(-17, GetIntValue({ true, false, false, false }));
  EXPECT_EQ(-17, GetIntValue({ true, false, false, true }));
  EXPECT_EQ(-18, GetIntValue({ true, false, true, false }));
  EXPECT_EQ(-18, GetIntValue({ true, false, true, true }));
  EXPECT_EQ(-17, GetIntValue({ true, true, false, false }));
  EXPECT_EQ(-17, GetIntValue({ true, true, false, true }));
  EXPECT_EQ(-18, GetIntValue({ true, true, true, false }));
  EXPECT_EQ(-18, GetIntValue({ true, true, true, true }));
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, {}, variables, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, false, false, false }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, false, false, true }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, false, true, false }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, false, true, true }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, true, false, false }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, true, false, true }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, true, true, false }));
  EXPECT_DOUBLE_EQ(42.0, GetDoubleValue({ false, true, true, true }));
  EXPECT_DOUBLE_EQ(-17.0, GetDoubleValue({ true, false, false, false }));
  EXPECT_DOUBLE_EQ(-17.0, GetDoubleValue({ true, false, false, true }));
  EXPECT_DOUBLE_EQ(-18.0, GetDoubleValue({ true, false, true, false }));
  EXPECT_DOUBLE_EQ(-18.0, GetDoubleValue({ true, false, true, true }));
  EXPECT_DOUBLE_EQ(-17.0, GetDoubleValue({ true, true, false, false }));
  EXPECT_DOUBLE_EQ(-17.0, GetDoubleValue({ true, true, false, true }));
  EXPECT_DOUBLE_EQ(-18.0, GetDoubleValue({ true, true, true, false }));
  EXPECT_DOUBLE_EQ(-18.0, GetDoubleValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, variables, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIntNegate) {
  const UnaryOperation expr(
      UnaryOperator::NEGATE,
      Conditional::Create(Identifier::Create("b"),
                          Identifier::Create("i"), IntLiteral::Create(-42)));
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } }, { "i", { Type::INT, 17, 1, 1 } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, variables, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(42, GetIntValue({ false, false, false, false }));
  EXPECT_EQ(42, GetIntValue({ false, false, false, true }));
  EXPECT_EQ(42, GetIntValue({ false, false, true, false }));
  EXPECT_EQ(42, GetIntValue({ false, false, true, true }));
  EXPECT_EQ(42, GetIntValue({ false, true, false, false }));
  EXPECT_EQ(42, GetIntValue({ false, true, false, true }));
  EXPECT_EQ(42, GetIntValue({ false, true, true, false }));
  EXPECT_EQ(42, GetIntValue({ false, true, true, true }));
  EXPECT_EQ(-17, GetIntValue({ true, false, false, false }));
  EXPECT_EQ(-17, GetIntValue({ true, false, false, true }));
  EXPECT_EQ(-18, GetIntValue({ true, false, true, false }));
  EXPECT_EQ(-18, GetIntValue({ true, false, true, true }));
  EXPECT_EQ(-17, GetIntValue({ true, true, false, false }));
  EXPECT_EQ(-17, GetIntValue({ true, true, false, true }));
  EXPECT_EQ(-18, GetIntValue({ true, true, true, false }));
  EXPECT_EQ(-18, GetIntValue({ true, true, true, true }));
}

TEST_F(CompileExpressionTest, ChecksDoubleNegate) {
  const UnaryOperation expr(UnaryOperator::NEGATE,
                            DoubleLiteral::Create(3.14159));
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(-3.14159, add_.Value());
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesDoubleNegate) {
  const UnaryOperation expr(UnaryOperator::NEGATE,
                            DoubleLiteral::Create(3.14159));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(-3.14159, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBoolNegate) {
  const UnaryOperation expr(UnaryOperator::NEGATE, BoolLiteral::Create(true));
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; unary operator - applied to bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; unary operator - applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; unary operator - applied to bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolNegate) {
  const UnaryOperation expr(UnaryOperator::NEGATE, BoolLiteral::Create(true));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; unary operator - applied to bool", error_);
}

TEST_F(CompileExpressionTest, ChecksIntNot) {
  const UnaryOperation expr(UnaryOperator::NOT, IntLiteral::Create(17));
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; unary operator ! applied to int", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; unary operator ! applied to int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; unary operator ! applied to int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIntNot) {
  const UnaryOperation expr(UnaryOperator::NOT, IntLiteral::Create(17));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; unary operator ! applied to int", error_);
}

TEST_F(CompileExpressionTest, ChecksDoubleNot) {
  const UnaryOperation expr(UnaryOperator::NOT, DoubleLiteral::Create(3.14159));
  EXPECT_FALSE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; unary operator ! applied to double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; unary operator ! applied to double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; unary operator ! applied to double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesDoubleNot) {
  const UnaryOperation expr(UnaryOperator::NOT, DoubleLiteral::Create(3.14159));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; unary operator ! applied to double", error_);
}

TEST_F(CompileExpressionTest, ChecksBoolNot) {
  const UnaryOperation expr(
      UnaryOperator::NOT,
      Conditional::Create(Identifier::Create("b"),
                          BoolLiteral::Create(false),
                          BoolLiteral::Create(true)));
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } } };
  EXPECT_TRUE(CompileExpression<bool>(
      expr, {}, {}, variables, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(GetBoolValue({ false, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, false, true }));
  EXPECT_FALSE(GetBoolValue({ false, false, true, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, true, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, true, true }));
  EXPECT_TRUE(GetBoolValue({ true, false, false, false }));
  EXPECT_TRUE(GetBoolValue({ true, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, true }));
  EXPECT_TRUE(GetBoolValue({ true, true, false, false }));
  EXPECT_TRUE(GetBoolValue({ true, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr, {}, {}, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolNot) {
  const UnaryOperation expr(
      UnaryOperator::NOT,
      Conditional::Create(Identifier::Create("b"),
                          BoolLiteral::Create(false),
                          BoolLiteral::Create(true)));
  const std::map<std::string, VariableInfo> variables =
      { { "b", { Type::BOOL, 0, 0, 0 } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, variables, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_FALSE(GetBoolValue({ false, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, false, true }));
  EXPECT_FALSE(GetBoolValue({ false, false, true, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, true, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, true, true }));
  EXPECT_TRUE(GetBoolValue({ true, false, false, false }));
  EXPECT_TRUE(GetBoolValue({ true, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, true }));
  EXPECT_TRUE(GetBoolValue({ true, true, false, false }));
  EXPECT_TRUE(GetBoolValue({ true, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, true }));
}

TEST_F(CompileExpressionTest, ChecksIntArithmetic) {
  const BinaryOperation expr(
      BinaryOperator::MULTIPLY,
      BinaryOperation::Create(BinaryOperator::MINUS,
                              Identifier::Create("i"), IntLiteral::Create(17)),
      BinaryOperation::Create(BinaryOperator::PLUS,
                              IntLiteral::Create(9), IntLiteral::Create(8)));
  const std::map<std::string, VariableInfo> variables =
      { { "i", { Type::INT, 16, 0, 1 } } };
  EXPECT_TRUE(CompileExpression<int>(
      expr, {}, {}, variables, dd_manager_, &add_, nullptr));
  EXPECT_EQ(-17, GetIntValue({ false, false, false, false }));
  EXPECT_EQ(-17, GetIntValue({ false, false, false, true }));
  EXPECT_EQ(0, GetIntValue({ false, false, true, false }));
  EXPECT_EQ(0, GetIntValue({ false, false, true, true }));
  EXPECT_EQ(-17, GetIntValue({ false, true, false, false }));
  EXPECT_EQ(-17, GetIntValue({ false, true, false, true }));
  EXPECT_EQ(0, GetIntValue({ false, true, true, false }));
  EXPECT_EQ(0, GetIntValue({ false, true, true, true }));
  EXPECT_EQ(17, GetIntValue({ true, false, false, false }));
  EXPECT_EQ(17, GetIntValue({ true, false, false, true }));
  EXPECT_EQ(34, GetIntValue({ true, false, true, false }));
  EXPECT_EQ(34, GetIntValue({ true, false, true, true }));
  EXPECT_EQ(17, GetIntValue({ true, true, false, false }));
  EXPECT_EQ(17, GetIntValue({ true, true, false, true }));
  EXPECT_EQ(34, GetIntValue({ true, true, true, false }));
  EXPECT_EQ(34, GetIntValue({ true, true, true, true }));
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, {}, variables, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(-17.0, GetIntValue({ false, false, false, false }));
  EXPECT_DOUBLE_EQ(-17.0, GetIntValue({ false, false, false, true }));
  EXPECT_DOUBLE_EQ(0.0, GetIntValue({ false, false, true, false }));
  EXPECT_DOUBLE_EQ(0.0, GetIntValue({ false, false, true, true }));
  EXPECT_DOUBLE_EQ(-17.0, GetIntValue({ false, true, false, false }));
  EXPECT_DOUBLE_EQ(-17.0, GetIntValue({ false, true, false, true }));
  EXPECT_DOUBLE_EQ(0.0, GetIntValue({ false, true, true, false }));
  EXPECT_DOUBLE_EQ(0.0, GetIntValue({ false, true, true, true }));
  EXPECT_DOUBLE_EQ(17.0, GetIntValue({ true, false, false, false }));
  EXPECT_DOUBLE_EQ(17.0, GetIntValue({ true, false, false, true }));
  EXPECT_DOUBLE_EQ(34.0, GetIntValue({ true, false, true, false }));
  EXPECT_DOUBLE_EQ(34.0, GetIntValue({ true, false, true, true }));
  EXPECT_DOUBLE_EQ(17.0, GetIntValue({ true, true, false, false }));
  EXPECT_DOUBLE_EQ(17.0, GetIntValue({ true, true, false, true }));
  EXPECT_DOUBLE_EQ(34.0, GetIntValue({ true, true, true, false }));
  EXPECT_DOUBLE_EQ(34.0, GetIntValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, variables, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesIntArithmetic) {
  const BinaryOperation expr(
      BinaryOperator::MULTIPLY,
      BinaryOperation::Create(BinaryOperator::MINUS,
                              Identifier::Create("i"), IntLiteral::Create(17)),
      BinaryOperation::Create(BinaryOperator::PLUS,
                              IntLiteral::Create(9), IntLiteral::Create(8)));
  const std::map<std::string, VariableInfo> variables =
      { { "i", { Type::INT, 16, 0, 1 } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, variables, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(-17, GetIntValue({ false, false, false, false }));
  EXPECT_EQ(-17, GetIntValue({ false, false, false, true }));
  EXPECT_EQ(0, GetIntValue({ false, false, true, false }));
  EXPECT_EQ(0, GetIntValue({ false, false, true, true }));
  EXPECT_EQ(-17, GetIntValue({ false, true, false, false }));
  EXPECT_EQ(-17, GetIntValue({ false, true, false, true }));
  EXPECT_EQ(0, GetIntValue({ false, true, true, false }));
  EXPECT_EQ(0, GetIntValue({ false, true, true, true }));
  EXPECT_EQ(17, GetIntValue({ true, false, false, false }));
  EXPECT_EQ(17, GetIntValue({ true, false, false, true }));
  EXPECT_EQ(34, GetIntValue({ true, false, true, false }));
  EXPECT_EQ(34, GetIntValue({ true, false, true, true }));
  EXPECT_EQ(17, GetIntValue({ true, true, false, false }));
  EXPECT_EQ(17, GetIntValue({ true, true, false, true }));
  EXPECT_EQ(34, GetIntValue({ true, true, true, false }));
  EXPECT_EQ(34, GetIntValue({ true, true, true, true }));
}

TEST_F(CompileExpressionTest, ChecksMixedTypeArithmetic) {
  const BinaryOperation expr(
      BinaryOperator::MULTIPLY,
      BinaryOperation::Create(BinaryOperator::MINUS,
                              IntLiteral::Create(1),
                              DoubleLiteral::Create(0.5)),
      BinaryOperation::Create(BinaryOperator::PLUS,
                              IntLiteral::Create(9), IntLiteral::Create(8)));
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(8.5, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesMixedTypeArithmetic) {
  const BinaryOperation expr(
      BinaryOperator::MULTIPLY,
      BinaryOperation::Create(BinaryOperator::MINUS,
                              IntLiteral::Create(1),
                              DoubleLiteral::Create(0.5)),
      BinaryOperation::Create(BinaryOperator::PLUS,
                              IntLiteral::Create(9), IntLiteral::Create(8)));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(8.5, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBoolArithmetic) {
  const BinaryOperation expr1(BinaryOperator::PLUS,
                               BoolLiteral::Create(true),
                               IntLiteral::Create(1));
  const BinaryOperation expr2(BinaryOperator::PLUS,
                              IntLiteral::Create(1),
                              BoolLiteral::Create(false));
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator + applied to bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator + applied to bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator + applied to bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator + applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator + applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator + applied to bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolArithmetic) {
  const BinaryOperation expr1(BinaryOperator::PLUS,
                               BoolLiteral::Create(true),
                               IntLiteral::Create(1));
  const BinaryOperation expr2(BinaryOperator::PLUS,
                              IntLiteral::Create(1),
                              BoolLiteral::Create(false));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator + applied to bool", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator + applied to bool", error_);
}

TEST_F(CompileExpressionTest, ChecksNumericDivision) {
  const BinaryOperation expr1(BinaryOperator::DIVIDE,
                              IntLiteral::Create(22), IntLiteral::Create(7));
  const BinaryOperation expr2(BinaryOperator::DIVIDE,
                              IntLiteral::Create(22),
                              DoubleLiteral::Create(-0.5));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_TRUE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(22.0 / 7.0, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(-44.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesNumericDivision) {
  const BinaryOperation expr1(BinaryOperator::DIVIDE,
                              IntLiteral::Create(22), IntLiteral::Create(7));
  const BinaryOperation expr2(BinaryOperator::DIVIDE,
                              IntLiteral::Create(22),
                              DoubleLiteral::Create(-0.5));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(22.0 / 7.0, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(-44.0, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBoolDivision) {
  const BinaryOperation expr1(BinaryOperator::DIVIDE,
                              BoolLiteral::Create(true), IntLiteral::Create(7));
  const BinaryOperation expr2(BinaryOperator::DIVIDE,
                              DoubleLiteral::Create(0.5),
                              BoolLiteral::Create(false));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator / applied to bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator / applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator / applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator / applied to bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator / applied to bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator / applied to bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolDivision) {
  const BinaryOperation expr1(BinaryOperator::DIVIDE,
                              BoolLiteral::Create(true), IntLiteral::Create(7));
  const BinaryOperation expr2(BinaryOperator::DIVIDE,
                              DoubleLiteral::Create(0.5),
                              BoolLiteral::Create(false));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator / applied to bool", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator / applied to bool", error_);
}

TEST_F(CompileExpressionTest, ChecksNumericComparison) {
  const BinaryOperation expr1(BinaryOperator::LESS,
                              IntLiteral::Create(17), IntLiteral::Create(42));
  const BinaryOperation expr2(BinaryOperator::LESS_EQUAL,
                              IntLiteral::Create(1),
                              DoubleLiteral::Create(0.5));
  const BinaryOperation expr3(BinaryOperator::GREATER_EQUAL,
                              DoubleLiteral::Create(17.0),
                              IntLiteral::Create(17));
  const BinaryOperation expr4(BinaryOperator::GREATER,
                              DoubleLiteral::Create(3.14159),
                              DoubleLiteral::Create(17.0));
  const BinaryOperation expr5(BinaryOperator::EQUAL,
                              IntLiteral::Create(17), IntLiteral::Create(42));
  const BinaryOperation expr6(BinaryOperator::NOT_EQUAL,
                              IntLiteral::Create(17), IntLiteral::Create(42));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr5, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr6, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr5, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr6, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_TRUE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr4, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr5, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr6, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
}

TEST_F(CompileExpressionTest, SynthesizesNumericComparison) {
  const BinaryOperation expr1(BinaryOperator::LESS,
                              IntLiteral::Create(17), IntLiteral::Create(42));
  const BinaryOperation expr2(BinaryOperator::LESS_EQUAL,
                              IntLiteral::Create(1),
                              DoubleLiteral::Create(0.5));
  const BinaryOperation expr3(BinaryOperator::GREATER_EQUAL,
                              DoubleLiteral::Create(17.0),
                              IntLiteral::Create(17));
  const BinaryOperation expr4(BinaryOperator::GREATER,
                              DoubleLiteral::Create(3.14159),
                              DoubleLiteral::Create(17.0));
  const BinaryOperation expr5(BinaryOperator::EQUAL,
                              IntLiteral::Create(17), IntLiteral::Create(42));
  const BinaryOperation expr6(BinaryOperator::NOT_EQUAL,
                              IntLiteral::Create(17), IntLiteral::Create(42));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr4, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr5, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr6, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
}

TEST_F(CompileExpressionTest, ChecksTypeMismatchComparison) {
  const BinaryOperation expr1(BinaryOperator::LESS,
                              IntLiteral::Create(1), BoolLiteral::Create(true));
  const BinaryOperation expr2(BinaryOperator::LESS_EQUAL,
                              BoolLiteral::Create(true),
                              DoubleLiteral::Create(0.5));
  const BinaryOperation expr3(BinaryOperator::GREATER_EQUAL,
                              BoolLiteral::Create(false),
                              IntLiteral::Create(17));
  const BinaryOperation expr4(BinaryOperator::GREATER,
                              DoubleLiteral::Create(3.14159),
                              BoolLiteral::Create(false));
  const BinaryOperation expr5(BinaryOperator::EQUAL,
                              BoolLiteral::Create(true), IntLiteral::Create(1));
  const BinaryOperation expr6(BinaryOperator::NOT_EQUAL,
                              BoolLiteral::Create(true), IntLiteral::Create(1));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator <; int and bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator <=; bool and double",
            error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator >=; bool and int", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator >; double and bool",
            error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr5, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator =; bool and int", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr6, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator !=; bool and int", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator <; int and bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator <=; bool and double",
            error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator >=; bool and int", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator >; double and bool",
            error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr5, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator =; bool and int", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr6, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("incompatible types for binary operator !=; bool and int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator <; int and bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator <=; bool and double",
            error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator >=; bool and int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr4, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator >; double and bool",
            error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr5, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator =; bool and int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr6, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator !=; bool and int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesTypeMismatchComparison) {
  const BinaryOperation expr1(BinaryOperator::LESS,
                              IntLiteral::Create(1), BoolLiteral::Create(true));
  const BinaryOperation expr2(BinaryOperator::LESS_EQUAL,
                              BoolLiteral::Create(true),
                              DoubleLiteral::Create(0.5));
  const BinaryOperation expr3(BinaryOperator::GREATER_EQUAL,
                              BoolLiteral::Create(false),
                              IntLiteral::Create(17));
  const BinaryOperation expr4(BinaryOperator::GREATER,
                              DoubleLiteral::Create(3.14159),
                              BoolLiteral::Create(false));
  const BinaryOperation expr5(BinaryOperator::EQUAL,
                              BoolLiteral::Create(true), IntLiteral::Create(1));
  const BinaryOperation expr6(BinaryOperator::NOT_EQUAL,
                              BoolLiteral::Create(true), IntLiteral::Create(1));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator <; int and bool", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator <=; bool and double",
            error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator >=; bool and int", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr4, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator >; double and bool",
            error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr5, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("incompatible types for binary operator =; bool and int", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr6, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
}

TEST_F(CompileExpressionTest, ChecksBoolComparison) {
  const BinaryOperation expr1(BinaryOperator::LESS,
                              BoolLiteral::Create(false),
                              BoolLiteral::Create(true));
  const BinaryOperation expr2(BinaryOperator::LESS_EQUAL,
                              BoolLiteral::Create(true),
                              BoolLiteral::Create(false));
  const BinaryOperation expr3(BinaryOperator::GREATER_EQUAL,
                              BoolLiteral::Create(true),
                              BoolLiteral::Create(true));
  const BinaryOperation expr4(BinaryOperator::GREATER,
                              BoolLiteral::Create(false),
                              BoolLiteral::Create(true));
  const BinaryOperation expr5(BinaryOperator::EQUAL,
                              BoolLiteral::Create(false),
                              BoolLiteral::Create(true));
  const BinaryOperation expr6(BinaryOperator::NOT_EQUAL,
                              BoolLiteral::Create(false),
                              BoolLiteral::Create(true));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr5, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr6, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr5, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr6, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
  EXPECT_TRUE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr4, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr5, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpression<bool>(
      expr6, {}, {}, {}, dd_manager_, &bdd_, nullptr));
  EXPECT_TRUE(bdd_.Value());
}

TEST_F(CompileExpressionTest, SynthesizesBoolComparison) {
  const BinaryOperation expr1(BinaryOperator::LESS,
                              BoolLiteral::Create(false),
                              BoolLiteral::Create(true));
  const BinaryOperation expr2(BinaryOperator::LESS_EQUAL,
                              BoolLiteral::Create(true),
                              BoolLiteral::Create(false));
  const BinaryOperation expr3(BinaryOperator::GREATER_EQUAL,
                              BoolLiteral::Create(true),
                              BoolLiteral::Create(true));
  const BinaryOperation expr4(BinaryOperator::GREATER,
                              BoolLiteral::Create(false),
                              BoolLiteral::Create(true));
  const BinaryOperation expr5(BinaryOperator::EQUAL,
                              BoolLiteral::Create(false),
                              BoolLiteral::Create(true));
  const BinaryOperation expr6(BinaryOperator::NOT_EQUAL,
                              BoolLiteral::Create(false),
                              BoolLiteral::Create(true));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr4, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr5, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_FALSE(bdd_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr6, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_TRUE(bdd_.Value());
}

TEST_F(CompileExpressionTest, ChecksMixedTypeProposition) {
  const BinaryOperation expr1(BinaryOperator::AND,
                              IntLiteral::Create(1), BoolLiteral::Create(true));
  const BinaryOperation expr2(BinaryOperator::OR,
                              BoolLiteral::Create(false),
                              DoubleLiteral::Create(0.5));
  const BinaryOperation expr3(BinaryOperator::IMPLY,
                              DoubleLiteral::Create(0.5),
                              IntLiteral::Create(1));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator & applied to int", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator | applied to double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator => applied to double", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator & applied to int", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator | applied to double", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; binary operator => applied to double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator & applied to int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator | applied to double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator => applied to double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesMixedTypeProposition) {
  const BinaryOperation expr1(BinaryOperator::AND,
                              IntLiteral::Create(1), BoolLiteral::Create(true));
  const BinaryOperation expr2(BinaryOperator::OR,
                              BoolLiteral::Create(false),
                              DoubleLiteral::Create(0.5));
  const BinaryOperation expr3(BinaryOperator::IMPLY,
                              DoubleLiteral::Create(0.5),
                              IntLiteral::Create(1));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator & applied to int", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator | applied to double", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; binary operator => applied to double", error_);
}

TEST_F(CompileExpressionTest, ChecksBoolProposition) {
  const BinaryOperation expr(
      BinaryOperator::AND,
      BinaryOperation::Create(BinaryOperator::OR,
                              Identifier::Create("b1"),
                              BoolLiteral::Create(false)),
      BinaryOperation::Create(BinaryOperator::IMPLY,
                              BoolLiteral::Create(true),
                              Identifier::Create("b2")));
  const std::map<std::string, VariableInfo> variables =
      { { "b1", { Type::BOOL, 0, 0, 0 } }, { "b2", { Type::BOOL, 0, 1, 1 } } };
  EXPECT_TRUE(CompileExpression<bool>(
      expr, {}, {}, variables, dd_manager_, &bdd_, nullptr));
  EXPECT_FALSE(GetBoolValue({ false, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, false, true }));
  EXPECT_FALSE(GetBoolValue({ false, false, true, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, true, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, true }));
  EXPECT_FALSE(CompileExpression<int>(
      expr, {}, {}, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr, {}, {}, variables, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting double, found bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBoolProposition) {
  const BinaryOperation expr(
      BinaryOperator::AND,
      BinaryOperation::Create(BinaryOperator::OR,
                              Identifier::Create("b1"),
                              BoolLiteral::Create(false)),
      BinaryOperation::Create(BinaryOperator::IMPLY,
                              BoolLiteral::Create(true),
                              Identifier::Create("b2")));
  const std::map<std::string, VariableInfo> variables =
      { { "b1", { Type::BOOL, 0, 0, 0 } }, { "b2", { Type::BOOL, 0, 1, 1 } } };
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, variables, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::BOOL, type);
  EXPECT_FALSE(GetBoolValue({ false, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, false, true }));
  EXPECT_FALSE(GetBoolValue({ false, false, true, false }));
  EXPECT_FALSE(GetBoolValue({ false, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, false, true }));
  EXPECT_FALSE(GetBoolValue({ false, true, true, false }));
  EXPECT_FALSE(GetBoolValue({ false, true, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, false, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, false, true, true }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, false }));
  EXPECT_FALSE(GetBoolValue({ true, true, false, true }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, false }));
  EXPECT_TRUE(GetBoolValue({ true, true, true, true }));
}

TEST_F(CompileExpressionTest, ChecksGoodMinAndMax) {
  const FunctionCall expr1(Function::MIN, ArgumentList(IntLiteral::Create(17)));
  const FunctionCall expr2(Function::MAX,
                           ArgumentList(DoubleLiteral::Create(3.14159),
                                        IntLiteral::Create(17)));
  const FunctionCall expr3(Function::MIN,
                           ArgumentList(IntLiteral::Create(-17),
                                        DoubleLiteral::Create(3.14159)));
  const FunctionCall expr4(Function::MAX,
                           ArgumentList(DoubleLiteral::Create(0.5),
                                        DoubleLiteral::Create(3.14159),
                                        DoubleLiteral::Create(-42.0)));
  EXPECT_TRUE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(17, add_.Value());
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_TRUE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(-17.0, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr4, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(3.14159, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr4, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesGoodMinAndMax) {
  const FunctionCall expr1(Function::MIN, ArgumentList(IntLiteral::Create(17)));
  const FunctionCall expr2(Function::MAX,
                           ArgumentList(DoubleLiteral::Create(3.14159),
                                        IntLiteral::Create(17)));
  const FunctionCall expr3(Function::MIN,
                           ArgumentList(IntLiteral::Create(-17),
                                        DoubleLiteral::Create(3.14159)));
  const FunctionCall expr4(Function::MAX,
                           ArgumentList(DoubleLiteral::Create(0.5),
                                        DoubleLiteral::Create(3.14159),
                                        DoubleLiteral::Create(-42.0)));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(17, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(-17.0, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr4, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(3.14159, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBadMinAndMax) {
  const FunctionCall expr1(Function::MIN, ArgumentList());
  const FunctionCall expr2(Function::MAX,
                           ArgumentList(IntLiteral::Create(0),
                                        BoolLiteral::Create(true)));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("min() requires at least one argument", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; max() applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("min() requires at least one argument", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; max() applied to bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("min() requires at least one argument", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; max() applied to bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBadMinAndMax) {
  const FunctionCall expr1(Function::MIN, ArgumentList());
  const FunctionCall expr2(Function::MAX,
                           ArgumentList(IntLiteral::Create(0),
                                        BoolLiteral::Create(true)));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("min() requires at least one argument", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; max() applied to bool", error_);
}

TEST_F(CompileExpressionTest, ChecksGoodFloorAndCeil) {
  const FunctionCall expr1(Function::FLOOR,
                           ArgumentList(IntLiteral::Create(17)));
  const FunctionCall expr2(Function::FLOOR,
                           ArgumentList(DoubleLiteral::Create(3.14159)));
  const FunctionCall expr3(Function::CEIL,
                           ArgumentList(IntLiteral::Create(17)));
  const FunctionCall expr4(Function::CEIL,
                           ArgumentList(DoubleLiteral::Create(3.14159)));
  EXPECT_TRUE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(17, add_.Value());
  EXPECT_TRUE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(3, add_.Value());
  EXPECT_TRUE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(17, add_.Value());
  EXPECT_TRUE(CompileExpression<int>(
      expr4, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(4, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(3.0, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(17.0, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr4, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(4.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr4, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesGoodFloorAndCeil) {
  const FunctionCall expr1(Function::FLOOR,
                           ArgumentList(IntLiteral::Create(17)));
  const FunctionCall expr2(Function::FLOOR,
                           ArgumentList(DoubleLiteral::Create(3.14159)));
  const FunctionCall expr3(Function::CEIL,
                           ArgumentList(IntLiteral::Create(17)));
  const FunctionCall expr4(Function::CEIL,
                           ArgumentList(DoubleLiteral::Create(3.14159)));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(17, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(3, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(17, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr4, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(4, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBadFloorAndCeil) {
  const FunctionCall expr1(Function::FLOOR, ArgumentList());
  const FunctionCall expr2(Function::CEIL,
                           ArgumentList(DoubleLiteral::Create(0.5),
                                        DoubleLiteral::Create(3.14159)));
  const FunctionCall expr3(Function::FLOOR,
                           ArgumentList(BoolLiteral::Create(true)));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("floor() requires exactly one argument", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("ceil() requires exactly one argument", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; floor() applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("floor() requires exactly one argument", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("ceil() requires exactly one argument", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; floor() applied to bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("floor() requires exactly one argument", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("ceil() requires exactly one argument", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; floor() applied to bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBadFloorAndCeil) {
  const FunctionCall expr1(Function::FLOOR, ArgumentList());
  const FunctionCall expr2(Function::CEIL,
                           ArgumentList(DoubleLiteral::Create(0.5),
                                        DoubleLiteral::Create(3.14159)));
  const FunctionCall expr3(Function::FLOOR,
                           ArgumentList(BoolLiteral::Create(true)));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("floor() requires exactly one argument", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("ceil() requires exactly one argument", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; floor() applied to bool", error_);
}

TEST_F(CompileExpressionTest, ChecksGoodPowAndLog) {
  const FunctionCall expr1(Function::POW,
                           ArgumentList(DoubleLiteral::Create(0.5),
                                        DoubleLiteral::Create(2.0)));
  const FunctionCall expr2(Function::LOG,
                           ArgumentList(DoubleLiteral::Create(1000.0),
                                        IntLiteral::Create(10)));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; expecting int, found double", error_);
  EXPECT_TRUE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(0.25, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(3.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found double", error_);
}

TEST_F(CompileExpressionTest, SynthesizesGoodPowAndLog) {
  const FunctionCall expr1(Function::POW,
                           ArgumentList(DoubleLiteral::Create(0.5),
                                        DoubleLiteral::Create(2.0)));
  const FunctionCall expr2(Function::LOG,
                           ArgumentList(DoubleLiteral::Create(1000.0),
                                        IntLiteral::Create(10)));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(0.25, add_.Value());
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::DOUBLE, type);
  EXPECT_DOUBLE_EQ(3.0, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBadPowAndLog) {
  const FunctionCall expr1(Function::POW, ArgumentList());
  const FunctionCall expr2(Function::LOG,
                           ArgumentList(DoubleLiteral::Create(0.5),
                                        IntLiteral::Create(17),
                                        DoubleLiteral::Create(3.14159)));
  const FunctionCall expr3(Function::POW,
                           ArgumentList(IntLiteral::Create(1),
                                        BoolLiteral::Create(false)));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("pow() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("log() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; pow() applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("pow() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("log() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; pow() applied to bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("pow() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("log() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; pow() applied to bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBadPowAndLog) {
  const FunctionCall expr1(Function::POW, ArgumentList());
  const FunctionCall expr2(Function::LOG,
                           ArgumentList(DoubleLiteral::Create(0.5),
                                        IntLiteral::Create(17),
                                        DoubleLiteral::Create(3.14159)));
  const FunctionCall expr3(Function::POW,
                           ArgumentList(IntLiteral::Create(1),
                                        BoolLiteral::Create(false)));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("pow() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("log() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; pow() applied to bool", error_);
}

TEST_F(CompileExpressionTest, ChecksGoodMod) {
  const FunctionCall expr(Function::MOD,
                          ArgumentList(IntLiteral::Create(7),
                                       IntLiteral::Create(3)));
  EXPECT_TRUE(CompileExpression<int>(
      expr, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_EQ(1, add_.Value());
  EXPECT_TRUE(CompileExpression<double>(
      expr, {}, {}, {}, dd_manager_, &add_, nullptr));
  EXPECT_DOUBLE_EQ(1.0, add_.Value());
  EXPECT_FALSE(CompileExpression<bool>(
      expr, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; expecting bool, found int", error_);
}

TEST_F(CompileExpressionTest, SynthesizesGoodMod) {
  const FunctionCall expr(Function::MOD,
                          ArgumentList(IntLiteral::Create(7),
                                       IntLiteral::Create(3)));
  Type type;
  EXPECT_TRUE(CompileExpressionAndSynthesizeType(
      expr, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, nullptr));
  EXPECT_EQ(Type::INT, type);
  EXPECT_EQ(1, add_.Value());
}

TEST_F(CompileExpressionTest, ChecksBadMod) {
  const FunctionCall expr1(Function::MOD, ArgumentList());
  const FunctionCall expr2(Function::MOD,
                           ArgumentList(IntLiteral::Create(1),
                                        IntLiteral::Create(2),
                                        IntLiteral::Create(3)));
  const FunctionCall expr3(Function::MOD,
                           ArgumentList(IntLiteral::Create(1),
                                        DoubleLiteral::Create(3.14159)));
  const FunctionCall expr4(Function::MOD,
                           ArgumentList(BoolLiteral::Create(false),
                                        IntLiteral::Create(1)));
  EXPECT_FALSE(CompileExpression<int>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("mod() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("mod() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; mod() applied to double", error_);
  EXPECT_FALSE(CompileExpression<int>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; mod() applied to bool", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr1, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("mod() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr2, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("mod() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr3, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; mod() applied to double", error_);
  EXPECT_FALSE(CompileExpression<double>(
      expr4, {}, {}, {}, dd_manager_, &add_, &error_));
  EXPECT_EQ("type mismatch; mod() applied to bool", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr1, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("mod() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr2, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("mod() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr3, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; mod() applied to double", error_);
  EXPECT_FALSE(CompileExpression<bool>(
      expr4, {}, {}, {}, dd_manager_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; mod() applied to bool", error_);
}

TEST_F(CompileExpressionTest, SynthesizesBadMod) {
  const FunctionCall expr1(Function::MOD, ArgumentList());
  const FunctionCall expr2(Function::MOD,
                           ArgumentList(IntLiteral::Create(1),
                                        IntLiteral::Create(2),
                                        IntLiteral::Create(3)));
  const FunctionCall expr3(Function::MOD,
                           ArgumentList(IntLiteral::Create(1),
                                        DoubleLiteral::Create(3.14159)));
  const FunctionCall expr4(Function::MOD,
                           ArgumentList(BoolLiteral::Create(false),
                                        IntLiteral::Create(1)));
  Type type;
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr1, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("mod() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr2, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("mod() requires exactly two arguments", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr3, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; mod() applied to double", error_);
  EXPECT_FALSE(CompileExpressionAndSynthesizeType(
      expr4, {}, {}, {}, dd_manager_, &type, &add_, &bdd_, &error_));
  EXPECT_EQ("type mismatch; mod() applied to bool", error_);
}

}  // namespace
