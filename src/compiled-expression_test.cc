// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2012 Google Inc
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

#include "compiled-expression.h"

#include "gtest/gtest.h"

namespace {

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerConstant) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr({ Operation::MakeICONST(17, 0) });
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleConstant) {
  CompiledExpressionEvaluator evaluator(0, 1);
  const CompiledExpression expr({ Operation::MakeDCONST(0.5, 0) });
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerVariable) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1({ Operation::MakeILOAD(0, 0) });
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr1, { 17, 42 }));
  const CompiledExpression expr2({ Operation::MakeILOAD(1, 0) });
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, { 17, 42 }));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesConversionToDouble) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr(
      { Operation::MakeICONST(17, 0),
        Operation::MakeI2D(0) });
  EXPECT_EQ(17.0, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerNegation) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeINEG(0) });
  EXPECT_EQ(-17, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(-42, 0),
        Operation::MakeINEG(0) });
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleNegation) {
  CompiledExpressionEvaluator evaluator(0, 1);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDNEG(0) });
  EXPECT_EQ(-0.5, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(-0.25, 0),
        Operation::MakeDNEG(0) });
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesLogicalNot) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(true, 0),
        Operation::MakeNOT(0) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(false, 0),
        Operation::MakeNOT(0) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerAddition) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIADD(0, 1) });
  EXPECT_EQ(17 + 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleAddition) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDADD(0, 1) });
  EXPECT_EQ(0.5 + 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerSubtraction) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeISUB(0, 1) });
  EXPECT_EQ(17 - 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleSubtraction) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDSUB(0, 1) });
  EXPECT_EQ(0.5 - 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMultiplication) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIMUL(0, 1) });
  EXPECT_EQ(17 * 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMultiplication) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDMUL(0, 1) });
  EXPECT_EQ(0.5 * 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleDivision) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDDIV(0, 1) });
  EXPECT_EQ(0.5 / 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIEQ(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIEQ(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeIEQ(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeDEQ(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDEQ(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDEQ(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerNotEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeINE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeINE(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeINE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleNotEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeDNE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDNE(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDNE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerLess) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeILT(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeILT(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeILT(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleLess) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeDLT(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDLT(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDLT(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerLessEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeILE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeILE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeILE(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleLessEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeDLE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDLE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDLE(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerGreaterEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIGE(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIGE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeIGE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleGreaterEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeDGE(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDGE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDGE(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerGreater) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIGT(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIGT(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeIGT(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleGreater) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeDGT(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDGT(0, 1) });
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDGT(0, 1) });
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesWithJumps) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(false, 0),
        Operation::MakeIFFALSE(0, 4),
        Operation::MakeICONST(17, 0),
        Operation::MakeGOTO(5),
        Operation::MakeICONST(42, 0) });
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(true, 0),
        Operation::MakeIFFALSE(0, 4),
        Operation::MakeICONST(17, 0),
        Operation::MakeGOTO(5),
        Operation::MakeICONST(42, 0) });
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      { Operation::MakeICONST(false, 0),
        Operation::MakeIFTRUE(0, 4),
        Operation::MakeICONST(17, 0),
        Operation::MakeGOTO(5),
        Operation::MakeICONST(42, 0) });
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr3, {}));
  const CompiledExpression expr4(
      { Operation::MakeICONST(true, 0),
        Operation::MakeIFTRUE(0, 4),
        Operation::MakeICONST(17, 0),
        Operation::MakeGOTO(5),
        Operation::MakeICONST(42, 0) });
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr4, {}));
}

}  // namespace
