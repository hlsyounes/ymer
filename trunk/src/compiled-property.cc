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

#include <deque>
#include <utility>

#include "unique-ptr-vector.h"

#include "glog/logging.h"

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
    case CompiledProbabilisticOperator::GREATER_EQUAL:
      *os_ << ">=";
      break;
    case CompiledProbabilisticOperator::GREATER:
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

std::unique_ptr<const CompiledAndProperty> CompiledAndProperty::New(
    UniquePtrVector<const CompiledProperty>&& operands) {
  return std::unique_ptr<const CompiledAndProperty>(
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

std::unique_ptr<const CompiledNotProperty> CompiledNotProperty::New(
    std::unique_ptr<const CompiledProperty>&& operand) {
  return std::unique_ptr<const CompiledNotProperty>(
      new CompiledNotProperty(std::move(operand)));
}

void CompiledNotProperty::DoAccept(CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledNotProperty(*this);
}

CompiledProbabilisticProperty::CompiledProbabilisticProperty(
    CompiledProbabilisticOperator op, double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property)
    : CompiledProperty(true),
      op_(op), threshold_(threshold), path_property_(std::move(path_property)) {
}

CompiledProbabilisticProperty::~CompiledProbabilisticProperty() = default;

std::unique_ptr<const CompiledProbabilisticProperty>
CompiledProbabilisticProperty::New(
    CompiledProbabilisticOperator op,
    double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property) {
  return std::unique_ptr<const CompiledProbabilisticProperty>(
      new CompiledProbabilisticProperty(op, threshold,
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

std::unique_ptr<const CompiledExpressionProperty>
CompiledExpressionProperty::New(const CompiledExpression& expr) {
  return std::unique_ptr<const CompiledExpressionProperty>(
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

std::unique_ptr<const CompiledUntilProperty> CompiledUntilProperty::New(
    double min_time, double max_time,
    std::unique_ptr<const CompiledProperty>&& pre,
    std::unique_ptr<const CompiledProperty>&& post,
    int index, const std::string& string, const Until* formula) {
  return std::unique_ptr<const CompiledUntilProperty>(new CompiledUntilProperty(
      min_time, max_time, std::move(pre), std::move(post), index, string,
      formula));
}

void CompiledUntilProperty::DoAccept(
    CompiledPathPropertyVisitor* visitor) const {
  visitor->VisitCompiledUntilProperty(*this);
}

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

CompiledPathPropertyVisitor::~CompiledPathPropertyVisitor() = default;

void CompiledPathPropertyVisitor::VisitCompiledUntilProperty(
    const CompiledUntilProperty& property) {
  DoVisitCompiledUntilProperty(property);
}

namespace {

class OptimizerState {
 public:
  OptimizerState();
  OptimizerState(OptimizerState&& state);
  explicit OptimizerState(const std::vector<Operation>& operations);
  explicit OptimizerState(
      std::unique_ptr<const CompiledProbabilisticProperty>&& property);

  void Negate();
  void ConjunctionWith(OptimizerState&& state);

  std::unique_ptr<const CompiledProperty> ReleaseProperty();
  void Clear();

 private:
  std::vector<Operation> operations_;
  std::deque<std::unique_ptr<const CompiledProperty> > conjuncts_;
  bool is_negated_;
};

OptimizerState::OptimizerState()
    : is_negated_(false) {
}

OptimizerState::OptimizerState(OptimizerState&& state)
    : operations_(std::move(state.operations_)),
      conjuncts_(std::move(state.conjuncts_)),
      is_negated_(state.is_negated_) {
  state.Clear();
}

OptimizerState::OptimizerState(const std::vector<Operation>& operations)
    : operations_(operations), is_negated_(false) {
}

OptimizerState::OptimizerState(
    std::unique_ptr<const CompiledProbabilisticProperty>&& property)
    : is_negated_(false) {
  conjuncts_.push_back(std::move(property));
}

void OptimizerState::Negate() {
  if (conjuncts_.empty()) {
    operations_.push_back(Operation::MakeNOT(0));
  } else {
    is_negated_ = !is_negated_;
  }
}

void OptimizerState::ConjunctionWith(OptimizerState&& state) {
  if (is_negated_) {
    conjuncts_.push_back(ReleaseProperty());
  }
  if (state.is_negated_) {
    conjuncts_.push_back(state.ReleaseProperty());
  } else {
    operations_ = MakeConjunction(operations_, state.operations_);
    for (auto i = state.conjuncts_.begin(); i != state.conjuncts_.end(); ++i) {
      conjuncts_.push_back(std::move(*i));
    }
    state.Clear();
  }
}

std::unique_ptr<const CompiledProperty> OptimizerState::ReleaseProperty() {
  if (operations_.empty() && conjuncts_.empty()) {
    CHECK(!is_negated_);
    // Empty state represents TRUE.
    return CompiledExpressionProperty::New(
        CompiledExpression({Operation::MakeICONST(true, 0)}));
  }
  if (!operations_.empty()) {
    CompiledExpression expr(operations_);
    conjuncts_.push_front(
        CompiledExpressionProperty::New(OptimizeIntExpression(expr)));
  }
  std::unique_ptr<const CompiledProperty> property = (conjuncts_.size() == 1)
      ? std::move(conjuncts_.front())
      : CompiledAndProperty::New(UniquePtrVector<const CompiledProperty>(
            conjuncts_.begin(), conjuncts_.end()));
  if (is_negated_) {
    property = CompiledNotProperty::New(std::move(property));
  }
  Clear();
  return property;
}

void OptimizerState::Clear() {
  operations_.clear();
  conjuncts_.clear();
  is_negated_ = false;
}

class CompiledPropertyOptimizer : public CompiledPropertyVisitor {
 public:
  std::unique_ptr<const CompiledProperty> release_property();

 private:
  virtual void DoVisitCompiledAndProperty(const CompiledAndProperty& property);
  virtual void DoVisitCompiledNotProperty(const CompiledNotProperty& property);
  virtual void DoVisitCompiledProbabilisticProperty(
      const CompiledProbabilisticProperty& property);
  virtual void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property);

  OptimizerState state_;
};

std::unique_ptr<const CompiledProperty>
CompiledPropertyOptimizer::release_property() {
  return state_.ReleaseProperty();
}

void CompiledPropertyOptimizer::DoVisitCompiledAndProperty(
    const CompiledAndProperty& property) {
  OptimizerState state = std::move(state_);
  for (const CompiledProperty& operand : property.operands()) {
    operand.Accept(this);
    state.ConjunctionWith(std::move(state_));
  }
  state_ = std::move(state);
}

void CompiledPropertyOptimizer::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  property.operand().Accept(this);
  state_.Negate();
}

void CompiledPropertyOptimizer::DoVisitCompiledProbabilisticProperty(
    const CompiledProbabilisticProperty& property) {
  if (property.op() == CompiledProbabilisticOperator::GREATER_EQUAL
      && property.threshold() <= 0) {
    // Probability is always >= 0, so replace with TRUE.
    state_ = OptimizerState({Operation::MakeICONST(true, 0)});
  } else if (property.op() == CompiledProbabilisticOperator::GREATER
             && property.threshold() >= 1) {
    // Probability is never > 1, so replace with FALSE.
    state_ = OptimizerState({Operation::MakeICONST(false, 0)});
  } else {
    state_ = OptimizerState(CompiledProbabilisticProperty::New(
        property.op(), property.threshold(),
        OptimizePathProperty(property.path_property())));
  }
}

void CompiledPropertyOptimizer::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  state_ = OptimizerState(property.expr().operations());
}

class CompiledPathPropertyOptimizer : public CompiledPathPropertyVisitor {
 public:
  std::unique_ptr<const CompiledPathProperty> release_property();

 private:
  virtual void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& property);

  std::unique_ptr<const CompiledPathProperty> property_;
};

std::unique_ptr<const CompiledPathProperty>
CompiledPathPropertyOptimizer::release_property() {
  return std::move(property_);
}

void CompiledPathPropertyOptimizer::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& property) {
  property_ = CompiledUntilProperty::New(
      property.min_time(), property.max_time(),
      OptimizeProperty(property.pre()), OptimizeProperty(property.post()),
      property.index(), property.string(), property.formula_ptr());
}

}  // namespace

std::unique_ptr<const CompiledProperty> OptimizeProperty(
    const CompiledProperty& property) {
  CompiledPropertyOptimizer optimizer;
  property.Accept(&optimizer);
  return optimizer.release_property();
}

std::unique_ptr<const CompiledPathProperty> OptimizePathProperty(
    const CompiledPathProperty& property) {
  CompiledPathPropertyOptimizer optimizer;
  property.Accept(&optimizer);
  return optimizer.release_property();
}
