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

#include "optional.h"

#include <string>

#include <gtest/gtest.h>

namespace {

TEST(OptionalTest, NoValue) {
  Optional<int> int_optional;
  EXPECT_FALSE(int_optional ? true : false);
  Optional<std::string> string_optional;
  EXPECT_FALSE(string_optional ? true : false);
}

TEST(OptionalTest, WithValue) {
  Optional<int> int_optional(17);
  ASSERT_TRUE(int_optional ? true : false);
  EXPECT_EQ(17, int_optional.get());
  Optional<std::string> string_optional("foo");
  ASSERT_TRUE(string_optional ? true : false);
  EXPECT_EQ("foo", string_optional.get());
}

}  // namespace
