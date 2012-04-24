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

std::vector<bool> sv(bool b1) {
  return std::vector<bool>(1, b1);
}

std::vector<bool> sv(bool b1, bool b2) {
  std::vector<bool> result;
  result.push_back(b1);
  result.push_back(b2);
  return result;
}

TEST(DecisionDiagramManagerTest, Constructs) {
  const DecisionDiagramManager manager(17);
  EXPECT_EQ(17, manager.GetNumVariables());
  EXPECT_EQ(std::numeric_limits<double>::min(), manager.GetEpsilon());
}

TEST(DecisionDiagramManagerTest, SetsAndGetsEpsilon) {
  DecisionDiagramManager manager(0);
  EXPECT_EQ(1e-10, manager.GetConstant(1e-10).Value());
  manager.SetEpsilon(1e-10);
  EXPECT_EQ(1e-10, manager.GetConstant(1e-10).Value());
  EXPECT_EQ(0.0, manager.GetConstant(0.5e-10).Value());
}

TEST(DecisionDiagramTest, IsConstant) {
  const DecisionDiagramManager manager(1);
  EXPECT_TRUE(manager.GetConstant(1).IsConstant());
  EXPECT_TRUE(manager.GetConstant(0).IsConstant());
  EXPECT_TRUE(manager.GetConstant(true).IsConstant());
  EXPECT_TRUE(manager.GetConstant(false).IsConstant());
  EXPECT_TRUE(manager.GetConstant(17).IsConstant());
  EXPECT_TRUE(manager.GetConstant(-42).IsConstant());
  EXPECT_TRUE(manager.GetConstant(3.14159).IsConstant());
  EXPECT_FALSE(manager.GetBddVariable(0).IsConstant());
  EXPECT_FALSE(manager.GetAddVariable(0).IsConstant());
}

TEST(DecisionDiagramTest, Value) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE(manager.GetConstant(true).Value());
  EXPECT_FALSE(manager.GetConstant(false).Value());
  EXPECT_EQ(1, manager.GetConstant(1).Value());
  EXPECT_EQ(0, manager.GetConstant(0).Value());
  EXPECT_EQ(17.0, manager.GetConstant(17.0).Value());
  EXPECT_EQ(-42.0, manager.GetConstant(-42.0).Value());
  EXPECT_EQ(3.14159, manager.GetConstant(3.14159).Value());
}

TEST(DecisionDiagramTest, ConvertsConstantBDDToADD) {
  const DecisionDiagramManager manager(0);
  EXPECT_EQ(1, ADD(manager.GetConstant(true)).Value());
  EXPECT_EQ(0, ADD(manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, ConvertsNonConstantBDDToADD) {
  const DecisionDiagramManager manager(2);
  const BDD dd = manager.GetBddVariable(0) || manager.GetBddVariable(1);
  EXPECT_EQ(1, ADD(dd).ValueInState(sv(true, true)));
  EXPECT_EQ(1, ADD(dd).ValueInState(sv(true, false)));
  EXPECT_EQ(1, ADD(dd).ValueInState(sv(false, true)));
  EXPECT_EQ(0, ADD(dd).ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, IntervalConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE(manager.GetConstant(3).Interval(3, 4).Value());
  EXPECT_TRUE(manager.GetConstant(-3).Interval(-4, -3).Value());
  EXPECT_TRUE(manager.GetConstant(3).Interval(-3, 4).Value());
  EXPECT_FALSE(manager.GetConstant(3).Interval(0, 2).Value());
  EXPECT_FALSE(manager.GetConstant(3).Interval(4, 17).Value());
}

TEST(DecisionDiagramTest, IntervalNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd =
      Ite(manager.GetBddVariable(0),
          Ite(manager.GetBddVariable(1),
              manager.GetConstant(2), manager.GetConstant(5)),
          Ite(manager.GetBddVariable(1),
              manager.GetConstant(3), manager.GetConstant(7))).Interval(3, 6);
  EXPECT_FALSE(dd.ValueInState(sv(true, true)));
  EXPECT_TRUE(dd.ValueInState(sv(true, false)));
  EXPECT_TRUE(dd.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, AddsConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd = manager.GetConstant(2) + manager.GetConstant(3);
  EXPECT_EQ(2 + 3, dd.Value());
}

TEST(DecisionDiagramTest, AddsNonConstants) {
  const DecisionDiagramManager manager(2);
  const ADD dd =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) +
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(3), manager.GetConstant(7));
  EXPECT_EQ(2 + 3, dd.ValueInState(sv(true, true)));
  EXPECT_EQ(2 + 7, dd.ValueInState(sv(true, false)));
  EXPECT_EQ(5 + 3, dd.ValueInState(sv(false, true)));
  EXPECT_EQ(5 + 7, dd.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, SubtractsConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd = manager.GetConstant(2) - manager.GetConstant(3);
  EXPECT_EQ(2 - 3, dd.Value());
}

TEST(DecisionDiagramTest, SubtractsNonConstants) {
  const DecisionDiagramManager manager(2);
  const ADD dd =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) -
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(3), manager.GetConstant(7));
  EXPECT_EQ(2 - 3, dd.ValueInState(sv(true, true)));
  EXPECT_EQ(2 - 7, dd.ValueInState(sv(true, false)));
  EXPECT_EQ(5 - 3, dd.ValueInState(sv(false, true)));
  EXPECT_EQ(5 - 7, dd.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, MultipliesConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd = manager.GetConstant(2) * manager.GetConstant(3);
  EXPECT_EQ(2 * 3, dd.Value());
}

TEST(DecisionDiagramTest, MultipliesNonConstants) {
  const DecisionDiagramManager manager(2);
  const ADD dd =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) *
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(3), manager.GetConstant(7));
  EXPECT_EQ(2 * 3, dd.ValueInState(sv(true, true)));
  EXPECT_EQ(2 * 7, dd.ValueInState(sv(true, false)));
  EXPECT_EQ(5 * 3, dd.ValueInState(sv(false, true)));
  EXPECT_EQ(5 * 7, dd.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, DividesConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd = manager.GetConstant(2) / manager.GetConstant(3);
  EXPECT_EQ(2.0 / 3.0, dd.Value());
}

TEST(DecisionDiagramTest, DividesNonConstants) {
  const DecisionDiagramManager manager(2);
  const ADD dd =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) /
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(3), manager.GetConstant(7));
  EXPECT_EQ(2.0 / 3.0, dd.ValueInState(sv(true, true)));
  EXPECT_EQ(2.0 / 7.0, dd.ValueInState(sv(true, false)));
  EXPECT_EQ(5.0 / 3.0, dd.ValueInState(sv(false, true)));
  EXPECT_EQ(5.0 / 7.0, dd.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, NotConstant) {
  const DecisionDiagramManager manager(0);
  EXPECT_FALSE((!manager.GetConstant(true)).Value());
  EXPECT_TRUE((!manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, NotNonConstant) {
  const DecisionDiagramManager manager(1);
  const BDD dd = !manager.GetBddVariable(0);
  EXPECT_FALSE(dd.ValueInState(sv(true)));
  EXPECT_TRUE(dd.ValueInState(sv(false)));
}

TEST(DecisionDiagramTest, AndConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE((manager.GetConstant(true) &&
               manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(true) &&
                manager.GetConstant(false)).Value());
  EXPECT_FALSE((manager.GetConstant(false) &&
                manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(false) &&
                manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, AndNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd = manager.GetBddVariable(0) && manager.GetBddVariable(1);
  EXPECT_TRUE(dd.ValueInState(sv(true, true)));
  EXPECT_FALSE(dd.ValueInState(sv(true, false)));
  EXPECT_FALSE(dd.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, OrConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE((manager.GetConstant(true) ||
               manager.GetConstant(true)).Value());
  EXPECT_TRUE((manager.GetConstant(true) ||
               manager.GetConstant(false)).Value());
  EXPECT_TRUE((manager.GetConstant(false) ||
               manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(false) ||
                manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, OrNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd = manager.GetBddVariable(0) || manager.GetBddVariable(1);
  EXPECT_TRUE(dd.ValueInState(sv(true, true)));
  EXPECT_TRUE(dd.ValueInState(sv(true, false)));
  EXPECT_TRUE(dd.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, EqualsConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE((manager.GetConstant(17.0) ==
               manager.GetConstant(17.0)).Value());
  EXPECT_FALSE((manager.GetConstant(17.0) ==
                manager.GetConstant(42.0)).Value());
  EXPECT_FALSE((manager.GetConstant(42.0) ==
                manager.GetConstant(17.0)).Value());
  EXPECT_TRUE((manager.GetConstant(true) ==
               manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(true) ==
                manager.GetConstant(false)).Value());
  EXPECT_FALSE((manager.GetConstant(false) ==
                manager.GetConstant(true)).Value());
  EXPECT_TRUE((manager.GetConstant(false) ==
               manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, EqualsNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd1 =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) ==
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(5), manager.GetConstant(7));
  EXPECT_FALSE(dd1.ValueInState(sv(true, true)));
  EXPECT_FALSE(dd1.ValueInState(sv(true, false)));
  EXPECT_TRUE(dd1.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd1.ValueInState(sv(false, false)));
  const BDD dd2 = manager.GetBddVariable(0) == manager.GetBddVariable(1);
  EXPECT_TRUE(dd2.ValueInState(sv(true, true)));
  EXPECT_FALSE(dd2.ValueInState(sv(true, false)));
  EXPECT_FALSE(dd2.ValueInState(sv(false, true)));
  EXPECT_TRUE(dd2.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, NotEqualsConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_FALSE((manager.GetConstant(17.0) !=
                manager.GetConstant(17.0)).Value());
  EXPECT_TRUE((manager.GetConstant(17.0) !=
               manager.GetConstant(42.0)).Value());
  EXPECT_TRUE((manager.GetConstant(42.0) !=
               manager.GetConstant(17.0)).Value());
  EXPECT_FALSE((manager.GetConstant(true) !=
                manager.GetConstant(true)).Value());
  EXPECT_TRUE((manager.GetConstant(true) !=
               manager.GetConstant(false)).Value());
  EXPECT_TRUE((manager.GetConstant(false) !=
               manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(false) !=
                manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, NotEqualsNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd1 =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) !=
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(5), manager.GetConstant(7));
  EXPECT_TRUE(dd1.ValueInState(sv(true, true)));
  EXPECT_TRUE(dd1.ValueInState(sv(true, false)));
  EXPECT_FALSE(dd1.ValueInState(sv(false, true)));
  EXPECT_TRUE(dd1.ValueInState(sv(false, false)));
  const BDD dd2 = manager.GetBddVariable(0) != manager.GetBddVariable(1);
  EXPECT_FALSE(dd2.ValueInState(sv(true, true)));
  EXPECT_TRUE(dd2.ValueInState(sv(true, false)));
  EXPECT_TRUE(dd2.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd2.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, LessConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_FALSE((manager.GetConstant(17.0) <
                manager.GetConstant(17.0)).Value());
  EXPECT_TRUE((manager.GetConstant(17.0) <
               manager.GetConstant(42.0)).Value());
  EXPECT_FALSE((manager.GetConstant(42.0) <
                manager.GetConstant(17.0)).Value());
  EXPECT_FALSE((manager.GetConstant(true) <
                manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(true) <
               manager.GetConstant(false)).Value());
  EXPECT_TRUE((manager.GetConstant(false) <
               manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(false) <
                manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, LessNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd1 =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) <
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(5), manager.GetConstant(7));
  EXPECT_TRUE(dd1.ValueInState(sv(true, true)));
  EXPECT_TRUE(dd1.ValueInState(sv(true, false)));
  EXPECT_FALSE(dd1.ValueInState(sv(false, true)));
  EXPECT_TRUE(dd1.ValueInState(sv(false, false)));
  const BDD dd2 = manager.GetBddVariable(0) < manager.GetBddVariable(1);
  EXPECT_FALSE(dd2.ValueInState(sv(true, true)));
  EXPECT_FALSE(dd2.ValueInState(sv(true, false)));
  EXPECT_TRUE(dd2.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd2.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, LessEqualsConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE((manager.GetConstant(17.0) <=
               manager.GetConstant(17.0)).Value());
  EXPECT_TRUE((manager.GetConstant(17.0) <=
               manager.GetConstant(42.0)).Value());
  EXPECT_FALSE((manager.GetConstant(42.0) <=
                manager.GetConstant(17.0)).Value());
  EXPECT_TRUE((manager.GetConstant(true) <=
               manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(true) <=
                manager.GetConstant(false)).Value());
  EXPECT_TRUE((manager.GetConstant(false) <=
               manager.GetConstant(true)).Value());
  EXPECT_TRUE((manager.GetConstant(false) <=
               manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, LessEqualsNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd1 =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) <=
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(5), manager.GetConstant(7));
  EXPECT_TRUE(dd1.ValueInState(sv(true, true)));
  EXPECT_TRUE(dd1.ValueInState(sv(true, false)));
  EXPECT_TRUE(dd1.ValueInState(sv(false, true)));
  EXPECT_TRUE(dd1.ValueInState(sv(false, false)));
  const BDD dd2 = manager.GetBddVariable(0) <= manager.GetBddVariable(1);
  EXPECT_TRUE(dd2.ValueInState(sv(true, true)));
  EXPECT_FALSE(dd2.ValueInState(sv(true, false)));
  EXPECT_TRUE(dd2.ValueInState(sv(false, true)));
  EXPECT_TRUE(dd2.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, GreaterEqualsConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE((manager.GetConstant(17.0) >=
               manager.GetConstant(17.0)).Value());
  EXPECT_FALSE((manager.GetConstant(17.0) >=
                manager.GetConstant(42.0)).Value());
  EXPECT_TRUE((manager.GetConstant(42.0) >=
               manager.GetConstant(17.0)).Value());
  EXPECT_TRUE((manager.GetConstant(true) >=
               manager.GetConstant(true)).Value());
  EXPECT_TRUE((manager.GetConstant(true) >=
               manager.GetConstant(false)).Value());
  EXPECT_FALSE((manager.GetConstant(false) >=
                manager.GetConstant(true)).Value());
  EXPECT_TRUE((manager.GetConstant(false) >=
               manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, GreaterEqualsNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd1 =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) >=
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(5), manager.GetConstant(7));
  EXPECT_FALSE(dd1.ValueInState(sv(true, true)));
  EXPECT_FALSE(dd1.ValueInState(sv(true, false)));
  EXPECT_TRUE(dd1.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd1.ValueInState(sv(false, false)));
  const BDD dd2 = manager.GetBddVariable(0) >= manager.GetBddVariable(1);
  EXPECT_TRUE(dd2.ValueInState(sv(true, true)));
  EXPECT_TRUE(dd2.ValueInState(sv(true, false)));
  EXPECT_FALSE(dd2.ValueInState(sv(false, true)));
  EXPECT_TRUE(dd2.ValueInState(sv(false, false)));
}

TEST(DecisionDiagramTest, GreaterConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_FALSE((manager.GetConstant(17.0) >
                manager.GetConstant(17.0)).Value());
  EXPECT_FALSE((manager.GetConstant(17.0) >
                manager.GetConstant(42.0)).Value());
  EXPECT_TRUE((manager.GetConstant(42.0) >
               manager.GetConstant(17.0)).Value());
  EXPECT_FALSE((manager.GetConstant(true) >
                manager.GetConstant(true)).Value());
  EXPECT_TRUE((manager.GetConstant(true) >
               manager.GetConstant(false)).Value());
  EXPECT_FALSE((manager.GetConstant(false) >
                manager.GetConstant(true)).Value());
  EXPECT_FALSE((manager.GetConstant(false) >
                manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, GreaterNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd1 =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) >
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(5), manager.GetConstant(7));
  EXPECT_FALSE(dd1.ValueInState(sv(true, true)));
  EXPECT_FALSE(dd1.ValueInState(sv(true, false)));
  EXPECT_FALSE(dd1.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd1.ValueInState(sv(false, false)));
  const BDD dd2 = manager.GetBddVariable(0) > manager.GetBddVariable(1);
  EXPECT_FALSE(dd2.ValueInState(sv(true, true)));
  EXPECT_TRUE(dd2.ValueInState(sv(true, false)));
  EXPECT_FALSE(dd2.ValueInState(sv(false, true)));
  EXPECT_FALSE(dd2.ValueInState(sv(false, false)));
}

}  // namespace
