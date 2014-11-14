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

#include "typed-value.h"

#include "strutil.h"

#include "gtest/gtest.h"

namespace {

TEST(TypeTest, Output) {
  EXPECT_EQ("int;double;bool",
            StrCat(Type::INT, ';', Type::DOUBLE, ';', Type::BOOL));
}

TEST(TypedValueTest, Type) {
  EXPECT_EQ(Type::INT, TypedValue(17).type());
  EXPECT_EQ(Type::INT, TypedValue(-4711).type());
  EXPECT_EQ(Type::DOUBLE, TypedValue(3.14159).type());
  EXPECT_EQ(Type::DOUBLE, TypedValue(-0.5).type());
  EXPECT_EQ(Type::BOOL, TypedValue(true).type());
  EXPECT_EQ(Type::BOOL, TypedValue(false).type());
}

TEST(TypedValueTest, Value) {
  EXPECT_EQ(17, TypedValue(17).value<int>());
  EXPECT_EQ(-4711, TypedValue(-4711).value<int>());
  EXPECT_DOUBLE_EQ(3.14159, TypedValue(3.14159).value<double>());
  EXPECT_DOUBLE_EQ(-0.5, TypedValue(-0.5).value<double>());
  EXPECT_TRUE(TypedValue(true).value<bool>());
  EXPECT_FALSE(TypedValue(false).value<bool>());
}

TEST(TypedValueTest, Equal) {
  EXPECT_TRUE(TypedValue(17) == TypedValue(17));
  EXPECT_TRUE(TypedValue(3.14159) == TypedValue(3.14159));
  EXPECT_TRUE(TypedValue(true) == TypedValue(true));
  EXPECT_TRUE(TypedValue(17) == TypedValue(17.0));
  EXPECT_TRUE(TypedValue(1) == TypedValue(true));
  EXPECT_TRUE(TypedValue(0.0) == TypedValue(false));
}

TEST(TypedValueTest, NotEqual) {
  EXPECT_TRUE(TypedValue(17) != TypedValue(42));
  EXPECT_TRUE(TypedValue(17.0) != TypedValue(42));
  EXPECT_TRUE(TypedValue(3.14159) != TypedValue(0.5));
  EXPECT_TRUE(TypedValue(true) != TypedValue(false));
}

TEST(TypedValueTest, Less) {
  EXPECT_TRUE(TypedValue(17) < TypedValue(42));
  EXPECT_TRUE(TypedValue(0.5) < TypedValue(1));
  EXPECT_TRUE(TypedValue(false) < TypedValue(true));
  EXPECT_TRUE(TypedValue(true) < TypedValue(3.14159));
}

TEST(TypedValueTest, LessEqual) {
  EXPECT_TRUE(TypedValue(17) <= TypedValue(42));
  EXPECT_TRUE(TypedValue(0.5) <= TypedValue(1));
  EXPECT_TRUE(TypedValue(false) <= TypedValue(true));
  EXPECT_TRUE(TypedValue(true) <= TypedValue(3.14159));

  EXPECT_TRUE(TypedValue(17) <= TypedValue(17));
  EXPECT_TRUE(TypedValue(3.14159) <= TypedValue(3.14159));
  EXPECT_TRUE(TypedValue(true) <= TypedValue(true));
  EXPECT_TRUE(TypedValue(17) <= TypedValue(17.0));
  EXPECT_TRUE(TypedValue(1) <= TypedValue(true));
  EXPECT_TRUE(TypedValue(0.0) <= TypedValue(false));
}

TEST(TypedValueTest, GreaterEqual) {
  EXPECT_TRUE(TypedValue(42) >= TypedValue(17));
  EXPECT_TRUE(TypedValue(1) >= TypedValue(0.5));
  EXPECT_TRUE(TypedValue(true) >= TypedValue(false));
  EXPECT_TRUE(TypedValue(3.14159) >= TypedValue(true));

  EXPECT_TRUE(TypedValue(17) >= TypedValue(17));
  EXPECT_TRUE(TypedValue(3.14159) >= TypedValue(3.14159));
  EXPECT_TRUE(TypedValue(true) >= TypedValue(true));
  EXPECT_TRUE(TypedValue(17) >= TypedValue(17.0));
  EXPECT_TRUE(TypedValue(1) >= TypedValue(true));
  EXPECT_TRUE(TypedValue(0.0) >= TypedValue(false));
}

TEST(TypedValueTest, Greater) {
  EXPECT_TRUE(TypedValue(42) > TypedValue(17));
  EXPECT_TRUE(TypedValue(1) > TypedValue(0.5));
  EXPECT_TRUE(TypedValue(true) > TypedValue(false));
  EXPECT_TRUE(TypedValue(3.14159) > TypedValue(true));
}

TEST(TypedValueTest, Negate) {
  EXPECT_EQ(-17, -TypedValue(17));
  EXPECT_EQ(0.5, -TypedValue(-0.5));
}

TEST(TypedValueTest, Plus) {
  EXPECT_EQ(17 + 42, TypedValue(17) + TypedValue(42));
  EXPECT_EQ(-4711 + 3.14159, TypedValue(-4711) + TypedValue(3.14159));
}

TEST(TypedValueTest, Minus) {
  EXPECT_EQ(17 - 42, TypedValue(17) - TypedValue(42));
  EXPECT_EQ(-4711 - 3.14159, TypedValue(-4711) - TypedValue(3.14159));
}

TEST(TypedValueTest, Multiply) {
  EXPECT_EQ(17 * 42, TypedValue(17) * TypedValue(42));
  EXPECT_EQ(-4711 * 3.14159, TypedValue(-4711) * TypedValue(3.14159));
}

TEST(TypedValueTest, Divide) {
  EXPECT_EQ(0.5, TypedValue(1) / TypedValue(2));
  EXPECT_EQ(-2, TypedValue(-6) / TypedValue(3));
  EXPECT_EQ(-2, TypedValue(1) / TypedValue(-0.5));
  EXPECT_EQ(0.25, TypedValue(0.5) / TypedValue(2));
}

TEST(TypedValueTest, Modulus) {
  EXPECT_EQ(0, TypedValue(0) % TypedValue(3));
  EXPECT_EQ(1, TypedValue(1) % TypedValue(3));
  EXPECT_EQ(2, TypedValue(2) % TypedValue(3));
  EXPECT_EQ(0, TypedValue(3) % TypedValue(3));
  EXPECT_EQ(1, TypedValue(4) % TypedValue(3));
}

TEST(TypedValueTest, Not) {
  EXPECT_EQ(true, !TypedValue(false));
  EXPECT_EQ(TypedValue(false), !TypedValue(true));
}

TEST(TypedValueTest, Floor) {
  EXPECT_EQ(1, floor(TypedValue(1.1)));
  EXPECT_EQ(-3, floor(TypedValue(-2.3)));
}

TEST(TypedValueTest, Ceil) {
  EXPECT_EQ(2, ceil(TypedValue(1.1)));
  EXPECT_EQ(-2, ceil(TypedValue(-2.3)));
}

TEST(TypedValueTest, Pow) { EXPECT_EQ(32, pow(TypedValue(2), TypedValue(5))); }

TEST(TypedValueTest, Log) { EXPECT_EQ(0, log(TypedValue(1))); }

TEST(TypedValueTest, Output) {
  const TypedValue a(17), b(0.5), c(true);
  EXPECT_EQ("17;0.5;true", StrCat(a, ';', b, ';', c));
}

}  // namespace
