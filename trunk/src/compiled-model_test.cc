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

#include "compiled-model.h"

#include "gtest/gtest.h"

namespace {

TEST(CompiledVariableTest, Constructs) {
  CompiledVariable v1("foo", 17, 42, 17);
  EXPECT_EQ("foo", v1.name());
  EXPECT_EQ(17, v1.min_value());
  EXPECT_EQ(42, v1.max_value());
  EXPECT_EQ(17, v1.init_value());

  CompiledVariable v2("bar", 17, 42, 42);
  EXPECT_EQ("bar", v2.name());
  EXPECT_EQ(17, v2.min_value());
  EXPECT_EQ(42, v2.max_value());
  EXPECT_EQ(42, v2.init_value());

  CompiledVariable v3("baz", 17, 42, 21);
  EXPECT_EQ("baz", v3.name());
  EXPECT_EQ(17, v3.min_value());
  EXPECT_EQ(42, v3.max_value());
  EXPECT_EQ(21, v3.init_value());
}

TEST(CompiledModelTest, NumBits) {
  CompiledModel m1;
  m1.AddVariable("foo", 0, 1, 0);
  EXPECT_EQ(1, m1.NumBits());

  CompiledModel m2;
  m2.AddVariable("foo", 0, 1, 0);
  m2.AddVariable("bar", 1, 3, 1);
  m2.AddVariable("baz", 8, 15, 8);
  EXPECT_EQ(6, m2.NumBits());
}

}  // namespace
