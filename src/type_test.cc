// Copyright (C) 2011 Google Inc
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

#include "type.h"

#include <string>

#include "gtest/gtest.h"

namespace {

TEST(TypeTest, Name) {
  EXPECT_EQ("int", Type_Name(Type::INT));
  EXPECT_EQ("double", Type_Name(Type::DOUBLE));
  EXPECT_EQ("bool", Type_Name(Type::BOOL));
}

TEST(ConvertsToTypeTest, ValidConversions) {
  EXPECT_TRUE(ConvertsToType(Type::INT, Type::INT, nullptr));
  EXPECT_TRUE(ConvertsToType(Type::INT, Type::DOUBLE, nullptr));
  EXPECT_TRUE(ConvertsToType(Type::DOUBLE, Type::DOUBLE, nullptr));
  EXPECT_TRUE(ConvertsToType(Type::BOOL, Type::BOOL, nullptr));
}

TEST(ConvertsToTypeTest, InvalidConversions) {
  EXPECT_FALSE(ConvertsToType(Type::INT, Type::BOOL, nullptr));
  EXPECT_FALSE(ConvertsToType(Type::DOUBLE, Type::INT, nullptr));
  EXPECT_FALSE(ConvertsToType(Type::DOUBLE, Type::BOOL, nullptr));
  EXPECT_FALSE(ConvertsToType(Type::BOOL, Type::INT, nullptr));
  EXPECT_FALSE(ConvertsToType(Type::BOOL, Type::DOUBLE, nullptr));
}

TEST(ConvertsToTypeTest, ErrorMessages) {
  std::string error;
  EXPECT_FALSE(ConvertsToType(Type::INT, Type::BOOL, &error));
  EXPECT_EQ("type mismatch; expecting bool, found int", error);
  EXPECT_FALSE(ConvertsToType(Type::DOUBLE, Type::INT, &error));
  EXPECT_EQ("type mismatch; expecting int, found double", error);
  EXPECT_FALSE(ConvertsToType(Type::DOUBLE, Type::BOOL, &error));
  EXPECT_EQ("type mismatch; expecting bool, found double", error);
  EXPECT_FALSE(ConvertsToType(Type::BOOL, Type::INT, &error));
  EXPECT_EQ("type mismatch; expecting int, found bool", error);
  EXPECT_FALSE(ConvertsToType(Type::BOOL, Type::DOUBLE, &error));
  EXPECT_EQ("type mismatch; expecting double, found bool", error);
}

TEST(TypeInfoTest, Type) {
  EXPECT_EQ(Type::INT, TypeInfo<int>::type());
  EXPECT_EQ(Type::DOUBLE, TypeInfo<double>::type());
  EXPECT_EQ(Type::BOOL, TypeInfo<bool>::type());
}

}  // namespace
