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

#include "ptrutil.h"
#include "strutil.h"

#include "gtest/gtest.h"

namespace {

TEST(ProcessAlgebraTest, Ouput) {
  const ParallelComposition expr1(
      ParallelCompositionOperator::ALPHABETIZED,
      MakeUnique<ModuleIdentifier>("foo"),
      MakeUnique<ParallelComposition>(ParallelCompositionOperator::ALPHABETIZED,
                                      MakeUnique<ModuleIdentifier>("bar"),
                                      MakeUnique<ModuleIdentifier>("baz")));
  EXPECT_EQ("foo || bar || baz", StrCat(expr1));
  const ParallelComposition expr2(
      ParallelCompositionOperator::ALPHABETIZED,
      MakeUnique<ModuleIdentifier>("foo"),
      MakeUnique<ParallelComposition>(ParallelCompositionOperator::ASYNCHRONOUS,
                                      MakeUnique<ModuleIdentifier>("bar"),
                                      MakeUnique<ModuleIdentifier>("baz")));
  EXPECT_EQ("foo || (bar ||| baz)", StrCat(expr2));
  const ParallelComposition expr3(
      ParallelCompositionOperator::ALPHABETIZED,
      MakeUnique<ModuleIdentifier>("foo"),
      MakeUnique<ActionHiding>(std::set<std::string>({"a", "b"}),
                               MakeUnique<ModuleIdentifier>("bar")));
  EXPECT_EQ("foo || bar / {a,b}", StrCat(expr3));
  const ParallelComposition expr4(
      ParallelCompositionOperator::ALPHABETIZED,
      MakeUnique<ParallelComposition>(ParallelCompositionOperator::ALPHABETIZED,
                                      MakeUnique<ModuleIdentifier>("foo"),
                                      MakeUnique<ModuleIdentifier>("bar")),
      MakeUnique<ModuleIdentifier>("baz"));
  EXPECT_EQ("foo || bar || baz", StrCat(expr4));
  const ParallelComposition expr5(ParallelCompositionOperator::ALPHABETIZED,
                                  MakeUnique<RestrictedParallelComposition>(
                                      std::set<std::string>({"a", "b"}),
                                      MakeUnique<ModuleIdentifier>("foo"),
                                      MakeUnique<ModuleIdentifier>("bar")),
                                  MakeUnique<ModuleIdentifier>("baz"));
  EXPECT_EQ("(foo |[a,b]| bar) || baz", StrCat(expr5));
  const ParallelComposition expr6(
      ParallelCompositionOperator::ALPHABETIZED,
      MakeUnique<ActionRenaming>(
          std::map<std::string, std::string>({{"a", "b"}, {"c", "d"}}),
          MakeUnique<ModuleIdentifier>("foo")),
      MakeUnique<ModuleIdentifier>("bar"));
  EXPECT_EQ("foo {a<-b,c<-d} || bar", StrCat(expr6));
  const ActionHiding expr7(
      {"a", "b"},
      MakeUnique<ParallelComposition>(ParallelCompositionOperator::ALPHABETIZED,
                                      MakeUnique<ModuleIdentifier>("foo"),
                                      MakeUnique<ModuleIdentifier>("bar")));
  EXPECT_EQ("(foo || bar) / {a,b}", StrCat(expr7));
}

}  // namespace
