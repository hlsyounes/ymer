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

#include <cmath>

#include "gtest/gtest.h"

namespace {

TEST(GetNumRegistersTest, Constant) {
  const CompiledExpression expr1({ Operation::MakeICONST(17, 3) });
  EXPECT_EQ(std::make_pair(4, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({ Operation::MakeDCONST(0.5, 2) });
  EXPECT_EQ(std::make_pair(0, 3), GetNumRegisters(expr2));
}

TEST(GetNumRegistersTest, Variable) {
  const CompiledExpression expr({ Operation::MakeILOAD(0, 5) });
  EXPECT_EQ(std::make_pair(6, 0), GetNumRegisters(expr));
}

TEST(GetNumRegistersTest, Conversion) {
  const CompiledExpression expr({ Operation::MakeI2D(0) });
  EXPECT_EQ(std::make_pair(1, 1), GetNumRegisters(expr));
}

TEST(GetNumRegistersTest, Negation) {
  const CompiledExpression expr1({ Operation::MakeINEG(1) });
  EXPECT_EQ(std::make_pair(2, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({ Operation::MakeDNEG(1) });
  EXPECT_EQ(std::make_pair(0, 2), GetNumRegisters(expr2));
}

TEST(GetNumRegistersTest, Not) {
  const CompiledExpression expr({ Operation::MakeNOT(0) });
  EXPECT_EQ(std::make_pair(1, 0), GetNumRegisters(expr));
}

TEST(GetNumRegistersTest, Computation) {
  const CompiledExpression expr1({ Operation::MakeIADD(5, 7) });
  EXPECT_EQ(std::make_pair(8, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({ Operation::MakeDMUL(0, 1) });
  EXPECT_EQ(std::make_pair(0, 2), GetNumRegisters(expr2));
}

TEST(GetNumRegistersTest, Comparison) {
  const CompiledExpression expr1({ Operation::MakeIEQ(0, 1) });
  EXPECT_EQ(std::make_pair(2, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({ Operation::MakeDGT(1, 4) });
  EXPECT_EQ(std::make_pair(2, 5), GetNumRegisters(expr2));
}

TEST(GetNumRegistersTest, Jump) {
  const CompiledExpression expr1({ Operation::MakeIFFALSE(17, 42) });
  EXPECT_EQ(std::make_pair(18, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({ Operation::MakeIFTRUE(42, 17) });
  EXPECT_EQ(std::make_pair(43, 0), GetNumRegisters(expr2));
  const CompiledExpression expr3({ Operation::MakeGOTO(17) });
  EXPECT_EQ(std::make_pair(0, 0), GetNumRegisters(expr3));
}

TEST(GetNumRegistersTest, Function) {
  const CompiledExpression expr1({ Operation::MakeIMAX(3, 2) });
  EXPECT_EQ(std::make_pair(4, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({ Operation::MakeDMIN(3, 2) });
  EXPECT_EQ(std::make_pair(0, 4), GetNumRegisters(expr2));
  const CompiledExpression expr3({ Operation::MakeFLOOR(2) });
  EXPECT_EQ(std::make_pair(3, 3), GetNumRegisters(expr3));
  const CompiledExpression expr4({ Operation::MakeCEIL(1) });
  EXPECT_EQ(std::make_pair(2, 2), GetNumRegisters(expr4));
  const CompiledExpression expr5({ Operation::MakePOW(3, 2) });
  EXPECT_EQ(std::make_pair(0, 4), GetNumRegisters(expr5));
  const CompiledExpression expr6({ Operation::MakeLOG(3, 2) });
  EXPECT_EQ(std::make_pair(0, 4), GetNumRegisters(expr6));
  const CompiledExpression expr7({ Operation::MakeMOD(3, 2) });
  EXPECT_EQ(std::make_pair(4, 0), GetNumRegisters(expr7));
}

TEST(GetNumRegistersTest, Program) {
  const CompiledExpression expr({
      Operation::MakeICONST(17, 3),
      Operation::MakeILOAD(0, 5),
      Operation::MakeMOD(3, 5),
      Operation::MakeI2D(3),
      Operation::MakeDCONST(0.5, 2),
      Operation::MakePOW(3, 2) });
  EXPECT_EQ(std::make_pair(6, 4), GetNumRegisters(expr));
}

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

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMin) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIMIN(0, 1) });
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeIMIN(0, 1) });
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMin) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeDMIN(0, 1) });
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDMIN(0, 1) });
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMax) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeIMAX(0, 1) });
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeIMAX(0, 1) });
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMax) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.25, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeDMAX(0, 1) });
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(0.25, 1),
        Operation::MakeDMAX(0, 1) });
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesFloor) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeFLOOR(0) });
  EXPECT_EQ(0, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(-0.5, 0),
        Operation::MakeFLOOR(0) });
  EXPECT_EQ(-1, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesCeil) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeCEIL(0) });
  EXPECT_EQ(1, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(-0.5, 0),
        Operation::MakeCEIL(0) });
  EXPECT_EQ(0, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesPow) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(4.0, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakePOW(0, 1) });
  EXPECT_EQ(pow(4.0, 0.5), evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(4.0, 1),
        Operation::MakePOW(0, 1) });
  EXPECT_EQ(pow(0.5, 4.0), evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesLog) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1(
      { Operation::MakeDCONST(4.0, 0),
        Operation::MakeDCONST(0.5, 1),
        Operation::MakeLOG(0, 1) });
  EXPECT_EQ(log(4.0) / log(0.5), evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeDCONST(0.5, 0),
        Operation::MakeDCONST(4.0, 1),
        Operation::MakeLOG(0, 1) });
  EXPECT_EQ(log(0.5) / log(4.0), evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesMod) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      { Operation::MakeICONST(17, 0),
        Operation::MakeICONST(42, 1),
        Operation::MakeMOD(0, 1) });
  EXPECT_EQ(17 % 42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      { Operation::MakeICONST(42, 0),
        Operation::MakeICONST(17, 1),
        Operation::MakeMOD(0, 1) });
  EXPECT_EQ(42 % 17, evaluator.EvaluateIntExpression(expr2, {}));
}

}  // namespace
