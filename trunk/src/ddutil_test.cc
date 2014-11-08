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

#include <cmath>
#include <vector>

#include "gtest/gtest.h"

namespace {

TEST(StateVariableInfoTest, Constructs) {
  StateVariableInfo v1("foo", 17, 42);
  EXPECT_EQ("foo", v1.name());
  EXPECT_EQ(17, v1.min_value());
  EXPECT_EQ(42, v1.bit_count());

  StateVariableInfo v2("bar", 42, 17);
  EXPECT_EQ("bar", v2.name());
  EXPECT_EQ(42, v2.min_value());
  EXPECT_EQ(17, v2.bit_count());
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

TEST(DecisionDiagramTest, ConvertsConstantADDToBDD) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE(BDD(manager.GetConstant(17.0)).Value());
  EXPECT_TRUE(BDD(manager.GetConstant(-42.0)).Value());
  EXPECT_FALSE(BDD(manager.GetConstant(0)).Value());
}

TEST(DecisionDiagramTest, ConvertsNonConstantADDToBDD) {
  const DecisionDiagramManager manager(2);
  const ADD dd =
      Ite(manager.GetBddVariable(0),
          Ite(manager.GetBddVariable(1),
              manager.GetConstant(1), manager.GetConstant(0)),
          Ite(manager.GetBddVariable(1),
              manager.GetConstant(0), manager.GetConstant(3.14159)));
  EXPECT_TRUE(BDD(dd).ValueInState({true, true}));
  EXPECT_FALSE(BDD(dd).ValueInState({true, false}));
  EXPECT_FALSE(BDD(dd).ValueInState({false, true}));
  EXPECT_TRUE(BDD(dd).ValueInState({false, false}));
}

TEST(DecisionDiagramTest, ConvertsConstantBDDToADD) {
  const DecisionDiagramManager manager(0);
  EXPECT_EQ(1, ADD(manager.GetConstant(true)).Value());
  EXPECT_EQ(0, ADD(manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, ConvertsNonConstantBDDToADD) {
  const DecisionDiagramManager manager(2);
  const BDD dd = manager.GetBddVariable(0) || manager.GetBddVariable(1);
  EXPECT_EQ(1, ADD(dd).ValueInState({true, true}));
  EXPECT_EQ(1, ADD(dd).ValueInState({true, false}));
  EXPECT_EQ(1, ADD(dd).ValueInState({false, true}));
  EXPECT_EQ(0, ADD(dd).ValueInState({false, false}));
}

TEST(DecisionDiagramTest, Permutation) {
  const DecisionDiagramManager manager(2);
  const BDD dd1 = manager.GetBddVariable(0).Permutation({1, 0});
  EXPECT_TRUE(dd1.ValueInState({true, true}));
  EXPECT_FALSE(dd1.ValueInState({true, false}));
  EXPECT_TRUE(dd1.ValueInState({false, true}));
  EXPECT_FALSE(dd1.ValueInState({false, false}));
  const BDD dd2 = manager.GetBddVariable(1).Permutation({1, 0});
  EXPECT_TRUE(dd2.ValueInState({true, true}));
  EXPECT_TRUE(dd2.ValueInState({true, false}));
  EXPECT_FALSE(dd2.ValueInState({false, true}));
  EXPECT_FALSE(dd2.ValueInState({false, false}));
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
  EXPECT_FALSE(dd.ValueInState({true, true}));
  EXPECT_TRUE(dd.ValueInState({true, false}));
  EXPECT_TRUE(dd.ValueInState({false, true}));
  EXPECT_FALSE(dd.ValueInState({false, false}));
}

TEST(DecisionDiagramTest, StrictThresholdConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_TRUE(manager.GetConstant(4).StrictThreshold(3).Value());
  EXPECT_FALSE(manager.GetConstant(3).StrictThreshold(3).Value());
  EXPECT_TRUE(manager.GetConstant(-3).StrictThreshold(-4).Value());
  EXPECT_FALSE(manager.GetConstant(-4).StrictThreshold(-4).Value());
}

TEST(DecisionDiagramTest, StrictThresholdNonConstants) {
  const DecisionDiagramManager manager(2);
  const BDD dd =
      Ite(manager.GetBddVariable(0),
          Ite(manager.GetBddVariable(1),
              manager.GetConstant(2),manager.GetConstant(5)),
          Ite(manager.GetBddVariable(1),
              manager.GetConstant(3), manager.GetConstant(7)))
      .StrictThreshold(3);
  EXPECT_FALSE(dd.ValueInState({true, true}));
  EXPECT_TRUE(dd.ValueInState({true, false}));
  EXPECT_FALSE(dd.ValueInState({false, true}));
  EXPECT_TRUE(dd.ValueInState({false, false}));
}

TEST(DecisionDiagramTest, NegatesConstant) {
  const DecisionDiagramManager manager(0);
  const ADD dd = -manager.GetConstant(2);
  EXPECT_EQ(-2, dd.Value());
}

TEST(DecisionDiagramTest, NegatesNonConstant) {
  const DecisionDiagramManager manager(1);
  const ADD dd = -Ite(manager.GetBddVariable(0),
                      manager.GetConstant(2), manager.GetConstant(-3));
  EXPECT_EQ(-2, dd.ValueInState({true}));
  EXPECT_EQ(3, dd.ValueInState({false}));
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
  EXPECT_EQ(2 + 3, dd.ValueInState({true, true}));
  EXPECT_EQ(2 + 7, dd.ValueInState({true, false}));
  EXPECT_EQ(5 + 3, dd.ValueInState({false, true}));
  EXPECT_EQ(5 + 7, dd.ValueInState({false, false}));
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
  EXPECT_EQ(2 - 3, dd.ValueInState({true, true}));
  EXPECT_EQ(2 - 7, dd.ValueInState({true, false}));
  EXPECT_EQ(5 - 3, dd.ValueInState({false, true}));
  EXPECT_EQ(5 - 7, dd.ValueInState({false, false}));
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
  EXPECT_EQ(2 * 3, dd.ValueInState({true, true}));
  EXPECT_EQ(2 * 7, dd.ValueInState({true, false}));
  EXPECT_EQ(5 * 3, dd.ValueInState({false, true}));
  EXPECT_EQ(5 * 7, dd.ValueInState({false, false}));
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
  EXPECT_EQ(2.0 / 3.0, dd.ValueInState({true, true}));
  EXPECT_EQ(2.0 / 7.0, dd.ValueInState({true, false}));
  EXPECT_EQ(5.0 / 3.0, dd.ValueInState({false, true}));
  EXPECT_EQ(5.0 / 7.0, dd.ValueInState({false, false}));
}
TEST(DecisionDiagramTest, ModuloConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd = manager.GetConstant(2) % manager.GetConstant(3);
  EXPECT_EQ(2 % 3, dd.Value());
}

TEST(DecisionDiagramTest, ModuloNonConstants) {
  const DecisionDiagramManager manager(2);
  const ADD dd =
      Ite(manager.GetBddVariable(0),
          manager.GetConstant(2), manager.GetConstant(5)) %
      Ite(manager.GetBddVariable(1),
          manager.GetConstant(3), manager.GetConstant(7));
  EXPECT_EQ(2 % 3, dd.ValueInState({true, true}));
  EXPECT_EQ(2 % 7, dd.ValueInState({true, false}));
  EXPECT_EQ(5 % 3, dd.ValueInState({false, true}));
  EXPECT_EQ(5 % 7, dd.ValueInState({false, false}));
}

TEST(DecisionDiagramTest, NotConstant) {
  const DecisionDiagramManager manager(0);
  EXPECT_FALSE((!manager.GetConstant(true)).Value());
  EXPECT_TRUE((!manager.GetConstant(false)).Value());
}

TEST(DecisionDiagramTest, NotNonConstant) {
  const DecisionDiagramManager manager(1);
  const BDD dd = !manager.GetBddVariable(0);
  EXPECT_FALSE(dd.ValueInState({true}));
  EXPECT_TRUE(dd.ValueInState({false}));
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
  EXPECT_TRUE(dd.ValueInState({true, true}));
  EXPECT_FALSE(dd.ValueInState({true, false}));
  EXPECT_FALSE(dd.ValueInState({false, true}));
  EXPECT_FALSE(dd.ValueInState({false, false}));
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
  EXPECT_TRUE(dd.ValueInState({true, true}));
  EXPECT_TRUE(dd.ValueInState({true, false}));
  EXPECT_TRUE(dd.ValueInState({false, true}));
  EXPECT_FALSE(dd.ValueInState({false, false}));
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
  EXPECT_FALSE(dd1.ValueInState({true, true}));
  EXPECT_FALSE(dd1.ValueInState({true, false}));
  EXPECT_TRUE(dd1.ValueInState({false, true}));
  EXPECT_FALSE(dd1.ValueInState({false, false}));
  const BDD dd2 = manager.GetBddVariable(0) == manager.GetBddVariable(1);
  EXPECT_TRUE(dd2.ValueInState({true, true}));
  EXPECT_FALSE(dd2.ValueInState({true, false}));
  EXPECT_FALSE(dd2.ValueInState({false, true}));
  EXPECT_TRUE(dd2.ValueInState({false, false}));
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
  EXPECT_TRUE(dd1.ValueInState({true, true}));
  EXPECT_TRUE(dd1.ValueInState({true, false}));
  EXPECT_FALSE(dd1.ValueInState({false, true}));
  EXPECT_TRUE(dd1.ValueInState({false, false}));
  const BDD dd2 = manager.GetBddVariable(0) != manager.GetBddVariable(1);
  EXPECT_FALSE(dd2.ValueInState({true, true}));
  EXPECT_TRUE(dd2.ValueInState({true, false}));
  EXPECT_TRUE(dd2.ValueInState({false, true}));
  EXPECT_FALSE(dd2.ValueInState({false, false}));
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
  EXPECT_TRUE(dd1.ValueInState({true, true}));
  EXPECT_TRUE(dd1.ValueInState({true, false}));
  EXPECT_FALSE(dd1.ValueInState({false, true}));
  EXPECT_TRUE(dd1.ValueInState({false, false}));
  const BDD dd2 = manager.GetBddVariable(0) < manager.GetBddVariable(1);
  EXPECT_FALSE(dd2.ValueInState({true, true}));
  EXPECT_FALSE(dd2.ValueInState({true, false}));
  EXPECT_TRUE(dd2.ValueInState({false, true}));
  EXPECT_FALSE(dd2.ValueInState({false, false}));
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
  EXPECT_TRUE(dd1.ValueInState({true, true}));
  EXPECT_TRUE(dd1.ValueInState({true, false}));
  EXPECT_TRUE(dd1.ValueInState({false, true}));
  EXPECT_TRUE(dd1.ValueInState({false, false}));
  const BDD dd2 = manager.GetBddVariable(0) <= manager.GetBddVariable(1);
  EXPECT_TRUE(dd2.ValueInState({true, true}));
  EXPECT_FALSE(dd2.ValueInState({true, false}));
  EXPECT_TRUE(dd2.ValueInState({false, true}));
  EXPECT_TRUE(dd2.ValueInState({false, false}));
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
  EXPECT_FALSE(dd1.ValueInState({true, true}));
  EXPECT_FALSE(dd1.ValueInState({true, false}));
  EXPECT_TRUE(dd1.ValueInState({false, true}));
  EXPECT_FALSE(dd1.ValueInState({false, false}));
  const BDD dd2 = manager.GetBddVariable(0) >= manager.GetBddVariable(1);
  EXPECT_TRUE(dd2.ValueInState({true, true}));
  EXPECT_TRUE(dd2.ValueInState({true, false}));
  EXPECT_FALSE(dd2.ValueInState({false, true}));
  EXPECT_TRUE(dd2.ValueInState({false, false}));
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
  EXPECT_FALSE(dd1.ValueInState({true, true}));
  EXPECT_FALSE(dd1.ValueInState({true, false}));
  EXPECT_FALSE(dd1.ValueInState({false, true}));
  EXPECT_FALSE(dd1.ValueInState({false, false}));
  const BDD dd2 = manager.GetBddVariable(0) > manager.GetBddVariable(1);
  EXPECT_FALSE(dd2.ValueInState({true, true}));
  EXPECT_TRUE(dd2.ValueInState({true, false}));
  EXPECT_FALSE(dd2.ValueInState({false, true}));
  EXPECT_FALSE(dd2.ValueInState({false, false}));
}

TEST(DecisionDiagramTest, IteConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd1 = Ite(manager.GetConstant(true),
                      manager.GetConstant(17), manager.GetConstant(42));
  EXPECT_EQ(17, dd1.Value());
  const ADD dd2 = Ite(manager.GetConstant(false),
                      manager.GetConstant(17), manager.GetConstant(42));
  EXPECT_EQ(42, dd2.Value());
}

TEST(DecisionDiagramTest, IteNonConstants) {
  const DecisionDiagramManager manager(3);
  const ADD dd =
      Ite(manager.GetBddVariable(0),
          manager.GetAddVariable(1) * manager.GetConstant(17),
          manager.GetAddVariable(2) * manager.GetConstant(42));
  EXPECT_EQ(17, dd.ValueInState({true, true, true}));
  EXPECT_EQ(17, dd.ValueInState({true, true, false}));
  EXPECT_EQ(0, dd.ValueInState({true, false, true}));
  EXPECT_EQ(0, dd.ValueInState({true, false, false}));
  EXPECT_EQ(42, dd.ValueInState({false, true, true}));
  EXPECT_EQ(0, dd.ValueInState({false, true, false}));
  EXPECT_EQ(42, dd.ValueInState({false, false, true}));
  EXPECT_EQ(0, dd.ValueInState({false, false, false}));
}

TEST(DecisionDiagramTest, MinConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd1 = min(manager.GetConstant(17), manager.GetConstant(42));
  EXPECT_EQ(17, dd1.Value());
  const ADD dd2 = min(manager.GetConstant(17), manager.GetConstant(-42));
  EXPECT_EQ(-42, dd2.Value());
}

TEST(DecisionDiagramTest, MinNonConstants) {
  const DecisionDiagramManager manager(2);
  const ADD dd = min(Ite(manager.GetBddVariable(0),
                         manager.GetConstant(2), manager.GetConstant(5)),
                     Ite(manager.GetBddVariable(1),
                         manager.GetConstant(3), manager.GetConstant(7)));
  EXPECT_EQ(2, dd.ValueInState({true, true}));
  EXPECT_EQ(2, dd.ValueInState({true, false}));
  EXPECT_EQ(3, dd.ValueInState({false, true}));
  EXPECT_EQ(5, dd.ValueInState({false, false}));
}

TEST(DecisionDiagramTest, MaxConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd1 = max(manager.GetConstant(17), manager.GetConstant(42));
  EXPECT_EQ(42, dd1.Value());
  const ADD dd2 = max(manager.GetConstant(17), manager.GetConstant(-42));
  EXPECT_EQ(17, dd2.Value());
}

TEST(DecisionDiagramTest, MaxNonConstants) {
  const DecisionDiagramManager manager(2);
  const ADD dd = max(Ite(manager.GetBddVariable(0),
                         manager.GetConstant(2), manager.GetConstant(5)),
                     Ite(manager.GetBddVariable(1),
                         manager.GetConstant(3), manager.GetConstant(7)));
  EXPECT_EQ(3, dd.ValueInState({true, true}));
  EXPECT_EQ(7, dd.ValueInState({true, false}));
  EXPECT_EQ(5, dd.ValueInState({false, true}));
  EXPECT_EQ(7, dd.ValueInState({false, false}));
}

TEST(DecisionDiagramTest, FloorConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_EQ(1, floor(manager.GetConstant(1.0)).Value());
  EXPECT_EQ(0, floor(manager.GetConstant(0.5)).Value());
  EXPECT_EQ(1, floor(manager.GetConstant(1.1)).Value());
  EXPECT_EQ(-1, floor(manager.GetConstant(-0.1)).Value());
}

TEST(DecisionDiagramTest, FloorNonConstants) {
  const DecisionDiagramManager manager(1);
  const ADD dd = floor(Ite(manager.GetBddVariable(0),
                           manager.GetConstant(0.1), manager.GetConstant(1.0)));
  EXPECT_EQ(0, dd.ValueInState({true}));
  EXPECT_EQ(1, dd.ValueInState({false}));
}

TEST(DecisionDiagramTest, CeilConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_EQ(1, ceil(manager.GetConstant(1.0)).Value());
  EXPECT_EQ(1, ceil(manager.GetConstant(0.5)).Value());
  EXPECT_EQ(2, ceil(manager.GetConstant(1.1)).Value());
  EXPECT_EQ(0, ceil(manager.GetConstant(-0.1)).Value());
}

TEST(DecisionDiagramTest, CeilNonConstants) {
  const DecisionDiagramManager manager(1);
  const ADD dd = ceil(Ite(manager.GetBddVariable(0),
                          manager.GetConstant(0.1), manager.GetConstant(1.0)));
  EXPECT_EQ(1, dd.ValueInState({true}));
  EXPECT_EQ(1, dd.ValueInState({false}));
}

TEST(DecisionDiagramTest, PowConstants) {
  const DecisionDiagramManager manager(0);
  const ADD dd1 = pow(manager.GetConstant(17.0), manager.GetConstant(42.0));
  EXPECT_DOUBLE_EQ(pow(17.0, 42.0), dd1.Value());
  const ADD dd2 = pow(manager.GetConstant(17.0), manager.GetConstant(-42.0));
  EXPECT_DOUBLE_EQ(pow(17.0, -42.0), dd2.Value());
}

TEST(DecisionDiagramTest, PowNonConstants) {
  const DecisionDiagramManager manager(2);
  const ADD dd = pow(Ite(manager.GetBddVariable(0),
                         manager.GetConstant(2), manager.GetConstant(5)),
                     Ite(manager.GetBddVariable(1),
                         manager.GetConstant(3), manager.GetConstant(7)));
  const ADD one = manager.GetConstant(1);
  const ADD zero = manager.GetConstant(0);
  EXPECT_DOUBLE_EQ(pow(2.0, 3.0), dd.ValueInState({true, true}));
  EXPECT_DOUBLE_EQ(pow(2.0, 7.0), dd.ValueInState({true, false}));
  EXPECT_DOUBLE_EQ(pow(5.0, 3.0), dd.ValueInState({false, true}));
  EXPECT_DOUBLE_EQ(pow(5.0, 7.0), dd.ValueInState({false, false}));
  EXPECT_EQ(1, pow(one, dd).Value());
  EXPECT_EQ(1, pow(dd, zero).Value());
}

TEST(DecisionDiagramTest, LogConstants) {
  const DecisionDiagramManager manager(0);
  EXPECT_DOUBLE_EQ(log(17.0), log(manager.GetConstant(17.0)).Value());
  EXPECT_DOUBLE_EQ(log(42.0), log(manager.GetConstant(42.0)).Value());
  EXPECT_DOUBLE_EQ(log(0.5), log(manager.GetConstant(0.5)).Value());
}

TEST(DecisionDiagramTest, LogNonConstants) {
  const DecisionDiagramManager manager(1);
  const ADD dd = log(Ite(manager.GetBddVariable(0),
                         manager.GetConstant(17.0), manager.GetConstant(0.5)));
  EXPECT_DOUBLE_EQ(log(17.0), dd.ValueInState({true}));
  EXPECT_DOUBLE_EQ(log(0.5), dd.ValueInState({false}));
}

TEST(DecisionDiagramTest, ComputesCubes) {
  const DecisionDiagramManager manager(2);
  const BDD dd1 = manager.GetCube(VariableArray<BDD>());
  EXPECT_TRUE(dd1.ValueInState({true, true}));
  EXPECT_TRUE(dd1.ValueInState({true, false}));
  EXPECT_TRUE(dd1.ValueInState({false, true}));
  EXPECT_TRUE(dd1.ValueInState({false, false}));
  const BDD dd2 = manager.GetCube(manager.GetBddVariableArray(0, 1, 1));
  EXPECT_TRUE(dd2.ValueInState({true, true}));
  EXPECT_TRUE(dd2.ValueInState({true, false}));
  EXPECT_FALSE(dd2.ValueInState({false, true}));
  EXPECT_FALSE(dd2.ValueInState({false, false}));
  const BDD dd3 = manager.GetCube(manager.GetBddVariableArray(1, 1, 2));
  EXPECT_TRUE(dd3.ValueInState({true, true}));
  EXPECT_FALSE(dd3.ValueInState({true, false}));
  EXPECT_TRUE(dd3.ValueInState({false, true}));
  EXPECT_FALSE(dd3.ValueInState({false, false}));
  const BDD dd4 = manager.GetCube(manager.GetBddVariableArray(0, 1, 2));
  EXPECT_TRUE(dd4.ValueInState({true, true}));
  EXPECT_FALSE(dd4.ValueInState({true, false}));
  EXPECT_FALSE(dd4.ValueInState({false, true}));
  EXPECT_FALSE(dd4.ValueInState({false, false}));
}

TEST(DecisionDiagramTest, BddValueInState) {
  const DecisionDiagramManager manager(12);
  const std::vector<StateVariableInfo> variables = {
      {"a", 4, 3}, {"b", 0, 1}, {"c", 1, 2}};
  const BDD dd = manager.GetBddVariable(0) || manager.GetBddVariable(2) ||
                 manager.GetBddVariable(6) || manager.GetBddVariable(10);
  EXPECT_FALSE(dd.ValueInState({4, 0, 1}, variables));
  EXPECT_FALSE(dd.ValueInState({5, 0, 1}, variables));
  EXPECT_TRUE(dd.ValueInState({6, 0, 1}, variables));
  EXPECT_TRUE(dd.ValueInState({7, 0, 1}, variables));
  EXPECT_TRUE(dd.ValueInState({8, 0, 1}, variables));
  EXPECT_TRUE(dd.ValueInState({4, 1, 1}, variables));
  EXPECT_TRUE(dd.ValueInState({4, 0, 2}, variables));
  EXPECT_FALSE(dd.ValueInState({4, 0, 3}, variables));
}

TEST(DecisionDiagramTest, AddValueInState) {
  const DecisionDiagramManager manager(12);
  const std::vector<StateVariableInfo> variables = {
      {"a", 4, 3}, {"b", 0, 1}, {"c", 1, 2}};
  const ADD dd = Ite(manager.GetBddVariable(0), manager.GetConstant(2),
                     manager.GetConstant(5)) +
                 Ite(manager.GetBddVariable(2), manager.GetConstant(3),
                     manager.GetConstant(7)) +
                 Ite(manager.GetBddVariable(6), manager.GetConstant(1),
                     manager.GetConstant(9)) +
                 Ite(manager.GetBddVariable(10), manager.GetConstant(4),
                     manager.GetConstant(8));
  EXPECT_EQ(5 + 7 + 9 + 8, dd.ValueInState({4, 0, 1}, variables));
  EXPECT_EQ(5 + 7 + 9 + 8, dd.ValueInState({5, 0, 1}, variables));
  EXPECT_EQ(5 + 3 + 9 + 8, dd.ValueInState({6, 0, 1}, variables));
  EXPECT_EQ(5 + 3 + 9 + 8, dd.ValueInState({7, 0, 1}, variables));
  EXPECT_EQ(2 + 7 + 9 + 8, dd.ValueInState({8, 0, 1}, variables));
  EXPECT_EQ(5 + 7 + 1 + 8, dd.ValueInState({4, 1, 1}, variables));
  EXPECT_EQ(5 + 7 + 9 + 4, dd.ValueInState({4, 0, 2}, variables));
  EXPECT_EQ(5 + 7 + 9 + 8, dd.ValueInState({4, 0, 3}, variables));
}

TEST(Log2Test, All) {
  EXPECT_EQ(0, Log2(1));
  EXPECT_EQ(1, Log2(2));
  EXPECT_EQ(1, Log2(3));
  EXPECT_EQ(2, Log2(4));
  EXPECT_EQ(7, Log2(255));
  EXPECT_EQ(8, Log2(256));
}

}  // namespace
