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

#include "pointer-vector.h"
#include "strutil.h"

#include "gtest/gtest.h"

namespace {

PointerVector<const Expression> MakeArguments(
    std::unique_ptr<const Expression>&& argument) {
  PointerVector<const Expression> arguments;
  arguments.push_back(std::move(argument));
  return std::move(arguments);
}

PointerVector<const Expression> MakeArguments(
    std::unique_ptr<const Expression>&& argument1,
    std::unique_ptr<const Expression>&& argument2) {
  PointerVector<const Expression> arguments;
  arguments.push_back(std::move(argument1));
  arguments.push_back(std::move(argument2));
  return std::move(arguments);
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
      BinaryOperation::New(BinaryOperator::PLUS,
                           Literal::New(17),
                           UnaryOperation::New(UnaryOperator::NEGATE,
                                               Identifier::New("b"))));
  const UnaryOperation expr2(
      UnaryOperator::NEGATE,
      BinaryOperation::New(BinaryOperator::MINUS,
                           Literal::New(17),
                           UnaryOperation::New(UnaryOperator::NEGATE,
                                               Identifier::New("b"))));
  const UnaryOperation expr3(
      UnaryOperator::NEGATE,
      BinaryOperation::New(BinaryOperator::MULTIPLY,
                           Literal::New(17),
                           UnaryOperation::New(UnaryOperator::NEGATE,
                                               Identifier::New("b"))));
  const UnaryOperation expr4(
      UnaryOperator::NEGATE,
      BinaryOperation::New(BinaryOperator::DIVIDE,
                           Literal::New(17),
                           UnaryOperation::New(UnaryOperator::NEGATE,
                                               Identifier::New("b"))));
  EXPECT_EQ("-(17+-b);-(17--b);-(17*-b);-(17/-b)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputAddition) {
  const BinaryOperation expr1(BinaryOperator::PLUS,
                              BinaryOperation::New(BinaryOperator::PLUS,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::MINUS,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr2(BinaryOperator::PLUS,
                              BinaryOperation::New(BinaryOperator::MINUS,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::MULTIPLY,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr3(BinaryOperator::PLUS,
                              BinaryOperation::New(BinaryOperator::MULTIPLY,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::DIVIDE,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr4(BinaryOperator::PLUS,
                              BinaryOperation::New(BinaryOperator::DIVIDE,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::PLUS,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  EXPECT_EQ("17+b+c-d;17-b+c*d;17*b+c/d;17/b+c+d",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputSubtraction) {
  const BinaryOperation expr1(BinaryOperator::MINUS,
                              BinaryOperation::New(BinaryOperator::PLUS,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::MINUS,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr2(BinaryOperator::MINUS,
                              BinaryOperation::New(BinaryOperator::MINUS,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::MULTIPLY,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr3(BinaryOperator::MINUS,
                              BinaryOperation::New(BinaryOperator::MULTIPLY,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::DIVIDE,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr4(BinaryOperator::MINUS,
                              BinaryOperation::New(BinaryOperator::DIVIDE,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::PLUS,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  EXPECT_EQ("17+b-(c-d);17-b-c*d;17*b-c/d;17/b-(c+d)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputMultiplication) {
  const BinaryOperation expr1(BinaryOperator::MULTIPLY,
                              BinaryOperation::New(BinaryOperator::PLUS,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::MINUS,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr2(BinaryOperator::MULTIPLY,
                              BinaryOperation::New(BinaryOperator::MINUS,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::MULTIPLY,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr3(BinaryOperator::MULTIPLY,
                              BinaryOperation::New(BinaryOperator::MULTIPLY,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::DIVIDE,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr4(BinaryOperator::MULTIPLY,
                              BinaryOperation::New(BinaryOperator::DIVIDE,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::PLUS,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  EXPECT_EQ("(17+b)*(c-d);(17-b)*c*d;17*b*c/d;17/b*(c+d)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

TEST(BinaryOperationTest, OutputDivision) {
  const BinaryOperation expr1(BinaryOperator::DIVIDE,
                              BinaryOperation::New(BinaryOperator::PLUS,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::MINUS,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr2(BinaryOperator::DIVIDE,
                              BinaryOperation::New(BinaryOperator::MINUS,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::MULTIPLY,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr3(BinaryOperator::DIVIDE,
                              BinaryOperation::New(BinaryOperator::MULTIPLY,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::DIVIDE,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  const BinaryOperation expr4(BinaryOperator::DIVIDE,
                              BinaryOperation::New(BinaryOperator::DIVIDE,
                                                   Literal::New(17),
                                                   Identifier::New("b")),
                              BinaryOperation::New(BinaryOperator::PLUS,
                                                   Identifier::New("c"),
                                                   Identifier::New("d")));
  EXPECT_EQ("(17+b)/(c-d);(17-b)/(c*d);17*b/(c/d);17/b/(c+d)",
            StrCat(expr1, ';', expr2, ';', expr3, ';', expr4));
}

}  // namespace
