// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2015 Google Inc
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

#include "process-algebra.h"

#include <memory>

#include "strutil.h"

#include "gtest/gtest.h"

namespace {

TEST(ProcessAlgebraTest, Ouput) {
  const ParallelComposition expr1(
      ParallelCompositionOperator::ALPHABETIZED,
      std::make_unique<ModuleIdentifier>("foo"),
      std::make_unique<ParallelComposition>(
          ParallelCompositionOperator::ALPHABETIZED,
          std::make_unique<ModuleIdentifier>("bar"),
          std::make_unique<ModuleIdentifier>("baz")));
  EXPECT_EQ("foo || bar || baz", StrCat(expr1));
  const ParallelComposition expr2(
      ParallelCompositionOperator::ALPHABETIZED,
      std::make_unique<ModuleIdentifier>("foo"),
      std::make_unique<ParallelComposition>(
          ParallelCompositionOperator::ASYNCHRONOUS,
          std::make_unique<ModuleIdentifier>("bar"),
          std::make_unique<ModuleIdentifier>("baz")));
  EXPECT_EQ("foo || (bar ||| baz)", StrCat(expr2));
  const ParallelComposition expr3(
      ParallelCompositionOperator::ALPHABETIZED,
      std::make_unique<ModuleIdentifier>("foo"),
      std::make_unique<ActionHiding>(
          std::set<std::string>({"a", "b"}),
          std::make_unique<ModuleIdentifier>("bar")));
  EXPECT_EQ("foo || bar / {a,b}", StrCat(expr3));
  const ParallelComposition expr4(
      ParallelCompositionOperator::ALPHABETIZED,
      std::make_unique<ParallelComposition>(
          ParallelCompositionOperator::ALPHABETIZED,
          std::make_unique<ModuleIdentifier>("foo"),
          std::make_unique<ModuleIdentifier>("bar")),
      std::make_unique<ModuleIdentifier>("baz"));
  EXPECT_EQ("foo || bar || baz", StrCat(expr4));
  const ParallelComposition expr5(
      ParallelCompositionOperator::ALPHABETIZED,
      std::make_unique<RestrictedParallelComposition>(
          std::set<std::string>({"a", "b"}),
          std::make_unique<ModuleIdentifier>("foo"),
          std::make_unique<ModuleIdentifier>("bar")),
      std::make_unique<ModuleIdentifier>("baz"));
  EXPECT_EQ("(foo |[a,b]| bar) || baz", StrCat(expr5));
  const ParallelComposition expr6(
      ParallelCompositionOperator::ALPHABETIZED,
      std::make_unique<ActionRenaming>(
          std::map<std::string, std::string>({{"a", "b"}, {"c", "d"}}),
          std::make_unique<ModuleIdentifier>("foo")),
      std::make_unique<ModuleIdentifier>("bar"));
  EXPECT_EQ("foo {a<-b,c<-d} || bar", StrCat(expr6));
  const ActionHiding expr7({"a", "b"},
                           std::make_unique<ParallelComposition>(
                               ParallelCompositionOperator::ALPHABETIZED,
                               std::make_unique<ModuleIdentifier>("foo"),
                               std::make_unique<ModuleIdentifier>("bar")));
  EXPECT_EQ("(foo || bar) / {a,b}", StrCat(expr7));
}

}  // namespace
