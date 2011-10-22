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

#include "ddutil.h"

#include <cmath>
#include <cstdio>  // cudd.h needs declaration for FILE.
#include <limits>
#include <memory>

#include "cudd.h"
#include "glog/logging.h"

namespace {

// Add operator for unary minus.
DdNode* AddNegate(DdManager* dd_manager, DdNode* dd_node) {
  if (Cudd_IsConstant(dd_node)) {
    return Cudd_addConst(dd_manager, -Cudd_V(dd_node));
  }
  return nullptr;
}

// ADD operator for modulo.
DdNode* AddMod(DdManager* dd_manager, DdNode** dd_node1, DdNode** dd_node2) {
  if (Cudd_IsConstant(*dd_node1) && Cudd_IsConstant(*dd_node2)) {
    return Cudd_addConst(dd_manager,
                         static_cast<int>(Cudd_V(*dd_node1)) %
                         static_cast<int>(Cudd_V(*dd_node2)));
  }
  return nullptr;
}

// ADD operator for floor().
DdNode* AddFloor(DdManager* dd_manager, DdNode* dd_node) {
  if (Cudd_IsConstant(dd_node)) {
    return Cudd_addConst(dd_manager, floor(Cudd_V(dd_node)));
  }
  return nullptr;
}

// ADD operator for ceil().
DdNode* AddCeil(DdManager* dd_manager, DdNode* dd_node) {
  if (Cudd_IsConstant(dd_node)) {
    return Cudd_addConst(dd_manager, ceil(Cudd_V(dd_node)));
  }
  return nullptr;
}

// ADD operator for pow().
DdNode* AddPow(DdManager* dd_manager, DdNode** dd_node1, DdNode** dd_node2) {
  // Special cases: pow(1, y) == 1; pow(x, 0) == 1.
  if (*dd_node1 == Cudd_ReadOne(dd_manager) ||
      *dd_node2 == Cudd_ReadZero(dd_manager)) {
    return Cudd_ReadOne(dd_manager);
  }
  if (Cudd_IsConstant(*dd_node1) && Cudd_IsConstant(*dd_node2)) {
    return Cudd_addConst(dd_manager, pow(Cudd_V(*dd_node1), Cudd_V(*dd_node2)));
  }
  return nullptr;
}

}  // namespace

DecisionDiagram::DecisionDiagram(DdManager* dd_manager, DdNode* dd_node)
    : dd_manager_(dd_manager), dd_node_(dd_node) {
  CHECK(dd_manager_);
  CHECK(dd_node_);
  Ref();
}

DecisionDiagram::DecisionDiagram(DecisionDiagram&& dd)
    : dd_manager_(dd.dd_manager_), dd_node_(dd.dd_node_) {
  // Avoid extra ref/deref by resetting dd.dd_node_, effectively transferring
  // the reference from dd to this decision diagram.
  dd.dd_node_ = nullptr;
}

DecisionDiagram::DecisionDiagram(const DecisionDiagram& dd)
    : dd_manager_(dd.dd_manager_), dd_node_(dd.dd_node_) {
  Ref();
}

DecisionDiagram& DecisionDiagram::operator=(const DecisionDiagram& dd) {
  Deref();
  dd_manager_ = dd.dd_manager_;
  dd_node_ = dd.dd_node_;
  Ref();
  return *this;
}

DecisionDiagram::~DecisionDiagram() {
  Deref();
}

bool DecisionDiagram::IsConstant() const {
  return Cudd_IsConstant(dd_node_);
}

void DecisionDiagram::PrintDebug(int verbosity) const {
  Cudd_PrintDebug(dd_manager_, dd_node_, Cudd_ReadSize(dd_manager_), verbosity);
}

void DecisionDiagram::Ref() {
  if (dd_node_) {
    Cudd_Ref(dd_node_);
  }
}

void DecisionDiagram::Deref() {
  if (dd_node_) {
    Cudd_RecursiveDeref(dd_manager_, dd_node_);
    dd_node_ = nullptr;
  }
}

BDD::BDD(DdManager* dd_manager, DdNode* dd_node)
    : DecisionDiagram(dd_manager, dd_node) {
  CHECK(dd_manager);
  CHECK(dd_node);
}

BDD::BDD(BDD&& dd)
    : DecisionDiagram(dd) {
}

BDD::BDD(const BDD& dd)
    : DecisionDiagram(dd) {
}

BDD& BDD::operator=(const BDD& dd) {
  DecisionDiagram::operator=(dd);
  return *this;
}

BDD::BDD(const ADD& dd)
    : DecisionDiagram(
        dd.dd_manager(),
        Cudd_Not(Cudd_addBddInterval(dd.dd_manager(), dd.dd_node(),
                                     0.0, 0.0))) {
}

bool BDD::Value() const {
  CHECK(IsConstant());
  return !Cudd_IsComplement(dd_node());
}

BDD BDD::operator!() const {
  return BDD(dd_manager(), Cudd_Not(dd_node()));
}

BDD BDD::operator&&(const BDD& dd) const {
  return Apply(Cudd_bddAnd, *this, dd);
}

BDD BDD::operator||(const BDD& dd) const {
  return Apply(Cudd_bddOr, *this, dd);
}

BDD BDD::operator==(const BDD& dd) const {
  return Apply(Cudd_bddXnor, *this, dd);
}

BDD BDD::operator!=(const BDD& dd) const {
  return !(*this == dd);
}

BDD BDD::operator<(const BDD& dd) const {
  return !*this && dd;
}

BDD BDD::operator<=(const BDD& dd) const {
  return !(*this > dd);
}

BDD BDD::operator>=(const BDD& dd) const {
  return !(*this < dd);
}

BDD BDD::operator>(const BDD& dd) const {
  return *this && !dd;
}

BDD BDD::ExistAbstract(const BDD& cube) const {
  CHECK_EQ(dd_manager(), cube.dd_manager());
  return BDD(dd_manager(),
             Cudd_bddExistAbstract(dd_manager(), dd_node(), cube.dd_node()));
}

BDD BDD::Apply(Op op, const BDD& dd1, const BDD& dd2) {
  CHECK_EQ(dd1.dd_manager(), dd2.dd_manager());
  return BDD(dd1.dd_manager(),
             op(dd1.dd_manager(), dd1.dd_node(), dd2.dd_node()));
}

ADD::ADD(DdManager* dd_manager, DdNode* dd_node)
    : DecisionDiagram(dd_manager, dd_node) {
  CHECK(dd_manager);
  CHECK(dd_node);
}

ADD::ADD(ADD&& dd)
    : DecisionDiagram(dd) {
}

ADD::ADD(const ADD& dd)
    : DecisionDiagram(dd) {
}

ADD& ADD::operator=(const ADD& dd) {
  DecisionDiagram::operator=(dd);
  return *this;
}

ADD::ADD(const BDD& dd)
    : DecisionDiagram(dd.dd_manager(),
                      Cudd_BddToAdd(dd.dd_manager(), dd.dd_node())) {
}

double ADD::Value() const {
  CHECK(IsConstant());
  return Cudd_V(dd_node());
}

ADD ADD::operator-() const {
  return MonadicApply(AddNegate, *this);
}

ADD ADD::operator*(const ADD& dd) const {
  return Apply(Cudd_addTimes, *this, dd);
}

ADD ADD::operator/(const ADD& dd) const {
  return Apply(Cudd_addDivide, *this, dd);
}

ADD ADD::operator+(const ADD& dd) const {
  return Apply(Cudd_addPlus, *this, dd);
}

ADD ADD::operator-(const ADD& dd) const {
  return Apply(Cudd_addMinus, *this, dd);
}

ADD ADD::operator%(const ADD& dd) const {
  return Apply(AddMod, *this, dd);
}

BDD ADD::operator==(const ADD& dd) const {
  return *this - dd == 0.0;
}

BDD ADD::operator!=(const ADD& dd) const {
  return !(*this == dd);
}

BDD ADD::operator<(const ADD& dd) const {
  return dd - *this > 0.0;
}

BDD ADD::operator<=(const ADD& dd) const {
  return !(*this > dd);
}

BDD ADD::operator>=(const ADD& dd) const {
  return !(*this < dd);
}

BDD ADD::operator>(const ADD& dd) const {
  return *this - dd > 0.0;
}

BDD ADD::operator==(double value) const {
  return BDD(dd_manager(),
             Cudd_addBddInterval(dd_manager(), dd_node(), value, value));
}

BDD ADD::operator>(double value) const {
  return BDD(dd_manager(),
             Cudd_addBddStrictThreshold(dd_manager(), dd_node(), value));
}

ADD ADD::ExistAbstract(const ADD& cube) const {
  CHECK_EQ(dd_manager(), cube.dd_manager());
  return ADD(dd_manager(),
             Cudd_addExistAbstract(dd_manager(), dd_node(), cube.dd_node()));
}

ADD ADD::Apply(Op op, const ADD& dd1, const ADD& dd2) {
  CHECK_EQ(dd1.dd_manager(), dd2.dd_manager());
  return ADD(dd1.dd_manager(),
             Cudd_addApply(dd1.dd_manager(), op, dd1.dd_node(), dd2.dd_node()));
}

ADD ADD::MonadicApply(MonadicOp op, const ADD& dd) {
  return ADD(dd.dd_manager(),
             Cudd_addMonadicApply(dd.dd_manager(), op, dd.dd_node()));
}

BDD Ite(const BDD& dd1, const BDD& dd2, const BDD& dd3) {
  CHECK_EQ(dd1.dd_manager(), dd2.dd_manager());
  CHECK_EQ(dd1.dd_manager(), dd3.dd_manager());
  return BDD(dd1.dd_manager(),
             Cudd_bddIte(dd1.dd_manager(),
                         dd1.dd_node(), dd2.dd_node(), dd3.dd_node()));
}

ADD Ite(const BDD& dd1, const ADD& dd2, const ADD& dd3) {
  CHECK_EQ(dd1.dd_manager(), dd2.dd_manager());
  CHECK_EQ(dd1.dd_manager(), dd3.dd_manager());
  return ADD(dd1.dd_manager(),
             Cudd_addIte(dd1.dd_manager(),
                         Cudd_BddToAdd(dd1.dd_manager(), dd1.dd_node()),
                         dd2.dd_node(), dd3.dd_node()));
}

ADD min(const ADD& dd1, const ADD& dd2) {
  return ADD::Apply(Cudd_addMinimum, dd1, dd2);
}

ADD max(const ADD& dd1, const ADD& dd2) {
  return ADD::Apply(Cudd_addMaximum, dd1, dd2);
}

ADD floor(const ADD& dd) {
  return ADD::MonadicApply(AddFloor, dd);
}

ADD ceil(const ADD& dd) {
  return ADD::MonadicApply(AddCeil, dd);
}

ADD pow(const ADD& dd1, const ADD& dd2) {
  return ADD::Apply(AddPow, dd1, dd2);
}

ADD log(const ADD& dd) {
  return ADD::MonadicApply(Cudd_addLog, dd);
}

DecisionDiagramManager::DecisionDiagramManager(int num_variables)
    : dd_manager_(Cudd_Init(num_variables, 0 /* num_zdd_variables */,
                            CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)) {
  SetEpsilon(std::numeric_limits<double>::min());
}

DecisionDiagramManager::~DecisionDiagramManager() {
  Cudd_CheckZeroRef(dd_manager_);
  Cudd_Quit(dd_manager_);
}

int DecisionDiagramManager::num_variables() const {
  return Cudd_ReadSize(dd_manager_);
}

void DecisionDiagramManager::SetEpsilon(double epsilon) {
  Cudd_SetEpsilon(dd_manager_, epsilon);
}

double DecisionDiagramManager::GetEpsilon() const {
  return Cudd_ReadEpsilon(dd_manager_);
}

BDD DecisionDiagramManager::GetConstant(bool value) const {
  return BDD(
      dd_manager_,
      value ? Cudd_ReadOne(dd_manager_) : Cudd_ReadLogicZero(dd_manager_));
}

ADD DecisionDiagramManager::GetConstant(int value) const {
  return ADD(dd_manager_, Cudd_addConst(dd_manager_, value));
}

ADD DecisionDiagramManager::GetConstant(double value) const {
  return ADD(dd_manager_, Cudd_addConst(dd_manager_, value));
}

BDD DecisionDiagramManager::GetBddVariable(int i) const {
  CHECK_LT(i, num_variables());
  return BDD(dd_manager_, Cudd_bddIthVar(dd_manager_, i));
}

ADD DecisionDiagramManager::GetAddVariable(int i) const {
  CHECK_LT(i, num_variables());
  return ADD(dd_manager_, Cudd_addIthVar(dd_manager_, i));
}

BDD DecisionDiagramManager::GetBddCube() const {
  std::unique_ptr<DdNode*[]> row_vars(new DdNode*[num_variables()]);
  for (int i = 0; i < num_variables(); ++i) {
    row_vars[i] = Cudd_bddIthVar(dd_manager_, i);
  }
  DdNode* cube = Cudd_bddComputeCube(
      dd_manager_, row_vars.get(), nullptr /* phase */, num_variables());
  return BDD(dd_manager_, cube);
}

ADD DecisionDiagramManager::GetAddCube() const {
  std::unique_ptr<DdNode*[]> row_vars(new DdNode*[num_variables()]);
  for (int i = 0; i < num_variables(); ++i) {
    row_vars[i] = Cudd_addIthVar(dd_manager_, i);
  }
  DdNode* cube = Cudd_addComputeCube(
      dd_manager_, row_vars.get(), nullptr /* phase */, num_variables());
  return ADD(dd_manager_, cube);
}

BDD DecisionDiagramManager::VariableIdentity(int low_bit, int high_bit) const {
  CHECK_LE(0, low_bit);
  CHECK_LE(low_bit, high_bit);
  CHECK_LT(2*high_bit + 1, num_variables());

  const int num_bits = high_bit - low_bit + 1;
  std::unique_ptr<DdNode*[]> row_vars(new DdNode*[num_bits]);
  std::unique_ptr<DdNode*[]> column_vars(new DdNode*[num_bits]);
  for (int i = low_bit; i <= high_bit; ++i) {
    row_vars[i - low_bit] = Cudd_bddIthVar(dd_manager_, 2 * i);
    column_vars[i - low_bit] = Cudd_bddIthVar(dd_manager_, 2 * i + 1);
  }
  return BDD(
      dd_manager_,
      Cudd_Xeqy(dd_manager_, num_bits, row_vars.get(), column_vars.get()));
}

bool GetValue(const DecisionDiagramManager& dd_manager,
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
  return (bits_dd && dd).ExistAbstract(dd_manager.GetBddCube()).Value();
}

double GetValue(const DecisionDiagramManager& dd_manager,
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
  return (bits_dd * dd).ExistAbstract(dd_manager.GetAddCube()).Value();
}
