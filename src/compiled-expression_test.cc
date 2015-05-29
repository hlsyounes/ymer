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

#include "ptrutil.h"

#include "gtest/gtest.h"

namespace {

FunctionCall MakeFunctionCall(Function function) {
  return FunctionCall(function, UniquePtrVector<const Expression>());
}

template <typename T>
FunctionCall MakeFunctionCall(Function function, T a) {
  return FunctionCall(
      function, UniquePtrVector<const Expression>(MakeUnique<Literal>(a)));
}

template <typename T, typename U>
FunctionCall MakeFunctionCall(Function function, T a, U b) {
  return FunctionCall(
      function, UniquePtrVector<const Expression>(MakeUnique<Literal>(a),
                                                  MakeUnique<Literal>(b)));
}

template <typename T, typename U, typename V>
FunctionCall MakeFunctionCall(Function function, T a, U b, V c) {
  return FunctionCall(
      function, UniquePtrVector<const Expression>(MakeUnique<Literal>(a),
                                                  MakeUnique<Literal>(b),
                                                  MakeUnique<Literal>(c)));
}

template <typename T, typename U>
BinaryOperation MakeBinaryOperation(BinaryOperator op, T a, U b) {
  return BinaryOperation(op, MakeUnique<Literal>(a), MakeUnique<Literal>(b));
}

template <typename T, typename U, typename V>
Conditional MakeConditional(T a, U b, V c) {
  return Conditional(MakeUnique<Literal>(a), MakeUnique<Literal>(b),
                     MakeUnique<Literal>(c));
}

TEST(OperationTest, Shift) {
  EXPECT_EQ(Operation::MakeICONST(17, 42),
            Operation::MakeICONST(17, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeICONST(false, 42),
            Operation::MakeICONST(false, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDCONST(0.5, 42),
            Operation::MakeDCONST(0.5, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeILOAD(4711, 42),
            Operation::MakeILOAD(4711, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeI2D(42), Operation::MakeI2D(40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeINEG(42), Operation::MakeINEG(40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDNEG(42), Operation::MakeDNEG(40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeNOT(42), Operation::MakeNOT(40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeIADD(17, 42),
            Operation::MakeIADD(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDADD(17, 42),
            Operation::MakeDADD(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeISUB(17, 42),
            Operation::MakeISUB(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDSUB(17, 42),
            Operation::MakeDSUB(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeIMUL(17, 42),
            Operation::MakeIMUL(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDMUL(17, 42),
            Operation::MakeDMUL(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDDIV(17, 42),
            Operation::MakeDDIV(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeIEQ(17, 42), Operation::MakeIEQ(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDEQ(17, 42), Operation::MakeDEQ(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeINE(17, 42), Operation::MakeINE(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDNE(17, 42), Operation::MakeDNE(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeILT(17, 42), Operation::MakeILT(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDLT(17, 42), Operation::MakeDLT(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeILE(17, 42), Operation::MakeILE(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDLE(17, 42), Operation::MakeDLE(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeIGE(17, 42), Operation::MakeIGE(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDGE(17, 42), Operation::MakeDGE(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeIGT(17, 42), Operation::MakeIGT(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDGT(17, 42), Operation::MakeDGT(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeIFFALSE(17, 42),
            Operation::MakeIFFALSE(15, 39).Shift(3, 2));
  EXPECT_EQ(Operation::MakeIFTRUE(17, 42),
            Operation::MakeIFTRUE(15, 39).Shift(3, 2));
  EXPECT_EQ(Operation::MakeGOTO(17), Operation::MakeGOTO(14).Shift(3, 2));
  EXPECT_EQ(Operation::MakeNOP(), Operation::MakeNOP().Shift(3, 2));
  EXPECT_EQ(Operation::MakeIMIN(17, 42),
            Operation::MakeIMIN(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDMIN(17, 42),
            Operation::MakeDMIN(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeIMAX(17, 42),
            Operation::MakeIMAX(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeDMAX(17, 42),
            Operation::MakeDMAX(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeFLOOR(42), Operation::MakeFLOOR(40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeCEIL(42), Operation::MakeCEIL(40).Shift(3, 2));
  EXPECT_EQ(Operation::MakePOW(17, 42), Operation::MakePOW(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeLOG(17, 42), Operation::MakeLOG(15, 40).Shift(3, 2));
  EXPECT_EQ(Operation::MakeMOD(17, 42), Operation::MakeMOD(15, 40).Shift(3, 2));
}

TEST(CompiledExpressionTest, WithAssignment) {
  const CompiledExpression expr(
      {Operation::MakeILOAD(0, 0), Operation::MakeICONST(4, 1),
       Operation::MakeILT(0, 1), Operation::MakeIFFALSE(0, 18),
       Operation::MakeILOAD(2, 0), Operation::MakeICONST(0, 1),
       Operation::MakeIEQ(0, 1), Operation::MakeIFFALSE(0, 18),
       Operation::MakeILOAD(0, 0), Operation::MakeICONST(1, 1),
       Operation::MakeIADD(0, 1), Operation::MakeILOAD(3, 1),
       Operation::MakeIEQ(0, 1), Operation::MakeIFFALSE(0, 17),
       Operation::MakeILOAD(1, 0), Operation::MakeILOAD(4, 1),
       Operation::MakeIEQ(0, 1), Operation::MakeNOT(0)},
      {});
  const std::vector<Operation> expected = {
      Operation::MakeICONST(1, 0), Operation::MakeICONST(4, 1),
      Operation::MakeILT(0, 1),    Operation::MakeIFFALSE(0, 18),
      Operation::MakeILOAD(2, 0),  Operation::MakeICONST(0, 1),
      Operation::MakeIEQ(0, 1),    Operation::MakeIFFALSE(0, 18),
      Operation::MakeICONST(1, 0), Operation::MakeICONST(1, 1),
      Operation::MakeIADD(0, 1),   Operation::MakeILOAD(3, 1),
      Operation::MakeIEQ(0, 1),    Operation::MakeIFFALSE(0, 17),
      Operation::MakeILOAD(1, 0),  Operation::MakeILOAD(4, 1),
      Operation::MakeIEQ(0, 1),    Operation::MakeNOT(0)};
  EXPECT_EQ(expected,
            expr.WithAssignment(IdentifierInfo::Variable(Type::INT, 0, 0, 1, 1),
                                1, {})
                .operations());
}

TEST(GetExpressionRegisterCountsTest, Constant) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 3)}, {});
  EXPECT_EQ(std::make_pair(4, 0), GetExpressionRegisterCounts(expr1));
  const CompiledExpression expr2({Operation::MakeDCONST(0.5, 2)}, {});
  EXPECT_EQ(std::make_pair(0, 3), GetExpressionRegisterCounts(expr2));
}

TEST(GetExpressionRegisterCountsTest, Variable) {
  const CompiledExpression expr({Operation::MakeILOAD(0, 5)}, {});
  EXPECT_EQ(std::make_pair(6, 0), GetExpressionRegisterCounts(expr));
}

TEST(GetExpressionRegisterCountsTest, Conversion) {
  const CompiledExpression expr({Operation::MakeI2D(0)}, {});
  EXPECT_EQ(std::make_pair(1, 1), GetExpressionRegisterCounts(expr));
}

TEST(GetExpressionRegisterCountsTest, Negation) {
  const CompiledExpression expr1({Operation::MakeINEG(1)}, {});
  EXPECT_EQ(std::make_pair(2, 0), GetExpressionRegisterCounts(expr1));
  const CompiledExpression expr2({Operation::MakeDNEG(1)}, {});
  EXPECT_EQ(std::make_pair(0, 2), GetExpressionRegisterCounts(expr2));
}

TEST(GetExpressionRegisterCountsTest, LogicalNot) {
  const CompiledExpression expr({Operation::MakeNOT(0)}, {});
  EXPECT_EQ(std::make_pair(1, 0), GetExpressionRegisterCounts(expr));
}

TEST(GetExpressionRegisterCountsTest, Computation) {
  const CompiledExpression expr1({Operation::MakeIADD(5, 7)}, {});
  EXPECT_EQ(std::make_pair(8, 0), GetExpressionRegisterCounts(expr1));
  const CompiledExpression expr2({Operation::MakeDMUL(0, 1)}, {});
  EXPECT_EQ(std::make_pair(0, 2), GetExpressionRegisterCounts(expr2));
}

TEST(GetExpressionRegisterCountsTest, Comparison) {
  const CompiledExpression expr1({Operation::MakeIEQ(0, 1)}, {});
  EXPECT_EQ(std::make_pair(2, 0), GetExpressionRegisterCounts(expr1));
  const CompiledExpression expr2({Operation::MakeDGT(1, 4)}, {});
  EXPECT_EQ(std::make_pair(2, 5), GetExpressionRegisterCounts(expr2));
}

TEST(GetExpressionRegisterCountsTest, Jump) {
  const CompiledExpression expr1({Operation::MakeIFFALSE(17, 42)}, {});
  EXPECT_EQ(std::make_pair(18, 0), GetExpressionRegisterCounts(expr1));
  const CompiledExpression expr2({Operation::MakeIFTRUE(42, 17)}, {});
  EXPECT_EQ(std::make_pair(43, 0), GetExpressionRegisterCounts(expr2));
  const CompiledExpression expr3({Operation::MakeGOTO(17)}, {});
  EXPECT_EQ(std::make_pair(0, 0), GetExpressionRegisterCounts(expr3));
}

TEST(GetExpressionRegisterCountsTest, Function) {
  const CompiledExpression expr1({Operation::MakeIMAX(3, 2)}, {});
  EXPECT_EQ(std::make_pair(4, 0), GetExpressionRegisterCounts(expr1));
  const CompiledExpression expr2({Operation::MakeDMIN(3, 2)}, {});
  EXPECT_EQ(std::make_pair(0, 4), GetExpressionRegisterCounts(expr2));
  const CompiledExpression expr3({Operation::MakeFLOOR(2)}, {});
  EXPECT_EQ(std::make_pair(3, 3), GetExpressionRegisterCounts(expr3));
  const CompiledExpression expr4({Operation::MakeCEIL(1)}, {});
  EXPECT_EQ(std::make_pair(2, 2), GetExpressionRegisterCounts(expr4));
  const CompiledExpression expr5({Operation::MakePOW(3, 2)}, {});
  EXPECT_EQ(std::make_pair(0, 4), GetExpressionRegisterCounts(expr5));
  const CompiledExpression expr6({Operation::MakeLOG(3, 2)}, {});
  EXPECT_EQ(std::make_pair(0, 4), GetExpressionRegisterCounts(expr6));
  const CompiledExpression expr7({Operation::MakeMOD(3, 2)}, {});
  EXPECT_EQ(std::make_pair(4, 0), GetExpressionRegisterCounts(expr7));
}

TEST(GetExpressionRegisterCountsTest, Program) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 3), Operation::MakeILOAD(0, 5),
       Operation::MakeMOD(3, 5), Operation::MakeI2D(3),
       Operation::MakeDCONST(0.5, 2), Operation::MakePOW(3, 2)},
      {});
  EXPECT_EQ(std::make_pair(6, 4), GetExpressionRegisterCounts(expr));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerConstant) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr({Operation::MakeICONST(17, 0)}, {});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleConstant) {
  CompiledExpressionEvaluator evaluator(0, 1);
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0)}, {});
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerVariable) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1({Operation::MakeILOAD(0, 0)}, {});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr1, {17, 42}));
  const CompiledExpression expr2({Operation::MakeILOAD(1, 0)}, {});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, {17, 42}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesConversionToDouble) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeI2D(0)}, {});
  EXPECT_EQ(17.0, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerNegation) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeINEG(0)}, {});
  EXPECT_EQ(-17, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(-42, 0), Operation::MakeINEG(0)}, {});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleNegation) {
  CompiledExpressionEvaluator evaluator(0, 1);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDNEG(0)}, {});
  EXPECT_EQ(-0.5, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.25, 0), Operation::MakeDNEG(0)}, {});
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesLogicalNot) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(true, 0), Operation::MakeNOT(0)}, {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(false, 0), Operation::MakeNOT(0)}, {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerAddition) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIADD(0, 1)},
      {});
  EXPECT_EQ(17 + 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleAddition) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDADD(0, 1)},
      {});
  EXPECT_EQ(0.5 + 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerSubtraction) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeISUB(0, 1)},
      {});
  EXPECT_EQ(17 - 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleSubtraction) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDSUB(0, 1)},
      {});
  EXPECT_EQ(0.5 - 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMultiplication) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIMUL(0, 1)},
      {});
  EXPECT_EQ(17 * 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMultiplication) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDMUL(0, 1)},
      {});
  EXPECT_EQ(0.5 * 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleDivision) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDDIV(0, 1)},
      {});
  EXPECT_EQ(0.5 / 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIEQ(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIEQ(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIEQ(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDEQ(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDEQ(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDEQ(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerNotEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeINE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeINE(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeINE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleNotEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDNE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDNE(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDNE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerLess) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeILT(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeILT(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeILT(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleLess) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDLT(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDLT(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDLT(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerLessEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeILE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeILE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeILE(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleLessEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDLE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDLE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDLE(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerGreaterEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIGE(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIGE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIGE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleGreaterEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDGE(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDGE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDGE(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerGreater) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIGT(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIGT(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIGT(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleGreater) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDGT(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDGT(0, 1)},
      {});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDGT(0, 1)},
      {});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesWithJumps) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)},
      {});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)},
      {});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeICONST(false, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)},
      {});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr3, {}));
  const CompiledExpression expr4(
      {Operation::MakeICONST(true, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)},
      {});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr4, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMin) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIMIN(0, 1)},
      {});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIMIN(0, 1)},
      {});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMin) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDMIN(0, 1)},
      {});
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDMIN(0, 1)},
      {});
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMax) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIMAX(0, 1)},
      {});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIMAX(0, 1)},
      {});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMax) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDMAX(0, 1)},
      {});
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDMAX(0, 1)},
      {});
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesFloor) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeFLOOR(0)}, {});
  EXPECT_EQ(0, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.5, 0), Operation::MakeFLOOR(0)}, {});
  EXPECT_EQ(-1, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesCeil) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeCEIL(0)}, {});
  EXPECT_EQ(1, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.5, 0), Operation::MakeCEIL(0)}, {});
  EXPECT_EQ(0, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesPow) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(4.0, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakePOW(0, 1)},
      {});
  EXPECT_EQ(pow(4.0, 0.5), evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(4.0, 1),
       Operation::MakePOW(0, 1)},
      {});
  EXPECT_EQ(pow(0.5, 4.0), evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesLog) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(4.0, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeLOG(0, 1)},
      {});
  EXPECT_EQ(log(4.0) / log(0.5), evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(4.0, 1),
       Operation::MakeLOG(0, 1)},
      {});
  EXPECT_EQ(log(0.5) / log(4.0), evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesMod) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeMOD(0, 1)},
      {});
  EXPECT_EQ(17 % 42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeMOD(0, 1)},
      {});
  EXPECT_EQ(42 % 17, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompileExpressionTest, IntLiteral) {
  const CompileExpressionResult result1 =
      CompileExpression(Literal(17), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0)}),
            result1.expr.operations());
  const CompileExpressionResult result2 =
      CompileExpression(Literal(17), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(17, 0), Operation::MakeI2D(0)}),
            result2.expr.operations());
  const CompileExpressionResult result3 =
      CompileExpression(Literal(17), Type::BOOL, {}, {}, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: 17"}),
      result3.errors);
}

TEST(CompileExpressionTest, DoubleLiteral) {
  const CompileExpressionResult result1 =
      CompileExpression(Literal(0.5), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found "
                 "double: 0.5"}),
            result1.errors);
  const CompileExpressionResult result2 =
      CompileExpression(Literal(0.5), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0)}),
            result2.expr.operations());
  const CompileExpressionResult result3 =
      CompileExpression(Literal(0.5), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: 0.5"}),
            result3.errors);
}

TEST(CompileExpressionTest, BoolLiteral) {
  const CompileExpressionResult result1 =
      CompileExpression(Literal(true), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "true"}),
            result1.errors);
  const CompileExpressionResult result2 =
      CompileExpression(Literal(true), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type double; found "
                 "bool: true"}),
            result2.errors);
  const CompileExpressionResult result3 =
      CompileExpression(Literal(true), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0)}),
            result3.expr.operations());
  const CompileExpressionResult result4 =
      CompileExpression(Literal(false), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(0, 0)}),
            result4.expr.operations());
}

TEST(CompileExpressionTest, Identifier) {
  const Identifier a("a");
  const Identifier e("e");
  const std::map<std::string, const Expression*> formulas_by_name = {{"h", &e},
                                                                     {"i", &a}};
  const std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"a", IdentifierInfo::Variable(Type::INT, 0, 0, 1, 0)},
      {"b", IdentifierInfo::Variable(Type::BOOL, 1, 2, 2, false)},
      {"c", IdentifierInfo::Constant(17)},
      {"d", IdentifierInfo::Constant(false)},
      {"e", IdentifierInfo::Variable(Type::DOUBLE, 2, 3, 10, 0.5)},
      {"f", IdentifierInfo::Constant(0.5)}};

  const CompileExpressionResult result1 = CompileExpression(
      Identifier("a"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeILOAD(0, 0)}),
            result1.expr.operations());
  const CompileExpressionResult result2 = CompileExpression(
      Identifier("a"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeILOAD(0, 0), Operation::MakeI2D(0)}),
            result2.expr.operations());
  const CompileExpressionResult result3 = CompileExpression(
      Identifier("a"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: a"}),
      result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      Identifier("b"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool: b"}),
      result4.errors);
  const CompileExpressionResult result5 = CompileExpression(
      Identifier("b"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type double; found "
                 "bool: b"}),
            result5.errors);
  const CompileExpressionResult result6 = CompileExpression(
      Identifier("b"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeILOAD(1, 0)}),
            result6.expr.operations());

  const CompileExpressionResult result7 = CompileExpression(
      Identifier("c"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0)}),
            result7.expr.operations());
  const CompileExpressionResult result8 = CompileExpression(
      Identifier("c"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(17, 0), Operation::MakeI2D(0)}),
            result8.expr.operations());
  const CompileExpressionResult result9 = CompileExpression(
      Identifier("c"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: c"}),
      result9.errors);

  const CompileExpressionResult result10 = CompileExpression(
      Identifier("d"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool: d"}),
      result10.errors);
  const CompileExpressionResult result11 = CompileExpression(
      Identifier("d"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type double; found "
                 "bool: d"}),
            result11.errors);
  const CompileExpressionResult result12 = CompileExpression(
      Identifier("d"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(0, 0)}),
            result12.expr.operations());

  const CompileExpressionResult result13 = CompileExpression(
      Identifier("e"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result13.errors);
  const CompileExpressionResult result14 = CompileExpression(
      Identifier("e"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result14.errors);
  const CompileExpressionResult result15 = CompileExpression(
      Identifier("e"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result15.errors);

  const CompileExpressionResult result16 = CompileExpression(
      Identifier("f"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double: f"}),
      result16.errors);
  const CompileExpressionResult result17 = CompileExpression(
      Identifier("f"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0)}),
            result17.expr.operations());
  const CompileExpressionResult result18 = CompileExpression(
      Identifier("f"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: f"}),
            result18.errors);

  const CompileExpressionResult result19 = CompileExpression(
      Identifier("g"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>({"undefined identifier 'g' in expression"}),
      result19.errors);
  const CompileExpressionResult result20 = CompileExpression(
      Identifier("g"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>({"undefined identifier 'g' in expression"}),
      result20.errors);

  const CompileExpressionResult result21 = CompileExpression(
      Identifier("g"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>({"undefined identifier 'g' in expression"}),
      result21.errors);

  const CompileExpressionResult result22 = CompileExpression(
      Identifier("h"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result22.errors);
  const CompileExpressionResult result23 = CompileExpression(
      Identifier("h"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result23.errors);
  const CompileExpressionResult result24 = CompileExpression(
      Identifier("h"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result24.errors);

  const CompileExpressionResult result25 = CompileExpression(
      Identifier("i"), Type::INT, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeILOAD(0, 0)}),
            result25.expr.operations());
  const CompileExpressionResult result26 = CompileExpression(
      Identifier("i"), Type::DOUBLE, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeILOAD(0, 0), Operation::MakeI2D(0)}),
            result26.expr.operations());
  const CompileExpressionResult result27 = CompileExpression(
      Identifier("i"), Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int: i"}),
      result27.errors);
}

TEST(CompileExpressionTest, MinFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::MIN, 17, 42, 4711), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
                 Operation::MakeIMIN(0, 1), Operation::MakeICONST(4711, 1),
                 Operation::MakeIMIN(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::MIN, 0.5, -17), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDMIN(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::MIN, true), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0)}),
            result3.expr.operations());

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::MIN), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"min applied to 0 arguments; expecting at least 1 argument"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::MIN, 0.5), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found "
                 "double: min(0.5)"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::MIN, 0.5, true), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types double and bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, MaxFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::MAX, 17, 42, 4711), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
                 Operation::MakeIMAX(0, 1), Operation::MakeICONST(4711, 1),
                 Operation::MakeIMAX(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::MAX, 0.5, -17), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDMAX(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::MAX, true), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0)}),
            result3.expr.operations());

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::MAX), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"max applied to 0 arguments; expecting at least 1 argument"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::MAX, 0.5), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found "
                 "double: max(0.5)"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::MAX, 0.5, true), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types double and bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, FloorFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::FLOOR, 17), Type::INT, {}, {}, {});
  EXPECT_EQ(
      std::vector<Operation>({Operation::MakeICONST(17, 0),
                              Operation::MakeI2D(0), Operation::MakeFLOOR(0)}),
      result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::FLOOR, 0.5), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(
      std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                              Operation::MakeFLOOR(0), Operation::MakeI2D(0)}),
      result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::FLOOR, 0.5), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found int: "
                 "floor(0.5)"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeFunctionCall(Function::FLOOR), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"floor applied to 0 arguments; expecting 1 argument"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::FLOOR, 0.5, -17), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"floor applied to 2 arguments; expecting 1 argument"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::FLOOR, true), Type::INT, {}, {}, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result6.errors);
}

TEST(CompileExpressionTest, CeilFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::CEIL, 17), Type::INT, {}, {}, {});
  EXPECT_EQ(
      std::vector<Operation>({Operation::MakeICONST(17, 0),
                              Operation::MakeI2D(0), Operation::MakeCEIL(0)}),
      result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::CEIL, 0.5), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(
      std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                              Operation::MakeCEIL(0), Operation::MakeI2D(0)}),
      result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::CEIL, 0.5), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found int: "
                 "ceil(0.5)"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeFunctionCall(Function::CEIL), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"ceil applied to 0 arguments; expecting 1 argument"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::CEIL, 0.5, -17), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"ceil applied to 2 arguments; expecting 1 argument"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::CEIL, true), Type::INT, {}, {}, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result6.errors);
}

TEST(CompileExpressionTest, PowFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::POW, 17, 42), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found "
                 "double: pow(17, 42)"}),
            result1.errors);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::POW, 0.5, 2), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(2, 1),
                 Operation::MakeI2D(1), Operation::MakePOW(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::POW, 0.5, 2), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: pow(0.5, 2)"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeFunctionCall(Function::POW, 0.5), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"pow applied to 1 argument; expecting 2 arguments"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::POW, 0.5, 2, -17), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"pow applied to 3 arguments; expecting 2 arguments"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::POW, 17, true), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeFunctionCall(Function::POW, true, 17), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result7.errors);
}

TEST(CompileExpressionTest, LogFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::LOG, 17, 42), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found "
                 "double: log(17, 42)"}),
            result1.errors);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::LOG, 0.5, 2), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(2, 1),
                 Operation::MakeI2D(1), Operation::MakeLOG(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::LOG, 0.5, 2), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: log(0.5, 2)"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeFunctionCall(Function::LOG, 0.5), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"log applied to 1 argument; expecting 2 arguments"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::LOG, 0.5, 2, -17), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"log applied to 3 arguments; expecting 2 arguments"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::LOG, 17, true), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeFunctionCall(Function::LOG, true, 17), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result7.errors);
}

TEST(CompileExpressionTest, ModFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, 42), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeMOD(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, 42), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
                 Operation::MakeMOD(0, 1), Operation::MakeI2D(0)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, 42), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found int: "
                 "mod(17, 42)"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"mod applied to 1 argument; expecting 2 arguments"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, 42, 4711), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"mod applied to 3 arguments; expecting 2 arguments"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, true), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting argument of type int; found bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeFunctionCall(Function::MOD, 0.5, 17), Type::INT, {}, {}, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type int; found double"}),
      result7.errors);
}

TEST(CompileExpressionTest, Negation) {
  const CompileExpressionResult result1 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, MakeUnique<Literal>(17)), Type::INT,
      {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(17, 0), Operation::MakeINEG(0)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, MakeUnique<Literal>(17)),
      Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(
      std::vector<Operation>({Operation::MakeICONST(17, 0),
                              Operation::MakeINEG(0), Operation::MakeI2D(0)}),
      result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, MakeUnique<Literal>(0.5)),
      Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeDNEG(0)}),
            result3.expr.operations());

  const CompileExpressionResult result4 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, MakeUnique<Literal>(true)),
      Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; unary operator - applied to bool"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, MakeUnique<Literal>(0.5)),
      Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found "
                 "double: -0.5"}),
            result5.errors);
}

TEST(CompileExpressionTest, LogicalNot) {
  const CompileExpressionResult result1 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, MakeUnique<Literal>(true)), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(1, 0), Operation::MakeNOT(0)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, MakeUnique<Literal>(17)), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; unary operator ! applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, MakeUnique<Literal>(0.5)), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; unary operator ! applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, MakeUnique<Literal>(true)), Type::INT,
      {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "!true"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, MakeUnique<Literal>(true)),
      Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type double; found "
                 "bool: !true"}),
            result5.errors);
}

TEST(CompileExpressionTest, Plus) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::PLUS, 17, 42), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeIADD(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::PLUS, 0.5, -17),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDADD(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::PLUS, 0.5, 2.0),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDADD(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::PLUS, 0.5, 2),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: 0.5 + 2"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::PLUS, 17, true),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator + applied to bool"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::PLUS, true, 17),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator + applied to bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, Minus) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MINUS, 17, 42),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeISUB(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MINUS, 0.5, -17),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDSUB(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MINUS, 0.5, 2.0),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDSUB(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MINUS, 0.5, 2),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: 0.5 - 2"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MINUS, 17, true),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator - applied to bool"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MINUS, true, 17),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator - applied to bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, Multiply) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MULTIPLY, 17, 42),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeIMUL(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MULTIPLY, 0.5, -17),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDMUL(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MULTIPLY, 0.5, 2.0),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDMUL(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MULTIPLY, 0.5, 2),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type bool; found "
                 "double: 0.5 * 2"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MULTIPLY, 17, true),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator * applied to bool"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MULTIPLY, true, 17),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator * applied to bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, Divide) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::DIVIDE, 17, 42),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
                 Operation::MakeI2D(0), Operation::MakeI2D(1),
                 Operation::MakeDDIV(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::DIVIDE, 0.5, -17),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDDIV(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::DIVIDE, 0.5, 2.0),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDDIV(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::DIVIDE, 0.5, 2),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found "
                 "double: 0.5 / 2"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::DIVIDE, 17, true),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator / applied to bool"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::DIVIDE, true, 17),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator / applied to bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, And) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::AND, true, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeIFFALSE(0, 3),
                                    Operation::MakeICONST(0, 0)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::AND, 17, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator & applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::AND, true, 0.5),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator & applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::AND, true, false),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "true & false"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::AND, true, false),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type double; found "
                 "bool: true & false"}),
            result5.errors);
}

TEST(CompileExpressionTest, Or) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::OR, true, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeIFTRUE(0, 3),
                                    Operation::MakeICONST(0, 0)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::OR, 17, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator | applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::OR, true, 0.5),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator | applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::OR, true, false),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "true | false"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::OR, true, false),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type double; found "
                 "bool: true | false"}),
            result5.errors);
}

TEST(CompileExpressionTest, Imply) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IMPLY, true, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(1, 0), Operation::MakeNOT(0),
                 Operation::MakeIFTRUE(0, 4), Operation::MakeICONST(0, 0)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IMPLY, 17, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator => applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IMPLY, true, 0.5),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator => applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IMPLY, true, false),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "true => false"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IMPLY, true, false),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type double; found "
                 "bool: true => false"}),
            result5.errors);
}

TEST(CompileExpressionTest, Iff) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IFF, true, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeICONST(0, 1),
                                    Operation::MakeIEQ(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IFF, 17, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator <=> applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IFF, true, 0.5),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator <=> applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IFF, true, false),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "true <=> false"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IFF, true, false),
                        Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type double; found "
                 "bool: true <=> false"}),
            result5.errors);
}

TEST(CompileExpressionTest, Less) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::LESS, 17, 42),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeILT(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::LESS, 0.5, -17),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDLT(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::LESS, 0.5, 2.0),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDLT(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::LESS, true, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeICONST(0, 1),
                                    Operation::MakeILT(0, 1)}),
            result4.expr.operations());

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS, 0.5, 2), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "0.5 < 2"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::LESS, 17, true),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::LESS, true, 0.5),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, LessEqual) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 17, 42),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeILE(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 0.5, -17), Type::BOOL, {},
      {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDLE(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 0.5, 2.0), Type::BOOL, {},
      {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDLE(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, true, false), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeICONST(0, 1),
                                    Operation::MakeILE(0, 1)}),
            result4.expr.operations());

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 0.5, 2),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "0.5 <= 2"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 17, true), Type::BOOL, {},
      {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, true, 0.5), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, GreaterEqual) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 17, 42), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeIGE(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 0.5, -17), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDGE(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 0.5, 2.0), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDGE(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, true, false),
      Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeICONST(0, 1),
                                    Operation::MakeIGE(0, 1)}),
            result4.expr.operations());

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 0.5, 2), Type::INT, {},
      {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "0.5 >= 2"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 17, true), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, true, 0.5), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, Greater) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::GREATER, 17, 42),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeIGT(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::GREATER, 0.5, -17),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDGT(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::GREATER, 0.5, 2.0),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDGT(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER, true, false), Type::BOOL, {},
      {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeICONST(0, 1),
                                    Operation::MakeIGT(0, 1)}),
            result4.expr.operations());

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::GREATER, 0.5, 2),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "0.5 > 2"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::GREATER, 17, true),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::GREATER, true, 0.5),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, Equal) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::EQUAL, 17, 42),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeIEQ(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::EQUAL, 0.5, -17),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDEQ(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::EQUAL, 0.5, 2.0),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDEQ(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::EQUAL, true, false),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeICONST(0, 1),
                                    Operation::MakeIEQ(0, 1)}),
            result4.expr.operations());

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::EQUAL, 0.5, 2),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "0.5 = 2"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::EQUAL, 17, true),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::EQUAL, true, 0.5),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, NotEqual) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 17, 42),
                        Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(17, 0),
                                    Operation::MakeICONST(42, 1),
                                    Operation::MakeINE(0, 1)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 0.5, -17), Type::BOOL, {},
      {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDNE(0, 1)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 0.5, 2.0), Type::BOOL, {},
      {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeDCONST(0.5, 0),
                                    Operation::MakeDCONST(2.0, 1),
                                    Operation::MakeDNE(0, 1)}),
            result3.expr.operations());

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, true, false), Type::BOOL,
      {}, {}, {});
  EXPECT_EQ(std::vector<Operation>({Operation::MakeICONST(1, 0),
                                    Operation::MakeICONST(0, 1),
                                    Operation::MakeINE(0, 1)}),
            result4.expr.operations());

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 0.5, 2),
                        Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting expression of type int; found bool: "
                 "0.5 != 2"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 17, true), Type::BOOL, {},
      {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, true, 0.5), Type::BOOL, {},
      {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, Conditional) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeConditional(true, 17, 42), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(1, 0), Operation::MakeIFFALSE(0, 5),
                 Operation::MakeICONST(17, 0), Operation::MakeNOP(),
                 Operation::MakeGOTO(6), Operation::MakeICONST(42, 0)}),
            result1.expr.operations());

  const CompileExpressionResult result2 = CompileExpression(
      MakeConditional(true, 0.5, 42), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(1, 0), Operation::MakeIFFALSE(0, 4),
                 Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(6),
                 Operation::MakeICONST(42, 0), Operation::MakeI2D(0)}),
            result2.expr.operations());

  const CompileExpressionResult result3 = CompileExpression(
      MakeConditional(true, 17, 0.5), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(1, 0), Operation::MakeIFFALSE(0, 5),
                 Operation::MakeICONST(17, 0), Operation::MakeI2D(0),
                 Operation::MakeGOTO(6), Operation::MakeDCONST(0.5, 0)}),
            result3.expr.operations());

  const CompileExpressionResult result4 = CompileExpression(
      MakeConditional(true, false, true), Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeICONST(1, 0), Operation::MakeIFFALSE(0, 4),
                 Operation::MakeICONST(0, 0), Operation::MakeGOTO(5),
                 Operation::MakeICONST(1, 0)}),
            result4.expr.operations());

  const CompileExpressionResult result5 =
      CompileExpression(MakeConditional(4711, 17, 42), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting condition of type bool; found int"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeConditional(true, 0.5, true), Type::DOUBLE, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible branch types double and bool"}),
            result6.errors);

  const CompileExpressionResult result7 =
      CompileExpression(MakeConditional(true, true, 17), Type::INT, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible branch types bool and int"}),
            result7.errors);
}

TEST(CompileExpressionTest, ProbabilityThresholdOperation) {
  const ProbabilityThresholdOperation expr(
      ProbabilityThresholdOperator::LESS, 0.25,
      MakeUnique<UntilProperty>(
          TimeRange(0, std::numeric_limits<double>::infinity()),
          MakeUnique<Literal>(true), MakeUnique<Identifier>("a")));
  const CompileExpressionResult result =
      CompileExpression(expr, Type::BOOL, {}, {}, {});
  EXPECT_EQ(std::vector<std::string>(
                {"unexpected probability threshold operation in expression"}),
            result.errors);
}

TEST(CompileExpressionTest, ComplexExpression) {
  const BinaryOperation f(BinaryOperator::EQUAL, MakeUnique<Identifier>("a"),
                          MakeUnique<Literal>(17));
  const std::map<std::string, const Expression*> formulas_by_name = {{"f", &f}};
  const std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"a", IdentifierInfo::Variable(Type::INT, 0, 0, 1, 17)},
      {"b", IdentifierInfo::Variable(Type::BOOL, 1, 2, 2, false)},
      {"c", IdentifierInfo::Variable(Type::INT, 2, 3, 4, 42)}};
  const BinaryOperation expr(
      BinaryOperator::AND, MakeUnique<BinaryOperation>(
                               BinaryOperator::AND, MakeUnique<Identifier>("f"),
                               MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Literal>(42)));
  const CompileExpressionResult result = CompileExpression(
      expr, Type::BOOL, formulas_by_name, identifiers_by_name, {});
  EXPECT_EQ(std::vector<Operation>(
                {Operation::MakeILOAD(0, 0), Operation::MakeICONST(17, 1),
                 Operation::MakeIEQ(0, 1), Operation::MakeIFFALSE(0, 5),
                 Operation::MakeILOAD(1, 0), Operation::MakeIFFALSE(0, 9),
                 Operation::MakeILOAD(2, 0), Operation::MakeICONST(42, 1),
                 Operation::MakeIEQ(0, 1)}),
            result.expr.operations());
}

TEST(IdentifierToAddTest, Constant) {
  const DecisionDiagramManager manager(0);
  ADD dd1 = IdentifierToAdd(manager, IdentifierInfo::Constant(false));
  EXPECT_EQ(0, dd1.Value());
  ADD dd2 = IdentifierToAdd(manager, IdentifierInfo::Constant(true));
  EXPECT_EQ(1, dd2.Value());
  ADD dd3 = IdentifierToAdd(manager, IdentifierInfo::Constant(17));
  EXPECT_EQ(17, dd3.Value());
  ADD dd4 = IdentifierToAdd(manager, IdentifierInfo::Constant(0.5));
  EXPECT_EQ(0.5, dd4.Value());
}

TEST(IdentifierToAddTest, Variable) {
  const DecisionDiagramManager manager(6);
  ADD dd1 = IdentifierToAdd(manager,
                            IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, 0));
  EXPECT_EQ(0, dd1.ValueInState({false, false, false, false, false, false}));
  EXPECT_EQ(1, dd1.ValueInState({true, false, false, false, false, false}));
  ADD dd2 = IdentifierToAdd(manager,
                            IdentifierInfo::Variable(Type::INT, 1, 1, 2, 17));
  EXPECT_EQ(17, dd2.ValueInState({false, false, false, false, false, false}));
  EXPECT_EQ(18, dd2.ValueInState({false, false, false, false, true, false}));
  EXPECT_EQ(19, dd2.ValueInState({false, false, true, false, false, false}));
  EXPECT_EQ(20, dd2.ValueInState({false, false, true, false, true, false}));
}

TEST(PrimedIdentifierToAddTest, Variable) {
  const DecisionDiagramManager manager(6);
  ADD dd1 = PrimedIdentifierToAdd(
      manager, IdentifierInfo::Variable(Type::BOOL, 0, 0, 0, 0));
  EXPECT_EQ(0, dd1.ValueInState({false, false, false, false, false, false}));
  EXPECT_EQ(1, dd1.ValueInState({false, true, false, false, false, false}));
  ADD dd2 = PrimedIdentifierToAdd(
      manager, IdentifierInfo::Variable(Type::INT, 1, 1, 2, 17));
  EXPECT_EQ(17, dd2.ValueInState({false, false, false, false, false, false}));
  EXPECT_EQ(18, dd2.ValueInState({false, false, false, false, false, true}));
  EXPECT_EQ(19, dd2.ValueInState({false, false, false, true, false, false}));
  EXPECT_EQ(20, dd2.ValueInState({false, false, false, true, false, true}));
}

TEST(OptimizeIntExpressionTest, Constant) {
  const CompiledExpression expr({Operation::MakeICONST(17, 0)}, {});
  EXPECT_EQ(expr.operations(), OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, Variable) {
  const CompiledExpression expr({Operation::MakeILOAD(0, 0)}, {});
  EXPECT_EQ(expr.operations(), OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantNegation) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeINEG(0)}, {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(-17, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(-42, 0), Operation::MakeINEG(0)}, {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, MultiNegation) {
  const CompiledExpression expr1(
      {Operation::MakeILOAD(0, 0), Operation::MakeINEG(0),
       Operation::MakeILOAD(1, 1), Operation::MakeIADD(0, 1),
       Operation::MakeINEG(0)},
      {});
  const std::vector<Operation> expected1 = {
      Operation::MakeILOAD(0, 0), Operation::MakeINEG(0),
      Operation::MakeILOAD(1, 1), Operation::MakeIADD(0, 1),
      Operation::MakeINEG(0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeILOAD(0, 0), Operation::MakeINEG(0),
       Operation::MakeILOAD(1, 1), Operation::MakeINEG(1),
       Operation::MakeINEG(1), Operation::MakeIADD(0, 1),
       Operation::MakeINEG(0), Operation::MakeINEG(0), Operation::MakeINEG(0),
       Operation::MakeINEG(0), Operation::MakeINEG(0)},
      {});
  const std::vector<Operation> expected2 = {
      Operation::MakeILOAD(0, 0), Operation::MakeINEG(0),
      Operation::MakeILOAD(1, 1), Operation::MakeIADD(0, 1),
      Operation::MakeINEG(0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantLogicalNot) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(true, 0), Operation::MakeNOT(0)}, {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(false, 0), Operation::MakeNOT(0)}, {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, MultiLogicalNot) {
  const CompiledExpression expr1(
      {Operation::MakeILOAD(0, 0), Operation::MakeNOT(0),
       Operation::MakeIFFALSE(0, 5), Operation::MakeILOAD(1, 0),
       Operation::MakeNOT(0), Operation::MakeNOT(0)},
      {});
  const std::vector<Operation> expected1 = {
      Operation::MakeILOAD(0, 0),   Operation::MakeNOT(0),
      Operation::MakeIFFALSE(0, 5), Operation::MakeILOAD(1, 0),
      Operation::MakeNOT(0),        Operation::MakeNOT(0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeILOAD(0, 0), Operation::MakeNOT(0),
       Operation::MakeIFFALSE(0, 6), Operation::MakeILOAD(1, 0),
       Operation::MakeNOT(0), Operation::MakeNOT(0), Operation::MakeNOT(0),
       Operation::MakeNOT(0), Operation::MakeNOT(0)},
      {});
  const std::vector<Operation> expected2 = {
      Operation::MakeILOAD(0, 0), Operation::MakeNOT(0),
      Operation::MakeIFFALSE(0, 4), Operation::MakeILOAD(1, 0),
      Operation::MakeNOT(0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantAddition) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIADD(0, 1)},
      {});
  const std::vector<Operation> expected = {Operation::MakeICONST(17 + 42, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantSubtraction) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeISUB(0, 1)},
      {});
  const std::vector<Operation> expected = {Operation::MakeICONST(17 - 42, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantMultiplication) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIMUL(0, 1)},
      {});
  const std::vector<Operation> expected = {Operation::MakeICONST(17 * 42, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntEqual) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIEQ(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIEQ(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIEQ(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleEqual) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDEQ(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDEQ(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDEQ(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntNotEqual) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeINE(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeINE(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeINE(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleNotEqual) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDNE(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDNE(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDNE(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntLess) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeILT(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeILT(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeILT(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleLess) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDLT(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDLT(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDLT(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntLessEqual) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeILE(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeILE(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeILE(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleLessEqual) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDLE(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDLE(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDLE(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntGreaterEqual) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIGE(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIGE(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIGE(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleGreaterEqual) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDGE(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDGE(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDGE(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntGreater) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIGT(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIGT(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIGT(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleGreater) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeDGT(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.25, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDGT(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDGT(0, 1)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantMin) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIMIN(0, 1)},
      {});
  const std::vector<Operation> expected = {Operation::MakeICONST(17, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantMax) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIMAX(0, 1)},
      {});
  const std::vector<Operation> expected = {Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantFloor) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeFLOOR(0)}, {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(0, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.5, 0), Operation::MakeFLOOR(0)}, {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(-1, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantCeil) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeCEIL(0)}, {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(1, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.5, 0), Operation::MakeCEIL(0)}, {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(0, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantMod) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
       Operation::MakeMOD(0, 1)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(17 % 42, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(42, 0), Operation::MakeICONST(17, 1),
       Operation::MakeMOD(0, 1)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(42 % 17, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIfFalse) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(17, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIfTrue) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(17, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ChainedIfFalse) {
  const CompiledExpression expr(
      {Operation::MakeILOAD(0, 0), Operation::MakeICONST(17, 1),
       Operation::MakeIEQ(0, 1), Operation::MakeIFFALSE(0, 5),
       Operation::MakeILOAD(1, 0), Operation::MakeIFFALSE(0, 9),
       Operation::MakeILOAD(3, 0), Operation::MakeICONST(42, 1),
       Operation::MakeIEQ(0, 1)},
      {});
  const std::vector<Operation> expected = {
      Operation::MakeIVEQ(0, 17, 0), Operation::MakeIFFALSE(0, 5),
      Operation::MakeILOAD(1, 0),    Operation::MakeIFFALSE(0, 5),
      Operation::MakeIVEQ(3, 42, 0),
  };
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ComplexJumps) {
  const CompiledExpression expr(
      {Operation::MakeILOAD(0, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeILOAD(1, 0), Operation::MakeGOTO(4),
       Operation::MakeNOT(0)},
      {});
  const std::vector<Operation> expected = {
      Operation::MakeILOAD(0, 0), Operation::MakeIFFALSE(0, 3),
      Operation::MakeILOAD(1, 0), Operation::MakeNOT(0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, EliminatesDeadCode) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeICONST(17, 0)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(17, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeILOAD(0, 0)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeILOAD(0, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeILOAD(0, 0),
       Operation::MakeINEG(0)},
      {});
  const std::vector<Operation> expected3 = {Operation::MakeILOAD(0, 0),
                                            Operation::MakeINEG(0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
  const CompiledExpression expr4(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeILOAD(0, 0),
       Operation::MakeIADD(0, 1)},
      {});
  const std::vector<Operation> expected4 = {Operation::MakeICONST(42, 1),
                                            Operation::MakeILOAD(0, 0),
                                            Operation::MakeIADD(0, 1)};
  EXPECT_EQ(expected4, OptimizeIntExpression(expr4).operations());
  const CompiledExpression expr5(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeILOAD(0, 0),
       Operation::MakeI2D(0), Operation::MakeFLOOR(0)},
      {});
  const std::vector<Operation> expected5 = {
      Operation::MakeILOAD(0, 0), Operation::MakeI2D(0),
      Operation::MakeFLOOR(0),
  };
  EXPECT_EQ(expected5, OptimizeIntExpression(expr5).operations());
  const CompiledExpression expr6(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeDCONST(0.5, 1), Operation::MakeILOAD(0, 0),
       Operation::MakeDCONST(0.25, 0), Operation::MakeI2D(0),
       Operation::MakeDEQ(0, 1)},
      {});
  const std::vector<Operation> expected6 = {
      Operation::MakeDCONST(0.5, 1), Operation::MakeILOAD(0, 0),
      Operation::MakeI2D(0), Operation::MakeDEQ(0, 1)};
  EXPECT_EQ(expected6, OptimizeIntExpression(expr6).operations());
}

TEST(OptimizeIntExpressionTest, ConstantFolding) {
  const CompiledExpression expr(
      {Operation::MakeICONST(1, 0), Operation::MakeICONST(4, 1),
       Operation::MakeILT(0, 1), Operation::MakeIFFALSE(0, 18),
       Operation::MakeILOAD(2, 0), Operation::MakeICONST(0, 1),
       Operation::MakeIEQ(0, 1), Operation::MakeIFFALSE(0, 18),
       Operation::MakeICONST(1, 0), Operation::MakeICONST(1, 1),
       Operation::MakeIADD(0, 1), Operation::MakeILOAD(3, 1),
       Operation::MakeIEQ(0, 1), Operation::MakeIFFALSE(0, 17),
       Operation::MakeILOAD(1, 0), Operation::MakeILOAD(4, 1),
       Operation::MakeIEQ(0, 1), Operation::MakeNOT(0)},
      {});
  const std::vector<Operation> expected = {
      Operation::MakeIVEQ(2, 0, 0), Operation::MakeIFFALSE(0, 8),
      Operation::MakeIVEQ(3, 2, 0), Operation::MakeIFFALSE(0, 7),
      Operation::MakeILOAD(1, 0),   Operation::MakeILOAD(4, 1),
      Operation::MakeIEQ(0, 1),     Operation::MakeNOT(0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, Constant) {
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0)}, {});
  EXPECT_EQ(expr.operations(), OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantConversion) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeI2D(0)}, {});
  const std::vector<Operation> expected = {Operation::MakeDCONST(17.0, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantNegation) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDNEG(0)}, {});
  const std::vector<Operation> expected1 = {Operation::MakeDCONST(-0.5, 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.25, 0), Operation::MakeDNEG(0)}, {});
  const std::vector<Operation> expected2 = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, MultiNegation) {
  const CompiledExpression expr1(
      {Operation::MakeILOAD(0, 0), Operation::MakeI2D(0),
       Operation::MakeDNEG(0), Operation::MakeILOAD(1, 1),
       Operation::MakeI2D(1), Operation::MakeDADD(0, 1), Operation::MakeDNEG(0),
       Operation::MakeCEIL(0)},
      {});
  const std::vector<Operation> expected1 = {
      Operation::MakeILOAD(0, 0), Operation::MakeI2D(0),
      Operation::MakeDNEG(0),     Operation::MakeILOAD(1, 1),
      Operation::MakeI2D(1),      Operation::MakeDADD(0, 1),
      Operation::MakeDNEG(0),     Operation::MakeCEIL(0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeILOAD(0, 0), Operation::MakeI2D(0),
       Operation::MakeDNEG(0), Operation::MakeILOAD(1, 1),
       Operation::MakeI2D(1), Operation::MakeDNEG(1), Operation::MakeDNEG(1),
       Operation::MakeDADD(0, 1), Operation::MakeDNEG(0),
       Operation::MakeDNEG(0), Operation::MakeDNEG(0), Operation::MakeDNEG(0),
       Operation::MakeDNEG(0), Operation::MakeCEIL(0)},
      {});
  const std::vector<Operation> expected2 = {
      Operation::MakeILOAD(0, 0), Operation::MakeI2D(0),
      Operation::MakeDNEG(0),     Operation::MakeILOAD(1, 1),
      Operation::MakeI2D(1),      Operation::MakeDADD(0, 1),
      Operation::MakeDNEG(0),     Operation::MakeCEIL(0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, AndOfOr) {
  const CompiledExpression expr(
      {Operation::MakeILOAD(2, 0), Operation::MakeIFTRUE(0, 7),
       Operation::MakeILOAD(6, 0), Operation::MakeIFTRUE(0, 7),
       Operation::MakeILOAD(10, 0), Operation::MakeIFTRUE(0, 7),
       Operation::MakeILOAD(14, 0), Operation::MakeIFFALSE(0, 9),
       Operation::MakeIVEQ(1, 2, 0)},
      {});
  // No optimization possible.
  EXPECT_EQ(expr.operations(), OptimizeIntExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantAddition) {
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDADD(0, 1)},
      {});
  const std::vector<Operation> expected = {
      Operation::MakeDCONST(0.5 + 0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantSubtraction) {
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDSUB(0, 1)},
      {});
  const std::vector<Operation> expected = {
      Operation::MakeDCONST(0.5 - 0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantMultiplication) {
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDMUL(0, 1)},
      {});
  const std::vector<Operation> expected = {
      Operation::MakeDCONST(0.5 * 0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantDivision) {
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDDIV(0, 1)},
      {});
  const std::vector<Operation> expected = {
      Operation::MakeDCONST(0.5 / 0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantMin) {
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDMIN(0, 1)},
      {});
  const std::vector<Operation> expected = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantMax) {
  const CompiledExpression expr(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(0.25, 1),
       Operation::MakeDMAX(0, 1)},
      {});
  const std::vector<Operation> expected = {Operation::MakeDCONST(0.5, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantPow) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(4.0, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakePOW(0, 1)},
      {});
  const std::vector<Operation> expected1 = {
      Operation::MakeDCONST(pow(4.0, 0.5), 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(4.0, 1),
       Operation::MakePOW(0, 1)},
      {});
  const std::vector<Operation> expected2 = {
      Operation::MakeDCONST(pow(0.5, 4.0), 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantLog) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(4.0, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeLOG(0, 1)},
      {});
  const std::vector<Operation> expected1 = {
      Operation::MakeDCONST(log(4.0) / log(0.5), 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDCONST(4.0, 1),
       Operation::MakeLOG(0, 1)},
      {});
  const std::vector<Operation> expected2 = {
      Operation::MakeDCONST(log(0.5) / log(4.0), 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantIfFalse) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(5),
       Operation::MakeDCONST(0.25, 0)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(5),
       Operation::MakeDCONST(0.25, 0)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeDCONST(0.5, 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantIfTrue) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(5),
       Operation::MakeDCONST(0.25, 0)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeDCONST(0.5, 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(5),
       Operation::MakeDCONST(0.25, 0)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, EliminatesDeadCode) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.125, 0), Operation::MakeICONST(42, 0),
       Operation::MakeDCONST(0.5, 1), Operation::MakeDCONST(0.25, 0)},
      {});
  const std::vector<Operation> expected1 = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.125, 0), Operation::MakeICONST(42, 0),
       Operation::MakeILOAD(0, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeI2D(0), Operation::MakeDNEG(0)},
      {});
  const std::vector<Operation> expected2 = {Operation::MakeILOAD(0, 0),
                                            Operation::MakeI2D(0),
                                            Operation::MakeDNEG(0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.125, 0), Operation::MakeICONST(42, 0),
       Operation::MakeILOAD(0, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeI2D(0), Operation::MakeDADD(0, 1)},
      {});
  const std::vector<Operation> expected3 = {
      Operation::MakeILOAD(0, 0), Operation::MakeDCONST(0.5, 1),
      Operation::MakeI2D(0), Operation::MakeDADD(0, 1)};
  EXPECT_EQ(expected3, OptimizeDoubleExpression(expr3).operations());
}

}  // namespace
