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

#include "ddutil.h"

#include <vector>

#include "gtest/gtest.h"

namespace {

std::vector<bool> sv(bool b1, bool b2) {
  std::vector<bool> result;
  result.push_back(b1);
  result.push_back(b2);
  return result;
}

TEST(DecisionDiagramManagerTest, Constructs) {
  const DecisionDiagramManager dd_manager(17);
  EXPECT_EQ(17, dd_manager.GetNumVariables());
  EXPECT_EQ(std::numeric_limits<double>::min(), dd_manager.GetEpsilon());
}

TEST(DecisionDiagramManagerTest, SetsAndGetsEpsilon) {
  DecisionDiagramManager dd_manager(0);
  EXPECT_EQ(1e-10, dd_manager.GetConstant(1e-10).Value());
  dd_manager.SetEpsilon(1e-10);
  EXPECT_EQ(1e-10, dd_manager.GetConstant(1e-10).Value());
  EXPECT_EQ(0.0, dd_manager.GetConstant(0.5e-10).Value());
}

TEST(DecisionDiagramTest, IsConstant) {
  const DecisionDiagramManager dd_manager(1);
  EXPECT_TRUE(dd_manager.GetConstant(1).IsConstant());
  EXPECT_TRUE(dd_manager.GetConstant(0).IsConstant());
  EXPECT_TRUE(dd_manager.GetConstant(true).IsConstant());
  EXPECT_TRUE(dd_manager.GetConstant(false).IsConstant());
  EXPECT_TRUE(dd_manager.GetConstant(17).IsConstant());
  EXPECT_TRUE(dd_manager.GetConstant(-42).IsConstant());
  EXPECT_TRUE(dd_manager.GetConstant(3.14159).IsConstant());
  EXPECT_FALSE(dd_manager.GetBddVariable(0).IsConstant());
  EXPECT_FALSE(dd_manager.GetAddVariable(0).IsConstant());
}

TEST(DecisionDiagramTest, Value) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_TRUE(dd_manager.GetConstant(true).Value());
  EXPECT_FALSE(dd_manager.GetConstant(false).Value());
  EXPECT_EQ(1, dd_manager.GetConstant(1).Value());
  EXPECT_EQ(0, dd_manager.GetConstant(0).Value());
  EXPECT_EQ(17.0, dd_manager.GetConstant(17.0).Value());
  EXPECT_EQ(-42.0, dd_manager.GetConstant(-42.0).Value());
  EXPECT_EQ(3.14159, dd_manager.GetConstant(3.14159).Value());
}

TEST(DecisionDiagramTest, AddsConstants) {
  const DecisionDiagramManager dd_manager(0);
  const ADD dd = dd_manager.GetConstant(2) + dd_manager.GetConstant(3);
  EXPECT_EQ(2 + 3, dd.Value());
}

TEST(DecisionDiagramTest, AddsNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) +
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(3), dd_manager.GetConstant(7));
  EXPECT_EQ(2 + 3, dd.ValueInState(sv(true, true)));
  EXPECT_EQ(2 + 7, dd.ValueInState(sv(true, false)));
  EXPECT_EQ(5 + 3, dd.ValueInState(sv(false, true)));
  EXPECT_EQ(5 + 7, dd.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, MultipliesConstants) {
  const DecisionDiagramManager dd_manager(0);
  const ADD dd = dd_manager.GetConstant(2) * dd_manager.GetConstant(3);
  EXPECT_EQ(2 * 3, dd.Value());
}

TEST(DecisionDiagramTest, MultipliesNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) *
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(3), dd_manager.GetConstant(7));
  EXPECT_EQ(2 * 3, dd.ValueInState(sv(true, true)));
  EXPECT_EQ(2 * 7, dd.ValueInState(sv(true, false)));
  EXPECT_EQ(5 * 3, dd.ValueInState(sv(false, true)));
  EXPECT_EQ(5 * 7, dd.ValueInState(sv(false, false)));
}

}  // namespace
