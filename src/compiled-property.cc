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
#include <vector>

#include "strutil.h"
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
class CompiledPropertyPrinter
    : public CompiledPropertyVisitor, public CompiledPathPropertyVisitor {
 public:
  explicit CompiledPropertyPrinter(std::ostream* os);

 private:
  virtual void DoVisitCompiledAndProperty(const CompiledAndProperty& property);
  virtual void DoVisitCompiledNotProperty(const CompiledNotProperty& property);
  virtual void DoVisitCompiledProbabilityThresholdOperation(
      const CompiledProbabilityThresholdOperation& property);
  virtual void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property);
  virtual void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& path_property);

  std::ostream* os_;
};

CompiledPropertyPrinter::CompiledPropertyPrinter(std::ostream* os)
    : os_(os) {
}

void CompiledPropertyPrinter::DoVisitCompiledAndProperty(
    const CompiledAndProperty& property) {
  const size_t operand_count =
      property.other_operands().size() + (property.has_expr_operand() ? 1 : 0);
  *os_ << "AND of " << operand_count << " operands";
  int operand_index = 0;
  if (property.has_expr_operand()) {
    *os_ << std::endl << "operand " << operand_index << ':' << std::endl
         << property.expr_operand();
    ++operand_index;
  }
  for (const CompiledProperty& operand : property.other_operands()) {
    *os_ << std::endl << "operand " << operand_index << ":" << std::endl;
    operand.Accept(this);
    ++operand_index;
  }
}

void CompiledPropertyPrinter::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  *os_ << "NOT of:" << std::endl;
  property.operand().Accept(this);
}

void CompiledPropertyPrinter::DoVisitCompiledProbabilityThresholdOperation(
    const CompiledProbabilityThresholdOperation& property) {
  *os_ << "P " << property.op() << ' ' << property.threshold() << std::endl;
  property.path_property().Accept(this);
}

void CompiledPropertyPrinter::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  *os_ << property.expr();
}

void CompiledPropertyPrinter::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  *os_ << "UNTIL [" << path_property.min_time() << ", "
       << path_property.max_time() << "]"
       << std::endl << "pre:" << std::endl;
  path_property.pre_property().Accept(this);
  *os_ << std::endl << "post:" << std::endl;
  path_property.post_property().Accept(this);
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

std::ostream& operator<<(std::ostream& os, const CompiledPathProperty& p) {
  CompiledPropertyPrinter printer(&os);
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
    const CompiledExpression& optional_expr_operand,
    UniquePtrVector<const CompiledProperty>&& other_operands)
    : CompiledProperty(HasOneProbabilistic(other_operands)),
      optional_expr_operand_(optional_expr_operand),
      other_operands_(std::move(other_operands)) {
}

std::unique_ptr<const CompiledAndProperty> CompiledAndProperty::New(
    const CompiledExpression& optional_expr_operand,
    UniquePtrVector<const CompiledProperty>&& other_operands) {
  return std::unique_ptr<const CompiledAndProperty>(new CompiledAndProperty(
      optional_expr_operand, std::move(other_operands)));
}

void CompiledAndProperty::DoAccept(CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledAndProperty(*this);
}

CompiledNotProperty::CompiledNotProperty(
    std::unique_ptr<const CompiledProperty>&& operand)
    : CompiledProperty(operand->is_probabilistic()),
      operand_(std::move(operand)) {
}

std::unique_ptr<const CompiledNotProperty> CompiledNotProperty::New(
    std::unique_ptr<const CompiledProperty>&& operand) {
  return std::unique_ptr<const CompiledNotProperty>(
      new CompiledNotProperty(std::move(operand)));
}

void CompiledNotProperty::DoAccept(CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledNotProperty(*this);
}

std::ostream& operator<<(std::ostream& os,
                         CompiledProbabilityThresholdOperator op) {
  switch (op) {
    case CompiledProbabilityThresholdOperator::GREATER_EQUAL:
      return os << ">=";
    case CompiledProbabilityThresholdOperator::GREATER:
      return os << '>';
  }
  LOG(FATAL) << "bad compiled probability threshold operator";
}

CompiledProbabilityThresholdOperation::CompiledProbabilityThresholdOperation(
    CompiledProbabilityThresholdOperator op, double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property)
    : CompiledProperty(true),
      op_(op), threshold_(threshold), path_property_(std::move(path_property)) {
}

std::unique_ptr<const CompiledProbabilityThresholdOperation>
CompiledProbabilityThresholdOperation::New(
    CompiledProbabilityThresholdOperator op,
    double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property) {
  return std::unique_ptr<const CompiledProbabilityThresholdOperation>(
      new CompiledProbabilityThresholdOperation(op, threshold,
                                                std::move(path_property)));
}

void CompiledProbabilityThresholdOperation::DoAccept(
    CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledProbabilityThresholdOperation(*this);
}

CompiledExpressionProperty::CompiledExpressionProperty(
    const CompiledExpression& expr)
    : CompiledProperty(false), expr_(expr) {
}

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
    std::unique_ptr<const CompiledProperty>&& pre_property,
    std::unique_ptr<const CompiledProperty>&& post_property,
    int index, const std::string& string, const Until* formula)
    : CompiledPathProperty(
          index,
          pre_property->is_probabilistic() || post_property->is_probabilistic(),
          string),
      min_time_(min_time), max_time_(max_time),
      pre_property_(std::move(pre_property)),
      post_property_(std::move(post_property)), formula_(formula) {
}

std::unique_ptr<const CompiledUntilProperty> CompiledUntilProperty::New(
    double min_time, double max_time,
    std::unique_ptr<const CompiledProperty>&& pre_property,
    std::unique_ptr<const CompiledProperty>&& post_property,
    int index, const std::string& string, const Until* formula) {
  return std::unique_ptr<const CompiledUntilProperty>(new CompiledUntilProperty(
      min_time, max_time, std::move(pre_property), std::move(post_property),
      index, string, formula));
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

void CompiledPropertyVisitor::VisitCompiledProbabilityThresholdOperation(
    const CompiledProbabilityThresholdOperation& property) {
  DoVisitCompiledProbabilityThresholdOperation(property);
}

void CompiledPropertyVisitor::VisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  DoVisitCompiledExpressionProperty(property);
}

CompiledPathPropertyVisitor::~CompiledPathPropertyVisitor() = default;

void CompiledPathPropertyVisitor::VisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  DoVisitCompiledUntilProperty(path_property);
}

namespace {

class PropertyCompiler : public ExpressionVisitor {
 public:
  PropertyCompiler(
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      std::vector<std::string>* errors);

  std::unique_ptr<const CompiledProperty> release_property() {
    return std::move(property_);
  }

  Type type() const { return type_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);
  virtual void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr);

  std::unique_ptr<const CompiledProperty> property_;
  Type type_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  std::vector<std::string>* errors_;
};

PropertyCompiler::PropertyCompiler(
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    std::vector<std::string>* errors)
    : identifiers_by_name_(identifiers_by_name), errors_(errors) {
}

void PropertyCompiler::DoVisitLiteral(const Literal& expr) {
  errors_->push_back("not implemented");
}

void PropertyCompiler::DoVisitIdentifier(const Identifier& expr) {
  errors_->push_back("not implemented");
}

void PropertyCompiler::DoVisitFunctionCall(const FunctionCall& expr) {
  errors_->push_back("not implemented");
}

void PropertyCompiler::DoVisitUnaryOperation(const UnaryOperation& expr) {
  errors_->push_back("not implemented");
}

void PropertyCompiler::DoVisitBinaryOperation(const BinaryOperation& expr) {
  errors_->push_back("not implemented");
}

void PropertyCompiler::DoVisitConditional(const Conditional& expr) {
  errors_->push_back("not implemented");
}

void PropertyCompiler::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  errors_->push_back("not implemented");
}

}  // namespace

CompilePropertyResult CompileProperty(
    const Expression& expr,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name) {
  CompilePropertyResult result;
  PropertyCompiler compiler(&identifiers_by_name, &result.errors);
  expr.Accept(&compiler);
  if (!result.errors.empty()) {
    return std::move(result);
  }
  if (compiler.type() != Type::BOOL) {
    result.errors.push_back(StrCat(
        "type mispatch; expecting property of type ", Type::BOOL, "; found ",
        compiler.type()));
    return std::move(result);
  }
  result.property = compiler.release_property();
  return std::move(result);
}

namespace {

class OptimizerState {
 public:
  OptimizerState();
  OptimizerState(OptimizerState&& state);
  explicit OptimizerState(const std::vector<Operation>& operations);
  explicit OptimizerState(
      std::unique_ptr<const CompiledProbabilityThresholdOperation>&& property);

  void Negate();
  void ConjunctionWith(OptimizerState&& state);

  std::unique_ptr<const CompiledProperty> ReleaseProperty();
  void Clear();

 private:
  std::vector<Operation> operations_;
  std::vector<std::unique_ptr<const CompiledProperty> > conjuncts_;
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
    std::unique_ptr<const CompiledProbabilityThresholdOperation>&& property)
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
  const size_t operand_count =
      conjuncts_.size() + (operations_.empty() ? 0 : 1);
  std::unique_ptr<const CompiledProperty> property;
  if (operand_count == 0) {
    property = CompiledExpressionProperty::New(
        CompiledExpression({Operation::MakeICONST(1, 0)}));
  } else if (operand_count > 1) {
    property = CompiledAndProperty::New(
        OptimizeIntExpression(CompiledExpression(operations_)),
        UniquePtrVector<const CompiledProperty>(
            conjuncts_.begin(), conjuncts_.end()));
  } else if (conjuncts_.empty()) {
    property = CompiledExpressionProperty::New(
        OptimizeIntExpression(CompiledExpression(operations_)));
  } else {
    property = std::move(conjuncts_[0]);
  }
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
  virtual void DoVisitCompiledProbabilityThresholdOperation(
      const CompiledProbabilityThresholdOperation& property);
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
  if (property.has_expr_operand()) {
    state.ConjunctionWith(OptimizerState(property.expr_operand().operations()));
  }
  for (const CompiledProperty& operand : property.other_operands()) {
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

void CompiledPropertyOptimizer::DoVisitCompiledProbabilityThresholdOperation(
    const CompiledProbabilityThresholdOperation& property) {
  if (property.op() == CompiledProbabilityThresholdOperator::GREATER_EQUAL
      && property.threshold() <= 0) {
    // Probability is always >= 0, so replace with TRUE.
    state_ = OptimizerState({Operation::MakeICONST(true, 0)});
  } else if (property.op() == CompiledProbabilityThresholdOperator::GREATER
             && property.threshold() >= 1) {
    // Probability is never > 1, so replace with FALSE.
    state_ = OptimizerState({Operation::MakeICONST(false, 0)});
  } else {
    state_ = OptimizerState(CompiledProbabilityThresholdOperation::New(
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
  std::unique_ptr<const CompiledPathProperty> release_path_property();

 private:
  virtual void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& property);

  std::unique_ptr<const CompiledPathProperty> path_property_;
};

std::unique_ptr<const CompiledPathProperty>
CompiledPathPropertyOptimizer::release_path_property() {
  return std::move(path_property_);
}

void CompiledPathPropertyOptimizer::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  path_property_ = CompiledUntilProperty::New(
      path_property.min_time(), path_property.max_time(),
      OptimizeProperty(path_property.pre_property()),
      OptimizeProperty(path_property.post_property()),
      path_property.index(), path_property.string(),
      path_property.formula_ptr());
}

}  // namespace

std::unique_ptr<const CompiledProperty> OptimizeProperty(
    const CompiledProperty& property) {
  CompiledPropertyOptimizer optimizer;
  property.Accept(&optimizer);
  return optimizer.release_property();
}

std::unique_ptr<const CompiledPathProperty> OptimizePathProperty(
    const CompiledPathProperty& path_property) {
  CompiledPathPropertyOptimizer optimizer;
  path_property.Accept(&optimizer);
  return optimizer.release_path_property();
}
