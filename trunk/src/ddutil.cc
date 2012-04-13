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

#include "cudd.h"
#include "glog/logging.h"

DecisionDiagram::DecisionDiagram(DdManager* manager, DdNode* node)
    : manager_(manager), node_(node) {
  Ref();
}

DecisionDiagram::DecisionDiagram(const DecisionDiagram& dd)
    : manager_(dd.manager_), node_(dd.node_) {
  Ref();
}

DecisionDiagram& DecisionDiagram::operator=(const DecisionDiagram& dd) {
  if (node_ != dd.node_) {
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
  Cudd_Ref(node_);
}

void DecisionDiagram::Deref() {
  Cudd_RecursiveDeref(manager_, node_);
  node_ = NULL;
}

bool DecisionDiagram::IsConstant() const {
  return Cudd_IsConstant(node_);
}

ADD::ADD(DdManager* manager, DdNode* node)
    : DecisionDiagram(manager, node) {
}

double ADD::Value() const {
  CHECK(IsConstant());
  return Cudd_V(node());
}

DecisionDiagramManager::DecisionDiagramManager(int num_variables)
    : manager_(Cudd_Init(num_variables, 0 /* num_zdd_variables */,
                         CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)) {
  CHECK(manager_);
  SetEpsilon(std::numeric_limits<double>::min());
}

DecisionDiagramManager::~DecisionDiagramManager() {
  CHECK_EQ(0, Cudd_CheckZeroRef(manager_));
  Cudd_Quit(manager_);
}
int DecisionDiagramManager::GetNumVariables() const {
  return Cudd_ReadSize(manager_);
}

void DecisionDiagramManager::SetEpsilon(double epsilon) {
  Cudd_SetEpsilon(manager_, epsilon);
}

double DecisionDiagramManager::GetEpsilon() const {
  return Cudd_ReadEpsilon(manager_);
}

ADD DecisionDiagramManager::GetConstant(double value) const {
  return ADD(manager_, CHECK_NOTNULL(Cudd_addConst(manager_, value)));
}
