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

#include <cstdio>  // cudd.h needs declaration for FILE.
#include <limits>
#include <vector>

#include "cudd.h"
#include "glog/logging.h"

namespace {

template <typename ValueType>
ValueType NodeValue(DdNode* dd);

template <>
bool NodeValue<bool>(DdNode* dd) {
  return !Cudd_IsComplement(dd);
}

template <>
double NodeValue<double>(DdNode* dd) {
  return Cudd_V(dd);
}

DdNode* ThenChild(DdNode* dd) {
  return Cudd_IsComplement(dd) ? Cudd_Not(Cudd_T(dd)) : Cudd_T(dd);
}

DdNode* ElseChild(DdNode* dd) {
  return Cudd_IsComplement(dd) ? Cudd_Not(Cudd_E(dd)) : Cudd_E(dd);
}

template <typename ValueType>
ValueType ValueInStateImpl(DdNode* dd, const std::vector<bool>& state) {
  while (!Cudd_IsConstant(dd)) {
    const int index = Cudd_Regular(dd)->index;
    dd = (state[index]) ? ThenChild(dd) : ElseChild(dd);
  }
  return NodeValue<ValueType>(dd);
}

}  // namespace

DecisionDiagram::DecisionDiagram(DdManager* manager, DdNode* node)
    : manager_(manager), node_(CHECK_NOTNULL(node)) {
  Ref();
}

DecisionDiagram::DecisionDiagram(const DecisionDiagram& dd)
    : manager_(dd.manager_), node_(dd.node_) {
  Ref();
}

DecisionDiagram& DecisionDiagram::operator=(const DecisionDiagram& dd) {
  if (this != &dd) {
    Deref();
    manager_ = dd.manager_;
    node_ = dd.node_;
    Ref();
  }
  return *this;
}

DecisionDiagram::~DecisionDiagram() {
  Deref();
}

void DecisionDiagram::Ref() {
  if (node_) {
    Cudd_Ref(node_);
  }
}

void DecisionDiagram::Deref() {
  if (node_) {
    Cudd_RecursiveDeref(manager_, node_);
    node_ = NULL;
  }
}

bool DecisionDiagram::IsConstant() const {
  return Cudd_IsConstant(node_);
}

DdNode* DecisionDiagram::release() {
  DdNode* const node = node_;
  node_ = NULL;
  return node;
}

BDD::BDD(DdManager* manager, DdNode* node)
    : DecisionDiagram(manager, node) {
}

bool BDD::Value() const {
  CHECK(IsConstant());
  return NodeValue<bool>(node());
}

bool BDD::ValueInState(const std::vector<bool>& state) const {
  CHECK_EQ(Cudd_ReadSize(manager()), state.size());
  return ValueInStateImpl<bool>(node(), state);
}

BDD BDD::operator!() const {
  return BDD(manager(), Cudd_Not(node()));
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

BDD BDD::Apply(Op op, const BDD& dd1, const BDD& dd2) {
  DdManager* const manager = dd1.manager();
  return BDD(manager, op(manager, dd1.node(), dd2.node()));
}

ADD::ADD(DdManager* manager, DdNode* node)
    : DecisionDiagram(manager, node) {
}

ADD::ADD(const BDD& dd)
    : DecisionDiagram(dd.manager(), Cudd_BddToAdd(dd.manager(), dd.node())) {
}

double ADD::Value() const {
  CHECK(IsConstant());
  return NodeValue<double>(node());
}

double ADD::ValueInState(const std::vector<bool>& state) const {
  CHECK_EQ(Cudd_ReadSize(manager()), state.size());
  return ValueInStateImpl<double>(node(), state);
}

BDD ADD::Interval(double low, double high) const {
  return BDD(manager(), Cudd_addBddInterval(manager(), node(), low, high));
}

BDD ADD::StrictThreshold(double threshold) const {
  return BDD(manager(),
             Cudd_addBddStrictThreshold(manager(), node(), threshold));
}

ADD ADD::operator+(const ADD& dd) const {
  return Apply(Cudd_addPlus, *this, dd);
}

ADD ADD::operator-(const ADD& dd) const {
  return Apply(Cudd_addMinus, *this, dd);
}

ADD ADD::operator*(const ADD& dd) const {
  return Apply(Cudd_addTimes, *this, dd);
}

ADD ADD::operator/(const ADD& dd) const {
  return Apply(Cudd_addDivide, *this, dd);
}

BDD ADD::operator==(const ADD& dd) const {
  return (*this - dd).Interval(0.0, 0.0);
}

BDD ADD::operator!=(const ADD& dd) const {
  return !(*this == dd);
}

BDD ADD::operator<(const ADD& dd) const {
  return (dd - *this).StrictThreshold(0);
}

BDD ADD::operator<=(const ADD& dd) const {
  return !(*this > dd);
}

BDD ADD::operator>=(const ADD& dd) const {
  return !(*this < dd);
}

BDD ADD::operator>(const ADD& dd) const {
  return (*this - dd).StrictThreshold(0);
}

ADD ADD::Apply(Op op, const ADD& dd1, const ADD& dd2) {
  DdManager* const manager = dd1.manager();
  return ADD(manager, Cudd_addApply(manager, op, dd1.node(), dd2.node()));
}

ADD Ite(const BDD& dd1, const ADD& dd2, const ADD& dd3) {
  DdManager* const manager = dd1.manager();
  const BDD cond(manager, Cudd_BddToAdd(manager, dd1.node()));
  return ADD(manager,
             Cudd_addIte(manager, cond.node(), dd2.node(), dd3.node()));
}

DecisionDiagramManager::DecisionDiagramManager(int num_variables)
    : manager_(Cudd_Init(num_variables, 0 /* num_zdd_variables */,
                         CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)) {
  CHECK(manager_);
  SetEpsilon(std::numeric_limits<double>::min());
}

DecisionDiagramManager::~DecisionDiagramManager() {
  CHECK_EQ(0, Cudd_CheckZeroRef(manager_)) << "unreleased DDs";
  Cudd_Quit(manager_);
}
int DecisionDiagramManager::GetNumVariables() const {
  return Cudd_ReadSize(manager_);
}

void DecisionDiagramManager::SetEpsilon(double epsilon) const {
  Cudd_SetEpsilon(manager_, epsilon);
}

double DecisionDiagramManager::GetEpsilon() const {
  return Cudd_ReadEpsilon(manager_);
}

BDD DecisionDiagramManager::GetConstant(bool value) const {
  return BDD(manager_,
             value ? Cudd_ReadOne(manager_) : Cudd_ReadLogicZero(manager_));
}

ADD DecisionDiagramManager::GetConstant(int value) const {
  return GetConstant(static_cast<double>(value));
}

ADD DecisionDiagramManager::GetConstant(double value) const {
  return ADD(manager_, Cudd_addConst(manager_, value));
}

BDD DecisionDiagramManager::GetBddVariable(int i) const {
  return BDD(manager_, Cudd_bddIthVar(manager_, i));
}

ADD DecisionDiagramManager::GetAddVariable(int i) const {
  return ADD(manager_, Cudd_addIthVar(manager_, i));
}

int Log2(int n) {
  CHECK_GT(n, 0);
  int m = 0;
  while (n >>= 1) {
    ++m;
  }
  return m;
}
