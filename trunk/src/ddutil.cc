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
#include <limits>
#include <vector>

#include "cudd.h"
#include "glog/logging.h"

namespace {

size_t ManagerSize(DdManager* manager) {
  return Cudd_ReadSize(manager);
}

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
    dd = state[index] ? ThenChild(dd) : ElseChild(dd);
  }
  return NodeValue<ValueType>(dd);
}

template <typename ValueType>
ValueType ValueInStateImpl(DdNode* dd, const std::vector<int>& values,
                           const std::vector<StateVariableInfo>& variables) {
  int current_variable = 0;
  int total_bit_count = 2 * variables[current_variable].bit_count();
  while (!Cudd_IsConstant(dd)) {
    const int index = Cudd_Regular(dd)->index;
    while (index >= total_bit_count) {
      ++current_variable;
      total_bit_count += 2 * variables[current_variable].bit_count();
    }
    const bool bit =
        (values[current_variable] - variables[current_variable].min_value()) &
        (1 << ((total_bit_count - index - 1) / 2));
    dd = bit ? ThenChild(dd) : ElseChild(dd);
  }
  return NodeValue<ValueType>(dd);
}

// Add operator for unary minus.
DdNode* AddNegate(DdManager* manager, DdNode* node) {
  if (Cudd_IsConstant(node)) {
    return Cudd_addConst(manager, -Cudd_V(node));
  }
  return nullptr;
}

// ADD operator for modulo.
DdNode* AddMod(DdManager* manager, DdNode** node1, DdNode** node2) {
  if (Cudd_IsConstant(*node1) && Cudd_IsConstant(*node2)) {
    return Cudd_addConst(manager,
                         static_cast<int>(Cudd_V(*node1)) %
                         static_cast<int>(Cudd_V(*node2)));
  }
  return nullptr;
}

// ADD operator for floor().
DdNode* AddFloor(DdManager* manager, DdNode* node) {
  if (Cudd_IsConstant(node)) {
    return Cudd_addConst(manager, floor(Cudd_V(node)));
  }
  return nullptr;
}

// ADD operator for ceil().
DdNode* AddCeil(DdManager* manager, DdNode* node) {
  if (Cudd_IsConstant(node)) {
    return Cudd_addConst(manager, ceil(Cudd_V(node)));
  }
  return nullptr;
}

// ADD operator for pow().
DdNode* AddPow(DdManager* manager, DdNode** node1, DdNode** node2) {
  // Special cases: pow(1, y) == 1; pow(x, 0) == 1.
  if (*node1 == Cudd_ReadOne(manager) || *node2 == Cudd_ReadZero(manager)) {
    return Cudd_ReadOne(manager);
  }
  if (Cudd_IsConstant(*node1) && Cudd_IsConstant(*node2)) {
    return Cudd_addConst(manager, pow(Cudd_V(*node1), Cudd_V(*node2)));
  }
  return nullptr;
}

}  // namespace

StateVariableInfo::StateVariableInfo(const std::string& name, int min_value,
                                     int bit_count)
    : name_(name), min_value_(min_value), bit_count_(bit_count) {}

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
    node_ = nullptr;
  }
}

bool DecisionDiagram::IsConstant() const {
  return Cudd_IsConstant(node_);
}

double DecisionDiagram::MintermCount(int num_variables) const {
  return Cudd_CountMinterm(manager_, node_, num_variables);
}

BDD::BDD(DdManager* manager, DdNode* node)
    : DecisionDiagram(manager, node) {
}

BDD::BDD(const ADD& dd)
    : DecisionDiagram(dd.manager(), (!dd.Interval(0, 0)).node()) {
}

bool BDD::Value() const {
  CHECK(IsConstant());
  return NodeValue<bool>(node());
}

bool BDD::ValueInState(const std::vector<bool>& state) const {
  CHECK_EQ(ManagerSize(manager()), state.size());
  return ValueInStateImpl<bool>(node(), state);
}

bool BDD::ValueInState(const std::vector<int>& values,
                       const std::vector<StateVariableInfo>& variables) const {
  return ValueInStateImpl<bool>(node(), values, variables);
}

BDD BDD::Permutation(const std::vector<int>& permutation) const {
  CHECK_EQ(ManagerSize(manager()), permutation.size());
  return BDD(manager(), Cudd_bddPermute(manager(), node(), permutation.data()));
}

BDD BDD::ExistAbstract(const BDD& cube) const {
  return BDD(manager(), Cudd_bddExistAbstract(manager(), node(), cube.node()));
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

BDD Ite(const BDD& dd1, const BDD& dd2, const BDD& dd3) {
  DdManager* const manager = dd1.manager();
  return BDD(manager, Cudd_bddIte(manager, dd1.node(), dd2.node(), dd3.node()));
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
  CHECK_EQ(ManagerSize(manager()), state.size());
  return ValueInStateImpl<double>(node(), state);
}

double ADD::ValueInState(
    const std::vector<int>& values,
    const std::vector<StateVariableInfo>& variables) const {
  return ValueInStateImpl<double>(node(), values, variables);
}

BDD ADD::Interval(double low, double high) const {
  return BDD(manager(), Cudd_addBddInterval(manager(), node(), low, high));
}

BDD ADD::StrictThreshold(double threshold) const {
  return BDD(manager(),
             Cudd_addBddStrictThreshold(manager(), node(), threshold));
}

ADD ADD::operator-() const {
  return MonadicApply(AddNegate, *this);
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

ADD ADD::operator%(const ADD& dd) const {
  return Apply(AddMod, *this, dd);
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

ADD ADD::MonadicApply(MonadicOp op, const ADD& dd) {
  return ADD(dd.manager(),
             Cudd_addMonadicApply(dd.manager(), op, dd.node()));
}

ADD Ite(const BDD& dd1, const ADD& dd2, const ADD& dd3) {
  DdManager* const manager = dd1.manager();
  const ADD cond(dd1);
  return ADD(manager,
             Cudd_addIte(manager, cond.node(), dd2.node(), dd3.node()));
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

VariableArray<BDD> DecisionDiagramManager::GetBddVariableArray(
    int start, int incr, int end) const {
  std::vector<BDD> variables;
  for (int i = start; i < end; i += incr) {
    variables.push_back(GetBddVariable(i));
  }
  return VariableArray<BDD>(variables);
}

BDD DecisionDiagramManager::GetCube(const VariableArray<BDD>& variables) const {
  return BDD(manager_, Cudd_bddComputeCube(manager_, variables.data(), nullptr,
                                           variables.size()));
}

int Log2(int n) {
  CHECK_GT(n, 0);
  int m = 0;
  while (n >>= 1) {
    ++m;
  }
  return m;
}
