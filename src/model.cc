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
  return os << update.variable() << "' = " << update.expr();
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
