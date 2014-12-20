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

#include "optional.h"

#include <memory>
#include <string>

#include "gtest/gtest.h"

namespace {

class NonDefaultConstructible {
 public:
  explicit NonDefaultConstructible(const std::string& name, int value)
      : name_(name), value_(value) {}

  bool operator==(const NonDefaultConstructible& rhs) const {
    return name_ == rhs.name_ && value_ == rhs.value_;
  }

 private:
  std::string name_;
  int value_;
};

class MoveOnly {
 public:
  MoveOnly() : value_ptr_(new int(4711)) {}

  bool operator==(const MoveOnly& rhs) const {
    return *value_ptr_ == *rhs.value_ptr_;
  }

 private:
  std::unique_ptr<int> value_ptr_;
};

TEST(OptionalTest, NoValue) {
  Optional<int> int_optional;
  EXPECT_FALSE(int_optional.has_value());
  Optional<std::string> string_optional;
  EXPECT_FALSE(string_optional.has_value());
  Optional<NonDefaultConstructible> non_default_constructible_optional;
  EXPECT_FALSE(non_default_constructible_optional.has_value());
  Optional<MoveOnly> move_only_optional;
  EXPECT_FALSE(move_only_optional.has_value());
}

TEST(OptionalTest, Value) {
  Optional<int> int_optional = 17;
  EXPECT_TRUE(int_optional.has_value());
  EXPECT_EQ(17, int_optional.value());
  Optional<std::string> string_optional = "foo";
  EXPECT_TRUE(string_optional.has_value());
  EXPECT_EQ("foo", string_optional.value());
  Optional<NonDefaultConstructible> non_default_constructible_optional =
      NonDefaultConstructible("bar", 42);
  EXPECT_TRUE(non_default_constructible_optional.has_value());
  EXPECT_EQ(NonDefaultConstructible("bar", 42),
            non_default_constructible_optional.value());
  Optional<MoveOnly> move_only_optional = MoveOnly();
  EXPECT_TRUE(move_only_optional.has_value());
  EXPECT_EQ(MoveOnly(), move_only_optional.value());
}

TEST(OptionalTest, Assignment) {
  Optional<std::string> string_optional;
  EXPECT_FALSE(string_optional.has_value());
  string_optional = "foo";
  EXPECT_TRUE(string_optional.has_value());
  EXPECT_EQ("foo", string_optional.value());
  string_optional = string_optional;
  EXPECT_TRUE(string_optional.has_value());
  EXPECT_EQ("foo", string_optional.value());
  string_optional = Optional<std::string>();
  EXPECT_FALSE(string_optional.has_value());
}

}  // namespace
