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

#include <limits>
#include <utility>
#include <vector>

#include "strutil.h"
#include "unique-ptr-vector.h"

#include "glog/logging.h"

CompiledProperty::CompiledProperty(bool is_probabilistic)
    : is_probabilistic_(is_probabilistic) {}

CompiledProperty::~CompiledProperty() = default;

void CompiledProperty::Accept(CompiledPropertyVisitor* visitor) const {
  return DoAccept(visitor);
}

namespace {

// A compiled property visitor that prints a compiled property to an output
// stream.
class CompiledPropertyPrinter : public CompiledPropertyVisitor,
                                public CompiledPathPropertyVisitor {
 public:
  explicit CompiledPropertyPrinter(std::ostream* os);

 private:
  void DoVisitCompiledNaryProperty(
      const CompiledNaryProperty& property) override;
  void DoVisitCompiledNotProperty(const CompiledNotProperty& property) override;
  void DoVisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property) override;
  void DoVisitCompiledProbabilityEstimationProperty(
      const CompiledProbabilityEstimationProperty& property) override;
  void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property) override;
  void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& path_property) override;

  std::ostream* os_;
};

CompiledPropertyPrinter::CompiledPropertyPrinter(std::ostream* os) : os_(os) {}

void CompiledPropertyPrinter::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  const size_t operand_count =
      property.other_operands().size() + (property.has_expr_operand() ? 1 : 0);
  *os_ << property.op() << " of " << operand_count << " operands:";
  int operand_index = 0;
  if (property.has_expr_operand()) {
    *os_ << std::endl << "operand " << operand_index << ':' << std::endl;
    property.expr_operand().Accept(this);
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

void CompiledPropertyPrinter::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  *os_ << "P " << property.op() << ' ' << property.threshold() << std::endl;
  property.path_property().Accept(this);
}

void CompiledPropertyPrinter::DoVisitCompiledProbabilityEstimationProperty(
    const CompiledProbabilityEstimationProperty& property) {
  *os_ << "P = ?" << std::endl;
  property.path_property().Accept(this);
}

void CompiledPropertyPrinter::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  *os_ << property.expr();
}

void CompiledPropertyPrinter::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  *os_ << path_property.index() << ": UNTIL [" << path_property.min_time()
       << ", " << path_property.max_time() << "]" << std::endl
       << "pre:" << std::endl;
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
                                           bool is_unbounded,
                                           const std::string& string)
    : index_(index),
      is_probabilistic_(is_probabilistic),
      is_unbounded_(is_unbounded),
      string_(string) {}

CompiledPathProperty::~CompiledPathProperty() = default;

void CompiledPathProperty::Accept(CompiledPathPropertyVisitor* visitor) const {
  return DoAccept(visitor);
}

std::ostream& operator<<(std::ostream& os, const CompiledPathProperty& p) {
  CompiledPropertyPrinter printer(&os);
  p.Accept(&printer);
  return os;
}

std::ostream& operator<<(std::ostream& os, CompiledNaryOperator op) {
  switch (op) {
    case CompiledNaryOperator::AND:
      return os << "AND";
    case CompiledNaryOperator::OR:
      return os << "OR";
    case CompiledNaryOperator::IFF:
      return os << "IFF";
  }
  LOG(FATAL) << "bad compiled n-ary operator";
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

CompiledNaryProperty::CompiledNaryProperty(
    CompiledNaryOperator op,
    std::unique_ptr<const CompiledExpressionProperty>&& optional_expr_operand,
    UniquePtrVector<const CompiledProperty>&& other_operands)
    : CompiledProperty(HasOneProbabilistic(other_operands)),
      op_(op),
      optional_expr_operand_(std::move(optional_expr_operand)),
      other_operands_(std::move(other_operands)) {}

std::unique_ptr<const CompiledNaryProperty> CompiledNaryProperty::New(
    CompiledNaryOperator op,
    std::unique_ptr<const CompiledExpressionProperty>&& optional_expr_operand,
    UniquePtrVector<const CompiledProperty>&& other_operands) {
  return std::unique_ptr<const CompiledNaryProperty>(new CompiledNaryProperty(
      op, std::move(optional_expr_operand), std::move(other_operands)));
}

void CompiledNaryProperty::DoAccept(CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledNaryProperty(*this);
}

CompiledNotProperty::CompiledNotProperty(
    std::unique_ptr<const CompiledProperty>&& operand)
    : CompiledProperty(operand->is_probabilistic()),
      operand_(std::move(operand)) {}

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

CompiledProbabilityThresholdProperty::CompiledProbabilityThresholdProperty(
    CompiledProbabilityThresholdOperator op, double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property)
    : CompiledProperty(true),
      op_(op),
      threshold_(threshold),
      path_property_(std::move(path_property)) {}

std::unique_ptr<const CompiledProbabilityThresholdProperty>
CompiledProbabilityThresholdProperty::New(
    CompiledProbabilityThresholdOperator op, double threshold,
    std::unique_ptr<const CompiledPathProperty>&& path_property) {
  return std::unique_ptr<const CompiledProbabilityThresholdProperty>(
      new CompiledProbabilityThresholdProperty(op, threshold,
                                               std::move(path_property)));
}

void CompiledProbabilityThresholdProperty::DoAccept(
    CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledProbabilityThresholdProperty(*this);
}

CompiledProbabilityEstimationProperty::CompiledProbabilityEstimationProperty(
    std::unique_ptr<const CompiledPathProperty>&& path_property)
    : CompiledProperty(true), path_property_(std::move(path_property)) {}

std::unique_ptr<const CompiledProbabilityEstimationProperty>
CompiledProbabilityEstimationProperty::New(
    std::unique_ptr<const CompiledPathProperty>&& path_property) {
  return std::unique_ptr<const CompiledProbabilityEstimationProperty>(
      new CompiledProbabilityEstimationProperty(std::move(path_property)));
}

void CompiledProbabilityEstimationProperty::DoAccept(
    CompiledPropertyVisitor* visitor) const {
  visitor->VisitCompiledProbabilityEstimationProperty(*this);
}

CompiledExpressionProperty::CompiledExpressionProperty(
    const CompiledExpression& expr)
    : CompiledProperty(false), expr_(expr) {}

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
    std::unique_ptr<const CompiledProperty>&& post_property, int index,
    const std::string& string)
    : CompiledPathProperty(
          index,
          pre_property->is_probabilistic() || post_property->is_probabilistic(),
          max_time == std::numeric_limits<double>::infinity(), string),
      min_time_(min_time),
      max_time_(max_time),
      pre_property_(std::move(pre_property)),
      post_property_(std::move(post_property)) {}

std::unique_ptr<const CompiledUntilProperty> CompiledUntilProperty::New(
    double min_time, double max_time,
    std::unique_ptr<const CompiledProperty>&& pre_property,
    std::unique_ptr<const CompiledProperty>&& post_property, int index,
    const std::string& string) {
  return std::unique_ptr<const CompiledUntilProperty>(
      new CompiledUntilProperty(min_time, max_time, std::move(pre_property),
                                std::move(post_property), index, string));
}

void CompiledUntilProperty::DoAccept(
    CompiledPathPropertyVisitor* visitor) const {
  visitor->VisitCompiledUntilProperty(*this);
}

CompiledPropertyVisitor::~CompiledPropertyVisitor() = default;

void CompiledPropertyVisitor::VisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  DoVisitCompiledNaryProperty(property);
}

void CompiledPropertyVisitor::VisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  DoVisitCompiledNotProperty(property);
}

void CompiledPropertyVisitor::VisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  DoVisitCompiledProbabilityThresholdProperty(property);
}

void CompiledPropertyVisitor::VisitCompiledProbabilityEstimationProperty(
    const CompiledProbabilityEstimationProperty& property) {
  DoVisitCompiledProbabilityEstimationProperty(property);
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

class CompilerState {
 public:
  CompilerState();
  explicit CompilerState(
      std::unique_ptr<const CompiledProbabilityEstimationProperty>&& property);
  explicit CompilerState(std::unique_ptr<const CompiledProperty>&& property);
  explicit CompilerState(const Expression* expr);

  bool has_expr() const { return expr_ != nullptr; }

  bool is_estimation() const { return estimation_; }

  std::unique_ptr<const CompiledProperty> ReleaseProperty(
      const std::map<std::string, const Expression*>& formulas_by_name,
      const std::map<std::string, const Expression*>& labels_by_name,
      const std::map<std::string, IdentifierInfo>& identifiers_by_name,
      const Optional<DecisionDiagramManager>& dd_manager,
      std::vector<std::string>* errors);

 private:
  bool estimation_;
  std::unique_ptr<const CompiledProperty> property_;
  const Expression* expr_;
};

CompilerState::CompilerState() : estimation_(false), expr_(nullptr) {}

CompilerState::CompilerState(
    std::unique_ptr<const CompiledProbabilityEstimationProperty>&& property)
    : estimation_(true), property_(std::move(property)), expr_(nullptr) {}

CompilerState::CompilerState(std::unique_ptr<const CompiledProperty>&& property)
    : estimation_(false), property_(std::move(property)), expr_(nullptr) {}

CompilerState::CompilerState(const Expression* expr)
    : estimation_(false), expr_(expr) {}

std::unique_ptr<const CompiledProperty> CompilerState::ReleaseProperty(
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, const Expression*>& labels_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager,
    std::vector<std::string>* errors) {
  if (has_expr()) {
    CHECK(expr_);
    CompileExpressionResult result = CompilePropertyExpression(
        *expr_, Type::BOOL, formulas_by_name, labels_by_name,
        identifiers_by_name, dd_manager);
    property_ = CompiledExpressionProperty::New(result.expr);
    errors->insert(errors->end(), result.errors.begin(), result.errors.end());
  }
  return std::move(property_);
}

class PropertyCompiler : public ExpressionVisitor, public PathPropertyVisitor {
 public:
  PropertyCompiler(
      const std::map<std::string, const Expression*>* formulas_by_name,
      const std::map<std::string, const Expression*>* labels_by_name,
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      const Optional<DecisionDiagramManager>* dd_manager,
      std::vector<std::string>* errors);

  std::unique_ptr<const CompiledProperty> release_property(
      CompilerState* state) {
    return state->ReleaseProperty(*formulas_by_name_, *labels_by_name_,
                                  *identifiers_by_name_, *dd_manager_, errors_);
  }

  std::unique_ptr<const CompiledProperty> release_property() {
    return release_property(&state_);
  }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitLabel(const Label& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;
  void DoVisitProbabilityEstimationOperation(
      const ProbabilityEstimationOperation& expr) override;
  void DoVisitUntilProperty(const UntilProperty& path_property) override;
  void DoVisitEventuallyProperty(
      const EventuallyProperty& path_property) override;

  CompilerState state_;
  int next_path_property_index_;
  std::unique_ptr<const CompiledPathProperty> path_property_;
  const std::map<std::string, const Expression*>* formulas_by_name_;
  const std::map<std::string, const Expression*>* labels_by_name_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  const Optional<DecisionDiagramManager>* dd_manager_;
  std::vector<std::string>* errors_;
};

PropertyCompiler::PropertyCompiler(
    const std::map<std::string, const Expression*>* formulas_by_name,
    const std::map<std::string, const Expression*>* labels_by_name,
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    const Optional<DecisionDiagramManager>* dd_manager,
    std::vector<std::string>* errors)
    : next_path_property_index_(0),
      formulas_by_name_(formulas_by_name),
      labels_by_name_(labels_by_name),
      identifiers_by_name_(identifiers_by_name),
      dd_manager_(dd_manager),
      errors_(errors) {}

void PropertyCompiler::DoVisitLiteral(const Literal& expr) {
  state_ = CompilerState(&expr);
}

void PropertyCompiler::DoVisitIdentifier(const Identifier& expr) {
  state_ = CompilerState(&expr);
}

void PropertyCompiler::DoVisitLabel(const Label& expr) {
  state_ = CompilerState(&expr);
}

void PropertyCompiler::DoVisitFunctionCall(const FunctionCall& expr) {
  state_ = CompilerState(&expr);
}

void PropertyCompiler::DoVisitUnaryOperation(const UnaryOperation& expr) {
  expr.operand().Accept(this);
  if (state_.has_expr()) {
    state_ = CompilerState(&expr);
    return;
  }
  if (state_.is_estimation()) {
    errors_->push_back("only top-level estimation is suported");
    return;
  }
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      errors_->push_back(StrCat("type mismatch; unary operator ", expr.op(),
                                "applied to ", Type::BOOL));
      return;
    case UnaryOperator::NOT:
      state_ = CompilerState(CompiledNotProperty::New(release_property()));
      return;
  }
  LOG(FATAL) << "bad unary operator";
}

void PropertyCompiler::DoVisitBinaryOperation(const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  CompilerState state1 = std::move(state_);
  expr.operand2().Accept(this);
  if (state1.has_expr() && state_.has_expr()) {
    state_ = CompilerState(&expr);
    return;
  }
  if (state_.is_estimation()) {
    errors_->push_back("only top-level estimation is suported");
    return;
  }
  switch (expr.op()) {
    case BinaryOperator::PLUS:
    case BinaryOperator::MINUS:
    case BinaryOperator::MULTIPLY:
    case BinaryOperator::DIVIDE:
      errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                " applied to ", Type::BOOL));
      return;
    case BinaryOperator::AND:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::AND, nullptr,
          UniquePtrVector<const CompiledProperty>(release_property(&state1),
                                                  release_property())));
      return;
    case BinaryOperator::OR:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::OR, nullptr,
          UniquePtrVector<const CompiledProperty>(release_property(&state1),
                                                  release_property())));
      return;
    case BinaryOperator::IMPLY:
    case BinaryOperator::LESS_EQUAL:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::OR, nullptr,
          UniquePtrVector<const CompiledProperty>(
              CompiledNotProperty::New(release_property(&state1)),
              release_property())));
      return;
    case BinaryOperator::IFF:
    case BinaryOperator::EQUAL:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::IFF, nullptr,
          UniquePtrVector<const CompiledProperty>(release_property(&state1),
                                                  release_property())));
      return;
    case BinaryOperator::LESS:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::AND, nullptr,
          UniquePtrVector<const CompiledProperty>(
              CompiledNotProperty::New(release_property(&state1)),
              release_property())));
      return;
    case BinaryOperator::GREATER_EQUAL:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::OR, nullptr,
          UniquePtrVector<const CompiledProperty>(
              release_property(&state1),
              CompiledNotProperty::New(release_property()))));
      return;
    case BinaryOperator::GREATER:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::AND, nullptr,
          UniquePtrVector<const CompiledProperty>(
              release_property(&state1),
              CompiledNotProperty::New(release_property()))));
      return;
    case BinaryOperator::NOT_EQUAL:
      state_ = CompilerState(CompiledNotProperty::New(CompiledNaryProperty::New(
          CompiledNaryOperator::IFF, nullptr,
          UniquePtrVector<const CompiledProperty>(release_property(&state1),
                                                  release_property()))));
      return;
  }
  LOG(FATAL) << "bad binary operator";
}

void PropertyCompiler::DoVisitConditional(const Conditional& expr) {
  state_ = CompilerState(&expr);
}

void PropertyCompiler::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  expr.path_property().Accept(this);
  if (expr.threshold() < 0 || expr.threshold() > 1) {
    errors_->push_back(
        StrCat("threshold ", expr.threshold(), " is not a probability"));
  }
  switch (expr.op()) {
    case ProbabilityThresholdOperator::LESS:
      state_ = CompilerState(
          CompiledNotProperty::New(CompiledProbabilityThresholdProperty::New(
              CompiledProbabilityThresholdOperator::GREATER_EQUAL,
              expr.threshold(), std::move(path_property_))));
      return;
    case ProbabilityThresholdOperator::LESS_EQUAL:
      state_ = CompilerState(
          CompiledNotProperty::New(CompiledProbabilityThresholdProperty::New(
              CompiledProbabilityThresholdOperator::GREATER, expr.threshold(),
              std::move(path_property_))));
      return;
    case ProbabilityThresholdOperator::GREATER_EQUAL:
      state_ = CompilerState(CompiledProbabilityThresholdProperty::New(
          CompiledProbabilityThresholdOperator::GREATER_EQUAL, expr.threshold(),
          std::move(path_property_)));
      return;
    case ProbabilityThresholdOperator::GREATER:
      state_ = CompilerState(CompiledProbabilityThresholdProperty::New(
          CompiledProbabilityThresholdOperator::GREATER, expr.threshold(),
          std::move(path_property_)));
      return;
  }
  LOG(FATAL) << "bad probability threshold operator";
}

void PropertyCompiler::DoVisitProbabilityEstimationOperation(
    const ProbabilityEstimationOperation& expr) {
  expr.path_property().Accept(this);
  state_ = CompilerState(
      CompiledProbabilityEstimationProperty::New(std::move(path_property_)));
}

void PropertyCompiler::DoVisitUntilProperty(
    const UntilProperty& path_property) {
  const double min_time = path_property.time_range().min();
  const double max_time = path_property.time_range().max();
  if (min_time > max_time) {
    errors_->push_back(StrCat("bad time range; ", min_time, " > ", max_time));
  }
  const int index = next_path_property_index_;
  ++next_path_property_index_;
  path_property.pre_expr().Accept(this);
  std::unique_ptr<const CompiledProperty> pre_property = release_property();
  path_property.post_expr().Accept(this);
  path_property_ = CompiledUntilProperty::New(
      min_time, max_time, std::move(pre_property), release_property(), index,
      StrCat(path_property));
}

void PropertyCompiler::DoVisitEventuallyProperty(
    const EventuallyProperty& path_property) {
  const double min_time = path_property.time_range().min();
  const double max_time = path_property.time_range().max();
  if (min_time > max_time) {
    errors_->push_back(StrCat("bad time range; ", min_time, " > ", max_time));
  }
  const int index = next_path_property_index_;
  ++next_path_property_index_;
  Literal pre_expr(true);
  pre_expr.Accept(this);
  std::unique_ptr<const CompiledProperty> pre_property = release_property();
  path_property.expr().Accept(this);
  path_property_ = CompiledUntilProperty::New(
      min_time, max_time, std::move(pre_property), release_property(), index,
      StrCat(path_property));
}

}  // namespace

CompilePropertyResult CompileProperty(
    const Expression& expr,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, const Expression*>& labels_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager) {
  CompilePropertyResult result;
  PropertyCompiler compiler(&formulas_by_name, &labels_by_name,
                            &identifiers_by_name, &dd_manager, &result.errors);
  expr.Accept(&compiler);
  result.property = compiler.release_property();
  return std::move(result);
}

namespace {

class OptimizerState {
 public:
  OptimizerState();
  explicit OptimizerState(
      std::unique_ptr<const CompiledExpressionProperty>&& expr_operand);
  explicit OptimizerState(
      std::unique_ptr<const CompiledProbabilityThresholdProperty>&& property);
  explicit OptimizerState(
      std::unique_ptr<const CompiledProbabilityEstimationProperty>&& property);

  bool is_expr() const {
    return expr_operand_ != nullptr && other_operands_.empty();
  }

  size_t operand_count() const {
    return (expr_operand_ ? 1 : 0) + other_operands_.size();
  }

  void Negate();
  void ComposeWith(CompiledNaryOperator op, OptimizerState&& state);

  std::unique_ptr<const CompiledProperty> ReleaseProperty();

 private:
  std::unique_ptr<const CompiledExpressionProperty> expr_operand_;
  std::vector<std::unique_ptr<const CompiledProperty>> other_operands_;
  bool is_negated_;
  CompiledNaryOperator op_;
};

OptimizerState::OptimizerState() {}

OptimizerState::OptimizerState(
    std::unique_ptr<const CompiledExpressionProperty>&& expr_operand)
    : expr_operand_(std::move(expr_operand)), is_negated_(false) {}

OptimizerState::OptimizerState(
    std::unique_ptr<const CompiledProbabilityThresholdProperty>&& property)
    : is_negated_(false) {
  other_operands_.push_back(std::move(property));
}

OptimizerState::OptimizerState(
    std::unique_ptr<const CompiledProbabilityEstimationProperty>&& property)
    : is_negated_(false) {
  other_operands_.push_back(std::move(property));
}

void OptimizerState::Negate() {
  if (is_expr()) {
    std::vector<Operation> operations(expr_operand_->expr().operations());
    operations.push_back(Operation::MakeNOT(0));
    Optional<ADD> dd;
    if (expr_operand_->expr().dd().has_value()) {
      dd = ADD(!BDD(expr_operand_->expr().dd().value()));
    }
    expr_operand_ =
        CompiledExpressionProperty::New(CompiledExpression(operations, dd));
  } else {
    is_negated_ = !is_negated_;
  }
}

std::vector<Operation> ComposeOperations(
    CompiledNaryOperator op, const std::vector<Operation>& operations1,
    const std::vector<Operation>& operations2) {
  std::vector<Operation> operations(operations1);
  const int pc_shift =
      operations1.size() + ((op == CompiledNaryOperator::IFF) ? 0 : 1);
  const int reg_shift = (op == CompiledNaryOperator::IFF) ? 1 : 0;
  if (op == CompiledNaryOperator::AND) {
    operations.push_back(
        Operation::MakeIFFALSE(0, pc_shift + operations2.size()));
  } else if (op == CompiledNaryOperator::OR) {
    operations.push_back(
        Operation::MakeIFTRUE(0, pc_shift + operations2.size()));
  }
  for (const Operation& o : operations2) {
    operations.push_back(o.Shift(pc_shift, reg_shift));
  }
  if (op == CompiledNaryOperator::IFF) {
    operations.push_back(Operation::MakeIEQ(0, 1));
  }
  return operations;
}

std::unique_ptr<const CompiledExpressionProperty> ComposeExpressions(
    CompiledNaryOperator op,
    std::unique_ptr<const CompiledExpressionProperty>&& expr1,
    std::unique_ptr<const CompiledExpressionProperty>&& expr2) {
  Optional<ADD> dd;
  if (expr1->expr().dd().has_value() && expr2->expr().dd().has_value()) {
    switch (op) {
      case CompiledNaryOperator::AND:
        dd = ADD(BDD(expr1->expr().dd().value()) &&
                 BDD(expr2->expr().dd().value()));
        break;
      case CompiledNaryOperator::OR:
        dd = ADD(BDD(expr1->expr().dd().value()) ||
                 BDD(expr2->expr().dd().value()));
        break;
      case CompiledNaryOperator::IFF:
        dd = expr1->expr().dd().value() == expr2->expr().dd().value();
        break;
    }
  }
  return CompiledExpressionProperty::New(
      CompiledExpression(ComposeOperations(op, expr1->expr().operations(),
                                           expr2->expr().operations()),
                         dd));
}

void OptimizerState::ComposeWith(CompiledNaryOperator op,
                                 OptimizerState&& state) {
  if (is_negated_ || (operand_count() > 1 && op_ != op)) {
    other_operands_.push_back(ReleaseProperty());
  }
  if (state.is_negated_ || (state.operand_count() > 1 && state.op_ != op)) {
    other_operands_.push_back(state.ReleaseProperty());
  } else {
    if (expr_operand_ == nullptr) {
      expr_operand_ = std::move(state.expr_operand_);
    } else if (state.expr_operand_ != nullptr) {
      expr_operand_ = ComposeExpressions(op, std::move(expr_operand_),
                                         std::move(state.expr_operand_));
    }
    for (auto i = state.other_operands_.begin();
         i != state.other_operands_.end(); ++i) {
      other_operands_.push_back(std::move(*i));
    }
    state.other_operands_.clear();
  }
  op_ = op;
}

std::unique_ptr<const CompiledProperty> OptimizerState::ReleaseProperty() {
  std::unique_ptr<const CompiledProperty> property;
  if (is_expr()) {
    property = CompiledExpressionProperty::New(
        OptimizeIntExpression(expr_operand_->expr()));
  } else {
    if (operand_count() == 1) {
      property = std::move(other_operands_[0]);
    } else {
      property = CompiledNaryProperty::New(
          op_, expr_operand_ == nullptr
                   ? nullptr
                   : CompiledExpressionProperty::New(
                         OptimizeIntExpression(expr_operand_->expr())),
          UniquePtrVector<const CompiledProperty>(other_operands_.begin(),
                                                  other_operands_.end()));
    }
    if (is_negated_) {
      property = CompiledNotProperty::New(std::move(property));
    }
  }
  expr_operand_.reset();
  other_operands_.clear();
  is_negated_ = false;
  return property;
}

class CompiledPropertyOptimizer : public CompiledPropertyVisitor,
                                  CompiledPathPropertyVisitor {
 public:
  explicit CompiledPropertyOptimizer(
      const Optional<DecisionDiagramManager>* dd_manager);

  std::unique_ptr<const CompiledProperty> release_property() {
    return state_.ReleaseProperty();
  }

 private:
  void DoVisitCompiledNaryProperty(
      const CompiledNaryProperty& property) override;
  void DoVisitCompiledNotProperty(const CompiledNotProperty& property) override;
  void DoVisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property) override;
  void DoVisitCompiledProbabilityEstimationProperty(
      const CompiledProbabilityEstimationProperty& property) override;
  void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property) override;
  void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& path_property) override;

  OptimizerState state_;
  std::unique_ptr<const CompiledPathProperty> path_property_;
  const Optional<DecisionDiagramManager>* dd_manager_;
};

CompiledPropertyOptimizer::CompiledPropertyOptimizer(
    const Optional<DecisionDiagramManager>* dd_manager)
    : dd_manager_(dd_manager) {}

void CompiledPropertyOptimizer::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  if (property.other_operands().empty()) {
    state_ = OptimizerState(
        CompiledExpressionProperty::New(property.expr_operand().expr()));
  } else {
    property.other_operands()[0].Accept(this);
    OptimizerState state = std::move(state_);
    for (size_t i = 1; i < property.other_operands().size(); ++i) {
      property.other_operands()[i].Accept(this);
      state.ComposeWith(property.op(), std::move(state_));
    }
    if (property.has_expr_operand()) {
      state.ComposeWith(property.op(),
                        OptimizerState(CompiledExpressionProperty::New(
                            property.expr_operand().expr())));
    }
    state_ = std::move(state);
  }
}

void CompiledPropertyOptimizer::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  property.operand().Accept(this);
  state_.Negate();
}

void CompiledPropertyOptimizer::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  if (property.op() == CompiledProbabilityThresholdOperator::GREATER_EQUAL &&
      property.threshold() <= 0) {
    // Probability is always >= 0, so replace with TRUE.
    Optional<ADD> dd;
    if (dd_manager_->has_value()) {
      dd = dd_manager_->value().GetConstant(1);
    }
    state_ = OptimizerState(CompiledExpressionProperty::New(
        CompiledExpression({Operation::MakeICONST(true, 0)}, dd)));
  } else if (property.op() == CompiledProbabilityThresholdOperator::GREATER &&
             property.threshold() >= 1) {
    // Probability is never > 1, so replace with FALSE.
    Optional<ADD> dd;
    if (dd_manager_->has_value()) {
      dd = dd_manager_->value().GetConstant(0);
    }
    state_ = OptimizerState(CompiledExpressionProperty::New(
        CompiledExpression({Operation::MakeICONST(false, 0)}, dd)));
  } else {
    property.path_property().Accept(this);
    state_ = OptimizerState(CompiledProbabilityThresholdProperty::New(
        property.op(), property.threshold(), std::move(path_property_)));
  }
}

void CompiledPropertyOptimizer::DoVisitCompiledProbabilityEstimationProperty(
    const CompiledProbabilityEstimationProperty& property) {
  property.path_property().Accept(this);
  state_ = OptimizerState(
      CompiledProbabilityEstimationProperty::New(std::move(path_property_)));
}

void CompiledPropertyOptimizer::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  state_ = OptimizerState(CompiledExpressionProperty::New(property.expr()));
}

void CompiledPropertyOptimizer::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  path_property.pre_property().Accept(this);
  std::unique_ptr<const CompiledProperty> pre_property = release_property();
  path_property.post_property().Accept(this);
  path_property_ = CompiledUntilProperty::New(
      path_property.min_time(), path_property.max_time(),
      std::move(pre_property), release_property(), path_property.index(),
      path_property.string());
}

}  // namespace

std::unique_ptr<const CompiledProperty> OptimizeProperty(
    const CompiledProperty& property,
    const Optional<DecisionDiagramManager>& dd_manager) {
  CompiledPropertyOptimizer optimizer(&dd_manager);
  property.Accept(&optimizer);
  return optimizer.release_property();
}

namespace {

class PropertyRegisterCounter : public CompiledPropertyVisitor,
                                public CompiledPathPropertyVisitor {
 public:
  PropertyRegisterCounter();

  std::pair<int, int> counts() const { return counts_; }

 private:
  void DoVisitCompiledNaryProperty(
      const CompiledNaryProperty& property) override;
  void DoVisitCompiledNotProperty(const CompiledNotProperty& property) override;
  void DoVisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property) override;
  void DoVisitCompiledProbabilityEstimationProperty(
      const CompiledProbabilityEstimationProperty& property) override;
  void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property) override;
  void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& path_property) override;

  std::pair<int, int> counts_;
};

PropertyRegisterCounter::PropertyRegisterCounter() : counts_(0, 0) {}

void PropertyRegisterCounter::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  if (property.has_expr_operand()) {
    property.expr_operand().Accept(this);
  }
  for (const CompiledProperty& operand : property.other_operands()) {
    operand.Accept(this);
  }
}

void PropertyRegisterCounter::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  property.operand().Accept(this);
}

void PropertyRegisterCounter::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  property.path_property().Accept(this);
}

void PropertyRegisterCounter::DoVisitCompiledProbabilityEstimationProperty(
    const CompiledProbabilityEstimationProperty& property) {
  property.path_property().Accept(this);
}

void PropertyRegisterCounter::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  auto expr_counts = GetExpressionRegisterCounts(property.expr());
  counts_.first = std::max(counts_.first, expr_counts.first);
  counts_.second = std::max(counts_.second, expr_counts.second);
}

void PropertyRegisterCounter::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  path_property.pre_property().Accept(this);
  path_property.post_property().Accept(this);
}

}  // namespace

std::pair<int, int> GetPropertyRegisterCounts(
    const CompiledProperty& property) {
  PropertyRegisterCounter counter;
  property.Accept(&counter);
  return counter.counts();
}
