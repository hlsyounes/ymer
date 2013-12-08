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

TEST(LiteralTest, Output) {
  const Literal a(17), b(0.5), c(true), d(false);
  EXPECT_EQ("17;0.5;true;false", StrCat(a, ';', b, ';', c, ';', d));
}

TEST(IdentifierTest, Output) {
  const Identifier a("foo"), b("bar"), c("baz");
  EXPECT_EQ("foo;bar;baz", StrCat(a, ';', b, ';', c));
}

TEST(ComputationTest, OutputAddition) {
  auto expr1 =
      Computation::Create(Computation::PLUS,
                          Computation::Create(Computation::PLUS,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::MINUS,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr2 =
      Computation::Create(Computation::PLUS,
                          Computation::Create(Computation::MINUS,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::MULTIPLY,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr3 =
      Computation::Create(Computation::PLUS,
                          Computation::Create(Computation::MULTIPLY,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::DIVIDE,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr4 =
      Computation::Create(Computation::PLUS,
                          Computation::Create(Computation::DIVIDE,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::PLUS,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  EXPECT_EQ("a+b+c-d;a-b+c*d;a*b+c/d;a/b+c+d",
            StrCat(*expr1, ';', *expr2, ';', *expr3, ';', *expr4));
}

TEST(ComputationTest, OutputSubtraction) {
  auto expr1 =
      Computation::Create(Computation::MINUS,
                          Computation::Create(Computation::PLUS,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::MINUS,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr2 =
      Computation::Create(Computation::MINUS,
                          Computation::Create(Computation::MINUS,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::MULTIPLY,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr3 =
      Computation::Create(Computation::MINUS,
                          Computation::Create(Computation::MULTIPLY,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::DIVIDE,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr4 =
      Computation::Create(Computation::MINUS,
                          Computation::Create(Computation::DIVIDE,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::PLUS,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  EXPECT_EQ("a+b-(c-d);a-b-c*d;a*b-c/d;a/b-(c+d)",
            StrCat(*expr1, ';', *expr2, ';', *expr3, ';', *expr4));
}

TEST(ComputationTest, OutputMultiplication) {
  auto expr1 =
      Computation::Create(Computation::MULTIPLY,
                          Computation::Create(Computation::PLUS,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::MINUS,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr2 =
      Computation::Create(Computation::MULTIPLY,
                          Computation::Create(Computation::MINUS,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::MULTIPLY,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr3 =
      Computation::Create(Computation::MULTIPLY,
                          Computation::Create(Computation::MULTIPLY,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::DIVIDE,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr4 =
      Computation::Create(Computation::MULTIPLY,
                          Computation::Create(Computation::DIVIDE,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::PLUS,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  EXPECT_EQ("(a+b)*(c-d);(a-b)*c*d;a*b*c/d;a/b*(c+d)",
            StrCat(*expr1, ';', *expr2, ';', *expr3, ';', *expr4));
}

TEST(ComputationTest, OutputDivision) {
  auto expr1 =
      Computation::Create(Computation::DIVIDE,
                          Computation::Create(Computation::PLUS,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::MINUS,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr2 =
      Computation::Create(Computation::DIVIDE,
                          Computation::Create(Computation::MINUS,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::MULTIPLY,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr3 =
      Computation::Create(Computation::DIVIDE,
                          Computation::Create(Computation::MULTIPLY,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::DIVIDE,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  auto expr4 =
      Computation::Create(Computation::DIVIDE,
                          Computation::Create(Computation::DIVIDE,
                                              Identifier::Create("a"),
                                              Identifier::Create("b")),
                          Computation::Create(Computation::PLUS,
                                              Identifier::Create("c"),
                                              Identifier::Create("d")));
  EXPECT_EQ("(a+b)/(c-d);(a-b)/(c*d);a*b/(c/d);a/b/(c+d)",
            StrCat(*expr1, ';', *expr2, ';', *expr3, ';', *expr4));
}

}  // namespace
