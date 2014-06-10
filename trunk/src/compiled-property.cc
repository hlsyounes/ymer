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

#include "unique-ptr-vector.h"

CompiledProperty::CompiledProperty(bool is_probabilistic)
    : is_probabilistic_(is_probabilistic) {
}

CompiledProperty::~CompiledProperty() = default;

void CompiledProperty::Accept(CompiledPropertyVisitor* visitor) const {
  return DoAccept(visitor);
}

namespace {

// A compiled property visitor that prints a compiled property to an output
// stream.
class CompiledPropertyPrinter : public CompiledPropertyVisitor {
 public:
  explicit CompiledPropertyPrinter(std::ostream* os);

 private:
  virtual void DoVisitCompiledAndProperty(const CompiledAndProperty& property);
  virtual void DoVisitCompiledNotProperty(const CompiledNotProperty& property);
  virtual void DoVisitCompiledProbabilisticProperty(
      const CompiledProbabilisticProperty& property);
  virtual void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property);

  std::ostream* os_;
};

CompiledPropertyPrinter::CompiledPropertyPrinter(std::ostream* os)
    : os_(os) {
}

void CompiledPropertyPrinter::DoVisitCompiledAndProperty(
    const CompiledAndProperty& property) {
  size_t n = property.operands().size();
  *os_ << "AND of " << n << " operands";
  for (size_t i = 0; i < n; ++i) {
    *os_ << std::endl << "operand " << i << ":"
         << std::endl << property.operands()[i];
  }
}

void CompiledPropertyPrinter::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  *os_ << "NOT of:" << std::endl << property.operand();
}

void CompiledPropertyPrinter::DoVisitCompiledProbabilisticProperty(
    const CompiledProbabilisticProperty& property) {
  *os_ << "P ";
  switch (property.op()) {
    case CompiledProbabilisticProperty::Operator::GREATER_EQUAL:
      *os_ << ">=";
      break;
    case CompiledProbabilisticProperty::Operator::GREATER:
      *os_ << ">";
      break;
  }
  *os_ << " " << property.threshold() << std::endl << property.path_property();
}

void CompiledPropertyPrinter::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  *os_ << property.expr();
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const CompiledProperty& p) {
  CompiledPropertyPrinter printer(&os);
  p.Accept(&printer);
  return os;
}

CompiledPathProperty::CompiledPathProperty(int index, bool is_probabilistic,
                                           const std::string& string)
    : index_(index), is_probabilistic_(is_probabilistic), string_(string) {
}

CompiledPathProperty::~CompiledPathProperty() = default;

void CompiledPathProperty::Accept(CompiledPathPropertyVisitor* visitor) const {
  return DoAccept(visitor);
}

namespace {

// A compiled path property visitor that prints a compiled path property to an
// output stream.
class CompiledPathPropertyPrinter : public CompiledPathPropertyVisitor {
 public:
  explicit CompiledPathPropertyPrinter(std::ostream* os);

 private:
  virtual void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& property);

  std::ostream* os_;
};

CompiledPathPropertyPrinter::CompiledPathPropertyPrinter(std::ostream* os)
    : os_(os) {
}

void CompiledPathPropertyPrinter::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& property) {
  *os_ << "UNTIL [" << property.min_time() << ", " << property.max_time() << "]"
       << std::endl << "pre:" << std::endl << property.pre()
       << std::endl << "post:" << std::endl << property.post();
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const CompiledPathProperty& p) {
  CompiledPathPropertyPrinter printer(&os);
  p.Accept(&printer);
  return os;
}

namespace {

bool HasOneProbabilistic(
    const UniquePtrVector<const CompiledProperty>& operands) {
  for (const CompiledProperty& operand : operands) {
    if (operand.is_probabilistic()) {
      return true;
    }
  }
  return false;
}

}  // namespace

CompiledAndProperty::CompiledAndProperty(
    UniquePtrVector<const CompiledProperty>&& operands)
    : CompiledProperty(HasOneProbabilistic(operands)),
      operands_(std::move(operands)) {
}

CompiledAndProperty::~CompiledAndProperty() = default;

std::unique_ptr<const CompiledProperty> CompiledAndProperty::Make(
    UniquePtrVector<const CompiledProperty>&& operands) {
  return std::unique_ptr<const CompiledProperty>(
      new CompiledAndProperty(std::move(operands)));
}

void CompiledAndProperty::DoAccept(CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledAndProperty(*this);
}

CompiledNotProperty::CompiledNotProperty(
    std::unique_ptr<const CompiledProperty>&& operand)
    : CompiledProperty(operand->is_probabilistic()),
      operand_(std::move(operand)) {
}

CompiledNotProperty::~CompiledNotProperty() = default;

std::unique_ptr<const CompiledProperty> CompiledNotProperty::Make(
    std::unique_ptr<const CompiledProperty>&& operand) {
  return std::unique_ptr<const CompiledProperty>(
      new CompiledNotProperty(std::move(operand)));
}

void CompiledNotProperty::DoAccept(CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledNotProperty(*this);
}

CompiledProbabilisticProperty::CompiledProbabilisticProperty(
    Operator op, double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property)
    : CompiledProperty(true),
      op_(op), threshold_(threshold), path_property_(std::move(path_property)) {
}

CompiledProbabilisticProperty::~CompiledProbabilisticProperty() = default;

std::unique_ptr<const CompiledProperty>
CompiledProbabilisticProperty::MakeGreaterEqual(
    double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property) {
  return std::unique_ptr<const CompiledProperty>(
      new CompiledProbabilisticProperty(Operator::GREATER_EQUAL, threshold,
                                        std::move(path_property)));
}

std::unique_ptr<const CompiledProperty>
CompiledProbabilisticProperty::MakeGreater(
    double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property) {
  return std::unique_ptr<const CompiledProperty>(
      new CompiledProbabilisticProperty(Operator::GREATER, threshold,
                                        std::move(path_property)));
}

void CompiledProbabilisticProperty::DoAccept(
    CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledProbabilisticProperty(*this);
}

CompiledExpressionProperty::CompiledExpressionProperty(
    const CompiledExpression& expr)
    : CompiledProperty(false), expr_(expr) {
}

CompiledExpressionProperty::~CompiledExpressionProperty() = default;

std::unique_ptr<const CompiledProperty> CompiledExpressionProperty::Make(
    const CompiledExpression& expr) {
  return std::unique_ptr<const CompiledProperty>(
      new CompiledExpressionProperty(expr));
}

void CompiledExpressionProperty::DoAccept(
    CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledExpressionProperty(*this);
}

CompiledUntilProperty::CompiledUntilProperty(
    double min_time, double max_time,
    std::unique_ptr<const CompiledProperty>&& pre,
    std::unique_ptr<const CompiledProperty>&& post,
    int index, const std::string& string, const Until* formula)
    : CompiledPathProperty(
          index, pre->is_probabilistic() || post->is_probabilistic(), string),
      min_time_(min_time), max_time_(max_time), pre_(std::move(pre)),
      post_(std::move(post)), formula_(formula) {
}

CompiledUntilProperty::~CompiledUntilProperty() = default;

std::unique_ptr<const CompiledPathProperty> CompiledUntilProperty::Make(
    double min_time, double max_time,
    std::unique_ptr<const CompiledProperty>&& pre,
    std::unique_ptr<const CompiledProperty>&& post,
    int index, const std::string& string, const Until* formula) {
  return std::unique_ptr<const CompiledPathProperty>(new CompiledUntilProperty(
      min_time, max_time, std::move(pre), std::move(post), index, string,
      formula));
}

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

void CompiledPropertyVisitor::VisitCompiledAndProperty(
    const CompiledAndProperty& property) {
  DoVisitCompiledAndProperty(property);
}

void CompiledPropertyVisitor::VisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  DoVisitCompiledNotProperty(property);
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
