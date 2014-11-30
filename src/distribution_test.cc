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

#include "distribution.h"

#include "expression.h"
#include "strutil.h"

#include "gtest/gtest.h"

namespace {

TEST(ExponentialTest, Output) {
  EXPECT_EQ("0.5", StrCat(Exponential(Literal::New(0.5))));
  EXPECT_EQ("lambda", StrCat(Exponential(Identifier::New("lambda"))));
}

TEST(WeibullTest, Output) {
  EXPECT_EQ("W(2.5, 0.5)",
            StrCat(Weibull(Literal::New(2.5), Literal::New(0.5))));
}

TEST(LognormalTest, Output) {
  EXPECT_EQ("L(2.5, 0.5)",
            StrCat(Lognormal(Literal::New(2.5), Literal::New(0.5))));
}

TEST(UniformTest, Output) {
  EXPECT_EQ("U(0.5, 17)", StrCat(Uniform(Literal::New(0.5), Literal::New(17))));
}

}  // namespace
