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

#include <sstream>

#include "gtest/gtest.h"

namespace {

TEST(LiteralTest, Output) {
  std::ostringstream out;
  Literal a(17), b(0.5), c(true), d(false);
  out << a << ';' << b << ';' << c << ';' << d;
  EXPECT_EQ("17;0.5;true;false", out.str());
}

TEST(VariableTest, Output) {
  std::ostringstream out;
  Variable a("foo"), b("bar"), c("baz");
  out << a << ';' << b << ';' << c;
  EXPECT_EQ("foo;bar;baz", out.str());
}

TEST(ComputationTest, OutputAddition) {
  std::ostringstream out;
  const Expression* expr1 =
      Computation::make(Computation::PLUS,
                        *Computation::make(Computation::PLUS,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::MINUS,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr2 =
      Computation::make(Computation::PLUS,
                        *Computation::make(Computation::MINUS,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::MULTIPLY,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr3 =
      Computation::make(Computation::PLUS,
                        *Computation::make(Computation::MULTIPLY,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::DIVIDE,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr4 =
      Computation::make(Computation::PLUS,
                        *Computation::make(Computation::DIVIDE,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::PLUS,
                                           *new Variable("c"),
                                           *new Variable("d")));
  out << *expr1 << ';' << *expr2 << ';' << *expr3 << ';' << *expr4;
  delete expr1;
  delete expr2;
  delete expr3;
  delete expr4;
  EXPECT_EQ("a+b+c-d;a-b+c*d;a*b+c/d;a/b+c+d", out.str());
}

TEST(ComputationTest, OutputSubtraction) {
  std::ostringstream out;
  const Expression* expr1 =
      Computation::make(Computation::MINUS,
                        *Computation::make(Computation::PLUS,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::MINUS,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr2 =
      Computation::make(Computation::MINUS,
                        *Computation::make(Computation::MINUS,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::MULTIPLY,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr3 =
      Computation::make(Computation::MINUS,
                        *Computation::make(Computation::MULTIPLY,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::DIVIDE,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr4 =
      Computation::make(Computation::MINUS,
                        *Computation::make(Computation::DIVIDE,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::PLUS,
                                           *new Variable("c"),
                                           *new Variable("d")));
  out << *expr1 << ';' << *expr2 << ';' << *expr3 << ';' << *expr4;
  delete expr1;
  delete expr2;
  delete expr3;
  delete expr4;
  EXPECT_EQ("a+b-(c-d);a-b-c*d;a*b-c/d;a/b-(c+d)", out.str());
}

TEST(ComputationTest, OutputMultiplication) {
  std::ostringstream out;
  const Expression* expr1 =
      Computation::make(Computation::MULTIPLY,
                        *Computation::make(Computation::PLUS,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::MINUS,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr2 =
      Computation::make(Computation::MULTIPLY,
                        *Computation::make(Computation::MINUS,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::MULTIPLY,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr3 =
      Computation::make(Computation::MULTIPLY,
                        *Computation::make(Computation::MULTIPLY,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::DIVIDE,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr4 =
      Computation::make(Computation::MULTIPLY,
                        *Computation::make(Computation::DIVIDE,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::PLUS,
                                           *new Variable("c"),
                                           *new Variable("d")));
  out << *expr1 << ';' << *expr2 << ';' << *expr3 << ';' << *expr4;
  delete expr1;
  delete expr2;
  delete expr3;
  delete expr4;
  EXPECT_EQ("(a+b)*(c-d);(a-b)*c*d;a*b*c/d;a/b*(c+d)", out.str());
}

TEST(ComputationTest, OutputDivision) {
  std::ostringstream out;
  const Expression* expr1 =
      Computation::make(Computation::DIVIDE,
                        *Computation::make(Computation::PLUS,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::MINUS,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr2 =
      Computation::make(Computation::DIVIDE,
                        *Computation::make(Computation::MINUS,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::MULTIPLY,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr3 =
      Computation::make(Computation::DIVIDE,
                        *Computation::make(Computation::MULTIPLY,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::DIVIDE,
                                           *new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr4 =
      Computation::make(Computation::DIVIDE,
                        *Computation::make(Computation::DIVIDE,
                                           *new Variable("a"),
                                           *new Variable("b")),
                        *Computation::make(Computation::PLUS,
                                           *new Variable("c"),
                                           *new Variable("d")));
  out << *expr1 << ';' << *expr2 << ';' << *expr3 << ';' << *expr4;
  delete expr1;
  delete expr2;
  delete expr3;
  delete expr4;
  EXPECT_EQ("(a+b)/(c-d);(a-b)/(c*d);a*b/(c/d);a/b/(c+d)", out.str());
}

}  // namespace
