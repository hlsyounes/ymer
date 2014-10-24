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

FunctionCall MakeFunctionCall(Function function) {
  return FunctionCall(function, UniquePtrVector<const Expression>());
}

template <typename T>
FunctionCall MakeFunctionCall(Function function, T a) {
  return FunctionCall(function,
                      UniquePtrVector<const Expression>(Literal::New(a)));
}

template <typename T, typename U>
FunctionCall MakeFunctionCall(Function function, T a, U b) {
  return FunctionCall(function, UniquePtrVector<const Expression>(
                                    Literal::New(a), Literal::New(b)));
}

template <typename T, typename U, typename V>
FunctionCall MakeFunctionCall(Function function, T a, U b, V c) {
  return FunctionCall(function,
                      UniquePtrVector<const Expression>(
                          Literal::New(a), Literal::New(b), Literal::New(c)));
}

template <typename T, typename U>
BinaryOperation MakeBinaryOperation(BinaryOperator op, T a, U b) {
  return BinaryOperation(op, Literal::New(a), Literal::New(b));
}

template <typename T, typename U, typename V>
Conditional MakeConditional(T a, U b, V c) {
  return Conditional(Literal::New(a), Literal::New(b), Literal::New(c));
}

TEST(MakeConjunctionTest, MakesConjunction) {
  const std::vector<Operation> operations1 = {Operation::MakeICONST(true, 0),
                                              Operation::MakeNOT(0)};
  const std::vector<Operation> operations2 = {
      Operation::MakeICONST(false, 0), Operation::MakeIFFALSE(0, 4),
      Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
      Operation::MakeICONST(42, 0)};
  const std::vector<Operation> empty_operations;
  const std::vector<Operation> expected1 = {
      Operation::MakeICONST(true, 0), Operation::MakeNOT(0),
      Operation::MakeIFFALSE(0, 8),   Operation::MakeICONST(false, 0),
      Operation::MakeIFFALSE(0, 7),   Operation::MakeICONST(17, 0),
      Operation::MakeGOTO(8),         Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected1, MakeConjunction(operations1, operations2));
  const std::vector<Operation> expected2 = {
      Operation::MakeICONST(false, 0), Operation::MakeIFFALSE(0, 4),
      Operation::MakeICONST(17, 0),    Operation::MakeGOTO(5),
      Operation::MakeICONST(42, 0),    Operation::MakeIFFALSE(0, 8),
      Operation::MakeICONST(true, 0),  Operation::MakeNOT(0)};
  EXPECT_EQ(expected2, MakeConjunction(operations2, operations1));
  EXPECT_EQ(operations1, MakeConjunction(operations1, empty_operations));
  EXPECT_EQ(operations1, MakeConjunction(empty_operations, operations1));
  EXPECT_EQ(operations2, MakeConjunction(operations2, empty_operations));
  EXPECT_EQ(operations2, MakeConjunction(empty_operations, operations2));
  EXPECT_EQ(empty_operations,
            MakeConjunction(empty_operations, empty_operations));
}

TEST(GetNumRegistersTest, Constant) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 3)});
  EXPECT_EQ(std::make_pair(4, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({Operation::MakeDCONST(0.5, 2)});
  EXPECT_EQ(std::make_pair(0, 3), GetNumRegisters(expr2));
}

TEST(GetNumRegistersTest, Variable) {
  const CompiledExpression expr({Operation::MakeILOAD(0, 5)});
  EXPECT_EQ(std::make_pair(6, 0), GetNumRegisters(expr));
}

TEST(GetNumRegistersTest, Conversion) {
  const CompiledExpression expr({Operation::MakeI2D(0)});
  EXPECT_EQ(std::make_pair(1, 1), GetNumRegisters(expr));
}

TEST(GetNumRegistersTest, Negation) {
  const CompiledExpression expr1({Operation::MakeINEG(1)});
  EXPECT_EQ(std::make_pair(2, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({Operation::MakeDNEG(1)});
  EXPECT_EQ(std::make_pair(0, 2), GetNumRegisters(expr2));
}

TEST(GetNumRegistersTest, LogicalNot) {
  const CompiledExpression expr({Operation::MakeNOT(0)});
  EXPECT_EQ(std::make_pair(1, 0), GetNumRegisters(expr));
}

TEST(GetNumRegistersTest, Computation) {
  const CompiledExpression expr1({Operation::MakeIADD(5, 7)});
  EXPECT_EQ(std::make_pair(8, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({Operation::MakeDMUL(0, 1)});
  EXPECT_EQ(std::make_pair(0, 2), GetNumRegisters(expr2));
}

TEST(GetNumRegistersTest, Comparison) {
  const CompiledExpression expr1({Operation::MakeIEQ(0, 1)});
  EXPECT_EQ(std::make_pair(2, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({Operation::MakeDGT(1, 4)});
  EXPECT_EQ(std::make_pair(2, 5), GetNumRegisters(expr2));
}

TEST(GetNumRegistersTest, Jump) {
  const CompiledExpression expr1({Operation::MakeIFFALSE(17, 42)});
  EXPECT_EQ(std::make_pair(18, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({Operation::MakeIFTRUE(42, 17)});
  EXPECT_EQ(std::make_pair(43, 0), GetNumRegisters(expr2));
  const CompiledExpression expr3({Operation::MakeGOTO(17)});
  EXPECT_EQ(std::make_pair(0, 0), GetNumRegisters(expr3));
}

TEST(GetNumRegistersTest, Function) {
  const CompiledExpression expr1({Operation::MakeIMAX(3, 2)});
  EXPECT_EQ(std::make_pair(4, 0), GetNumRegisters(expr1));
  const CompiledExpression expr2({Operation::MakeDMIN(3, 2)});
  EXPECT_EQ(std::make_pair(0, 4), GetNumRegisters(expr2));
  const CompiledExpression expr3({Operation::MakeFLOOR(2)});
  EXPECT_EQ(std::make_pair(3, 3), GetNumRegisters(expr3));
  const CompiledExpression expr4({Operation::MakeCEIL(1)});
  EXPECT_EQ(std::make_pair(2, 2), GetNumRegisters(expr4));
  const CompiledExpression expr5({Operation::MakePOW(3, 2)});
  EXPECT_EQ(std::make_pair(0, 4), GetNumRegisters(expr5));
  const CompiledExpression expr6({Operation::MakeLOG(3, 2)});
  EXPECT_EQ(std::make_pair(0, 4), GetNumRegisters(expr6));
  const CompiledExpression expr7({Operation::MakeMOD(3, 2)});
  EXPECT_EQ(std::make_pair(4, 0), GetNumRegisters(expr7));
}

TEST(GetNumRegistersTest, Program) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 3), Operation::MakeILOAD(0, 5),
       Operation::MakeMOD(3, 5), Operation::MakeI2D(3),
       Operation::MakeDCONST(0.5, 2), Operation::MakePOW(3, 2)});
  EXPECT_EQ(std::make_pair(6, 4), GetNumRegisters(expr));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerConstant) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr({Operation::MakeICONST(17, 0)});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleConstant) {
  CompiledExpressionEvaluator evaluator(0, 1);
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0)});
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerVariable) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1({Operation::MakeILOAD(0, 0)});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr1, {17, 42}));
  const CompiledExpression expr2({Operation::MakeILOAD(1, 0)});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, {17, 42}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesConversionToDouble) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeI2D(0)});
  EXPECT_EQ(17.0, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerNegation) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeINEG(0)});
  EXPECT_EQ(-17, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(-42, 0), Operation::MakeINEG(0)});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleNegation) {
  CompiledExpressionEvaluator evaluator(0, 1);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDNEG(0)});
  EXPECT_EQ(-0.5, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.25, 0), Operation::MakeDNEG(0)});
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesLogicalNot) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(true, 0), Operation::MakeNOT(0)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(false, 0), Operation::MakeNOT(0)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerAddition) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr({Operation::MakeICONST(17, 0),
                                 Operation::MakeICONST(42, 1),
                                 Operation::MakeIADD(0, 1)});
  EXPECT_EQ(17 + 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleAddition) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDADD(0, 1)});
  EXPECT_EQ(0.5 + 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerSubtraction) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr({Operation::MakeICONST(17, 0),
                                 Operation::MakeICONST(42, 1),
                                 Operation::MakeISUB(0, 1)});
  EXPECT_EQ(17 - 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleSubtraction) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDSUB(0, 1)});
  EXPECT_EQ(0.5 - 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMultiplication) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr({Operation::MakeICONST(17, 0),
                                 Operation::MakeICONST(42, 1),
                                 Operation::MakeIMUL(0, 1)});
  EXPECT_EQ(17 * 42, evaluator.EvaluateIntExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMultiplication) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDMUL(0, 1)});
  EXPECT_EQ(0.5 * 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleDivision) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDDIV(0, 1)});
  EXPECT_EQ(0.5 / 0.25, evaluator.EvaluateDoubleExpression(expr, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIEQ(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIEQ(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeIEQ(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDEQ(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDEQ(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDEQ(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerNotEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeINE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeINE(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeINE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleNotEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDNE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDNE(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDNE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerLess) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeILT(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeILT(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeILT(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleLess) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDLT(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDLT(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDLT(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerLessEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeILE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeILE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeILE(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleLessEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDLE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDLE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDLE(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerGreaterEqual) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIGE(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIGE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeIGE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleGreaterEqual) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDGE(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDGE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDGE(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerGreater) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIGT(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIGT(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeIGT(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleGreater) {
  CompiledExpressionEvaluator evaluator(1, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDGT(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDGT(0, 1)});
  EXPECT_FALSE(evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDGT(0, 1)});
  EXPECT_TRUE(evaluator.EvaluateIntExpression(expr3, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesWithJumps) {
  CompiledExpressionEvaluator evaluator(1, 0);
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr2, {}));
  const CompiledExpression expr3(
      {Operation::MakeICONST(false, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr3, {}));
  const CompiledExpression expr4(
      {Operation::MakeICONST(true, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr4, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMin) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIMIN(0, 1)});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeIMIN(0, 1)});
  EXPECT_EQ(17, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMin) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDMIN(0, 1)});
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDMIN(0, 1)});
  EXPECT_EQ(0.25, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesIntegerMax) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIMAX(0, 1)});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeIMAX(0, 1)});
  EXPECT_EQ(42, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesDoubleMax) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDMAX(0, 1)});
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDMAX(0, 1)});
  EXPECT_EQ(0.5, evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesFloor) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeFLOOR(0)});
  EXPECT_EQ(0, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.5, 0), Operation::MakeFLOOR(0)});
  EXPECT_EQ(-1, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesCeil) {
  CompiledExpressionEvaluator evaluator(1, 1);
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeCEIL(0)});
  EXPECT_EQ(1, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.5, 0), Operation::MakeCEIL(0)});
  EXPECT_EQ(0, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesPow) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(4.0, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakePOW(0, 1)});
  EXPECT_EQ(pow(4.0, 0.5), evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(4.0, 1),
                                  Operation::MakePOW(0, 1)});
  EXPECT_EQ(pow(0.5, 4.0), evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesLog) {
  CompiledExpressionEvaluator evaluator(0, 2);
  const CompiledExpression expr1({Operation::MakeDCONST(4.0, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeLOG(0, 1)});
  EXPECT_EQ(log(4.0) / log(0.5), evaluator.EvaluateDoubleExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(4.0, 1),
                                  Operation::MakeLOG(0, 1)});
  EXPECT_EQ(log(0.5) / log(4.0), evaluator.EvaluateDoubleExpression(expr2, {}));
}

TEST(CompiledExpressionEvaluatorTest, EvaluatesMod) {
  CompiledExpressionEvaluator evaluator(2, 0);
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeMOD(0, 1)});
  EXPECT_EQ(17 % 42, evaluator.EvaluateIntExpression(expr1, {}));
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeMOD(0, 1)});
  EXPECT_EQ(42 % 17, evaluator.EvaluateIntExpression(expr2, {}));
}

TEST(CompileExpressionTest, IntLiteral) {
  const CompileExpressionResult result1 =
      CompileExpression(Literal(17), Type::INT, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0)}), result1.expr);
  const CompileExpressionResult result2 =
      CompileExpression(Literal(17), Type::DOUBLE, {});
  EXPECT_EQ(
      CompiledExpression({Operation::MakeICONST(17, 0), Operation::MakeI2D(0)}),
      result2.expr);
  const CompileExpressionResult result3 =
      CompileExpression(Literal(17), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result3.errors);
}

TEST(CompileExpressionTest, DoubleLiteral) {
  const CompileExpressionResult result1 =
      CompileExpression(Literal(0.5), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double"}),
      result1.errors);
  const CompileExpressionResult result2 =
      CompileExpression(Literal(0.5), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0)}), result2.expr);
  const CompileExpressionResult result3 =
      CompileExpression(Literal(0.5), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result3.errors);
}

TEST(CompileExpressionTest, BoolLiteral) {
  const CompileExpressionResult result1 =
      CompileExpression(Literal(true), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result1.errors);
  const CompileExpressionResult result2 =
      CompileExpression(Literal(true), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type double; found bool"}),
      result2.errors);
  const CompileExpressionResult result3 =
      CompileExpression(Literal(true), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0)}), result3.expr);
  const CompileExpressionResult result4 =
      CompileExpression(Literal(false), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(0, 0)}), result4.expr);
}

TEST(CompileExpressionTest, Identifier) {
  std::map<std::string, IdentifierInfo> identifiers_by_name = {
      {"a", IdentifierInfo::Variable(Type::INT, 0)},
      {"b", IdentifierInfo::Variable(Type::BOOL, 1)},
      {"c", IdentifierInfo::Constant(17)},
      {"d", IdentifierInfo::Constant(false)},
      {"e", IdentifierInfo::Variable(Type::DOUBLE, 2)},
      {"f", IdentifierInfo::Constant(0.5)}};

  const CompileExpressionResult result1 =
      CompileExpression(Identifier("a"), Type::INT, identifiers_by_name);
  EXPECT_EQ(CompiledExpression({Operation::MakeILOAD(0, 0)}), result1.expr);
  const CompileExpressionResult result2 =
      CompileExpression(Identifier("a"), Type::DOUBLE, identifiers_by_name);
  EXPECT_EQ(
      CompiledExpression({Operation::MakeILOAD(0, 0), Operation::MakeI2D(0)}),
      result2.expr);
  const CompileExpressionResult result3 =
      CompileExpression(Identifier("a"), Type::BOOL, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(Identifier("b"), Type::INT, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result4.errors);
  const CompileExpressionResult result5 =
      CompileExpression(Identifier("b"), Type::DOUBLE, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type double; found bool"}),
      result5.errors);
  const CompileExpressionResult result6 =
      CompileExpression(Identifier("b"), Type::BOOL, identifiers_by_name);
  EXPECT_EQ(CompiledExpression({Operation::MakeILOAD(1, 0)}), result6.expr);

  const CompileExpressionResult result7 =
      CompileExpression(Identifier("c"), Type::INT, identifiers_by_name);
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0)}), result7.expr);
  const CompileExpressionResult result8 =
      CompileExpression(Identifier("c"), Type::DOUBLE, identifiers_by_name);
  EXPECT_EQ(
      CompiledExpression({Operation::MakeICONST(17, 0), Operation::MakeI2D(0)}),
      result8.expr);
  const CompileExpressionResult result9 =
      CompileExpression(Identifier("c"), Type::BOOL, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result9.errors);

  const CompileExpressionResult result10 =
      CompileExpression(Identifier("d"), Type::INT, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result10.errors);
  const CompileExpressionResult result11 =
      CompileExpression(Identifier("d"), Type::DOUBLE, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type double; found bool"}),
      result11.errors);
  const CompileExpressionResult result12 =
      CompileExpression(Identifier("d"), Type::BOOL, identifiers_by_name);
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(0, 0)}), result12.expr);

  const CompileExpressionResult result13 =
      CompileExpression(Identifier("e"), Type::INT, identifiers_by_name);
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result13.errors);
  const CompileExpressionResult result14 =
      CompileExpression(Identifier("e"), Type::DOUBLE, identifiers_by_name);
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result14.errors);
  const CompileExpressionResult result15 =
      CompileExpression(Identifier("e"), Type::BOOL, identifiers_by_name);
  EXPECT_EQ(std::vector<std::string>({"double variables not supported"}),
            result15.errors);

  const CompileExpressionResult result16 =
      CompileExpression(Identifier("f"), Type::INT, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double"}),
      result16.errors);
  const CompileExpressionResult result17 =
      CompileExpression(Identifier("f"), Type::DOUBLE, identifiers_by_name);
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0)}), result17.expr);
  const CompileExpressionResult result18 =
      CompileExpression(Identifier("f"), Type::BOOL, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result18.errors);

  const CompileExpressionResult result19 =
      CompileExpression(Identifier("g"), Type::INT, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>({"undefined identifier 'g' in expression"}),
      result19.errors);
  const CompileExpressionResult result20 =
      CompileExpression(Identifier("g"), Type::BOOL, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>({"undefined identifier 'g' in expression"}),
      result20.errors);

  const CompileExpressionResult result21 =
      CompileExpression(Identifier("g"), Type::DOUBLE, identifiers_by_name);
  EXPECT_EQ(
      std::vector<std::string>({"undefined identifier 'g' in expression"}),
      result21.errors);
}

TEST(CompileExpressionTest, MinFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::MIN, 17, 42, 4711), Type::INT, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
                 Operation::MakeIMIN(0, 1), Operation::MakeICONST(4711, 1),
                 Operation::MakeIMIN(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::MIN, 0.5, -17), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDMIN(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 =
      CompileExpression(MakeFunctionCall(Function::MIN, true), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0)}), result3.expr);

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::MIN), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"min applied to 0 arguments; expecting at least 1 argument"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeFunctionCall(Function::MIN, 0.5), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double"}),
      result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::MIN, 0.5, true), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types double and bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, MaxFunctionCall) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeFunctionCall(Function::MAX, 17, 42, 4711), Type::INT, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
                 Operation::MakeIMAX(0, 1), Operation::MakeICONST(4711, 1),
                 Operation::MakeIMAX(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::MAX, 0.5, -17), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDMAX(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 =
      CompileExpression(MakeFunctionCall(Function::MAX, true), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0)}), result3.expr);

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::MAX), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"max applied to 0 arguments; expecting at least 1 argument"}),
            result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeFunctionCall(Function::MAX, 0.5), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double"}),
      result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::MAX, 0.5, true), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types double and bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, FloorFunctionCall) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeFunctionCall(Function::FLOOR, 17), Type::INT, {});
  EXPECT_EQ(
      CompiledExpression({Operation::MakeICONST(17, 0), Operation::MakeI2D(0),
                          Operation::MakeFLOOR(0)}),
      result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::FLOOR, 0.5), Type::DOUBLE, {});
  EXPECT_EQ(
      CompiledExpression({Operation::MakeDCONST(0.5, 0),
                          Operation::MakeFLOOR(0), Operation::MakeI2D(0)}),
      result2.expr);

  const CompileExpressionResult result3 =
      CompileExpression(MakeFunctionCall(Function::FLOOR, 0.5), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::FLOOR), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"floor applied to 0 arguments; expecting 1 argument"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::FLOOR, 0.5, -17), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"floor applied to 2 arguments; expecting 1 argument"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeFunctionCall(Function::FLOOR, true), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result6.errors);
}

TEST(CompileExpressionTest, CeilFunctionCall) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeFunctionCall(Function::CEIL, 17), Type::INT, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeI2D(0), Operation::MakeCEIL(0)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::CEIL, 0.5), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeCEIL(0), Operation::MakeI2D(0)}),
            result2.expr);

  const CompileExpressionResult result3 =
      CompileExpression(MakeFunctionCall(Function::CEIL, 0.5), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::CEIL), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"ceil applied to 0 arguments; expecting 1 argument"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::CEIL, 0.5, -17), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"ceil applied to 2 arguments; expecting 1 argument"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeFunctionCall(Function::CEIL, true), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result6.errors);
}

TEST(CompileExpressionTest, PowFunctionCall) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeFunctionCall(Function::POW, 17, 42), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double"}),
      result1.errors);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::POW, 0.5, 2), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(2, 1),
                 Operation::MakeI2D(1), Operation::MakePOW(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::POW, 0.5, 2), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::POW, 0.5), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"pow applied to 1 argument; expecting 2 arguments"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::POW, 0.5, 2, -17), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"pow applied to 3 arguments; expecting 2 arguments"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::POW, 17, true), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeFunctionCall(Function::POW, true, 17), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result7.errors);
}

TEST(CompileExpressionTest, LogFunctionCall) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeFunctionCall(Function::LOG, 17, 42), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double"}),
      result1.errors);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::LOG, 0.5, 2), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(2, 1),
                 Operation::MakeI2D(1), Operation::MakeLOG(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::LOG, 0.5, 2), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::LOG, 0.5), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"log applied to 1 argument; expecting 2 arguments"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::LOG, 0.5, 2, -17), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"log applied to 3 arguments; expecting 2 arguments"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::LOG, 17, true), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeFunctionCall(Function::LOG, true, 17), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type double; found bool"}),
      result7.errors);
}

TEST(CompileExpressionTest, ModFunctionCall) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeFunctionCall(Function::MOD, 17, 42), Type::INT, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeMOD(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, 42), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(17, 0), Operation::MakeICONST(42, 1),
                 Operation::MakeMOD(0, 1), Operation::MakeI2D(0)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, 42), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found int"}),
      result3.errors);

  const CompileExpressionResult result4 =
      CompileExpression(MakeFunctionCall(Function::MOD, 17), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"mod applied to 1 argument; expecting 2 arguments"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, 42, 4711), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"mod applied to 3 arguments; expecting 2 arguments"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeFunctionCall(Function::MOD, 17, true), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting argument of type int; found bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeFunctionCall(Function::MOD, 0.5, 17), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting argument of type int; found double"}),
      result7.errors);
}

TEST(CompileExpressionTest, Negation) {
  const CompileExpressionResult result1 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, Literal::New(17)), Type::INT, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(17, 0), Operation::MakeINEG(0)}),
            result1.expr);

  const CompileExpressionResult result2 =
      CompileExpression(UnaryOperation(UnaryOperator::NEGATE, Literal::New(17)),
                        Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeINEG(0), Operation::MakeI2D(0)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, Literal::New(0.5)), Type::DOUBLE,
      {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeDNEG(0)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, Literal::New(true)), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; unary operator - applied to bool"}),
            result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      UnaryOperation(UnaryOperator::NEGATE, Literal::New(0.5)), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double"}),
      result5.errors);
}

TEST(CompileExpressionTest, LogicalNot) {
  const CompileExpressionResult result1 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, Literal::New(true)), Type::BOOL, {});
  EXPECT_EQ(
      CompiledExpression({Operation::MakeICONST(1, 0), Operation::MakeNOT(0)}),
      result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, Literal::New(17)), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; unary operator ! applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, Literal::New(0.5)), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; unary operator ! applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, Literal::New(true)), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      UnaryOperation(UnaryOperator::NOT, Literal::New(true)), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type double; found bool"}),
      result5.errors);
}

TEST(CompileExpressionTest, Plus) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::PLUS, 17, 42), Type::INT, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeIADD(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::PLUS, 0.5, -17), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDADD(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::PLUS, 0.5, 2.0), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDADD(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::PLUS, 0.5, 2), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::PLUS, 17, true), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator + applied to bool"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::PLUS, true, 17), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator + applied to bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, Minus) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MINUS, 17, 42), Type::INT, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeISUB(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MINUS, 0.5, -17), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDSUB(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MINUS, 0.5, 2.0), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDSUB(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MINUS, 0.5, 2), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MINUS, 17, true), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator - applied to bool"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MINUS, true, 17), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator - applied to bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, Multiply) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MULTIPLY, 17, 42), Type::INT, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeIMUL(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MULTIPLY, 0.5, -17),
                        Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDMUL(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::MULTIPLY, 0.5, 2.0),
                        Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDMUL(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MULTIPLY, 0.5, 2), Type::BOOL, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type bool; found double"}),
      result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MULTIPLY, 17, true), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator * applied to bool"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::MULTIPLY, true, 17), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator * applied to bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, Divide) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::DIVIDE, 17, 42), Type::DOUBLE, {});
  EXPECT_EQ(
      CompiledExpression({Operation::MakeICONST(17, 0),
                          Operation::MakeICONST(42, 1), Operation::MakeI2D(0),
                          Operation::MakeI2D(1), Operation::MakeDDIV(0, 1)}),
      result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::DIVIDE, 0.5, -17), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDDIV(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::DIVIDE, 0.5, 2.0), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDDIV(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::DIVIDE, 0.5, 2), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found double"}),
      result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::DIVIDE, 17, true), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator / applied to bool"}),
            result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::DIVIDE, true, 17), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator / applied to bool"}),
            result6.errors);
}

TEST(CompileExpressionTest, And) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::AND, true, false), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeIFFALSE(0, 3),
                                Operation::MakeICONST(0, 0)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::AND, 17, false), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator & applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::AND, true, 0.5), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator & applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::AND, true, false), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::AND, true, false), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type double; found bool"}),
      result5.errors);
}

TEST(CompileExpressionTest, Or) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::OR, true, false), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeIFTRUE(0, 3),
                                Operation::MakeICONST(0, 0)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::OR, 17, false), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator | applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::OR, true, 0.5), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator | applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::OR, true, false), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::OR, true, false), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type double; found bool"}),
      result5.errors);
}

TEST(CompileExpressionTest, Imply) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IMPLY, true, false), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(1, 0), Operation::MakeNOT(0),
                 Operation::MakeIFTRUE(0, 4), Operation::MakeICONST(0, 0)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IMPLY, 17, false), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator => applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IMPLY, true, 0.5), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator => applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IMPLY, true, false), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result4.errors);

  const CompileExpressionResult result5 =
      CompileExpression(MakeBinaryOperation(BinaryOperator::IMPLY, true, false),
                        Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type double; found bool"}),
      result5.errors);
}

TEST(CompileExpressionTest, Iff) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IFF, true, false), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeICONST(0, 1),
                                Operation::MakeIEQ(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IFF, 17, false), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator <=> applied to int"}),
            result2.errors);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IFF, true, 0.5), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; binary operator <=> applied to double"}),
            result3.errors);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IFF, true, false), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result4.errors);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::IFF, true, false), Type::DOUBLE, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type double; found bool"}),
      result5.errors);
}

TEST(CompileExpressionTest, Less) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS, 17, 42), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeILT(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS, 0.5, -17), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDLT(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS, 0.5, 2.0), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDLT(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS, true, false), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeICONST(0, 1),
                                Operation::MakeILT(0, 1)}),
            result4.expr);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS, 0.5, 2), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS, 17, true), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS, true, 0.5), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, LessEqual) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 17, 42), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeILE(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 0.5, -17), Type::BOOL,
      {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDLE(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 0.5, 2.0), Type::BOOL,
      {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDLE(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, true, false), Type::BOOL,
      {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeICONST(0, 1),
                                Operation::MakeILE(0, 1)}),
            result4.expr);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 0.5, 2), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, 17, true), Type::BOOL,
      {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::LESS_EQUAL, true, 0.5), Type::BOOL,
      {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, GreaterEqual) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 17, 42), Type::BOOL,
      {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeIGE(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 0.5, -17), Type::BOOL,
      {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDGE(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 0.5, 2.0), Type::BOOL,
      {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDGE(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, true, false),
      Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeICONST(0, 1),
                                Operation::MakeIGE(0, 1)}),
            result4.expr);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 0.5, 2), Type::INT,
      {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, 17, true), Type::BOOL,
      {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER_EQUAL, true, 0.5), Type::BOOL,
      {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, Greater) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER, 17, 42), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeIGT(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER, 0.5, -17), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDGT(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER, 0.5, 2.0), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDGT(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER, true, false), Type::BOOL,
      {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeICONST(0, 1),
                                Operation::MakeIGT(0, 1)}),
            result4.expr);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER, 0.5, 2), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER, 17, true), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::GREATER, true, 0.5), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, Equal) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::EQUAL, 17, 42), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeIEQ(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::EQUAL, 0.5, -17), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDEQ(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::EQUAL, 0.5, 2.0), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDEQ(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::EQUAL, true, false), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeICONST(0, 1),
                                Operation::MakeIEQ(0, 1)}),
            result4.expr);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::EQUAL, 0.5, 2), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::EQUAL, 17, true), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::EQUAL, true, 0.5), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, NotEqual) {
  const CompileExpressionResult result1 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 17, 42), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(17, 0),
                                Operation::MakeICONST(42, 1),
                                Operation::MakeINE(0, 1)}),
            result1.expr);

  const CompileExpressionResult result2 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 0.5, -17), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeDCONST(0.5, 0), Operation::MakeICONST(-17, 1),
                 Operation::MakeI2D(1), Operation::MakeDNE(0, 1)}),
            result2.expr);

  const CompileExpressionResult result3 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 0.5, 2.0), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression({Operation::MakeDCONST(0.5, 0),
                                Operation::MakeDCONST(2.0, 1),
                                Operation::MakeDNE(0, 1)}),
            result3.expr);

  const CompileExpressionResult result4 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, true, false), Type::BOOL,
      {});
  EXPECT_EQ(CompiledExpression({Operation::MakeICONST(1, 0),
                                Operation::MakeICONST(0, 1),
                                Operation::MakeINE(0, 1)}),
            result4.expr);

  const CompileExpressionResult result5 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 0.5, 2), Type::INT, {});
  EXPECT_EQ(
      std::vector<std::string>(
          {"type mismatch; expecting expression of type int; found bool"}),
      result5.errors);

  const CompileExpressionResult result6 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, 17, true), Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types int and bool"}),
            result6.errors);

  const CompileExpressionResult result7 = CompileExpression(
      MakeBinaryOperation(BinaryOperator::NOT_EQUAL, true, 0.5), Type::BOOL,
      {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible argument types bool and double"}),
            result7.errors);
}

TEST(CompileExpressionTest, Conditional) {
  const CompileExpressionResult result1 =
      CompileExpression(MakeConditional(true, 17, 42), Type::INT, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(1, 0), Operation::MakeIFFALSE(0, 5),
                 Operation::MakeICONST(17, 0), Operation::MakeNOP(),
                 Operation::MakeGOTO(6), Operation::MakeICONST(42, 0)}),
            result1.expr);

  const CompileExpressionResult result2 =
      CompileExpression(MakeConditional(true, 0.5, 42), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(1, 0), Operation::MakeIFFALSE(0, 4),
                 Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(6),
                 Operation::MakeICONST(42, 0), Operation::MakeI2D(0)}),
            result2.expr);

  const CompileExpressionResult result3 =
      CompileExpression(MakeConditional(true, 17, 0.5), Type::DOUBLE, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(1, 0), Operation::MakeIFFALSE(0, 5),
                 Operation::MakeICONST(17, 0), Operation::MakeI2D(0),
                 Operation::MakeGOTO(6), Operation::MakeDCONST(0.5, 0)}),
            result3.expr);

  const CompileExpressionResult result4 =
      CompileExpression(MakeConditional(true, false, true), Type::BOOL, {});
  EXPECT_EQ(CompiledExpression(
                {Operation::MakeICONST(1, 0), Operation::MakeIFFALSE(0, 4),
                 Operation::MakeICONST(0, 0), Operation::MakeGOTO(5),
                 Operation::MakeICONST(1, 0)}),
            result4.expr);

  const CompileExpressionResult result5 =
      CompileExpression(MakeConditional(4711, 17, 42), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; expecting condition of type bool; found int"}),
            result5.errors);

  const CompileExpressionResult result6 =
      CompileExpression(MakeConditional(true, 0.5, true), Type::DOUBLE, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible branch types double and bool"}),
            result6.errors);

  const CompileExpressionResult result7 =
      CompileExpression(MakeConditional(true, true, 17), Type::INT, {});
  EXPECT_EQ(std::vector<std::string>(
                {"type mismatch; incompatible branch types bool and int"}),
            result7.errors);
}

TEST(CompileExpressionTest, ProbabilityThresholdOperation) {
  const ProbabilityThresholdOperation expr(
      ProbabilityThresholdOperator::LESS, 0.25,
      UntilProperty::New(0, std::numeric_limits<double>::infinity(),
                         Literal::New(true), Identifier::New("a")));
  const CompileExpressionResult result =
      CompileExpression(expr, Type::BOOL, {});
  EXPECT_EQ(std::vector<std::string>(
                {"unexpected probability threshold operation in expression"}),
            result.errors);
}

TEST(OptimizeIntExpressionTest, Constant) {
  const CompiledExpression expr({Operation::MakeICONST(17, 0)});
  EXPECT_EQ(expr, OptimizeIntExpression(expr));
}

TEST(OptimizeIntExpressionTest, Variable) {
  const CompiledExpression expr({Operation::MakeILOAD(0, 0)});
  EXPECT_EQ(expr, OptimizeIntExpression(expr));
}

TEST(OptimizeIntExpressionTest, ConstantNegation) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(17, 0), Operation::MakeINEG(0)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(-17, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(-42, 0), Operation::MakeINEG(0)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, MultiNegation) {
  const CompiledExpression expr1(
      {Operation::MakeILOAD(0, 0), Operation::MakeINEG(0),
       Operation::MakeILOAD(1, 1), Operation::MakeIADD(0, 1),
       Operation::MakeINEG(0)});
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
       Operation::MakeINEG(0), Operation::MakeINEG(0)});
  const std::vector<Operation> expected2 = {
      Operation::MakeILOAD(0, 0), Operation::MakeINEG(0),
      Operation::MakeILOAD(1, 1), Operation::MakeIADD(0, 1),
      Operation::MakeINEG(0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantLogicalNot) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(true, 0), Operation::MakeNOT(0)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(false, 0), Operation::MakeNOT(0)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, MultiLogicalNot) {
  const CompiledExpression expr1(
      {Operation::MakeILOAD(0, 0), Operation::MakeNOT(0),
       Operation::MakeIFFALSE(0, 5), Operation::MakeILOAD(1, 0),
       Operation::MakeNOT(0), Operation::MakeNOT(0)});
  const std::vector<Operation> expected1 = {
      Operation::MakeILOAD(0, 0),   Operation::MakeNOT(0),
      Operation::MakeIFFALSE(0, 5), Operation::MakeILOAD(1, 0),
      Operation::MakeNOT(0),        Operation::MakeNOT(0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeILOAD(0, 0), Operation::MakeNOT(0),
       Operation::MakeIFFALSE(0, 6), Operation::MakeILOAD(1, 0),
       Operation::MakeNOT(0), Operation::MakeNOT(0), Operation::MakeNOT(0),
       Operation::MakeNOT(0), Operation::MakeNOT(0)});
  const std::vector<Operation> expected2 = {
      Operation::MakeILOAD(0, 0), Operation::MakeNOT(0),
      Operation::MakeIFFALSE(0, 4), Operation::MakeILOAD(1, 0),
      Operation::MakeNOT(0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantAddition) {
  const CompiledExpression expr({Operation::MakeICONST(17, 0),
                                 Operation::MakeICONST(42, 1),
                                 Operation::MakeIADD(0, 1)});
  const std::vector<Operation> expected = {Operation::MakeICONST(17 + 42, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantSubtraction) {
  const CompiledExpression expr({Operation::MakeICONST(17, 0),
                                 Operation::MakeICONST(42, 1),
                                 Operation::MakeISUB(0, 1)});
  const std::vector<Operation> expected = {Operation::MakeICONST(17 - 42, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantMultiplication) {
  const CompiledExpression expr({Operation::MakeICONST(17, 0),
                                 Operation::MakeICONST(42, 1),
                                 Operation::MakeIMUL(0, 1)});
  const std::vector<Operation> expected = {Operation::MakeICONST(17 * 42, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntEqual) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIEQ(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIEQ(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeIEQ(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleEqual) {
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDEQ(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDEQ(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDEQ(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntNotEqual) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeINE(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeINE(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeINE(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleNotEqual) {
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDNE(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDNE(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDNE(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntLess) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeILT(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeILT(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeILT(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleLess) {
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDLT(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDLT(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDLT(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntLessEqual) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeILE(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeILE(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeILE(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleLessEqual) {
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDLE(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDLE(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDLE(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntGreaterEqual) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIGE(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIGE(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeIGE(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleGreaterEqual) {
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDGE(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDGE(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDGE(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIntGreater) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIGT(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeIGT(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeIGT(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantDoubleGreater) {
  const CompiledExpression expr1({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeDGT(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeDCONST(0.25, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDGT(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(false, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(0.25, 1),
                                  Operation::MakeDGT(0, 1)});
  const std::vector<Operation> expected3 = {Operation::MakeICONST(true, 0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
}

TEST(OptimizeIntExpressionTest, ConstantMin) {
  const CompiledExpression expr({Operation::MakeICONST(17, 0),
                                 Operation::MakeICONST(42, 1),
                                 Operation::MakeIMIN(0, 1)});
  const std::vector<Operation> expected = {Operation::MakeICONST(17, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantMax) {
  const CompiledExpression expr({Operation::MakeICONST(17, 0),
                                 Operation::MakeICONST(42, 1),
                                 Operation::MakeIMAX(0, 1)});
  const std::vector<Operation> expected = {Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected, OptimizeIntExpression(expr).operations());
}

TEST(OptimizeIntExpressionTest, ConstantFloor) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeFLOOR(0)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(0, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.5, 0), Operation::MakeFLOOR(0)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(-1, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantCeil) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeCEIL(0)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(1, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.5, 0), Operation::MakeCEIL(0)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(0, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantMod) {
  const CompiledExpression expr1({Operation::MakeICONST(17, 0),
                                  Operation::MakeICONST(42, 1),
                                  Operation::MakeMOD(0, 1)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(17 % 42, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeICONST(42, 0),
                                  Operation::MakeICONST(17, 1),
                                  Operation::MakeMOD(0, 1)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(42 % 17, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIfFalse) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(17, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, ConstantIfTrue) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(17, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeICONST(17, 0), Operation::MakeGOTO(5),
       Operation::MakeICONST(42, 0)});
  const std::vector<Operation> expected2 = {Operation::MakeICONST(42, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeIntExpressionTest, EliminatesDeadCode) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeICONST(17, 0)});
  const std::vector<Operation> expected1 = {Operation::MakeICONST(17, 0)};
  EXPECT_EQ(expected1, OptimizeIntExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeILOAD(0, 0)});
  const std::vector<Operation> expected2 = {Operation::MakeILOAD(0, 0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeILOAD(0, 0),
       Operation::MakeINEG(0)});
  const std::vector<Operation> expected3 = {Operation::MakeILOAD(0, 0),
                                            Operation::MakeINEG(0)};
  EXPECT_EQ(expected3, OptimizeIntExpression(expr3).operations());
  const CompiledExpression expr4(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeILOAD(0, 0),
       Operation::MakeIADD(0, 1)});
  const std::vector<Operation> expected4 = {Operation::MakeICONST(42, 1),
                                            Operation::MakeILOAD(0, 0),
                                            Operation::MakeIADD(0, 1)};
  EXPECT_EQ(expected4, OptimizeIntExpression(expr4).operations());
  const CompiledExpression expr5(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeICONST(42, 1), Operation::MakeILOAD(0, 0),
       Operation::MakeI2D(0), Operation::MakeFLOOR(0)});
  const std::vector<Operation> expected5 = {
      Operation::MakeILOAD(0, 0), Operation::MakeI2D(0),
      Operation::MakeFLOOR(0),
  };
  EXPECT_EQ(expected5, OptimizeIntExpression(expr5).operations());
  const CompiledExpression expr6(
      {Operation::MakeICONST(4711, 0), Operation::MakeDCONST(0.5, 0),
       Operation::MakeDCONST(0.5, 1), Operation::MakeILOAD(0, 0),
       Operation::MakeDCONST(0.25, 0), Operation::MakeI2D(0),
       Operation::MakeDEQ(0, 1)});
  const std::vector<Operation> expected6 = {
      Operation::MakeDCONST(0.5, 1), Operation::MakeILOAD(0, 0),
      Operation::MakeI2D(0), Operation::MakeDEQ(0, 1)};
  EXPECT_EQ(expected6, OptimizeIntExpression(expr6).operations());
}

TEST(OptimizeDoubleExpressionTest, Constant) {
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0)});
  EXPECT_EQ(expr, OptimizeDoubleExpression(expr));
}

TEST(OptimizeDoubleExpressionTest, ConstantConversion) {
  const CompiledExpression expr(
      {Operation::MakeICONST(17, 0), Operation::MakeI2D(0)});
  const std::vector<Operation> expected = {Operation::MakeDCONST(17.0, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantNegation) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.5, 0), Operation::MakeDNEG(0)});
  const std::vector<Operation> expected1 = {Operation::MakeDCONST(-0.5, 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(-0.25, 0), Operation::MakeDNEG(0)});
  const std::vector<Operation> expected2 = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, MultiNegation) {
  const CompiledExpression expr1(
      {Operation::MakeILOAD(0, 0), Operation::MakeI2D(0),
       Operation::MakeDNEG(0), Operation::MakeILOAD(1, 1),
       Operation::MakeI2D(1), Operation::MakeDADD(0, 1), Operation::MakeDNEG(0),
       Operation::MakeCEIL(0)});
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
       Operation::MakeDNEG(0), Operation::MakeCEIL(0)});
  const std::vector<Operation> expected2 = {
      Operation::MakeILOAD(0, 0), Operation::MakeI2D(0),
      Operation::MakeDNEG(0),     Operation::MakeILOAD(1, 1),
      Operation::MakeI2D(1),      Operation::MakeDADD(0, 1),
      Operation::MakeDNEG(0),     Operation::MakeCEIL(0)};
  EXPECT_EQ(expected2, OptimizeIntExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantAddition) {
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDADD(0, 1)});
  const std::vector<Operation> expected = {
      Operation::MakeDCONST(0.5 + 0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantSubtraction) {
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDSUB(0, 1)});
  const std::vector<Operation> expected = {
      Operation::MakeDCONST(0.5 - 0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantMultiplication) {
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDMUL(0, 1)});
  const std::vector<Operation> expected = {
      Operation::MakeDCONST(0.5 * 0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantDivision) {
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDDIV(0, 1)});
  const std::vector<Operation> expected = {
      Operation::MakeDCONST(0.5 / 0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantMin) {
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDMIN(0, 1)});
  const std::vector<Operation> expected = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantMax) {
  const CompiledExpression expr({Operation::MakeDCONST(0.5, 0),
                                 Operation::MakeDCONST(0.25, 1),
                                 Operation::MakeDMAX(0, 1)});
  const std::vector<Operation> expected = {Operation::MakeDCONST(0.5, 0)};
  EXPECT_EQ(expected, OptimizeDoubleExpression(expr).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantPow) {
  const CompiledExpression expr1({Operation::MakeDCONST(4.0, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakePOW(0, 1)});
  const std::vector<Operation> expected1 = {
      Operation::MakeDCONST(pow(4.0, 0.5), 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(4.0, 1),
                                  Operation::MakePOW(0, 1)});
  const std::vector<Operation> expected2 = {
      Operation::MakeDCONST(pow(0.5, 4.0), 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantLog) {
  const CompiledExpression expr1({Operation::MakeDCONST(4.0, 0),
                                  Operation::MakeDCONST(0.5, 1),
                                  Operation::MakeLOG(0, 1)});
  const std::vector<Operation> expected1 = {
      Operation::MakeDCONST(log(4.0) / log(0.5), 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2({Operation::MakeDCONST(0.5, 0),
                                  Operation::MakeDCONST(4.0, 1),
                                  Operation::MakeLOG(0, 1)});
  const std::vector<Operation> expected2 = {
      Operation::MakeDCONST(log(0.5) / log(4.0), 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantIfFalse) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(5),
       Operation::MakeDCONST(0.25, 0)});
  const std::vector<Operation> expected1 = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFFALSE(0, 4),
       Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(5),
       Operation::MakeDCONST(0.25, 0)});
  const std::vector<Operation> expected2 = {Operation::MakeDCONST(0.5, 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, ConstantIfTrue) {
  const CompiledExpression expr1(
      {Operation::MakeICONST(false, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(5),
       Operation::MakeDCONST(0.25, 0)});
  const std::vector<Operation> expected1 = {Operation::MakeDCONST(0.5, 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeICONST(true, 0), Operation::MakeIFTRUE(0, 4),
       Operation::MakeDCONST(0.5, 0), Operation::MakeGOTO(5),
       Operation::MakeDCONST(0.25, 0)});
  const std::vector<Operation> expected2 = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
}

TEST(OptimizeDoubleExpressionTest, EliminatesDeadCode) {
  const CompiledExpression expr1(
      {Operation::MakeDCONST(0.125, 0), Operation::MakeICONST(42, 0),
       Operation::MakeDCONST(0.5, 1), Operation::MakeDCONST(0.25, 0)});
  const std::vector<Operation> expected1 = {Operation::MakeDCONST(0.25, 0)};
  EXPECT_EQ(expected1, OptimizeDoubleExpression(expr1).operations());
  const CompiledExpression expr2(
      {Operation::MakeDCONST(0.125, 0), Operation::MakeICONST(42, 0),
       Operation::MakeILOAD(0, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeI2D(0), Operation::MakeDNEG(0)});
  const std::vector<Operation> expected2 = {Operation::MakeILOAD(0, 0),
                                            Operation::MakeI2D(0),
                                            Operation::MakeDNEG(0)};
  EXPECT_EQ(expected2, OptimizeDoubleExpression(expr2).operations());
  const CompiledExpression expr3(
      {Operation::MakeDCONST(0.125, 0), Operation::MakeICONST(42, 0),
       Operation::MakeILOAD(0, 0), Operation::MakeDCONST(0.5, 1),
       Operation::MakeI2D(0), Operation::MakeDADD(0, 1)});
  const std::vector<Operation> expected3 = {
      Operation::MakeILOAD(0, 0), Operation::MakeDCONST(0.5, 1),
      Operation::MakeI2D(0), Operation::MakeDADD(0, 1)};
  EXPECT_EQ(expected3, OptimizeDoubleExpression(expr3).operations());
}

}  // namespace
