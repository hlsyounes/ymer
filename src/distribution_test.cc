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

#include <memory>

#include "expression.h"
#include "strutil.h"

#include "gtest/gtest.h"

namespace {

TEST(MemorylessTest, Output) {
  EXPECT_EQ("0.5", StrCat(Memoryless(std::make_unique<Literal>(0.5))));
  EXPECT_EQ("lambda",
            StrCat(Memoryless(std::make_unique<Identifier>("lambda"))));
}

TEST(WeibullTest, Output) {
  EXPECT_EQ("W(2.5, 0.5)", StrCat(Weibull(std::make_unique<Literal>(2.5),
                                          std::make_unique<Literal>(0.5))));
}

TEST(LognormalTest, Output) {
  EXPECT_EQ("L(2.5, 0.5)", StrCat(Lognormal(std::make_unique<Literal>(2.5),
                                            std::make_unique<Literal>(0.5))));
}

TEST(UniformTest, Output) {
  EXPECT_EQ("U(0.5, 17)", StrCat(Uniform(std::make_unique<Literal>(0.5),
                                         std::make_unique<Literal>(17))));
}

}  // namespace
