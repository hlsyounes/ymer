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

#include "gtest/gtest.h"

namespace {

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
