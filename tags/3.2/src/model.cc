// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2014 Google Inc
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

#include "model.h"

#include <map>
#include <set>

#include "strutil.h"

#include "glog/logging.h"

Update::Update(const std::string& variable,
               std::unique_ptr<const Expression>&& expr)
    : variable_(variable), expr_(std::move(expr)) {}

std::ostream& operator<<(std::ostream& os, const Update& update) {
  return os << "(" << update.variable() << "' = " << update.expr() << ")";
}

Outcome::Outcome(std::unique_ptr<const Distribution>&& delay,
                 std::vector<Update>&& updates)
    : delay_(std::move(delay)), updates_(std::move(updates)) {}

std::ostream& operator<<(std::ostream& os, const Outcome& outcome) {
  os << outcome.delay() << " : ";
  if (outcome.updates().empty()) {
    os << "true";
  } else {
    auto i = outcome.updates().begin();
    os << *i;
    for (++i; i != outcome.updates().end(); ++i) {
      os << " & " << *i;
    }
  }
  return os;
}

Command::Command(const std::string& action,
                 std::unique_ptr<const Expression>&& guard,
                 std::vector<Outcome>&& outcomes)
    : action_(action),
      guard_(std::move(guard)),
      outcomes_(std::move(outcomes)) {}

std::ostream& operator<<(std::ostream& os, const Command& command) {
  os << "[" << command.action() << "] " << command.guard() << " -> ";
  auto i = command.outcomes().begin();
  os << *i;
  for (++i; i != command.outcomes().end(); ++i) {
    os << " + " << *i;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, ModelType model_type) {
  switch (model_type) {
    case ModelType::DEFAULT:
    case ModelType::MDP:
      return os << "mdp";
    case ModelType::DTMC:
      return os << "dtmc";
    case ModelType::CTMC:
      return os << "ctmc";
    case ModelType::GSMP:
      return os << "gsmp";
  }
  LOG(FATAL) << "bad model type";
}

ParsedConstant::ParsedConstant(const std::string& name, Type type,
                               std::unique_ptr<const Expression>&& init)
    : name_(name), type_(type), init_(std::move(init)) {}

namespace {

struct ConstantInfo {
  ConstantInfo(Type type_in, const Expression* init_in)
      : type(type_in), init(init_in) {}

  Type type;
  const Expression* init;
};

TypedValue GetDefaultValue(Type type) {
  switch (type) {
    case Type::INT:
      return 0;
    case Type::BOOL:
      return false;
    case Type::DOUBLE:
      return 0.0;
  }
  LOG(FATAL) << "bad type";
}

class ConstantExpressionEvaluator : public ExpressionVisitor {
 public:
  ConstantExpressionEvaluator(
      const std::map<std::string, ConstantInfo>* all_constants,
      std::map<std::string, TypedValue>* constant_values,
      std::vector<std::string>* errors);

 private:
  bool CallAccept(const Expression& expr);

  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;

  TypedValue value_;
  std::set<std::string> pending_constants_;
  const std::map<std::string, ConstantInfo>* all_constants_;
  std::map<std::string, TypedValue>* constant_values_;
  std::vector<std::string>* errors_;
};

ConstantExpressionEvaluator::ConstantExpressionEvaluator(
    const std::map<std::string, ConstantInfo>* all_constants,
    std::map<std::string, TypedValue>* constant_values,
    std::vector<std::string>* errors)
    : value_(0),
      all_constants_(all_constants),
      constant_values_(constant_values),
      errors_(errors) {}

bool ConstantExpressionEvaluator::CallAccept(const Expression& expr) {
  const size_t errors_size = errors_->size();
  expr.Accept(this);
  return errors_size == errors_->size();
}

void ConstantExpressionEvaluator::DoVisitLiteral(const Literal& expr) {
  value_ = expr.value();
}

void ConstantExpressionEvaluator::DoVisitIdentifier(
    const Identifier& expr) {
  auto i = constant_values_->find(expr.name());
  if (i != constant_values_->end()) {
    value_ = i->second;
    return;
  }
  auto j = all_constants_->find(expr.name());
  if (j != all_constants_->end()) {
    const ConstantInfo& info = j->second;
    if (pending_constants_.find(expr.name()) != pending_constants_.end()) {
      errors_->push_back(
          StrCat("cyclic evaluation for constant ", expr.name()));
      constant_values_->emplace(expr.name(), GetDefaultValue(info.type));
      return;
    }
    if (info.init == nullptr) {
      errors_->push_back(StrCat("uninitialized constant ", expr.name()));
      constant_values_->emplace(expr.name(), GetDefaultValue(info.type));
      return;
    }
    pending_constants_.insert(expr.name());
    const bool new_errors = !CallAccept(*info.init);
    pending_constants_.erase(expr.name());
    if (new_errors) {
      constant_values_->emplace(expr.name(), GetDefaultValue(info.type));
      return;
    }
    if (value_.type() != info.type &&
        !(value_.type() == Type::INT && info.type == Type::DOUBLE)) {
      errors_->push_back(StrCat("type mismatch for constant ", expr.name(),
                                "; expecting value of type ", info.type,
                                "; found ", value_.type()));
      constant_values_->emplace(expr.name(), GetDefaultValue(info.type));
      return;
    }
    constant_values_->emplace(expr.name(), value_);
  }
}

void ConstantExpressionEvaluator::DoVisitFunctionCall(
    const FunctionCall& expr) {
  const size_t n = expr.arguments().size();
  switch (expr.function()) {
    case Function::UNKNOWN:
      errors_->push_back("unknown function call");
      return;
    case Function::MIN:
    case Function::MAX:
      if (n == 0) {
        errors_->push_back(
            StrCat(expr.function(),
                   " applied to 0 arguments; expecting at least 1 argument"));
        return;
      }
      if (!CallAccept(expr.arguments()[0])) {
        return;
      }
      for (size_t i = 1; i < n; ++i) {
        TypedValue value = value_;
        if (!CallAccept(expr.arguments()[i])) {
          return;
        }
        if (value.type() != value_.type() &&
            (value.type() == Type::BOOL || value_.type() == Type::BOOL)) {
          errors_->push_back(
              StrCat("type mismatch; incompatible argument types ",
                     value.type(), " and ", value_.type()));
          return;
        }
        if (expr.function() == Function::MIN) {
          value_ = std::min(value, value_);
        } else {
          value_ = std::max(value, value_);
        }
      }
      return;
    case Function::FLOOR:
    case Function::CEIL:
      if (n != 1) {
        errors_->push_back(StrCat(expr.function(), " applied to ", n,
                                  " arguments; expecting 1 argument"));
        return;
      }
      if (!CallAccept(expr.arguments()[0])) {
        return;
      }
      if (value_.type() == Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; expecting argument of type ",
                                  Type::DOUBLE, "; found ", value_.type()));
        return;
      }
      if (expr.function() == Function::FLOOR) {
        value_ = floor(value_);
      } else {
        value_ = ceil(value_);
      }
      return;
      case Function::POW:
      case Function::LOG: {
        if (n != 2) {
          errors_->push_back(StrCat(expr.function(), " applied to ", n,
                                    " argument", (n == 1) ? "" : "s",
                                    "; expecting 2 arguments"));
          return;
        }
        std::vector<TypedValue> values;
        values.reserve(2);
        for (size_t i = 0; i < 2; ++i) {
          if (!CallAccept(expr.arguments()[i])) {
            return;
          }
          if (value_.type() == Type::BOOL) {
            errors_->push_back(
                StrCat("type mismatch; expecting argument of type ",
                       Type::DOUBLE, "; found ", value_.type()));
            return;
          }
          values.push_back(value_);
        }
        if (expr.function() == Function::POW) {
          value_ = pow(values[0], values[1]);
        } else {
          value_ = log(values[0]) / log(values[1]);
        }
        return;
      }
    case Function::MOD: {
      if (n != 2) {
        errors_->push_back(StrCat(expr.function(), " applied to ", n,
                                  " argument", (n == 1) ? "" : "s",
                                  "; expecting 2 arguments"));
        return;
      }
      std::vector<TypedValue> values;
      values.reserve(2);
      for (size_t i = 0; i < 2; ++i) {
        if (!CallAccept(expr.arguments()[i])) {
          return;
        }
        if (value_.type() != Type::INT) {
          errors_->push_back(
              StrCat("type mismatch; expecting argument of type ", Type::INT,
                     "; found ", value_.type()));
          return;
        }
        values.push_back(value_);
      }
      value_ = values[0] % values[1];
      return;
    }
  }
  LOG(FATAL) << "bad function";
}

void ConstantExpressionEvaluator::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  if (!CallAccept(expr.operand())) {
    return;
  }
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      if (value_.type() == Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; unary operator ", expr.op(),
                                  " applied to ", value_.type()));
        return;
      }
      value_ = -value_;
      return;
    case UnaryOperator::NOT:
      if (value_.type() != Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; unary operator ", expr.op(),
                                  " applied to ", value_.type()));
        return;
      }
      value_ = !value_.value<bool>();
      return;
  }
  LOG(FATAL) << "bad unary operator";
}

void ConstantExpressionEvaluator::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  if (!CallAccept(expr.operand1())) {
    return;
  }
  switch (expr.op()) {
    case BinaryOperator::PLUS:
    case BinaryOperator::MINUS:
    case BinaryOperator::MULTIPLY:
    case BinaryOperator::DIVIDE: {
      if (value_.type() == Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                  " applied to ", value_.type()));
        return;
      }
      const TypedValue value = value_;
      if (!CallAccept(expr.operand2())) {
        return;
      }
      if (value_.type() == Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                  " applied to ", value_.type()));
        return;
      }
      if (expr.op() == BinaryOperator::PLUS) {
        value_ = value + value_;
      } else if (expr.op() == BinaryOperator::MINUS) {
        value_ = value - value_;
      } else if (expr.op() == BinaryOperator::MULTIPLY) {
        value_ = value * value_;
      } else {
        value_ = value / value_;
      }
      return;
    }
    case BinaryOperator::AND:
    case BinaryOperator::OR:
    case BinaryOperator::IMPLY:
    case BinaryOperator::IFF: {
      if (value_.type() != Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                  " applied to ", value_.type()));
        return;
      }
      const TypedValue value = value_;
      if (!CallAccept(expr.operand2())) {
        return;
      }
      if (value_.type() != Type::BOOL) {
        errors_->push_back(StrCat("type mismatch; binary operator ", expr.op(),
                                  " applied to ", value_.type()));
        return;
      }
      if (expr.op() == BinaryOperator::AND) {
        value_ = value.value<bool>() && value_.value<bool>();
      } else if (expr.op() == BinaryOperator::OR) {
        value_ = value.value<bool>() || value_.value<bool>();
      } else if (expr.op() == BinaryOperator::IMPLY) {
        value_ = !value.value<bool>() || value_.value<bool>();
      } else {
        value_ = value.value<bool>() == value_.value<bool>();
      }
      return;
    }
    case BinaryOperator::LESS:
    case BinaryOperator::LESS_EQUAL:
    case BinaryOperator::GREATER_EQUAL:
    case BinaryOperator::GREATER:
    case BinaryOperator::EQUAL:
    case BinaryOperator::NOT_EQUAL: {
      const TypedValue value = value_;
      if (!CallAccept(expr.operand2())) {
        return;
      }
      if (value.type() != value_.type() &&
          (value.type() == Type::BOOL || value_.type() == Type::BOOL)) {
        errors_->push_back(StrCat("type mismatch; incompatible argument types ",
                                  value.type(), " and ", value_.type()));
        return;
      }
      if (expr.op() == BinaryOperator::LESS) {
        value_ = value < value_;
      } else if (expr.op() == BinaryOperator::LESS_EQUAL) {
        value_ = value <= value_;
      } else if (expr.op() == BinaryOperator::GREATER_EQUAL) {
        value_ = value >= value_;
      } else if (expr.op() == BinaryOperator::GREATER) {
        value_ = value > value_;
      } else if (expr.op() == BinaryOperator::EQUAL) {
        value_ = value == value_;
      } else {
        value_ = value != value_;
      }
      return;
    }
  }
  LOG(FATAL) << "bad binary operator";
}

void ConstantExpressionEvaluator::DoVisitConditional(const Conditional& expr) {
  if (!CallAccept(expr.condition())) {
    return;
  }
  if (value_.type() != Type::BOOL) {
    errors_->push_back(
        StrCat("type mismatch; expecting condition of type bool; found ",
               value_.type()));
    return;
  }
  if (value_.value<bool>()) {
    expr.if_branch().Accept(this);
  } else {
    expr.else_branch().Accept(this);
  }
}

void ConstantExpressionEvaluator::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  errors_->push_back(
      "unexpected probability threshold operation in expression");
}

}  // namespace

void ResolveConstants(const std::vector<ParsedConstant>& constants,
                      std::map<std::string, TypedValue>* constant_values,
                      std::vector<std::string>* errors) {
  CHECK(constant_values);
  CHECK(errors);
  std::map<std::string, ConstantInfo> all_constants;
  for (const auto& c : constants) {
    all_constants.insert({c.name(), ConstantInfo(c.type(), c.init())});
    auto i = constant_values->find(c.name());
    if (i != constant_values->end() && i->second.type() != c.type() &&
        !(i->second.type() == Type::INT && c.type() == Type::DOUBLE)) {
      errors->push_back(StrCat("type mismatch for constant ", c.name(),
                               "; expecting value of type ", c.type(),
                               "; found ", i->second.type()));
      i->second = GetDefaultValue(c.type());
    }
  }
  ConstantExpressionEvaluator evaluator(&all_constants, constant_values,
                                        errors);
  for (const auto& c : constants) {
    Identifier(c.name()).Accept(&evaluator);
  }
}

ParsedVariable::ParsedVariable(const std::string& name, Type type,
                               std::unique_ptr<const Expression>&& min,
                               std::unique_ptr<const Expression>&& max,
                               std::unique_ptr<const Expression>&& init)
    : name_(name),
      type_(type),
      min_(std::move(min)),
      max_(std::move(max)),
      init_(std::move(init)) {}

ParsedModule::ParsedModule(const std::string& name) : name_(name) {}

namespace {

// Invalid module index.
const int kNoModule = -1;

}  // namespace

Model::Model() : type_(ModelType::DEFAULT), current_module_(kNoModule) {}

bool Model::SetType(ModelType type) {
  if (type_ == ModelType::DEFAULT) {
    type_ = type;
    return true;
  }
  return false;
}

std::string Model::IdentifierIndexTypeToString(IdentifierIndex::Type type) {
  switch (type) {
    case IdentifierIndex::kConstant:
      return "constant";
    case IdentifierIndex::kVariable:
      return "variable";
    case IdentifierIndex::kAction:
      return "action";
  }
  LOG(FATAL) << "bad identifier index type";
}

bool Model::AddConstant(const std::string& name, Type type,
                        std::unique_ptr<const Expression>&& init,
                        std::vector<std::string>* errors) {
  auto i = identifier_indices_.find(name);
  if (i != identifier_indices_.end()) {
    if (i->second.type == IdentifierIndex::kConstant) {
      errors->push_back(StrCat("duplicate constant ", name));
    } else {
      errors->push_back(StrCat("constant ", name, " previously defined as ",
                               IdentifierIndexTypeToString(i->second.type)));
    }
    return false;
  }
  identifier_indices_.insert(
      {name, {IdentifierIndex::kConstant, constants_.size()}});
  constants_.emplace_back(name, type, std::move(init));
  return true;
}

bool Model::AddIntVariable(const std::string& name,
                           std::unique_ptr<const Expression>&& min,
                           std::unique_ptr<const Expression>&& max,
                           std::unique_ptr<const Expression>&& init,
                           std::vector<std::string>* errors) {
  return AddVariable(name, Type::INT, std::move(min), std::move(max),
                     std::move(init), errors);
}

bool Model::AddBoolVariable(const std::string& name,
                            std::unique_ptr<const Expression>&& init,
                            std::vector<std::string>* errors) {
  return AddVariable(name, Type::BOOL, Literal::New(false), Literal::New(true),
                     std::move(init), errors);
}

bool Model::AddVariable(const std::string& name, Type type,
                        std::unique_ptr<const Expression>&& min,
                        std::unique_ptr<const Expression>&& max,
                        std::unique_ptr<const Expression>&& init,
                        std::vector<std::string>* errors) {
  auto i = identifier_indices_.find(name);
  if (i != identifier_indices_.end()) {
    if (i->second.type == IdentifierIndex::kVariable) {
      errors->push_back(StrCat("duplicate variable ", name));
    } else {
      errors->push_back(StrCat("variable ", name, " previously defined as ",
                               IdentifierIndexTypeToString(i->second.type)));
    }
    return false;
  }
  const size_t index = variables_.size();
  identifier_indices_.insert({name, {IdentifierIndex::kVariable, index}});
  variables_.emplace_back(name, type, std::move(min), std::move(max),
                          std::move(init));
  if (current_module_ == kNoModule) {
    global_variables_.insert(index);
  } else {
    modules_[current_module_].add_variable(index);
  }
  return true;
}

bool Model::AddAction(const std::string& name,
                      std::vector<std::string>* errors) {
  if (!name.empty()) {
    auto result = identifier_indices_.insert(
        {name, {IdentifierIndex::kAction, actions_.size()}});
    if (!result.second &&
        result.first->second.type != IdentifierIndex::kAction) {
      errors->push_back(
          StrCat("action ", name, " previously defined as ",
                 IdentifierIndexTypeToString(result.first->second.type)));
      return false;
    }
    if (result.second) {
      actions_.push_back(name);
    }
  }
  return true;
}

bool Model::StartModule(const std::string& name) {
  CHECK_EQ(current_module_, kNoModule);
  current_module_ = modules_.size();
  if (!module_indices_.emplace(name, current_module_).second) {
    return false;
  }
  modules_.emplace_back(name);
  return true;
}

bool Model::AddCommand(Command&& command, std::vector<std::string>* errors) {
  CHECK_NE(current_module_, kNoModule);
  if (!AddAction(command.action(), errors)) {
    return false;
  }
  for (const auto& outcome : command.outcomes()) {
    for (const auto& update : outcome.updates()) {
      auto i = identifier_indices_.find(update.variable());
      if (i == identifier_indices_.end()) {
        errors->push_back(StrCat("command is updating undefined variable ",
                                 update.variable()));
        return false;
      } else if (i->second.type != IdentifierIndex::kVariable) {
        errors->push_back(StrCat("command is updating ",
                                 IdentifierIndexTypeToString(i->second.type),
                                 " ", update.variable()));
        return false;
      } else if (!command.action().empty() &&
                 global_variables_.find(i->second.index) !=
                     global_variables_.end()) {
        errors->push_back(StrCat("command with action ", command.action(),
                                 " is updating global variable ",
                                 update.variable()));
        return false;
      } else if (modules_[current_module_].variables().find(i->second.index) ==
                     modules_[current_module_].variables().end() &&
                 global_variables_.find(i->second.index) ==
                     global_variables_.end()) {
        errors->push_back(StrCat("command is updating variable ",
                                 update.variable(),
                                 " that belongs to a different module"));
        return false;
      }
    }
  }
  modules_[current_module_].add_command(std::move(command));
  return true;
}

namespace {

std::string RewriteIdentifier(
    const std::string& name,
    const std::map<std::string, std::string>& substitutions) {
  auto i = substitutions.find(name);
  return (i == substitutions.end()) ? name : i->second;
}

class ExpressionRewriter : public ExpressionVisitor {
 public:
  explicit ExpressionRewriter(
      const std::map<std::string, std::string>* substitutions);

  std::unique_ptr<const Expression> release_expr() { return std::move(expr_); }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;

  std::unique_ptr<const Expression> expr_;
  const std::map<std::string, std::string>* substitutions_;
};

ExpressionRewriter::ExpressionRewriter(
    const std::map<std::string, std::string>* substitutions)
    : substitutions_(substitutions) {}

void ExpressionRewriter::DoVisitLiteral(const Literal& expr) {
  expr_ = Literal::New(expr.value());
}

void ExpressionRewriter::DoVisitIdentifier(const Identifier& expr) {
  expr_ = Identifier::New(RewriteIdentifier(expr.name(), *substitutions_));
}

void ExpressionRewriter::DoVisitFunctionCall(const FunctionCall& expr) {
  UniquePtrVector<const Expression> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(release_expr());
  }
  expr_ = FunctionCall::New(expr.function(), std::move(arguments));
}

void ExpressionRewriter::DoVisitUnaryOperation(const UnaryOperation& expr) {
  expr.operand().Accept(this);
  expr_ = UnaryOperation::New(expr.op(), release_expr());
}

void ExpressionRewriter::DoVisitBinaryOperation(const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  auto operand1 = release_expr();
  expr.operand2().Accept(this);
  expr_ = BinaryOperation::New(expr.op(), std::move(operand1), release_expr());
}

void ExpressionRewriter::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  auto condition = release_expr();
  expr.if_branch().Accept(this);
  auto if_branch = release_expr();
  expr.else_branch().Accept(this);
  expr_ = Conditional::New(std::move(condition), std::move(if_branch),
                           release_expr());
}

void ExpressionRewriter::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {}

std::unique_ptr<const Expression> RewriteExpression(
    const Expression& expr,
    const std::map<std::string, std::string>& substitutions) {
  ExpressionRewriter rewriter(&substitutions);
  expr.Accept(&rewriter);
  return rewriter.release_expr();
}

class DistributionRewriter : public DistributionVisitor {
 public:
  explicit DistributionRewriter(
      const std::map<std::string, std::string>* substitutions);

  std::unique_ptr<const Distribution> release_dist() {
    return std::move(dist_);
  }

 private:
  void DoVisitMemoryless(const Memoryless& dist) override;
  void DoVisitWeibull(const Weibull& dist) override;
  void DoVisitLognormal(const Lognormal& dist) override;
  void DoVisitUniform(const Uniform& dist) override;

  std::unique_ptr<const Distribution> dist_;
  const std::map<std::string, std::string>* substitutions_;
};

DistributionRewriter::DistributionRewriter(
    const std::map<std::string, std::string>* substitutions)
    : substitutions_(substitutions) {}

void DistributionRewriter::DoVisitMemoryless(const Memoryless& dist) {
  dist_ = Memoryless::New(RewriteExpression(dist.weight(), *substitutions_));
}

void DistributionRewriter::DoVisitWeibull(const Weibull& dist) {
  dist_ = Weibull::New(RewriteExpression(dist.scale(), *substitutions_),
                       RewriteExpression(dist.shape(), *substitutions_));
}

void DistributionRewriter::DoVisitLognormal(const Lognormal& dist) {
  dist_ = Lognormal::New(RewriteExpression(dist.scale(), *substitutions_),
                         RewriteExpression(dist.shape(), *substitutions_));
}

void DistributionRewriter::DoVisitUniform(const Uniform& dist) {
  dist_ = Uniform::New(RewriteExpression(dist.low(), *substitutions_),
                       RewriteExpression(dist.high(), *substitutions_));
}

std::unique_ptr<const Distribution> RewriteDistribution(
    const Distribution& dist,
    const std::map<std::string, std::string>& substitutions) {
  DistributionRewriter rewriter(&substitutions);
  dist.Accept(&rewriter);
  return rewriter.release_dist();
}

std::vector<Update> RewriteUpdates(
    const std::vector<Update>& updates,
    const std::map<std::string, std::string>& substitutions) {
  std::vector<Update> new_updates;
  for (const Update& update : updates) {
    new_updates.emplace_back(
        RewriteIdentifier(update.variable(), substitutions),
        RewriteExpression(update.expr(), substitutions));
  }
  return std::move(new_updates);
}

std::vector<Outcome> RewriteOutcomes(
    const std::vector<Outcome>& outcomes,
    const std::map<std::string, std::string>& substitutions) {
  std::vector<Outcome> new_outcomes;
  for (const Outcome& outcome : outcomes) {
    new_outcomes.emplace_back(
        RewriteDistribution(outcome.delay(), substitutions),
        RewriteUpdates(outcome.updates(), substitutions));
  }
  return std::move(new_outcomes);
}

Command RewriteCommand(
    const Command& command,
    const std::map<std::string, std::string>& substitutions) {
  return Command(RewriteIdentifier(command.action(), substitutions),
                 RewriteExpression(command.guard(), substitutions),
                 RewriteOutcomes(command.outcomes(), substitutions));
}

}  // namespace

bool Model::AddFromModule(
    const std::string& from_name,
    const std::map<std::string, std::string>& substitutions,
    std::vector<std::string>* errors) {
  CHECK_NE(current_module_, kNoModule);
  auto i = module_indices_.find(from_name);
  if (i == module_indices_.end()) {
    errors->push_back(StrCat("undefined module ", from_name));
    return false;
  }
  const ParsedModule& from_module = modules_[i->second];
  // Add renamed module variables, maintaining the same order as in the source
  // module.
  for (int from_variable_index : from_module.variables()) {
    const auto& from_variable = variables_[from_variable_index];
    auto j = substitutions.find(from_variable.name());
    if (j == substitutions.end()) {
      errors->push_back(
          StrCat("missing substitution for variable ", from_variable.name()));
      return false;
    }
    const std::string& to_variable_name = j->second;
    if (!AddVariable(
            to_variable_name, from_variable.type(),
            RewriteExpression(from_variable.min(), substitutions),
            RewriteExpression(from_variable.max(), substitutions),
            from_variable.has_explicit_init()
                ? RewriteExpression(from_variable.init(), substitutions)
                : nullptr,
            errors)) {
      return false;
    }
  }
  // Add module commands.
  for (const auto& from_command : from_module.commands()) {
    if (!AddCommand(RewriteCommand(from_command, substitutions), errors)) {
      return false;
    }
  }

  return true;
}

void Model::EndModule() {
  CHECK_NE(current_module_, kNoModule);
  current_module_ = kNoModule;
}

size_t Model::ActionIndex(const std::string& name) const {
  auto i = identifier_indices_.find(name);
  CHECK(i != identifier_indices_.end());
  CHECK(i->second.type == IdentifierIndex::kAction);
  return i->second.index;
}

std::ostream& operator<<(std::ostream& os, const Model& m) {
  os << m.type();
  if (!m.constants().empty()) {
    os << std::endl;
    for (const auto& c : m.constants()) {
      os << std::endl << "const " << c.type() << " " << c.name();
      if (c.init() != nullptr) {
        os << " = " << *c.init();
      }
      os << ";";
    }
  }
  if (!m.global_variables().empty()) {
    os << std::endl;
    for (int variable_index : m.global_variables()) {
      const ParsedVariable& v = m.variables()[variable_index];
      os << std::endl << "global " << v.name() << " : ";
      if (v.type() == Type::INT) {
        os << "[" << v.min() << ".." << v.max() << "]";
      } else {
        os << v.type();
      }
      if (v.has_explicit_init()) {
        os << " init " << v.init();
      }
      os << ';';
    }
  }
  for (const auto& module : m.modules()) {
    os << std::endl << std::endl << "module " << module.name();
    for (int variable_index : module.variables()) {
      const ParsedVariable& v = m.variables()[variable_index];
      os << std::endl << "  " << v.name() << " : ";
      if (v.type() == Type::INT) {
        os << "[" << v.min() << ".." << v.max() << "]";
      } else {
        os << v.type();
      }
      if (v.has_explicit_init()) {
        os << " init " << v.init();
      }
      os << ';';
    }
    if (!module.variables().empty() && !module.commands().empty()) {
      os << std::endl;
    }
    for (const auto& command : module.commands()) {
      os << std::endl << "  " << command << ';';
    }
    os << std::endl << "endmodule";
  }
  return os;
}
