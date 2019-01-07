// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2015 Google Inc
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

#include "glog/logging.h"

ProcessAlgebra::~ProcessAlgebra() = default;

void ProcessAlgebra::Accept(ProcessAlgebraVisitor* visitor) const {
  DoAccept(visitor);
}

namespace {

// A process algebra visitor that prints process algebra expressions to an
// output stream.
class Printer final : public ProcessAlgebraVisitor {
 public:
  explicit Printer(std::ostream* os);

 private:
  void DoVisitModuleIdentifier(
      const ModuleIdentifier& process_algebra) override;
  void DoVisitParallelComposition(
      const ParallelComposition& process_algebra) override;
  void DoVisitRestrictedParallelComposition(
      const RestrictedParallelComposition& process_algebra) override;
  void DoVisitActionHiding(const ActionHiding& process_algebra) override;
  void DoVisitActionRenaming(const ActionRenaming& process_algebra) override;

  std::ostream* os_;
  int parent_precedence_;
  int parent_binary_op_;
};

Printer::Printer(std::ostream* os) : os_(os), parent_precedence_(0) {}

void Printer::DoVisitModuleIdentifier(const ModuleIdentifier& process_algebra) {
  *os_ << process_algebra.name();
}

void Printer::DoVisitParallelComposition(
    const ParallelComposition& process_algebra) {
  int precedence = 1;
  int binary_op =
      (process_algebra.op() == ParallelCompositionOperator::ALPHABETIZED) ? 0
                                                                          : 1;
  if (parent_precedence_ > precedence ||
      (parent_precedence_ == precedence && parent_binary_op_ != binary_op)) {
    *os_ << '(';
  }
  std::swap(parent_precedence_, precedence);
  std::swap(parent_binary_op_, binary_op);
  process_algebra.operand1().Accept(this);
  std::swap(parent_binary_op_, binary_op);
  std::swap(parent_precedence_, precedence);
  *os_ << ' ' << process_algebra.op() << ' ';
  std::swap(parent_precedence_, precedence);
  std::swap(parent_binary_op_, binary_op);
  process_algebra.operand2().Accept(this);
  std::swap(parent_binary_op_, binary_op);
  std::swap(parent_precedence_, precedence);
  if (parent_precedence_ > precedence ||
      (parent_precedence_ == precedence && parent_binary_op_ != binary_op)) {
    *os_ << ')';
  }
}

void Printer::DoVisitRestrictedParallelComposition(
    const RestrictedParallelComposition& process_algebra) {
  int precedence = 1;
  int binary_op = 2;
  if (parent_precedence_ > precedence ||
      (parent_precedence_ == precedence && parent_binary_op_ != binary_op)) {
    *os_ << '(';
  }
  std::swap(parent_precedence_, precedence);
  std::swap(parent_binary_op_, binary_op);
  process_algebra.operand1().Accept(this);
  std::swap(parent_binary_op_, binary_op);
  std::swap(parent_precedence_, precedence);
  *os_ << " |[";
  bool first = true;
  for (const auto& action : process_algebra.actions()) {
    if (first) {
      first = false;
    } else {
      *os_ << ',';
    }
    *os_ << action;
  }
  *os_ << "]| ";
  std::swap(parent_precedence_, precedence);
  std::swap(parent_binary_op_, binary_op);
  process_algebra.operand2().Accept(this);
  std::swap(parent_binary_op_, binary_op);
  std::swap(parent_precedence_, precedence);
  if (parent_precedence_ > precedence ||
      (parent_precedence_ == precedence && parent_binary_op_ != binary_op)) {
    *os_ << ')';
  }
}

void Printer::DoVisitActionHiding(const ActionHiding& process_algebra) {
  int precedence = 2;
  std::swap(parent_precedence_, precedence);
  process_algebra.operand().Accept(this);
  std::swap(parent_precedence_, precedence);
  *os_ << " / {";
  bool first = true;
  for (const auto& action : process_algebra.actions()) {
    if (first) {
      first = false;
    } else {
      *os_ << ',';
    }
    *os_ << action;
  }
  *os_ << "}";
}

void Printer::DoVisitActionRenaming(const ActionRenaming& process_algebra) {
  int precedence = 2;
  std::swap(parent_precedence_, precedence);
  process_algebra.operand().Accept(this);
  std::swap(parent_precedence_, precedence);
  *os_ << " {";
  bool first = true;
  for (const auto& substitution : process_algebra.substitutions()) {
    if (first) {
      first = false;
    } else {
      *os_ << ',';
    }
    *os_ << substitution.first << "<-" << substitution.second;
  }
  *os_ << "}";
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const ProcessAlgebra& a) {
  Printer printer(&os);
  a.Accept(&printer);
  return os;
}

ModuleIdentifier::ModuleIdentifier(const std::string& name) : name_(name) {}

void ModuleIdentifier::DoAccept(ProcessAlgebraVisitor* visitor) const {
  visitor->VisitModuleIdentifier(*this);
}

std::ostream& operator<<(std::ostream& os, ParallelCompositionOperator op) {
  switch (op) {
    case ParallelCompositionOperator::ALPHABETIZED:
      return os << "||";
    case ParallelCompositionOperator::ASYNCHRONOUS:
      return os << "|||";
  }
  LOG(FATAL) << "bad parallel composition operator";
}

ParallelComposition::ParallelComposition(
    ParallelCompositionOperator op,
    std::unique_ptr<const ProcessAlgebra>&& operand1,
    std::unique_ptr<const ProcessAlgebra>&& operand2)
    : op_(op), operand1_(std::move(operand1)), operand2_(std::move(operand2)) {}

void ParallelComposition::DoAccept(ProcessAlgebraVisitor* visitor) const {
  visitor->VisitParallelComposition(*this);
}

RestrictedParallelComposition::RestrictedParallelComposition(
    const std::set<std::string>& actions,
    std::unique_ptr<const ProcessAlgebra>&& operand1,
    std::unique_ptr<const ProcessAlgebra>&& operand2)
    : actions_(actions),
      operand1_(std::move(operand1)),
      operand2_(std::move(operand2)) {}

void RestrictedParallelComposition::DoAccept(
    ProcessAlgebraVisitor* visitor) const {
  visitor->VisitRestrictedParallelComposition(*this);
}

ActionHiding::ActionHiding(const std::set<std::string>& actions,
                           std::unique_ptr<const ProcessAlgebra>&& operand)
    : actions_(actions), operand_(std::move(operand)) {}

void ActionHiding::DoAccept(ProcessAlgebraVisitor* visitor) const {
  visitor->VisitActionHiding(*this);
}

ActionRenaming::ActionRenaming(
    const std::map<std::string, std::string>& substitutions,
    std::unique_ptr<const ProcessAlgebra>&& operand)
    : substitutions_(substitutions), operand_(std::move(operand)) {}

void ActionRenaming::DoAccept(ProcessAlgebraVisitor* visitor) const {
  visitor->VisitActionRenaming(*this);
}

ProcessAlgebraVisitor::~ProcessAlgebraVisitor() = default;

void ProcessAlgebraVisitor::VisitModuleIdentifier(
    const ModuleIdentifier& process_algebra) {
  DoVisitModuleIdentifier(process_algebra);
}

void ProcessAlgebraVisitor::VisitParallelComposition(
    const ParallelComposition& process_algebra) {
  DoVisitParallelComposition(process_algebra);
}

void ProcessAlgebraVisitor::VisitRestrictedParallelComposition(
    const RestrictedParallelComposition& process_algebra) {
  DoVisitRestrictedParallelComposition(process_algebra);
}

void ProcessAlgebraVisitor::VisitActionHiding(
    const ActionHiding& process_algebra) {
  DoVisitActionHiding(process_algebra);
}

void ProcessAlgebraVisitor::VisitActionRenaming(
    const ActionRenaming& process_algebra) {
  DoVisitActionRenaming(process_algebra);
}
