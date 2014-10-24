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

#include "expression.h"

#include "strutil.h"
#include "unique-ptr-vector.h"

#include "gtest/gtest.h"

namespace {

UniquePtrVector<const Expression> MakeArguments(
    std::unique_ptr<const Expression>&& argument) {
  return UniquePtrVector<const Expression>(std::move(argument));
}

UniquePtrVector<const Expression> MakeArguments(
    std::unique_ptr<const Expression>&& argument1,
    std::unique_ptr<const Expression>&& argument2) {
  return UniquePtrVector<const Expression>(std::move(argument1),
                                           std::move(argument2));
}

TEST(LiteralTest, Constructor) {
  const Literal a(17), b(0.5), c(true), d(false);
  EXPECT_EQ(17, a.value().value<int>());
  EXPECT_EQ(Type::INT, a.value().type());
  EXPECT_EQ(0.5, b.value().value<double>());
  EXPECT_EQ(Type::DOUBLE, b.value().type());
  EXPECT_TRUE(c.value().value<bool>());
  EXPECT_EQ(Type::BOOL, c.value().type());
  EXPECT_FALSE(d.value().value<bool>());
  EXPECT_EQ(Type::BOOL, d.value().type());
}

TEST(LiteralTest, Output) {
  const Literal a(17), b(0.5), c(true), d(false);
  EXPECT_EQ("17;0.5;true;false", StrCat(a, ';', b, ';', c, ';', d));
}

TEST(IdentifierTest, Constructor) {
  const Identifier a("foo"), b("bar"), c("baz");
  EXPECT_EQ("foo", a.name());
  EXPECT_EQ("bar", b.name());
  EXPECT_EQ("baz", c.name());
}

TEST(IdentifierTest, Output) {
  const Identifier a("foo"), b("bar"), c("baz");
  EXPECT_EQ("foo;bar;baz", StrCat(a, ';', b, ';', c));
}

TEST(FunctionCallTest, OutputUnknownFunction) {
  const FunctionCall expr1(Function::UNKNOWN, {});
  EXPECT_EQ("<<unknown function>>()", StrCat(expr1));
}

TEST(FunctionCallTest, OutputMin) {
  const FunctionCall expr1(Function::MIN, MakeArguments(Literal::New(17)));
  const FunctionCall expr2(Function::MIN,
                           MakeArguments(Literal::New(42), Literal::New(0.5)));
  EXPECT_EQ("min(17);min(42, 0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputMax) {
  const FunctionCall expr1(Function::MAX, MakeArguments(Literal::New(17)));
  const FunctionCall expr2(Function::MAX,
                           MakeArguments(Literal::New(42), Literal::New(0.5)));
  EXPECT_EQ("max(17);max(42, 0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputFloor) {
  const FunctionCall expr1(Function::FLOOR, MakeArguments(Literal::New(17)));
  const FunctionCall expr2(Function::FLOOR, MakeArguments(Literal::New(0.5)));
  EXPECT_EQ("floor(17);floor(0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputCeil) {
  const FunctionCall expr1(Function::CEIL, MakeArguments(Literal::New(17)));
  const FunctionCall expr2(Function::CEIL, MakeArguments(Literal::New(0.5)));
  EXPECT_EQ("ceil(17);ceil(0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputPow) {
  const FunctionCall expr1(Function::POW,
                           MakeArguments(Literal::New(17), Literal::New(-0.5)));
  const FunctionCall expr2(Function::POW,
                           MakeArguments(Literal::New(42), Literal::New(0.5)));
  EXPECT_EQ("pow(17, -0.5);pow(42, 0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputLog) {
  const FunctionCall expr1(Function::LOG,
                           MakeArguments(Literal::New(17), Literal::New(2.5)));
  const FunctionCall expr2(Function::LOG,
                           MakeArguments(Literal::New(42), Literal::New(0.5)));
  EXPECT_EQ("log(17, 2.5);log(42, 0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputMod) {
  const FunctionCall expr1(Function::MOD,
                           MakeArguments(Literal::New(17), Literal::New(42)));
  const FunctionCall expr2(Function::MOD,
                           MakeArguments(Literal::New(42), Literal::New(-17)));
  EXPECT_EQ("mod(17, 42);mod(42, -17)", StrCat(expr1, ';', expr2));
}

TEST(UnaryOperationTest, OutputNegation) {
  const UnaryOperation expr1(
      UnaryOperator::NEGATE,
      BinaryOperation::New(
          BinaryOperator::PLUS, Literal::New(17),
          UnaryOperation::New(UnaryOperator::NEGATE, Identifier::New("b"))));
  const UnaryOperation expr2(
      UnaryOperator::NEGATE,
      BinaryOperation::New(
          BinaryOperator::MINUS, Literal::New(17),
          UnaryOperation::New(UnaryOperator::NEGATE, Identifier::New("b"))));
  const UnaryOperation expr3(
      UnaryOperator::NEGATE,
      BinaryOperation::New(
          BinaryOperator::MULTIPLY, Literal::New(17),
          UnaryOperation::New(UnaryOperator::NEGATE, Identifier::New("b"))));
  const UnaryOperation expr4(
      UnaryOperator::NEGATE,
      BinaryOperation::New(
          BinaryOperator::DIVIDE, Literal::New(17),
          UnaryOperation::New(UnaryOperator::NEGATE, Identifier::New("b"))));
  EXPECT_EQ("-(17 + -b);-(17 - -b);-(17 * -b);-(17 / -b)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(UnaryOperationTest, OutputLogicalNot) {
  const UnaryOperation expr1(
      UnaryOperator::NOT,
      BinaryOperation::New(
          BinaryOperator::AND, Literal::New(true),
          UnaryOperation::New(UnaryOperator::NOT, Identifier::New("b"))));
  const UnaryOperation expr2(
      UnaryOperator::NOT,
      BinaryOperation::New(
          BinaryOperator::OR, Literal::New(true),
          UnaryOperation::New(UnaryOperator::NOT, Identifier::New("b"))));
  const UnaryOperation expr3(
      UnaryOperator::NOT,
      BinaryOperation::New(
          BinaryOperator::IMPLY, Literal::New(true),
          UnaryOperation::New(UnaryOperator::NOT, Identifier::New("b"))));
  const UnaryOperation expr4(
      UnaryOperator::NOT,
      BinaryOperation::New(
          BinaryOperator::IFF, Literal::New(true),
          UnaryOperation::New(UnaryOperator::NOT, Identifier::New("b"))));
  EXPECT_EQ("!(true & !b);!(true | !b);!(true => !b);!(true <=> !b)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputAddition) {
  const BinaryOperation expr1(
      BinaryOperator::PLUS,
      BinaryOperation::New(BinaryOperator::PLUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MINUS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::PLUS,
      BinaryOperation::New(BinaryOperator::MINUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MULTIPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::PLUS,
      BinaryOperation::New(BinaryOperator::MULTIPLY, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::DIVIDE, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::PLUS,
      BinaryOperation::New(BinaryOperator::DIVIDE, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::PLUS, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("17 + b + c - d;17 - b + c * d;17 * b + c / d;17 / b + c + d",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputSubtraction) {
  const BinaryOperation expr1(
      BinaryOperator::MINUS,
      BinaryOperation::New(BinaryOperator::PLUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MINUS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::MINUS,
      BinaryOperation::New(BinaryOperator::MINUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MULTIPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::MINUS,
      BinaryOperation::New(BinaryOperator::MULTIPLY, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::DIVIDE, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::MINUS,
      BinaryOperation::New(BinaryOperator::DIVIDE, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::PLUS, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("17 + b - (c - d);17 - b - c * d;17 * b - c / d;17 / b - (c + d)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputMultiplication) {
  const BinaryOperation expr1(
      BinaryOperator::MULTIPLY,
      BinaryOperation::New(BinaryOperator::PLUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MINUS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::MULTIPLY,
      BinaryOperation::New(BinaryOperator::MINUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MULTIPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::MULTIPLY,
      BinaryOperation::New(BinaryOperator::MULTIPLY, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::DIVIDE, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::MULTIPLY,
      BinaryOperation::New(BinaryOperator::DIVIDE, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::PLUS, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ(
      "(17 + b) * (c - d);(17 - b) * c * d;17 * b * c / d;17 / b * (c + d)",
      StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputDivision) {
  const BinaryOperation expr1(
      BinaryOperator::DIVIDE,
      BinaryOperation::New(BinaryOperator::PLUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MINUS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::DIVIDE,
      BinaryOperation::New(BinaryOperator::MINUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MULTIPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::DIVIDE,
      BinaryOperation::New(BinaryOperator::MULTIPLY, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::DIVIDE, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::DIVIDE,
      BinaryOperation::New(BinaryOperator::DIVIDE, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::PLUS, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ(
      "(17 + b) / (c - d);(17 - b) / (c * d);17 * b / (c / d);17 / b / (c + d)",
      StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputAnd) {
  const BinaryOperation expr1(
      BinaryOperator::AND,
      BinaryOperation::New(BinaryOperator::AND, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::OR, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::AND,
      BinaryOperation::New(BinaryOperator::OR, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IMPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::AND,
      BinaryOperation::New(BinaryOperator::IMPLY, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IFF, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::AND,
      BinaryOperation::New(BinaryOperator::IFF, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::AND, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("true & b & (c | d);(true | b) & (c => d)",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("(true => b) & (c <=> d);(true <=> b) & c & d",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputOr) {
  const BinaryOperation expr1(
      BinaryOperator::OR,
      BinaryOperation::New(BinaryOperator::AND, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::OR, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::OR,
      BinaryOperation::New(BinaryOperator::OR, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IMPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::OR,
      BinaryOperation::New(BinaryOperator::IMPLY, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IFF, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::OR,
      BinaryOperation::New(BinaryOperator::IFF, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::AND, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("true & b | c | d;true | b | (c => d)", StrCat(expr1, ';', expr2));
  EXPECT_EQ("(true => b) | (c <=> d);(true <=> b) | c & d",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputImply) {
  const BinaryOperation expr1(
      BinaryOperator::IMPLY,
      BinaryOperation::New(BinaryOperator::AND, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::OR, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::IMPLY,
      BinaryOperation::New(BinaryOperator::OR, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IMPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::IMPLY,
      BinaryOperation::New(BinaryOperator::IMPLY, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IFF, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::IMPLY,
      BinaryOperation::New(BinaryOperator::IFF, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::AND, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("true & b => c | d;true | b => (c => d)",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("true => b => (c <=> d);(true <=> b) => c & d",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputIff) {
  const BinaryOperation expr1(
      BinaryOperator::IFF,
      BinaryOperation::New(BinaryOperator::AND, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::OR, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::IFF,
      BinaryOperation::New(BinaryOperator::OR, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IMPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::IFF,
      BinaryOperation::New(BinaryOperator::IMPLY, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IFF, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::IFF,
      BinaryOperation::New(BinaryOperator::IFF, Literal::New(true),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::AND, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("true & b <=> c | d;true | b <=> c => d",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("true => b <=> (c <=> d);true <=> b <=> c & d",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputLess) {
  const BinaryOperation expr1(
      BinaryOperator::LESS,
      BinaryOperation::New(BinaryOperator::PLUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MULTIPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::LESS,
      BinaryOperation::New(BinaryOperator::GREATER_EQUAL, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::EQUAL, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::LESS,
      BinaryOperation::New(BinaryOperator::MINUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::DIVIDE, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::LESS,
      BinaryOperation::New(BinaryOperator::GREATER, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::NOT_EQUAL, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("17 + b < c * d;17 >= b < (c = d)", StrCat(expr1, ';', expr2));
  EXPECT_EQ("17 - b < c / d;17 > b < (c != d)", StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputLessEqual) {
  const BinaryOperation expr1(
      BinaryOperator::LESS_EQUAL,
      BinaryOperation::New(BinaryOperator::MINUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::DIVIDE, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::LESS_EQUAL,
      BinaryOperation::New(BinaryOperator::GREATER, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::NOT_EQUAL, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::LESS_EQUAL,
      BinaryOperation::New(BinaryOperator::AND, Literal::New(false),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::OR, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::LESS_EQUAL,
      BinaryOperation::New(BinaryOperator::EQUAL, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::LESS, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("17 - b <= c / d;17 > b <= (c != d)", StrCat(expr1, ';', expr2));
  EXPECT_EQ("(false & b) <= (c | d);(17 = b) <= (c < d)",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputGreaterEqual) {
  const BinaryOperation expr1(
      BinaryOperator::GREATER_EQUAL,
      BinaryOperation::New(BinaryOperator::AND, Literal::New(false),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::OR, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::GREATER_EQUAL,
      BinaryOperation::New(BinaryOperator::EQUAL, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::LESS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::GREATER_EQUAL,
      BinaryOperation::New(BinaryOperator::IMPLY, Literal::New(false),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IFF, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::GREATER_EQUAL,
      BinaryOperation::New(BinaryOperator::NOT_EQUAL, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::LESS_EQUAL, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("(false & b) >= (c | d);(17 = b) >= (c < d)",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("(false => b) >= (c <=> d);(17 != b) >= (c <= d)",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputGreater) {
  const BinaryOperation expr1(
      BinaryOperator::GREATER,
      BinaryOperation::New(BinaryOperator::IMPLY, Literal::New(false),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::IFF, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::GREATER,
      BinaryOperation::New(BinaryOperator::NOT_EQUAL, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::LESS_EQUAL, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::GREATER,
      BinaryOperation::New(BinaryOperator::MULTIPLY, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::PLUS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::GREATER,
      BinaryOperation::New(BinaryOperator::LESS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::GREATER_EQUAL, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("(false => b) > (c <=> d);(17 != b) > (c <= d)",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("17 * b > c + d;17 < b > (c >= d)", StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputEqual) {
  const BinaryOperation expr1(
      BinaryOperator::EQUAL,
      BinaryOperation::New(BinaryOperator::MULTIPLY, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::PLUS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::EQUAL,
      BinaryOperation::New(BinaryOperator::LESS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::GREATER_EQUAL, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::EQUAL,
      BinaryOperation::New(BinaryOperator::DIVIDE, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MINUS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::EQUAL,
      BinaryOperation::New(BinaryOperator::LESS_EQUAL, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::GREATER, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("17 * b = c + d;17 < b = c >= d", StrCat(expr1, ';', expr2));
  EXPECT_EQ("17 / b = c - d;17 <= b = c > d", StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputNotEqual) {
  const BinaryOperation expr1(
      BinaryOperator::NOT_EQUAL,
      BinaryOperation::New(BinaryOperator::DIVIDE, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MINUS, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr2(
      BinaryOperator::NOT_EQUAL,
      BinaryOperation::New(BinaryOperator::LESS_EQUAL, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::GREATER, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr3(
      BinaryOperator::NOT_EQUAL,
      BinaryOperation::New(BinaryOperator::PLUS, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::MULTIPLY, Identifier::New("c"),
                           Identifier::New("d")));
  const BinaryOperation expr4(
      BinaryOperator::NOT_EQUAL,
      BinaryOperation::New(BinaryOperator::GREATER_EQUAL, Literal::New(17),
                           Identifier::New("b")),
      BinaryOperation::New(BinaryOperator::EQUAL, Identifier::New("c"),
                           Identifier::New("d")));
  EXPECT_EQ("17 / b != c - d;17 <= b != c > d", StrCat(expr1, ';', expr2));
  EXPECT_EQ("17 + b != c * d;17 >= b != (c = d)", StrCat(expr3, ';', expr4));
}

TEST(ConditionalTest, OutputConditional) {
  const Conditional expr1(
      Literal::New(true),
      Conditional::New(
          BinaryOperation::New(BinaryOperator::PLUS, Identifier::New("a"),
                               Identifier::New("b")),
          UnaryOperation::New(
              UnaryOperator::NEGATE,
              FunctionCall::New(Function::CEIL,
                                MakeArguments(Identifier::New("c")))),
          Identifier::New("d")),
      BinaryOperation::New(
          BinaryOperator::PLUS,
          Conditional::New(Literal::New(false), Identifier::New("e"),
                           Identifier::New("f")),
          Literal::New(42)));
  EXPECT_EQ("true ? a + b ? -ceil(c) : d : (false ? e : f) + 42",
            StrCat(expr1));
}

TEST(ProbabilityThresholdOperationTest, OutputProbabilityThresholdOperation) {
  const ProbabilityThresholdOperation expr1(
      ProbabilityThresholdOperator::LESS, 0.25,
      UntilProperty::New(0, std::numeric_limits<double>::infinity(),
                         Literal::New(true), Identifier::New("a")));
  const ProbabilityThresholdOperation expr2(
      ProbabilityThresholdOperator::LESS_EQUAL, 0.5,
      UntilProperty::New(0.5, 17, Literal::New(true), Identifier::New("b")));
  const ProbabilityThresholdOperation expr3(
      ProbabilityThresholdOperator::GREATER_EQUAL, 0.75,
      UntilProperty::New(0, 42, Identifier::New("c"), Identifier::New("d")));
  const ProbabilityThresholdOperation expr4(
      ProbabilityThresholdOperator::GREATER, 1,
      UntilProperty::New(4711, std::numeric_limits<double>::infinity(),
                         Literal::New(false), Literal::New(true)));
  EXPECT_EQ("P<0.25[ true U a ];P<=0.5[ true U[0.5,17] b ]",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("P>=0.75[ c U<=42 d ];P>1[ false U>=4711 true ]",
            StrCat(expr3, ';', expr4));
}

}  // namespace
