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

#include <limits>

#include "ptrutil.h"
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

TEST(LabelTest, Constructor) {
  const Label a("foo"), b("bar"), c("baz");
  EXPECT_EQ("foo", a.name());
  EXPECT_EQ("bar", b.name());
  EXPECT_EQ("baz", c.name());
}

TEST(LabelTest, Output) {
  const Label a("foo"), b("bar"), c("baz");
  EXPECT_EQ("foo;bar;baz", StrCat(a, ';', b, ';', c));
}

TEST(FunctionCallTest, OutputUnknownFunction) {
  const FunctionCall expr1(Function::UNKNOWN, {});
  EXPECT_EQ("<<unknown function>>()", StrCat(expr1));
}

TEST(FunctionCallTest, OutputMin) {
  const FunctionCall expr1(Function::MIN,
                           MakeArguments(MakeUnique<Literal>(17)));
  const FunctionCall expr2(
      Function::MIN,
      MakeArguments(MakeUnique<Literal>(42), MakeUnique<Literal>(0.5)));
  EXPECT_EQ("min(17);min(42, 0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputMax) {
  const FunctionCall expr1(Function::MAX,
                           MakeArguments(MakeUnique<Literal>(17)));
  const FunctionCall expr2(
      Function::MAX,
      MakeArguments(MakeUnique<Literal>(42), MakeUnique<Literal>(0.5)));
  EXPECT_EQ("max(17);max(42, 0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputFloor) {
  const FunctionCall expr1(Function::FLOOR,
                           MakeArguments(MakeUnique<Literal>(17)));
  const FunctionCall expr2(Function::FLOOR,
                           MakeArguments(MakeUnique<Literal>(0.5)));
  EXPECT_EQ("floor(17);floor(0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputCeil) {
  const FunctionCall expr1(Function::CEIL,
                           MakeArguments(MakeUnique<Literal>(17)));
  const FunctionCall expr2(Function::CEIL,
                           MakeArguments(MakeUnique<Literal>(0.5)));
  EXPECT_EQ("ceil(17);ceil(0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputPow) {
  const FunctionCall expr1(
      Function::POW,
      MakeArguments(MakeUnique<Literal>(17), MakeUnique<Literal>(-0.5)));
  const FunctionCall expr2(
      Function::POW,
      MakeArguments(MakeUnique<Literal>(42), MakeUnique<Literal>(0.5)));
  EXPECT_EQ("pow(17, -0.5);pow(42, 0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputLog) {
  const FunctionCall expr1(
      Function::LOG,
      MakeArguments(MakeUnique<Literal>(17), MakeUnique<Literal>(2.5)));
  const FunctionCall expr2(
      Function::LOG,
      MakeArguments(MakeUnique<Literal>(42), MakeUnique<Literal>(0.5)));
  EXPECT_EQ("log(17, 2.5);log(42, 0.5)", StrCat(expr1, ';', expr2));
}

TEST(FunctionCallTest, OutputMod) {
  const FunctionCall expr1(
      Function::MOD,
      MakeArguments(MakeUnique<Literal>(17), MakeUnique<Literal>(42)));
  const FunctionCall expr2(
      Function::MOD,
      MakeArguments(MakeUnique<Literal>(42), MakeUnique<Literal>(-17)));
  EXPECT_EQ("mod(17, 42);mod(42, -17)", StrCat(expr1, ';', expr2));
}

TEST(UnaryOperationTest, OutputNegation) {
  const UnaryOperation expr1(
      UnaryOperator::NEGATE,
      MakeUnique<BinaryOperation>(
          BinaryOperator::PLUS, MakeUnique<Literal>(17),
          MakeUnique<UnaryOperation>(UnaryOperator::NEGATE,
                                     MakeUnique<Identifier>("b"))));
  const UnaryOperation expr2(
      UnaryOperator::NEGATE,
      MakeUnique<BinaryOperation>(
          BinaryOperator::MINUS, MakeUnique<Literal>(17),
          MakeUnique<UnaryOperation>(UnaryOperator::NEGATE,
                                     MakeUnique<Identifier>("b"))));
  const UnaryOperation expr3(
      UnaryOperator::NEGATE,
      MakeUnique<BinaryOperation>(
          BinaryOperator::MULTIPLY, MakeUnique<Literal>(17),
          MakeUnique<UnaryOperation>(UnaryOperator::NEGATE,
                                     MakeUnique<Identifier>("b"))));
  const UnaryOperation expr4(
      UnaryOperator::NEGATE,
      MakeUnique<BinaryOperation>(
          BinaryOperator::DIVIDE, MakeUnique<Literal>(17),
          MakeUnique<UnaryOperation>(UnaryOperator::NEGATE,
                                     MakeUnique<Identifier>("b"))));
  EXPECT_EQ("-(17 + -b);-(17 - -b);-(17 * -b);-(17 / -b)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(UnaryOperationTest, OutputLogicalNot) {
  const UnaryOperation expr1(
      UnaryOperator::NOT,
      MakeUnique<BinaryOperation>(
          BinaryOperator::AND, MakeUnique<Literal>(true),
          MakeUnique<UnaryOperation>(UnaryOperator::NOT,
                                     MakeUnique<Identifier>("b"))));
  const UnaryOperation expr2(
      UnaryOperator::NOT,
      MakeUnique<BinaryOperation>(
          BinaryOperator::OR, MakeUnique<Literal>(true),
          MakeUnique<UnaryOperation>(UnaryOperator::NOT,
                                     MakeUnique<Identifier>("b"))));
  const UnaryOperation expr3(
      UnaryOperator::NOT,
      MakeUnique<BinaryOperation>(
          BinaryOperator::IMPLY, MakeUnique<Literal>(true),
          MakeUnique<UnaryOperation>(UnaryOperator::NOT,
                                     MakeUnique<Identifier>("b"))));
  const UnaryOperation expr4(
      UnaryOperator::NOT,
      MakeUnique<BinaryOperation>(
          BinaryOperator::IFF, MakeUnique<Literal>(true),
          MakeUnique<UnaryOperation>(UnaryOperator::NOT,
                                     MakeUnique<Identifier>("b"))));
  EXPECT_EQ("!(true & !b);!(true | !b);!(true => !b);!(true <=> !b)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputAddition) {
  const BinaryOperation expr1(
      BinaryOperator::PLUS,
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS, MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::PLUS, MakeUnique<BinaryOperation>(
                                BinaryOperator::MINUS, MakeUnique<Literal>(17),
                                MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::PLUS,
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::PLUS, MakeUnique<BinaryOperation>(
                                BinaryOperator::DIVIDE, MakeUnique<Literal>(17),
                                MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("17 + b + c - d;17 - b + c * d;17 * b + c / d;17 / b + c + d",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputSubtraction) {
  const BinaryOperation expr1(
      BinaryOperator::MINUS,
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS, MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::MINUS, MakeUnique<BinaryOperation>(
                                 BinaryOperator::MINUS, MakeUnique<Literal>(17),
                                 MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::MINUS,
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::MINUS,
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("17 + b - (c - d);17 - b - c * d;17 * b - c / d;17 / b - (c + d)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputMultiplication) {
  const BinaryOperation expr1(
      BinaryOperator::MULTIPLY,
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS, MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::MULTIPLY,
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::MULTIPLY,
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::MULTIPLY,
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ(
      "(17 + b) * (c - d);(17 - b) * c * d;17 * b * c / d;17 / b * (c + d)",
      StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputDivision) {
  const BinaryOperation expr1(
      BinaryOperator::DIVIDE,
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS, MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::DIVIDE,
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::DIVIDE,
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::DIVIDE,
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ(
      "(17 + b) / (c - d);(17 - b) / (c * d);17 * b / (c / d);17 / b / (c + d)",
      StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputAnd) {
  const BinaryOperation expr1(
      BinaryOperator::AND, MakeUnique<BinaryOperation>(
                               BinaryOperator::AND, MakeUnique<Literal>(true),
                               MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::OR,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::AND,
      MakeUnique<BinaryOperation>(BinaryOperator::OR, MakeUnique<Literal>(true),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IMPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::AND, MakeUnique<BinaryOperation>(
                               BinaryOperator::IMPLY, MakeUnique<Literal>(true),
                               MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IFF,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::AND, MakeUnique<BinaryOperation>(
                               BinaryOperator::IFF, MakeUnique<Literal>(true),
                               MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::AND,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("true & b & (c | d);(true | b) & (c => d)",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("(true => b) & (c <=> d);(true <=> b) & c & d",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputOr) {
  const BinaryOperation expr1(
      BinaryOperator::OR, MakeUnique<BinaryOperation>(
                              BinaryOperator::AND, MakeUnique<Literal>(true),
                              MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::OR,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::OR,
      MakeUnique<BinaryOperation>(BinaryOperator::OR, MakeUnique<Literal>(true),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IMPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::OR, MakeUnique<BinaryOperation>(
                              BinaryOperator::IMPLY, MakeUnique<Literal>(true),
                              MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IFF,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::OR, MakeUnique<BinaryOperation>(
                              BinaryOperator::IFF, MakeUnique<Literal>(true),
                              MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::AND,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("true & b | c | d;true | b | (c => d)", StrCat(expr1, ';', expr2));
  EXPECT_EQ("(true => b) | (c <=> d);(true <=> b) | c & d",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputImply) {
  const BinaryOperation expr1(
      BinaryOperator::IMPLY, MakeUnique<BinaryOperation>(
                                 BinaryOperator::AND, MakeUnique<Literal>(true),
                                 MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::OR,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::IMPLY,
      MakeUnique<BinaryOperation>(BinaryOperator::OR, MakeUnique<Literal>(true),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IMPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::IMPLY,
      MakeUnique<BinaryOperation>(BinaryOperator::IMPLY,
                                  MakeUnique<Literal>(true),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IFF,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::IMPLY, MakeUnique<BinaryOperation>(
                                 BinaryOperator::IFF, MakeUnique<Literal>(true),
                                 MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::AND,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("true & b => c | d;true | b => (c => d)",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("true => b => (c <=> d);(true <=> b) => c & d",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputIff) {
  const BinaryOperation expr1(
      BinaryOperator::IFF, MakeUnique<BinaryOperation>(
                               BinaryOperator::AND, MakeUnique<Literal>(true),
                               MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::OR,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::IFF,
      MakeUnique<BinaryOperation>(BinaryOperator::OR, MakeUnique<Literal>(true),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IMPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::IFF, MakeUnique<BinaryOperation>(
                               BinaryOperator::IMPLY, MakeUnique<Literal>(true),
                               MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IFF,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::IFF, MakeUnique<BinaryOperation>(
                               BinaryOperator::IFF, MakeUnique<Literal>(true),
                               MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::AND,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("true & b <=> c | d;true | b <=> c => d",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("true => b <=> (c <=> d);true <=> b <=> c & d",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputLess) {
  const BinaryOperation expr1(
      BinaryOperator::LESS,
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS, MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::LESS,
      MakeUnique<BinaryOperation>(BinaryOperator::GREATER_EQUAL,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::LESS, MakeUnique<BinaryOperation>(
                                BinaryOperator::MINUS, MakeUnique<Literal>(17),
                                MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::LESS,
      MakeUnique<BinaryOperation>(BinaryOperator::GREATER,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::NOT_EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("17 + b < c * d;17 >= b < (c = d)", StrCat(expr1, ';', expr2));
  EXPECT_EQ("17 - b < c / d;17 > b < (c != d)", StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputLessEqual) {
  const BinaryOperation expr1(
      BinaryOperator::LESS_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::LESS_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::GREATER,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::NOT_EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::LESS_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::AND,
                                  MakeUnique<Literal>(false),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::OR,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::LESS_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::EQUAL,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::LESS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("17 - b <= c / d;17 > b <= (c != d)", StrCat(expr1, ';', expr2));
  EXPECT_EQ("(false & b) <= (c | d);(17 = b) <= (c < d)",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputGreaterEqual) {
  const BinaryOperation expr1(
      BinaryOperator::GREATER_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::AND,
                                  MakeUnique<Literal>(false),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::OR,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::GREATER_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::EQUAL,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::LESS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::GREATER_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::IMPLY,
                                  MakeUnique<Literal>(false),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IFF,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::GREATER_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::NOT_EQUAL,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::LESS_EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("(false & b) >= (c | d);(17 = b) >= (c < d)",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("(false => b) >= (c <=> d);(17 != b) >= (c <= d)",
            StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputGreater) {
  const BinaryOperation expr1(
      BinaryOperator::GREATER,
      MakeUnique<BinaryOperation>(BinaryOperator::IMPLY,
                                  MakeUnique<Literal>(false),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::IFF,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::GREATER,
      MakeUnique<BinaryOperation>(BinaryOperator::NOT_EQUAL,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::LESS_EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::GREATER,
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::GREATER,
      MakeUnique<BinaryOperation>(BinaryOperator::LESS, MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::GREATER_EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("(false => b) > (c <=> d);(17 != b) > (c <= d)",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("17 * b > c + d;17 < b > (c >= d)", StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputEqual) {
  const BinaryOperation expr1(
      BinaryOperator::EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::LESS, MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::GREATER_EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::LESS_EQUAL,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::GREATER,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("17 * b = c + d;17 < b = c >= d", StrCat(expr1, ';', expr2));
  EXPECT_EQ("17 / b = c - d;17 <= b = c > d", StrCat(expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputNotEqual) {
  const BinaryOperation expr1(
      BinaryOperator::NOT_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::DIVIDE,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MINUS,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr2(
      BinaryOperator::NOT_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::LESS_EQUAL,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::GREATER,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr3(
      BinaryOperator::NOT_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::PLUS, MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::MULTIPLY,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  const BinaryOperation expr4(
      BinaryOperator::NOT_EQUAL,
      MakeUnique<BinaryOperation>(BinaryOperator::GREATER_EQUAL,
                                  MakeUnique<Literal>(17),
                                  MakeUnique<Identifier>("b")),
      MakeUnique<BinaryOperation>(BinaryOperator::EQUAL,
                                  MakeUnique<Identifier>("c"),
                                  MakeUnique<Identifier>("d")));
  EXPECT_EQ("17 / b != c - d;17 <= b != c > d", StrCat(expr1, ';', expr2));
  EXPECT_EQ("17 + b != c * d;17 >= b != (c = d)", StrCat(expr3, ';', expr4));
}

TEST(ConditionalTest, OutputConditional) {
  const Conditional expr1(
      MakeUnique<Literal>(true),
      MakeUnique<Conditional>(
          MakeUnique<BinaryOperation>(BinaryOperator::PLUS,
                                      MakeUnique<Identifier>("a"),
                                      MakeUnique<Identifier>("b")),
          MakeUnique<UnaryOperation>(
              UnaryOperator::NEGATE,
              MakeUnique<FunctionCall>(
                  Function::CEIL, MakeArguments(MakeUnique<Identifier>("c")))),
          MakeUnique<Identifier>("d")),
      MakeUnique<BinaryOperation>(
          BinaryOperator::PLUS,
          MakeUnique<Conditional>(MakeUnique<Literal>(false),
                                  MakeUnique<Identifier>("e"),
                                  MakeUnique<Identifier>("f")),
          MakeUnique<Literal>(42)));
  EXPECT_EQ("true ? a + b ? -ceil(c) : d : (false ? e : f) + 42",
            StrCat(expr1));
}

TEST(ProbabilityThresholdOperationTest, OutputProbabilityThresholdOperation) {
  const ProbabilityThresholdOperation expr1(
      ProbabilityThresholdOperator::LESS, 0.25,
      MakeUnique<UntilProperty>(
          TimeRange(0, std::numeric_limits<double>::infinity()),
          MakeUnique<Literal>(true), MakeUnique<Identifier>("a")));
  const ProbabilityThresholdOperation expr2(
      ProbabilityThresholdOperator::LESS_EQUAL, 0.5,
      MakeUnique<UntilProperty>(TimeRange(0.5, 17), MakeUnique<Literal>(true),
                                MakeUnique<Identifier>("b")));
  const ProbabilityThresholdOperation expr3(
      ProbabilityThresholdOperator::GREATER_EQUAL, 0.75,
      MakeUnique<UntilProperty>(TimeRange(0, 42), MakeUnique<Identifier>("c"),
                                MakeUnique<Identifier>("d")));
  const ProbabilityThresholdOperation expr4(
      ProbabilityThresholdOperator::GREATER, 1,
      MakeUnique<UntilProperty>(
          TimeRange(4711, std::numeric_limits<double>::infinity()),
          MakeUnique<Literal>(false), MakeUnique<Literal>(true)));
  EXPECT_EQ("P<0.25[ true U a ];P<=0.5[ true U[0.5,17] b ]",
            StrCat(expr1, ';', expr2));
  EXPECT_EQ("P>=0.75[ c U<=42 d ];P>1[ false U>=4711 true ]",
            StrCat(expr3, ';', expr4));
}

TEST(ProbabilityEstimationOperationTest, OutputProbabilityEstimationOperation) {
  const ProbabilityEstimationOperation expr(MakeUnique<EventuallyProperty>(
      TimeRange(0, std::numeric_limits<double>::infinity()),
      MakeUnique<Identifier>("a")));
  EXPECT_EQ("P=?[ F a ]", StrCat(expr));
}

TEST(TimeRangeTest, Output) {
  EXPECT_EQ("", StrCat(TimeRange(0, std::numeric_limits<double>::infinity())));
  EXPECT_EQ(">=0.5",
            StrCat(TimeRange(0.5, std::numeric_limits<double>::infinity())));
  EXPECT_EQ("<=17", StrCat(TimeRange(0, 17)));
  EXPECT_EQ("[0.5,17]", StrCat(TimeRange(0.5, 17)));
}

TEST(EventuallyPropertyTest, Output) {
  EXPECT_EQ("F a", StrCat(EventuallyProperty(
                       {0, std::numeric_limits<double>::infinity()},
                       MakeUnique<Identifier>("a"))));
  EXPECT_EQ("F[0.5,17] b",
            StrCat(EventuallyProperty({0.5, 17}, MakeUnique<Identifier>("b"))));
  EXPECT_EQ("F<=42 d",
            StrCat(EventuallyProperty({0, 42}, MakeUnique<Identifier>("d"))));
  EXPECT_EQ("F>=4711 true", StrCat(EventuallyProperty(
                                {4711, std::numeric_limits<double>::infinity()},
                                MakeUnique<Literal>(true))));
}

}  // namespace
