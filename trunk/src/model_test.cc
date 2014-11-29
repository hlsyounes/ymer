// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2014 Google Inc
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

#include "model.h"

#include "strutil.h"

#include "gtest/gtest.h"

namespace {

TEST(UpdateTest, Output) {
  EXPECT_EQ("a' = 17", StrCat(Update("a", Literal::New(17))));
}

TEST(ModelTypeTest, Output) {
  EXPECT_EQ("mdp", StrCat(ModelType::DEFAULT));
  EXPECT_EQ("mdp", StrCat(ModelType::MDP));
  EXPECT_EQ("dtmc", StrCat(ModelType::DTMC));
  EXPECT_EQ("ctmc", StrCat(ModelType::CTMC));
  EXPECT_EQ("gsmp", StrCat(ModelType::GSMP));
}

}  // namespace
