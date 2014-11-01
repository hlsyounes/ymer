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

void CompiledPropertyPrinter::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  *os_ << "P " << property.op() << ' ' << property.threshold() << std::endl;
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
                                           const std::string& string)
    : index_(index), is_probabilistic_(is_probabilistic), string_(string) {}

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
    CompiledNaryOperator op, const CompiledExpression& optional_expr_operand,
    const BDD& expr_operand_bdd,
    UniquePtrVector<const CompiledProperty>&& other_operands)
    : CompiledProperty(HasOneProbabilistic(other_operands)),
      op_(op),
      optional_expr_operand_(optional_expr_operand),
      expr_operand_bdd_(expr_operand_bdd),
      other_operands_(std::move(other_operands)) {}

std::unique_ptr<const CompiledNaryProperty> CompiledNaryProperty::New(
    CompiledNaryOperator op, const CompiledExpression& optional_expr_operand,
    const BDD& expr_operand_bdd,
    UniquePtrVector<const CompiledProperty>&& other_operands) {
  return std::unique_ptr<const CompiledNaryProperty>(new CompiledNaryProperty(
      op, optional_expr_operand, expr_operand_bdd, std::move(other_operands)));
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

CompiledExpressionProperty::CompiledExpressionProperty(
    const CompiledExpression& expr, const BDD& bdd)
    : CompiledProperty(false), expr_(expr), bdd_(bdd) {}

std::unique_ptr<const CompiledExpressionProperty>
CompiledExpressionProperty::New(const CompiledExpression& expr,
                                const BDD& bdd) {
  return std::unique_ptr<const CompiledExpressionProperty>(
      new CompiledExpressionProperty(expr, bdd));
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
    : CompiledPathProperty(index, pre_property->is_probabilistic() ||
                                      post_property->is_probabilistic(),
                           string),
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

class ExpressionToBddConverter : public ExpressionVisitor {
 public:
  explicit ExpressionToBddConverter(
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      const DecisionDiagramManager* dd_manager);

  BDD bdd() const {
    CHECK(has_bdd_);
    return bdd_;
  }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;

  bool has_bdd_;
  BDD bdd_;
  ADD add_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  const DecisionDiagramManager* dd_manager_;
};

ExpressionToBddConverter::ExpressionToBddConverter(
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    const DecisionDiagramManager* dd_manager)
    : has_bdd_(false),
      bdd_(dd_manager->GetConstant(false)),
      add_(dd_manager->GetConstant(0)),
      identifiers_by_name_(identifiers_by_name),
      dd_manager_(dd_manager) {}

void ExpressionToBddConverter::DoVisitLiteral(const Literal& expr) {
  if (expr.value().type() == Type::BOOL) {
    has_bdd_ = true;
    bdd_ = dd_manager_->GetConstant(expr.value().value<bool>());
  } else {
    has_bdd_ = false;
    add_ = dd_manager_->GetConstant(expr.value().value<double>());
  }
}

void ExpressionToBddConverter::DoVisitIdentifier(const Identifier& expr) {
  auto i = identifiers_by_name_->find(expr.name());
  CHECK(i != identifiers_by_name_->end());
  const IdentifierInfo& info = i->second;
  if (info.type() == Type::BOOL) {
    has_bdd_ = true;
    if (info.is_variable()) {
      bdd_ = dd_manager_->GetBddVariable(2 * info.low_bit());
    } else {
      bdd_ = dd_manager_->GetConstant(info.constant_value().value<bool>());
    }
  } else {
    has_bdd_ = false;
    if (info.is_variable()) {
      add_ = dd_manager_->GetConstant(0);
      for (int i = info.high_bit(); i >= info.low_bit(); --i) {
        add_ = add_ + (dd_manager_->GetAddVariable(2 * i) *
                       dd_manager_->GetConstant(1 << (info.high_bit() - i)));
      }
      add_ = add_ + dd_manager_->GetConstant(info.min_value().value<double>());
    } else {
      add_ = dd_manager_->GetConstant(info.constant_value().value<double>());
    }
  }
}

void ExpressionToBddConverter::DoVisitFunctionCall(const FunctionCall& expr) {
  CHECK(!expr.arguments().empty());
  expr.arguments()[0].Accept(this);
  if (has_bdd_) {
    BDD bdd = bdd_;
    switch (expr.function()) {
      case Function::UNKNOWN:
        LOG(FATAL) << "bad function call";
      case Function::MIN:
        for (size_t i = 1; i < expr.arguments().size(); ++i) {
          expr.arguments()[i].Accept(this);
          CHECK(has_bdd_);
          bdd = Ite(bdd < bdd_, bdd, bdd_);
        }
        bdd_ = bdd;
        break;
      case Function::MAX:
        for (size_t i = 1; i < expr.arguments().size(); ++i) {
          expr.arguments()[i].Accept(this);
          CHECK(has_bdd_);
          bdd = Ite(bdd > bdd_, bdd, bdd_);
        }
        bdd_ = bdd;
        break;
    case Function::FLOOR:
    case Function::CEIL:
    case Function::POW:
    case Function::LOG:
    case Function::MOD:
      LOG(FATAL) << "type mismatch";
    }
  } else {
    std::vector<ADD> arguments;
    arguments.push_back(add_);
    for (size_t i = 1; i < arguments.size(); ++i) {
      expr.arguments()[i].Accept(this);
      CHECK(!has_bdd_);
      arguments.push_back(add_);
    }
    switch (expr.function()) {
      case Function::UNKNOWN:
        LOG(FATAL) << "bad function call";
      case Function::MIN:
        CHECK(!arguments.empty());
        add_ = arguments[0];
        for (size_t i = 1; i < arguments.size(); ++i) {
          add_ = min(add_, arguments[i]);
        }
        break;
      case Function::MAX:
        CHECK(!arguments.empty());
        add_ = arguments[0];
        for (size_t i = 1; i < arguments.size(); ++i) {
          add_ = max(add_, arguments[i]);
        }
        break;
      case Function::FLOOR:
        CHECK_EQ(arguments.size(), 1);
        add_ = floor(arguments[0]);
        break;
      case Function::CEIL:
        CHECK_EQ(arguments.size(), 1);
        add_ = ceil(arguments[0]);
        break;
      case Function::POW:
        CHECK_EQ(arguments.size(), 2);
        add_ = pow(arguments[0], arguments[1]);
        break;
      case Function::LOG:
        CHECK_EQ(arguments.size(), 2);
        add_ = log(arguments[0]) / log(arguments[1]);
        break;
      case Function::MOD:
        CHECK_EQ(arguments.size(), 2);
        add_ = arguments[0] % arguments[1];
        break;
    }
  }
}

void ExpressionToBddConverter::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  expr.operand().Accept(this);
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      CHECK(!has_bdd_);
      add_ = -add_;
      break;
    case UnaryOperator::NOT:
      CHECK(has_bdd_);
      bdd_ = !bdd_;
      break;
  }
}

void ExpressionToBddConverter::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  if (has_bdd_) {
    BDD operand1 = bdd_;
    expr.operand2().Accept(this);
    CHECK(has_bdd_);
    switch (expr.op()) {
      case BinaryOperator::PLUS:
      case BinaryOperator::MINUS:
      case BinaryOperator::MULTIPLY:
      case BinaryOperator::DIVIDE:
        LOG(FATAL) << "type mismatch";
      case BinaryOperator::AND:
        bdd_ = operand1 && bdd_;
        break;
      case BinaryOperator::OR:
        bdd_ = operand1 || bdd_;
        break;
      case BinaryOperator::IMPLY:
        bdd_ = !operand1 || bdd_;
        break;
      case BinaryOperator::IFF:
      case BinaryOperator::EQUAL:
        bdd_ = operand1 == bdd_;
        break;
      case BinaryOperator::LESS:
        bdd_ = operand1 < bdd_;
        break;
      case BinaryOperator::LESS_EQUAL:
        bdd_ = operand1 <= bdd_;
        break;
      case BinaryOperator::GREATER_EQUAL:
        bdd_ = operand1 >= bdd_;
        break;
      case BinaryOperator::GREATER:
        bdd_ = operand1 > bdd_;
        break;
      case BinaryOperator::NOT_EQUAL:
        bdd_ = operand1 != bdd_;
        break;
    }
  } else {
    ADD operand1 = add_;
    expr.operand2().Accept(this);
    CHECK(!has_bdd_);
    switch (expr.op()) {
      case BinaryOperator::PLUS:
        add_ = operand1 + add_;
        break;
      case BinaryOperator::MINUS:
        add_ = operand1 - add_;
        break;
      case BinaryOperator::MULTIPLY:
        add_ = operand1 * add_;
        break;
      case BinaryOperator::DIVIDE:
        add_ = operand1 / add_;
        break;
      case BinaryOperator::AND:
      case BinaryOperator::OR:
      case BinaryOperator::IMPLY:
      case BinaryOperator::IFF:
        LOG(FATAL) << "type mismatch";
      case BinaryOperator::LESS:
        has_bdd_ = true;
        bdd_ = operand1 < add_;
        break;
      case BinaryOperator::LESS_EQUAL:
        has_bdd_ = true;
        bdd_ = operand1 <= add_;
        break;
      case BinaryOperator::GREATER_EQUAL:
        has_bdd_ = true;
        bdd_ = operand1 >= add_;
        break;
      case BinaryOperator::GREATER:
        has_bdd_ = true;
        bdd_ = operand1 > add_;
        break;
      case BinaryOperator::EQUAL:
        has_bdd_ = true;
        bdd_ = operand1 == add_;
        break;
      case BinaryOperator::NOT_EQUAL:
        has_bdd_ = true;
        bdd_ = operand1 != add_;
        break;
    }
  }
}

void ExpressionToBddConverter::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  CHECK(has_bdd_);
  BDD condition = bdd_;
  expr.if_branch().Accept(this);
  if (has_bdd_) {
    BDD if_branch = bdd_;
    expr.else_branch().Accept(this);
    CHECK(has_bdd_);
    bdd_ = Ite(condition, if_branch, bdd_);
  } else {
    ADD if_branch = add_;
    expr.else_branch().Accept(this);
    CHECK(!has_bdd_);
    add_ = Ite(condition, if_branch, add_);
  }
}

void ExpressionToBddConverter::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  LOG(FATAL) << "not an expression";
}

BDD ExpressionToBdd(
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const DecisionDiagramManager& dd_manager, const Expression& expr) {
  ExpressionToBddConverter converter(&identifiers_by_name, &dd_manager);
  expr.Accept(&converter);
  return converter.bdd();
}

class CompilerState {
 public:
  CompilerState();
  explicit CompilerState(std::unique_ptr<const CompiledProperty>&& property);
  explicit CompilerState(const Expression* expr);

  bool has_expr() const { return expr_ != nullptr; }

  std::unique_ptr<const CompiledProperty> ReleaseProperty(
      const std::map<std::string, IdentifierInfo>& identifiers_by_name,
      const DecisionDiagramManager& dd_manager,
      std::vector<std::string>* errors);

 private:
  std::unique_ptr<const CompiledProperty> property_;
  const Expression* expr_;
};

CompilerState::CompilerState() : expr_(nullptr) {}

CompilerState::CompilerState(std::unique_ptr<const CompiledProperty>&& property)
    : property_(std::move(property)), expr_(nullptr) {}

CompilerState::CompilerState(const Expression* expr) : expr_(expr) {}

std::unique_ptr<const CompiledProperty> CompilerState::ReleaseProperty(
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const DecisionDiagramManager& dd_manager,
    std::vector<std::string>* errors) {
  if (has_expr()) {
    CHECK(expr_);
    CompileExpressionResult result =
        CompileExpression(*expr_, Type::BOOL, identifiers_by_name);
    const BDD bdd = result.errors.empty() ? ExpressionToBdd(identifiers_by_name,
                                                            dd_manager, *expr_)
                                          : dd_manager.GetConstant(false);
    property_ = CompiledExpressionProperty::New(result.expr, bdd);
    errors->insert(errors->end(), result.errors.begin(), result.errors.end());
  }
  return std::move(property_);
}

class PropertyCompiler : public ExpressionVisitor, public PathPropertyVisitor {
 public:
  PropertyCompiler(
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      const DecisionDiagramManager* dd_manager,
      std::vector<std::string>* errors);

  std::unique_ptr<const CompiledProperty> release_property(
      CompilerState* state) {
    return state->ReleaseProperty(*identifiers_by_name_, *dd_manager_, errors_);
  }

  std::unique_ptr<const CompiledProperty> release_property() {
    return release_property(&state_);
  }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;
  void DoVisitUntilProperty(const UntilProperty& path_property) override;

  CompilerState state_;
  int next_path_property_index_;
  std::unique_ptr<const CompiledPathProperty> path_property_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  const DecisionDiagramManager* dd_manager_;
  std::vector<std::string>* errors_;
};

PropertyCompiler::PropertyCompiler(
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    const DecisionDiagramManager* dd_manager, std::vector<std::string>* errors)
    : next_path_property_index_(0),
      identifiers_by_name_(identifiers_by_name),
      dd_manager_(dd_manager),
      errors_(errors) {}

void PropertyCompiler::DoVisitLiteral(const Literal& expr) {
  state_ = CompilerState(&expr);
}

void PropertyCompiler::DoVisitIdentifier(const Identifier& expr) {
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
          CompiledNaryOperator::AND, CompiledExpression({}),
          dd_manager_->GetConstant(false),
          UniquePtrVector<const CompiledProperty>(release_property(&state1),
                                                  release_property())));
      return;
    case BinaryOperator::OR:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::OR, CompiledExpression({}),
          dd_manager_->GetConstant(false),
          UniquePtrVector<const CompiledProperty>(release_property(&state1),
                                                  release_property())));
      return;
    case BinaryOperator::IMPLY:
    case BinaryOperator::LESS_EQUAL:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::OR, CompiledExpression({}),
          dd_manager_->GetConstant(false),
          UniquePtrVector<const CompiledProperty>(
              CompiledNotProperty::New(release_property(&state1)),
              release_property())));
      return;
    case BinaryOperator::IFF:
    case BinaryOperator::EQUAL:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::IFF, CompiledExpression({}),
          dd_manager_->GetConstant(false),
          UniquePtrVector<const CompiledProperty>(release_property(&state1),
                                                  release_property())));
      return;
    case BinaryOperator::LESS:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::AND, CompiledExpression({}),
          dd_manager_->GetConstant(false),
          UniquePtrVector<const CompiledProperty>(
              CompiledNotProperty::New(release_property(&state1)),
              release_property())));
      return;
    case BinaryOperator::GREATER_EQUAL:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::OR, CompiledExpression({}),
          dd_manager_->GetConstant(false),
          UniquePtrVector<const CompiledProperty>(
              release_property(&state1),
              CompiledNotProperty::New(release_property()))));
      return;
    case BinaryOperator::GREATER:
      state_ = CompilerState(CompiledNaryProperty::New(
          CompiledNaryOperator::AND, CompiledExpression({}),
          dd_manager_->GetConstant(false),
          UniquePtrVector<const CompiledProperty>(
              release_property(&state1),
              CompiledNotProperty::New(release_property()))));
      return;
    case BinaryOperator::NOT_EQUAL:
      state_ = CompilerState(CompiledNotProperty::New(CompiledNaryProperty::New(
          CompiledNaryOperator::IFF, CompiledExpression({}),
          dd_manager_->GetConstant(false),
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

void PropertyCompiler::DoVisitUntilProperty(
    const UntilProperty& path_property) {
  const double min_time = path_property.min_time();
  const double max_time = path_property.max_time();
  if (min_time > max_time) {
    errors_->push_back(StrCat("bad time range; ", min_time, " > ", max_time));
  }
  const int index = next_path_property_index_;
  ++next_path_property_index_;
  path_property.pre_expr().Accept(this);
  std::unique_ptr<const CompiledProperty>&& pre_property = release_property();
  path_property.post_expr().Accept(this);
  path_property_ = CompiledUntilProperty::New(
      min_time, max_time, std::move(pre_property), release_property(), index,
      StrCat(path_property));
}

}  // namespace

CompilePropertyResult CompileProperty(
    const Expression& expr,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const DecisionDiagramManager& dd_manager) {
  CompilePropertyResult result;
  PropertyCompiler compiler(&identifiers_by_name, &dd_manager, &result.errors);
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

  bool is_expr() const {
    return expr_operand_ != nullptr && other_operands_.empty();
  }

  size_t operand_count() const {
    return (expr_operand_ ? 1 : 0) + other_operands_.size();
  }

  void Negate();
  void ComposeWith(CompiledNaryOperator op, OptimizerState&& state,
                   const DecisionDiagramManager& dd_manager);

  std::unique_ptr<const CompiledProperty> ReleaseProperty(
      const DecisionDiagramManager& dd_manager);

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

void OptimizerState::Negate() {
  if (is_expr()) {
    std::vector<Operation> operations(expr_operand_->expr().operations());
    operations.push_back(Operation::MakeNOT(0));
    expr_operand_ = CompiledExpressionProperty::New(
        CompiledExpression(operations), !expr_operand_->bdd());
  } else {
    is_negated_ = !is_negated_;
  }
}

std::vector<Operation> ComposeOperations(
    CompiledNaryOperator op, const std::vector<Operation>& operations1,
    const std::vector<Operation>& operations2) {
  std::vector<Operation> operations(operations1);
  const int pc_shift =
      operations1.size() + (op == CompiledNaryOperator::IFF) ? 0 : 1;
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
  BDD bdd = expr1->bdd();
  switch (op) {
    case CompiledNaryOperator::AND:
      bdd = bdd && expr2->bdd();
      break;
    case CompiledNaryOperator::OR:
      bdd = bdd || expr2->bdd();
      break;
    case CompiledNaryOperator::IFF:
      bdd = bdd == expr2->bdd();
      break;
  }
  return CompiledExpressionProperty::New(CompiledExpression(ComposeOperations(
      op, expr1->expr().operations(), expr2->expr().operations())), bdd);
}

void OptimizerState::ComposeWith(CompiledNaryOperator op,
                                 OptimizerState&& state,
                                 const DecisionDiagramManager& dd_manager) {
  if (is_negated_ || (operand_count() > 1 && op_ != op)) {
    other_operands_.push_back(ReleaseProperty(dd_manager));
  }
  if (state.is_negated_ || (state.operand_count() > 1 && state.op_ != op)) {
    other_operands_.push_back(state.ReleaseProperty(dd_manager));
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

std::unique_ptr<const CompiledProperty> OptimizerState::ReleaseProperty(
    const DecisionDiagramManager& dd_manager) {
  std::unique_ptr<const CompiledProperty> property;
  if (is_expr()) {
    property = CompiledExpressionProperty::New(
        OptimizeIntExpression(expr_operand_->expr()), expr_operand_->bdd());
  } else {
    if (operand_count() == 1) {
      property = std::move(other_operands_[0]);
    } else {
      property = CompiledNaryProperty::New(
          op_, OptimizeIntExpression(expr_operand_ ? expr_operand_->expr()
                                                   : CompiledExpression({})),
          expr_operand_ ? expr_operand_->bdd() : dd_manager.GetConstant(false),
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
  explicit CompiledPropertyOptimizer(const DecisionDiagramManager* dd_manager);

  std::unique_ptr<const CompiledProperty> release_property();

 private:
  void DoVisitCompiledNaryProperty(
      const CompiledNaryProperty& property) override;
  void DoVisitCompiledNotProperty(const CompiledNotProperty& property) override;
  void DoVisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property) override;
  void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property) override;
  void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& path_property) override;

  OptimizerState state_;
  std::unique_ptr<const CompiledPathProperty> path_property_;
  const DecisionDiagramManager* dd_manager_;
};

CompiledPropertyOptimizer::CompiledPropertyOptimizer(
    const DecisionDiagramManager* dd_manager)
    : dd_manager_(dd_manager) {}

std::unique_ptr<const CompiledProperty>
CompiledPropertyOptimizer::release_property() {
  return state_.ReleaseProperty(*dd_manager_);
}

void CompiledPropertyOptimizer::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  if (property.other_operands().empty()) {
    state_ = OptimizerState(CompiledExpressionProperty::New(
        property.expr_operand(), property.expr_operand_bdd()));
  } else {
    property.other_operands()[0].Accept(this);
    OptimizerState state = std::move(state_);
    for (size_t i = 1; i < property.other_operands().size(); ++i) {
      property.other_operands()[i].Accept(this);
      state.ComposeWith(property.op(), std::move(state_), *dd_manager_);
    }
    if (property.has_expr_operand()) {
      state.ComposeWith(
          property.op(),
          OptimizerState(CompiledExpressionProperty::New(
              property.expr_operand(), property.expr_operand_bdd())),
          *dd_manager_);
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
    state_ = OptimizerState(CompiledExpressionProperty::New(
        CompiledExpression({Operation::MakeICONST(true, 0)}),
        dd_manager_->GetConstant(true)));
  } else if (property.op() == CompiledProbabilityThresholdOperator::GREATER &&
             property.threshold() >= 1) {
    // Probability is never > 1, so replace with FALSE.
    state_ = OptimizerState(CompiledExpressionProperty::New(
        CompiledExpression({Operation::MakeICONST(false, 0)}),
        dd_manager_->GetConstant(false)));
  } else {
    property.path_property().Accept(this);
    state_ = OptimizerState(CompiledProbabilityThresholdProperty::New(
        property.op(), property.threshold(), std::move(path_property_)));
  }
}

void CompiledPropertyOptimizer::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  state_ = OptimizerState(
      CompiledExpressionProperty::New(property.expr(), property.bdd()));
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
    const DecisionDiagramManager& dd_manager) {
  CompiledPropertyOptimizer optimizer(&dd_manager);
  property.Accept(&optimizer);
  return optimizer.release_property();
}
