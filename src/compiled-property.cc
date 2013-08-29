// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2013 Google Inc
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

#include "compiled-property.h"

#include <utility>

#include "pointer-vector.h"

CompiledProperty::CompiledProperty() = default;

CompiledProperty::~CompiledProperty() = default;

void CompiledProperty::Accept(CompiledPropertyVisitor* visitor) const {
  return DoAccept(visitor);
}

CompiledPathProperty::CompiledPathProperty() = default;

CompiledPathProperty::~CompiledPathProperty() = default;

void CompiledPathProperty::Accept(CompiledPathPropertyVisitor* visitor) const {
  return DoAccept(visitor);
}

CompiledLogicalOperationProperty::CompiledLogicalOperationProperty(
    Operator op, PointerVector<CompiledProperty>&& operands)
    : op_(op), operands_(std::move(operands)) {
}

CompiledLogicalOperationProperty::~CompiledLogicalOperationProperty() = default;

void CompiledLogicalOperationProperty::DoAccept(
    CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledLogicalOperationProperty(*this);
}

CompiledProbabilisticProperty::CompiledProbabilisticProperty(
    Operator op, double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property)
    : op_(op), threshold_(threshold), path_property_(std::move(path_property)) {
}

CompiledProbabilisticProperty::~CompiledProbabilisticProperty() = default;

void CompiledProbabilisticProperty::DoAccept(
    CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledProbabilisticProperty(*this);
}

CompiledExpressionProperty::CompiledExpressionProperty(
    const CompiledExpression& expr)
    : expr_(expr) {
}

CompiledExpressionProperty::~CompiledExpressionProperty() = default;

void CompiledExpressionProperty::DoAccept(
    CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledExpressionProperty(*this);
}

CompiledUntilProperty::CompiledUntilProperty(
    double min_time, double max_time,
    std::unique_ptr<const CompiledProperty>&& pre,
    std::unique_ptr<const CompiledProperty>&& post)
    : min_time_(min_time), max_time_(max_time), pre_(std::move(pre)),
      post_(std::move(post)) {
}

CompiledUntilProperty::~CompiledUntilProperty() = default;

void CompiledUntilProperty::DoAccept(
    CompiledPathPropertyVisitor* visitor) const {
  visitor->VisitCompiledUntilProperty(*this);
}

CompiledPropertyVisitor::CompiledPropertyVisitor() = default;

CompiledPropertyVisitor::CompiledPropertyVisitor(
    const CompiledPropertyVisitor&) = default;

CompiledPropertyVisitor& CompiledPropertyVisitor::operator=(
    const CompiledPropertyVisitor&) = default;

CompiledPropertyVisitor::~CompiledPropertyVisitor() = default;

void CompiledPropertyVisitor::VisitCompiledLogicalOperationProperty(
    const CompiledLogicalOperationProperty& property) {
  DoVisitCompiledLogicalOperationProperty(property);
}

void CompiledPropertyVisitor::VisitCompiledProbabilisticProperty(
    const CompiledProbabilisticProperty& property) {
  DoVisitCompiledProbabilisticProperty(property);
}

void CompiledPropertyVisitor::VisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  DoVisitCompiledExpressionProperty(property);
}

CompiledPathPropertyVisitor::CompiledPathPropertyVisitor() = default;

CompiledPathPropertyVisitor::CompiledPathPropertyVisitor(
    const CompiledPathPropertyVisitor&) = default;

CompiledPathPropertyVisitor& CompiledPathPropertyVisitor::operator=(
    const CompiledPathPropertyVisitor&) = default;

CompiledPathPropertyVisitor::~CompiledPathPropertyVisitor() = default;

void CompiledPathPropertyVisitor::VisitCompiledUntilProperty(
    const CompiledUntilProperty& property) {
  DoVisitCompiledUntilProperty(property);
}
