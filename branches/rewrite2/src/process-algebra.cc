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

#include "process-algebra.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include "glog/logging.h"

ProcessAlgebra::ProcessAlgebra() = default;

ProcessAlgebra::~ProcessAlgebra() = default;

ModuleIdentifier::ModuleIdentifier(const std::string& name)
    : name_(name) {
}

ModuleIdentifier::~ModuleIdentifier() = default;

std::unique_ptr<const ModuleIdentifier> ModuleIdentifier::Create(
    const std::string& name) {
  return std::unique_ptr<const ModuleIdentifier>(new ModuleIdentifier(name));
}

void ModuleIdentifier::Accept(ProcessAlgebraVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitModuleIdentifier(*this);
}

BinaryProcessAlgebraOperation::BinaryProcessAlgebraOperation(
    std::unique_ptr<const ProcessAlgebra>&& operand1,
    std::unique_ptr<const ProcessAlgebra>&& operand2)
    : operand1_(std::move(operand1)), operand2_(std::move(operand2)) {
  CHECK(operand1_);
  CHECK(operand2_);
}

BinaryProcessAlgebraOperation::~BinaryProcessAlgebraOperation() = default;

ParallelComposition::ParallelComposition(
    ParallelCompositionOperator op,
    std::unique_ptr<const ProcessAlgebra>&& operand1,
    std::unique_ptr<const ProcessAlgebra>&& operand2)
    : BinaryProcessAlgebraOperation(std::move(operand1), std::move(operand2)),
      op_(op) {
}

ParallelComposition::~ParallelComposition() = default;

std::unique_ptr<const ParallelComposition> ParallelComposition::Create(
    ParallelCompositionOperator op,
    std::unique_ptr<const ProcessAlgebra>&& operand1,
    std::unique_ptr<const ProcessAlgebra>&& operand2) {
  return std::unique_ptr<const ParallelComposition>(new ParallelComposition(
      op, std::move(operand1), std::move(operand2)));
}

void ParallelComposition::Accept(ProcessAlgebraVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitParallelComposition(*this);
}

RestrictedParallelComposition::RestrictedParallelComposition(
    const std::set<std::string>& actions,
    std::unique_ptr<const ProcessAlgebra>&& operand1,
    std::unique_ptr<const ProcessAlgebra>&& operand2)
    : BinaryProcessAlgebraOperation(std::move(operand1), std::move(operand2)),
      actions_(actions) {
}

RestrictedParallelComposition::~RestrictedParallelComposition() = default;

std::unique_ptr<const RestrictedParallelComposition>
RestrictedParallelComposition::Create(
    const std::set<std::string>& actions,
    std::unique_ptr<const ProcessAlgebra>&& operand1,
    std::unique_ptr<const ProcessAlgebra>&& operand2) {
  return std::unique_ptr<const RestrictedParallelComposition>(
      new RestrictedParallelComposition(
          actions, std::move(operand1), std::move(operand2)));
}

void RestrictedParallelComposition::Accept(
    ProcessAlgebraVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitRestrictedParallelComposition(*this);
}

UnaryProcessAlgebraOperation::UnaryProcessAlgebraOperation(
    std::unique_ptr<const ProcessAlgebra>&& operand)
    : operand_(std::move(operand)) {
  CHECK(operand_);
}

UnaryProcessAlgebraOperation::~UnaryProcessAlgebraOperation() = default;

ActionHiding::ActionHiding(const std::set<std::string>& actions,
                           std::unique_ptr<const ProcessAlgebra>&& operand)
    : UnaryProcessAlgebraOperation(std::move(operand)), actions_(actions) {
}

ActionHiding::~ActionHiding() = default;

std::unique_ptr<const ActionHiding> ActionHiding::Create(
    const std::set<std::string>& actions,
    std::unique_ptr<const ProcessAlgebra>&& operand) {
  return std::unique_ptr<const ActionHiding>(new ActionHiding(
      actions, std::move(operand)));
}

void ActionHiding::Accept(ProcessAlgebraVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitActionHiding(*this);
}

ActionRenaming::ActionRenaming(
    const std::map<std::string, std::string>& substitutions,
    std::unique_ptr<const ProcessAlgebra>&& operand)
    : UnaryProcessAlgebraOperation(std::move(operand)),
      substitutions_(substitutions) {
}

ActionRenaming::~ActionRenaming() = default;

std::unique_ptr<const ActionRenaming> ActionRenaming::Create(
    const std::map<std::string, std::string>& substitutions,
    std::unique_ptr<const ProcessAlgebra>&& operand) {
  return std::unique_ptr<const ActionRenaming>(new ActionRenaming(
      substitutions, std::move(operand)));
}

void ActionRenaming::Accept(ProcessAlgebraVisitor* visitor) const {
  CHECK(visitor);
  visitor->VisitActionRenaming(*this);
}

ProcessAlgebraVisitor::ProcessAlgebraVisitor() = default;

ProcessAlgebraVisitor::~ProcessAlgebraVisitor() = default;
