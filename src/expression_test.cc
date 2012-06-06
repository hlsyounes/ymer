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
      Addition::make(*Addition::make(*new Variable("a"),
                                     *new Variable("b")),
                     *Subtraction::make(*new Variable("c"),
                                        *new Variable("d")));
  const Expression* expr2 =
      Addition::make(*Subtraction::make(*new Variable("a"),
                                        *new Variable("b")),
                     *Multiplication::make(*new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr3 =
      Addition::make(*Multiplication::make(*new Variable("a"),
                                           *new Variable("b")),
                     *Division::make(*new Variable("c"),
                                     *new Variable("d")));
  const Expression* expr4 =
      Addition::make(*Division::make(*new Variable("a"),
                                     *new Variable("b")),
                     *Addition::make(*new Variable("c"),
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
      Subtraction::make(*Addition::make(*new Variable("a"),
                                        *new Variable("b")),
                        *Subtraction::make(*new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr2 =
      Subtraction::make(*Subtraction::make(*new Variable("a"),
                                           *new Variable("b")),
                        *Multiplication::make(*new Variable("c"),
                                              *new Variable("d")));
  const Expression* expr3 =
      Subtraction::make(*Multiplication::make(*new Variable("a"),
                                              *new Variable("b")),
                        *Division::make(*new Variable("c"),
                                        *new Variable("d")));
  const Expression* expr4 =
      Subtraction::make(*Division::make(*new Variable("a"),
                                        *new Variable("b")),
                        *Addition::make(*new Variable("c"),
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
      Multiplication::make(*Addition::make(*new Variable("a"),
                                           *new Variable("b")),
                           *Subtraction::make(*new Variable("c"),
                                              *new Variable("d")));
  const Expression* expr2 =
      Multiplication::make(*Subtraction::make(*new Variable("a"),
                                              *new Variable("b")),
                           *Multiplication::make(*new Variable("c"),
                                                 *new Variable("d")));
  const Expression* expr3 =
      Multiplication::make(*Multiplication::make(*new Variable("a"),
                                                 *new Variable("b")),
                           *Division::make(*new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr4 =
      Multiplication::make(*Division::make(*new Variable("a"),
                                           *new Variable("b")),
                           *Addition::make(*new Variable("c"),
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
      Division::make(*Addition::make(*new Variable("a"),
                                     *new Variable("b")),
                     *Subtraction::make(*new Variable("c"),
                                        *new Variable("d")));
  const Expression* expr2 =
      Division::make(*Subtraction::make(*new Variable("a"),
                                        *new Variable("b")),
                     *Multiplication::make(*new Variable("c"),
                                           *new Variable("d")));
  const Expression* expr3 =
      Division::make(*Multiplication::make(*new Variable("a"),
                                           *new Variable("b")),
                     *Division::make(*new Variable("c"),
                                     *new Variable("d")));
  const Expression* expr4 =
      Division::make(*Division::make(*new Variable("a"),
                                     *new Variable("b")),
                     *Addition::make(*new Variable("c"),
                                     *new Variable("d")));
  out << *expr1 << ';' << *expr2 << ';' << *expr3 << ';' << *expr4;
  delete expr1;
  delete expr2;
  delete expr3;
  delete expr4;
  EXPECT_EQ("(a+b)/(c-d);(a-b)/(c*d);a*b/(c/d);a/b/(c+d)", out.str());
}

}  // namespace
