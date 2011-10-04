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

#include "src/ddutil.h"

#include <cmath>
#include <limits>

#include "glog/logging.h"
#include "gtest/gtest.h"

namespace {

// Returns the value of dd for a given variable assignment.
double DoubleValue(const DecisionDiagramManager& dd_manager,
                   const ADD& dd, const std::vector<bool>& bits) {
  CHECK_EQ(dd_manager.num_variables(), bits.size());
  ADD bits_dd = dd_manager.GetConstant(1);
  for (int i = 0; i < bits.size(); ++i) {
    ADD var_dd = dd_manager.GetAddVariable(i);
    if (!bits[i]) {
      var_dd = dd_manager.GetConstant(1) - var_dd;
    }
    bits_dd = bits_dd * var_dd;
  }
  return (bits_dd * dd).ExistAbstract(dd_manager.GetAddCube()).DoubleValue();
}

// Returns the value of dd for a given variable assignment.
bool BoolValue(const DecisionDiagramManager& dd_manager,
               const BDD& dd, const std::vector<bool>& bits) {
  CHECK_EQ(dd_manager.num_variables(), bits.size());
  BDD bits_dd = dd_manager.GetConstant(true);
  for (int i = 0; i < bits.size(); ++i) {
    BDD var_dd = dd_manager.GetBddVariable(i);
    if (!bits[i]) {
      var_dd = !var_dd;
    }
    bits_dd = bits_dd && var_dd;
  }
  return (bits_dd && dd).ExistAbstract(dd_manager.GetBddCube()).BoolValue();
}

TEST(DecisionDiagramManagerTest, Constructs) {
  const DecisionDiagramManager dd_manager(17);
  EXPECT_EQ(17, dd_manager.num_variables());
  EXPECT_EQ(std::numeric_limits<double>::min(), dd_manager.GetEpsilon());
}

TEST(DecisionDiagramManagerTest, SetsAndGetsEpsilon) {
  DecisionDiagramManager dd_manager(0);
  EXPECT_EQ(1e-10, dd_manager.GetConstant(1e-10).DoubleValue());
  dd_manager.SetEpsilon(1e-10);
  EXPECT_EQ(1e-10, dd_manager.GetConstant(1e-10).DoubleValue());
  EXPECT_EQ(0.0, dd_manager.GetConstant(0.5e-10).DoubleValue());
}

TEST(DecisionDiagramManagerTest, VariableIdentity) {
  DecisionDiagramManager dd_manager(4);
  BDD dd1 = dd_manager.VariableIdentity(1, 1);
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, true, true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, true, true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, true, false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, true, false, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, false, true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, false, true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, false, false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, false, false, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, true, true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, true, true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, true, false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, true, false, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, false, true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, false, true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, false, false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, false, false, false }));
  BDD dd2 = dd_manager.VariableIdentity(0, 1);
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { true, true, true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, true, true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, true, false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { true, true, false, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, false, true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, false, true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, false, false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, false, false, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, true, true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, true, true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, true, false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, true, false, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { false, false, true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, false, true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, false, false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { false, false, false, false }));
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

TEST(DecisionDiagramTest, DoubleValue) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_DOUBLE_EQ(1.0, dd_manager.GetConstant(1).DoubleValue());
  EXPECT_DOUBLE_EQ(0.0, dd_manager.GetConstant(0).DoubleValue());
  EXPECT_DOUBLE_EQ(17.0, dd_manager.GetConstant(17.0).DoubleValue());
  EXPECT_DOUBLE_EQ(-42.0, dd_manager.GetConstant(-42.0).DoubleValue());
  EXPECT_DOUBLE_EQ(3.14159, dd_manager.GetConstant(3.14159).DoubleValue());
}

TEST(DecisionDiagramTest, BoolValue) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_TRUE(dd_manager.GetConstant(true).BoolValue());
  EXPECT_FALSE(dd_manager.GetConstant(false).BoolValue());
}

TEST(DecisionDiagramTest, NegatesConstant) {
  const DecisionDiagramManager dd_manager(0);
  const ADD dd = -dd_manager.GetConstant(17);
  EXPECT_DOUBLE_EQ(-17.0, dd.DoubleValue());
}

TEST(DecisionDiagramTest, NegatesNonConstant) {
  const DecisionDiagramManager dd_manager(1);
  const ADD dd = -Ite(dd_manager.GetBddVariable(0),
                      dd_manager.GetConstant(17), dd_manager.GetConstant(-42));
  EXPECT_DOUBLE_EQ(-17.0, DoubleValue(dd_manager, dd, { true }));
  EXPECT_DOUBLE_EQ(42.0, DoubleValue(dd_manager, dd, { false }));
}

TEST(DecisionDiagramTest, MultipliesConstants) {
  const DecisionDiagramManager dd_manager(0);
  const ADD dd = dd_manager.GetConstant(2) * dd_manager.GetConstant(3);
  EXPECT_DOUBLE_EQ(2.0 * 3.0, dd.DoubleValue());
}

TEST(DecisionDiagramTest, MultipliesNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) *
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(3), dd_manager.GetConstant(7));
  EXPECT_DOUBLE_EQ(2.0 * 3.0, DoubleValue(dd_manager, dd, { true, true }));
  EXPECT_DOUBLE_EQ(2.0 * 7.0, DoubleValue(dd_manager, dd, { true, false }));
  EXPECT_DOUBLE_EQ(5.0 * 3.0, DoubleValue(dd_manager, dd, { false, true }));
  EXPECT_DOUBLE_EQ(5.0 * 7.0, DoubleValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, DividesConstants) {
  const DecisionDiagramManager dd_manager(0);
  const ADD dd = dd_manager.GetConstant(2) / dd_manager.GetConstant(3);
  EXPECT_DOUBLE_EQ(2.0 / 3.0, dd.DoubleValue());
}

TEST(DecisionDiagramTest, DividesNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) /
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(3), dd_manager.GetConstant(7));
  EXPECT_DOUBLE_EQ(2.0 / 3.0, DoubleValue(dd_manager, dd, { true, true }));
  EXPECT_DOUBLE_EQ(2.0 / 7.0, DoubleValue(dd_manager, dd, { true, false }));
  EXPECT_DOUBLE_EQ(5.0 / 3.0, DoubleValue(dd_manager, dd, { false, true }));
  EXPECT_DOUBLE_EQ(5.0 / 7.0, DoubleValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, AddsConstants) {
  const DecisionDiagramManager dd_manager(0);
  const ADD dd = dd_manager.GetConstant(2) + dd_manager.GetConstant(3);
  EXPECT_DOUBLE_EQ(2.0 + 3.0, dd.DoubleValue());
}

TEST(DecisionDiagramTest, AddsNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) +
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(3), dd_manager.GetConstant(7));
  EXPECT_DOUBLE_EQ(2.0 + 3.0, DoubleValue(dd_manager, dd, { true, true }));
  EXPECT_DOUBLE_EQ(2.0 + 7.0, DoubleValue(dd_manager, dd, { true, false }));
  EXPECT_DOUBLE_EQ(5.0 + 3.0, DoubleValue(dd_manager, dd, { false, true }));
  EXPECT_DOUBLE_EQ(5.0 + 7.0, DoubleValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, SubtractsConstants) {
  const DecisionDiagramManager dd_manager(0);
  const ADD dd = dd_manager.GetConstant(2) - dd_manager.GetConstant(3);
  EXPECT_DOUBLE_EQ(2.0 - 3.0, dd.DoubleValue());
}

TEST(DecisionDiagramTest, SubtractsNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) -
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(3), dd_manager.GetConstant(7));
  EXPECT_DOUBLE_EQ(2.0 - 3.0, DoubleValue(dd_manager, dd, { true, true }));
  EXPECT_DOUBLE_EQ(2.0 - 7.0, DoubleValue(dd_manager, dd, { true, false }));
  EXPECT_DOUBLE_EQ(5.0 - 3.0, DoubleValue(dd_manager, dd, { false, true }));
  EXPECT_DOUBLE_EQ(5.0 - 7.0, DoubleValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, ModuloConstants) {
  const DecisionDiagramManager dd_manager(0);
  const ADD dd = dd_manager.GetConstant(2) % dd_manager.GetConstant(3);
  EXPECT_DOUBLE_EQ(2 % 3, dd.DoubleValue());
}

TEST(DecisionDiagramTest, ModuloNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) %
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(3), dd_manager.GetConstant(7));
  EXPECT_DOUBLE_EQ(2 % 3, DoubleValue(dd_manager, dd, { true, true }));
  EXPECT_DOUBLE_EQ(2 % 7, DoubleValue(dd_manager, dd, { true, false }));
  EXPECT_DOUBLE_EQ(5 % 3, DoubleValue(dd_manager, dd, { false, true }));
  EXPECT_DOUBLE_EQ(5 % 7, DoubleValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, NotConstant) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_FALSE((!dd_manager.GetConstant(true)).BoolValue());
  EXPECT_TRUE((!dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, NotNonConstant) {
  const DecisionDiagramManager dd_manager(1);
  const BDD dd = !dd_manager.GetBddVariable(0);
  EXPECT_FALSE(BoolValue(dd_manager, dd, { true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd, { false }));
}

TEST(DecisionDiagramTest, AndConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_TRUE((dd_manager.GetConstant(true) &&
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(true) &&
                dd_manager.GetConstant(false)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) &&
                dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) &&
                dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, AndNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const BDD dd = dd_manager.GetBddVariable(0) && dd_manager.GetBddVariable(1);
  EXPECT_TRUE(BoolValue(dd_manager, dd, { true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd, { true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd, { false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, OrConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_TRUE((dd_manager.GetConstant(true) ||
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(true) ||
               dd_manager.GetConstant(false)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(false) ||
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) ||
                dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, OrNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const BDD dd = dd_manager.GetBddVariable(0) || dd_manager.GetBddVariable(1);
  EXPECT_TRUE(BoolValue(dd_manager, dd, { true, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd, { true, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd, { false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, EqualsConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_TRUE((dd_manager.GetConstant(17.0) ==
               dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(17.0) ==
                dd_manager.GetConstant(42.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(42.0) ==
               dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(true) ==
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(true) ==
                dd_manager.GetConstant(false)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) ==
                dd_manager.GetConstant(true)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(false) ==
               dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, EqualsNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const BDD dd1 =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) ==
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(5), dd_manager.GetConstant(7));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, false }));
  const BDD dd2 = dd_manager.GetBddVariable(0) == dd_manager.GetBddVariable(1);
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { false, false }));
}

TEST(DecisionDiagramTest, NotEqualsConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_FALSE((dd_manager.GetConstant(17.0) !=
                dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(17.0) !=
               dd_manager.GetConstant(42.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(42.0) !=
               dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(true) !=
                dd_manager.GetConstant(true)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(true) !=
               dd_manager.GetConstant(false)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(false) !=
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) !=
                dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, NotEqualsNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const BDD dd1 =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) !=
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(5), dd_manager.GetConstant(7));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, false }));
  const BDD dd2 = dd_manager.GetBddVariable(0) != dd_manager.GetBddVariable(1);
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { true, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, false }));
}

TEST(DecisionDiagramTest, LessConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_FALSE((dd_manager.GetConstant(17.0) <
                dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(17.0) <
               dd_manager.GetConstant(42.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(42.0) <
                dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(true) <
                dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(true) <
               dd_manager.GetConstant(false)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(false) <
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) <
                dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, LessNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const BDD dd1 =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) <
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(5), dd_manager.GetConstant(7));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, false }));
  const BDD dd2 = dd_manager.GetBddVariable(0) < dd_manager.GetBddVariable(1);
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, false }));
}

TEST(DecisionDiagramTest, LessEqualsConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_TRUE((dd_manager.GetConstant(17.0) <=
               dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(17.0) <=
               dd_manager.GetConstant(42.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(42.0) <=
                dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(true) <=
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(true) <=
                dd_manager.GetConstant(false)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(false) <=
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(false) <=
               dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, LessEqualsNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const BDD dd1 =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) <=
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(5), dd_manager.GetConstant(7));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { true, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, false }));
  const BDD dd2 = dd_manager.GetBddVariable(0) <= dd_manager.GetBddVariable(1);
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { false, false }));
}

TEST(DecisionDiagramTest, GreaterEqualsConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_TRUE((dd_manager.GetConstant(17.0) >=
               dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(17.0) >=
                dd_manager.GetConstant(42.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(42.0) >=
               dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(true) >=
               dd_manager.GetConstant(true)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(true) >=
               dd_manager.GetConstant(false)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) >=
                dd_manager.GetConstant(true)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(false) >=
               dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, GreaterEqualsNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const BDD dd1 =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) >=
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(5), dd_manager.GetConstant(7));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, false }));
  EXPECT_TRUE(BoolValue(dd_manager, dd1, { false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, false }));
  const BDD dd2 = dd_manager.GetBddVariable(0) >= dd_manager.GetBddVariable(1);
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { true, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { false, false }));
}

TEST(DecisionDiagramTest, GreaterConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_FALSE((dd_manager.GetConstant(17.0) >
                dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(17.0) >
                dd_manager.GetConstant(42.0)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(42.0) >
               dd_manager.GetConstant(17.0)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(true) >
                dd_manager.GetConstant(true)).BoolValue());
  EXPECT_TRUE((dd_manager.GetConstant(true) >
               dd_manager.GetConstant(false)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) >
                dd_manager.GetConstant(true)).BoolValue());
  EXPECT_FALSE((dd_manager.GetConstant(false) >
                dd_manager.GetConstant(false)).BoolValue());
}

TEST(DecisionDiagramTest, GreaterNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const BDD dd1 =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetConstant(2), dd_manager.GetConstant(5)) >
      Ite(dd_manager.GetBddVariable(1),
          dd_manager.GetConstant(5), dd_manager.GetConstant(7));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd1, { false, false }));
  const BDD dd2 = dd_manager.GetBddVariable(0) > dd_manager.GetBddVariable(1);
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { true, true }));
  EXPECT_TRUE(BoolValue(dd_manager, dd2, { true, false }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, true }));
  EXPECT_FALSE(BoolValue(dd_manager, dd2, { false, false }));
}

TEST(DecisionDiagramTest, IteConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_EQ(17.0,
            Ite(dd_manager.GetConstant(true),
                dd_manager.GetConstant(17.0),
                dd_manager.GetConstant(42.0)).DoubleValue());
  EXPECT_EQ(42.0,
            Ite(dd_manager.GetConstant(false),
                dd_manager.GetConstant(17.0),
                dd_manager.GetConstant(42.0)).DoubleValue());
}

TEST(DecisionDiagramTest, IteNonConstants) {
  const DecisionDiagramManager dd_manager(3);
  const ADD dd =
      Ite(dd_manager.GetBddVariable(0),
          dd_manager.GetAddVariable(1) * dd_manager.GetConstant(17.0),
          dd_manager.GetAddVariable(2) * dd_manager.GetConstant(42.0));
  EXPECT_DOUBLE_EQ(17.0, DoubleValue(dd_manager, dd, { true, true, true }));
  EXPECT_DOUBLE_EQ(17.0, DoubleValue(dd_manager, dd, { true, true, false }));
  EXPECT_DOUBLE_EQ(0.0, DoubleValue(dd_manager, dd, { true, false, true }));
  EXPECT_DOUBLE_EQ(0.0, DoubleValue(dd_manager, dd, { true, false, false }));
  EXPECT_DOUBLE_EQ(42.0, DoubleValue(dd_manager, dd, { false, true, true }));
  EXPECT_DOUBLE_EQ(0.0, DoubleValue(dd_manager, dd, { false, true, false }));
  EXPECT_DOUBLE_EQ(42.0, DoubleValue(dd_manager, dd, { false, false, true }));
  EXPECT_DOUBLE_EQ(0.0, DoubleValue(dd_manager, dd, { false, false, false }));
}

TEST(DecisionDiagramTest, MinConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_DOUBLE_EQ(17.0,
                   min(dd_manager.GetConstant(17.0),
                       dd_manager.GetConstant(42.0)).DoubleValue());
  EXPECT_DOUBLE_EQ(-42.0,
                   min(dd_manager.GetConstant(17.0),
                       dd_manager.GetConstant(-42.0)).DoubleValue());
}

TEST(DecisionDiagramTest, MinNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd = min(Ite(dd_manager.GetBddVariable(0),
                         dd_manager.GetConstant(2), dd_manager.GetConstant(5)),
                     Ite(dd_manager.GetBddVariable(1),
                         dd_manager.GetConstant(3), dd_manager.GetConstant(7)));
  EXPECT_DOUBLE_EQ(2.0, DoubleValue(dd_manager, dd, { true, true }));
  EXPECT_DOUBLE_EQ(2.0, DoubleValue(dd_manager, dd, { true, false }));
  EXPECT_DOUBLE_EQ(3.0, DoubleValue(dd_manager, dd, { false, true }));
  EXPECT_DOUBLE_EQ(5.0, DoubleValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, MaxConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_DOUBLE_EQ(42.0,
                   max(dd_manager.GetConstant(17.0),
                       dd_manager.GetConstant(42.0)).DoubleValue());
  EXPECT_DOUBLE_EQ(17.0,
                   max(dd_manager.GetConstant(17.0),
                       dd_manager.GetConstant(-42.0)).DoubleValue());
}

TEST(DecisionDiagramTest, MaxNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd = max(Ite(dd_manager.GetBddVariable(0),
                         dd_manager.GetConstant(2), dd_manager.GetConstant(5)),
                     Ite(dd_manager.GetBddVariable(1),
                         dd_manager.GetConstant(3), dd_manager.GetConstant(7)));
  EXPECT_DOUBLE_EQ(3.0, DoubleValue(dd_manager, dd, { true, true }));
  EXPECT_DOUBLE_EQ(7.0, DoubleValue(dd_manager, dd, { true, false }));
  EXPECT_DOUBLE_EQ(5.0, DoubleValue(dd_manager, dd, { false, true }));
  EXPECT_DOUBLE_EQ(7.0, DoubleValue(dd_manager, dd, { false, false }));
}

TEST(DecisionDiagramTest, FloorConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_DOUBLE_EQ(1.0, floor(dd_manager.GetConstant(1.0)).DoubleValue());
  EXPECT_DOUBLE_EQ(0.0, floor(dd_manager.GetConstant(0.5)).DoubleValue());
  EXPECT_DOUBLE_EQ(1.0, floor(dd_manager.GetConstant(1.1)).DoubleValue());
  EXPECT_DOUBLE_EQ(-1.0, floor(dd_manager.GetConstant(-0.1)).DoubleValue());
}

TEST(DecisionDiagramTest, FloorNonConstants) {
  const DecisionDiagramManager dd_manager(1);
  const ADD dd =
      floor(Ite(dd_manager.GetBddVariable(0),
                dd_manager.GetConstant(0.1), dd_manager.GetConstant(1.0)));
  EXPECT_DOUBLE_EQ(0.0, DoubleValue(dd_manager, dd, { true }));
  EXPECT_DOUBLE_EQ(1.0, DoubleValue(dd_manager, dd, { false }));
}

TEST(DecisionDiagramTest, CeilConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_DOUBLE_EQ(1.0, ceil(dd_manager.GetConstant(1.0)).DoubleValue());
  EXPECT_DOUBLE_EQ(1.0, ceil(dd_manager.GetConstant(0.5)).DoubleValue());
  EXPECT_DOUBLE_EQ(2.0, ceil(dd_manager.GetConstant(1.1)).DoubleValue());
  EXPECT_DOUBLE_EQ(0.0, ceil(dd_manager.GetConstant(-0.1)).DoubleValue());
}

TEST(DecisionDiagramTest, CeilNonConstants) {
  const DecisionDiagramManager dd_manager(1);
  const ADD dd =
      ceil(Ite(dd_manager.GetBddVariable(0),
               dd_manager.GetConstant(0.1), dd_manager.GetConstant(1.0)));
  EXPECT_DOUBLE_EQ(1.0, DoubleValue(dd_manager, dd, { true }));
  EXPECT_DOUBLE_EQ(1.0, DoubleValue(dd_manager, dd, { false }));
}

TEST(DecisionDiagramTest, PowConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_DOUBLE_EQ(pow(17.0, 42.0),
                   pow(dd_manager.GetConstant(17.0),
                       dd_manager.GetConstant(42.0)).DoubleValue());
  EXPECT_DOUBLE_EQ(pow(17.0, -42.0),
                   pow(dd_manager.GetConstant(17.0),
                       dd_manager.GetConstant(-42.0)).DoubleValue());
}

TEST(DecisionDiagramTest, PowNonConstants) {
  const DecisionDiagramManager dd_manager(2);
  const ADD dd =
      pow(Ite(dd_manager.GetBddVariable(0),
              dd_manager.GetConstant(2), dd_manager.GetConstant(5)),
          Ite(dd_manager.GetBddVariable(1),
              dd_manager.GetConstant(3), dd_manager.GetConstant(7)));
  const ADD one = dd_manager.GetConstant(1.0);
  const ADD zero = dd_manager.GetConstant(0.0);
  EXPECT_DOUBLE_EQ(pow(2.0, 3.0),
                   DoubleValue(dd_manager, dd, { true, true }));
  EXPECT_DOUBLE_EQ(pow(2.0, 7.0),
                   DoubleValue(dd_manager, dd, { true, false }));
  EXPECT_DOUBLE_EQ(pow(5.0, 3.0),
                   DoubleValue(dd_manager, dd, { false, true }));
  EXPECT_DOUBLE_EQ(pow(5.0, 7.0),
                   DoubleValue(dd_manager, dd, { false, false }));
  EXPECT_DOUBLE_EQ(1.0, pow(one, dd).DoubleValue());
  EXPECT_DOUBLE_EQ(1.0, pow(dd, zero).DoubleValue());
}

TEST(DecisionDiagramTest, LogConstants) {
  const DecisionDiagramManager dd_manager(0);
  EXPECT_DOUBLE_EQ(log(17.0), log(dd_manager.GetConstant(17.0)).DoubleValue());
  EXPECT_DOUBLE_EQ(log(42.0), log(dd_manager.GetConstant(42.0)).DoubleValue());
  EXPECT_DOUBLE_EQ(log(0.5), log(dd_manager.GetConstant(0.5)).DoubleValue());
}

TEST(DecisionDiagramTest, LogNonConstants) {
  const DecisionDiagramManager dd_manager(1);
  const ADD dd =
      log(Ite(dd_manager.GetBddVariable(0),
              dd_manager.GetConstant(17.0), dd_manager.GetConstant(0.5)));
  EXPECT_DOUBLE_EQ(log(17.0), DoubleValue(dd_manager, dd, { true }));
  EXPECT_DOUBLE_EQ(log(0.5), DoubleValue(dd_manager, dd, { false }));
}

}  // namespace
