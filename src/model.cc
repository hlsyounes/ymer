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

#include "model.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"

#include "expression.h"
#include "process-algebra.h"
#include "type.h"

Range::Range(std::unique_ptr<const Expression>&& min,
             std::unique_ptr<const Expression>&& max)
    : min_(std::move(min)), max_(std::move(max)) {
  CHECK(min_);
  CHECK(max_);
}

Range::Range(Range&& range)
    : min_(std::move(range.min_)), max_(std::move(range.max_)) {
}

Update::Update(const std::string& variable,
               std::unique_ptr<const Expression>&& expr)
    : variable_(variable), expr_(std::move(expr)) {
  CHECK(expr_);
}

Update::Update(Update&& update)
    : variable_(std::move(update.variable_)), expr_(std::move(update.expr_)) {
}

Update& Update::operator=(Update&& update) {
  variable_ = std::move(update.variable_);
  expr_ = std::move(update.expr_);
  return *this;
}

Outcome::Outcome(std::unique_ptr<const Expression>&& probability,
                 std::vector<Update>&& updates)
    : probability_(std::move(probability)), updates_(std::move(updates)) {
  CHECK(probability_);
}

Outcome::Outcome(Outcome&& outcome)
    : probability_(std::move(outcome.probability_)),
      updates_(std::move(outcome.updates_)) {
}

Outcome& Outcome::operator=(Outcome&& outcome) {
  probability_ = std::move(outcome.probability_);
  updates_ = std::move(outcome.updates_);
  return *this;
}

Command::Command(std::unique_ptr<const std::string>&& action,
                 std::unique_ptr<const Expression>&& guard,
                 std::vector<Outcome>&& outcomes)
    : action_(std::move(action)), guard_(std::move(guard)),
      outcomes_(std::move(outcomes)) {
  CHECK(guard_);
}

Command::Command(Command&& command)
    : action_(std::move(command.action_)), guard_(std::move(command.guard_)),
      outcomes_(std::move(command.outcomes_)) {
}

Command& Command::operator=(Command&& command) {
  action_ = std::move(command.action_);
  guard_ = std::move(command.guard_);
  outcomes_ = std::move(command.outcomes_);
  return *this;
}

StateReward::StateReward(std::unique_ptr<const Expression>&& guard,
                         std::unique_ptr<const Expression>&& reward)
    : guard_(std::move(guard)), reward_(std::move(reward)) {
  CHECK(guard_);
  CHECK(reward_);
}

StateReward::StateReward(StateReward&& state_reward)
    : guard_(std::move(state_reward.guard_)),
      reward_(std::move(state_reward.reward_)) {
}

StateReward& StateReward::operator=(StateReward&& state_reward) {
  guard_ = std::move(state_reward.guard_);
  reward_ = std::move(state_reward.reward_);
  return *this;
}

TransitionReward::TransitionReward(std::unique_ptr<const std::string>&& action,
                                   std::unique_ptr<const Expression>&& guard,
                                   std::unique_ptr<const Expression>&& reward)
    : action_(std::move(action)),
      guard_(std::move(guard)),
      reward_(std::move(reward)) {
}

TransitionReward::TransitionReward(TransitionReward&& transition_reward)
    : action_(std::move(transition_reward.action_)),
      guard_(std::move(transition_reward.guard_)),
      reward_(std::move(transition_reward.reward_)) {
}

TransitionReward& TransitionReward::operator=(
    TransitionReward&& transition_reward) {
  action_ = std::move(transition_reward.action_);
  guard_ = std::move(transition_reward.guard_);
  reward_ = std::move(transition_reward.reward_);
  return *this;
}

namespace {

// Invalid module index.
const int kNoModule = -1;
// Invalid rewards index.
const int kNoRewards = -1;

}  // namespace

Model::Model()
    : type_(ModelType::NONE),
      current_module_(kNoModule), current_rewards_(kNoRewards) {
}

Model::~Model() = default;

bool Model::SetType(ModelType type) {
  CHECK(type != ModelType::NONE);
  if (type_ != ModelType::NONE) {
    return false;
  }
  type_ = type;
  return true;
}

namespace {

// Each identifier is assigned a unique integer ID.  There are three different
// kinds of identifiers.  Each kind of identifier is assigned an index in
// [0,N_kind), where N_kind is the number of identifier of the given kind.  An
// index is mapped to a unique ID as follows:
//
//   id = index * num_kinds + kind_offset
//
// where kind_offset is in [0,num_kinds).

// The offsets for the different kinds of identifiers.
enum {
  kConstantOffset = 0,
  kVariableOffset = 1,
  kFormulaOffset = 2,
  kActionOffset = 3,
  kNumIdentifierOffsets
};

// Returns the offset for the given unique ID.
int IdentifierOffset(int id) {
  return id % kNumIdentifierOffsets;
}

// Returns the index for the given unique ID.
int IdentifierIndex(int id) {
  return id / kNumIdentifierOffsets;
}

// Returns the unique ID for the given index and offset.
int IdentifierUniqueId(int index, int offset) {
  return index * kNumIdentifierOffsets + offset;
}

}  // namespace

bool Model::AddConstant(const std::string& name, Type type,
                        std::unique_ptr<const Expression>&& init) {
  const int index = constant_types_.size();
  const int id = IdentifierUniqueId(index, kConstantOffset);
  if (!identifiers_.insert({ name, id }).second) {
    return false;
  }
  constant_types_.push_back(type);
  if (init) {
    constant_inits_.insert(std::make_pair(index, std::move(init)));
  }
  return true;
}

const Expression* Model::GetConstantInit(int i) const {
  auto j = constant_inits_.find(i);
  return (j != constant_inits_.end()) ? j->second.get() : nullptr;
}

const std::string& Model::GetConstantName(int i) const {
  const int id = IdentifierUniqueId(i, kConstantOffset);
  for (const std::map<std::string, int>::value_type& j: identifiers_) {
    if (j.second == id) {
      return j.first;
    }
  }
  LOG(FATAL) << "invalid constant index " << i;
}

bool Model::AddVariable(const std::string& name, Type type, Range* range,
                        std::unique_ptr<const Expression>&& init) {
  const int index = variable_types_.size();
  const int id = IdentifierUniqueId(index, kVariableOffset);
  if (!identifiers_.insert({ name, id }).second) {
    return false;
  }
  variable_types_.push_back(type);
  if (range) {
    CHECK(type == Type::INT);
    variable_ranges_.insert(std::make_pair(index, std::move(*range)));
  } else {
    CHECK(type == Type::BOOL);
  }
  if (init) {
    variable_inits_.insert(std::make_pair(index, std::move(init)));
  }
  if (current_module_ != kNoModule) {
    module_variables_[current_module_].insert(index);
  }
  return true;
}

bool Model::AddIntVariable(const std::string& name, Range&& range,
                           std::unique_ptr<const Expression>&& init) {
  return AddVariable(name, Type::INT, &range, std::move(init));
}

bool Model::AddBoolVariable(const std::string& name,
                            std::unique_ptr<const Expression>&& init) {
  return AddVariable(name, Type::BOOL, nullptr /* range */, std::move(init));
}

bool Model::GetVariableIndex(const std::string& name, int* index) const {
  CHECK(index);
  auto i = identifiers_.find(name);
  if (i != identifiers_.end() &&
      IdentifierOffset(i->second) == kVariableOffset) {
    *index = IdentifierIndex(i->second);
    return true;
  } else {
    return false;
  }
}

const Range* Model::GetVariableRange(int i) const {
  auto j = variable_ranges_.find(i);
  return (j != variable_ranges_.end()) ? &j->second : nullptr;
}

const Expression* Model::GetVariableInit(int i) const {
  auto j = variable_inits_.find(i);
  return (j != variable_inits_.end()) ? j->second.get() : nullptr;
}

const std::string& Model::GetVariableName(int i) const {
  const int id = IdentifierUniqueId(i, kVariableOffset);
  for (const std::map<std::string, int>::value_type& j: identifiers_) {
    if (j.second == id) {
      return j.first;
    }
  }
  LOG(FATAL) << "invalid variable index " << i;
}

bool Model::AddFormula(const std::string& name,
                       std::unique_ptr<const Expression>&& expr) {
  CHECK(expr);
  const int id = IdentifierUniqueId(formula_exprs_.size(), kFormulaOffset);
  if (!identifiers_.insert({ name, id }).second) {
    return false;
  }
  formula_exprs_.push_back(std::move(expr));
  return true;
}

const std::string& Model::GetFormulaName(int i) const {
  const int id = IdentifierUniqueId(i, kFormulaOffset);
  for (const std::map<std::string, int>::value_type& j: identifiers_) {
    if (j.second == id) {
      return j.first;
    }
  }
  LOG(FATAL) << "invalid formula index " << i;
}

bool Model::AddAction(const std::string* name) {
  if (name) {
    // We reuse the same index for all actions, since we just need to ensure
    // that actions do not clash with other identifier kinds.
    const int id = IdentifierUniqueId(0, kActionOffset);
    auto result = identifiers_.insert({ *name, id });
    return result.second
        || IdentifierOffset(result.first->second) == kActionOffset;
  }
  return true;
}

bool Model::StartModule(const std::string& name) {
  CHECK_EQ(current_module_, kNoModule);
  current_module_ = modules_.size();
  if (!modules_.insert({ name, current_module_ }).second) {
    return false;
  }
  module_variables_.emplace_back();
  module_commands_.emplace_back();
  return true;
}

bool Model::GetModuleIndex(const std::string& name, int* index) const {
  CHECK(index);
  auto i = modules_.find(name);
  if (i != modules_.end()) {
    *index = i->second;
    return true;
  } else {
    return false;
  }
}

const std::string& Model::GetModuleName(int i) const {
  for (const std::map<std::string, int>::value_type& j: modules_) {
    if (j.second == i) {
      return j.first;
    }
  }
  LOG(FATAL) << "invalid module index " << i;
}

bool Model::AddCommand(Command&& command) {
  CHECK_NE(current_module_, kNoModule);
  if (!AddAction(command.action())) {
    return false;
  }
  module_commands_[current_module_].push_back(std::move(command));
  return true;
}

namespace {

// Rewrites the given name subject to the given identifier substitutions.
std::string RewriteName(
    const std::string& name,
    const std::map<std::string, std::string>& substitutions) {
  auto i = substitutions.find(name);
  return (i != substitutions.end()) ? i->second : name;
}

// Expression visitor that rewrites an expression subject to a set of
// identifier substitutions.
class ExpressionRewriter : public ExpressionVisitor {
 public:
  // Constructs an expression rewriter with the given substitutions.
  explicit ExpressionRewriter(
      const std::map<std::string, std::string>* substitutions)
      : substitutions_(substitutions) {
    CHECK(substitutions);
  }

  virtual ~ExpressionRewriter();

  // Releases the latest rewritten expression to the caller.
  std::unique_ptr<const Expression> new_expr() { return std::move(new_expr_); }

  virtual void VisitIntLiteral(const IntLiteral& expr) {
    new_expr_ = IntLiteral::Create(expr.value());
  }

  virtual void VisitDoubleLiteral(const DoubleLiteral& expr) {
    new_expr_ = DoubleLiteral::Create(expr.value());
  }

  virtual void VisitBoolLiteral(const BoolLiteral& expr) {
    new_expr_ = BoolLiteral::Create(expr.value());
  }

  virtual void VisitIdentifier(const Identifier& expr) {
    new_expr_ = Identifier::Create(RewriteName(expr.name(), *substitutions_));
  }

  virtual void VisitFunctionCall(const FunctionCall& expr) {
    ArgumentList new_arguments;
    ExpressionRewriter argument_rewriter(substitutions_);
    for (const Expression& argument: expr.arguments()) {
      argument.Accept(&argument_rewriter);
      new_arguments.push_back(argument_rewriter.new_expr());
    }
    new_expr_ = FunctionCall::Create(expr.function(), std::move(new_arguments));
  }

  virtual void VisitUnaryOperation(const UnaryOperation& expr) {
    ExpressionRewriter operand_rewriter(substitutions_);
    expr.operand().Accept(&operand_rewriter);
    new_expr_ = UnaryOperation::Create(expr.op(), operand_rewriter.new_expr());
  }

  virtual void VisitBinaryOperation(const BinaryOperation& expr) {
    ExpressionRewriter operand1_rewriter(substitutions_);
    expr.operand1().Accept(&operand1_rewriter);
    ExpressionRewriter operand2_rewriter(substitutions_);
    expr.operand2().Accept(&operand2_rewriter);
    new_expr_ = BinaryOperation::Create(expr.op(),
                                        operand1_rewriter.new_expr(),
                                        operand2_rewriter.new_expr());
  }

  virtual void VisitConditional(const Conditional& expr) {
    ExpressionRewriter condition_rewriter(substitutions_);
    expr.condition().Accept(&condition_rewriter);
    ExpressionRewriter if_expr_rewriter(substitutions_);
    expr.if_expr().Accept(&if_expr_rewriter);
    ExpressionRewriter else_expr_rewriter(substitutions_);
    expr.else_expr().Accept(&else_expr_rewriter);
    new_expr_ = Conditional::Create(condition_rewriter.new_expr(),
                                    if_expr_rewriter.new_expr(),
                                    else_expr_rewriter.new_expr());
  }

 private:
  const std::map<std::string, std::string>* substitutions_;
  std::unique_ptr<const Expression> new_expr_;
};

ExpressionRewriter::~ExpressionRewriter() = default;

// Rewrites the given expression subject to the given identifier
// substitutions.
std::unique_ptr<const Expression> RewriteExpression(
    const Expression* expr,
    const std::map<std::string, std::string>& substitutions) {
  if (expr) {
    ExpressionRewriter expr_rewriter(&substitutions);
    expr->Accept(&expr_rewriter);
    return expr_rewriter.new_expr();
  } else {
    return nullptr;
  }
}

// Rewrites the given range subject to the given identifier substitutions.
Range RewriteRange(const Range& range,
                   const std::map<std::string, std::string>& substitutions) {
  ExpressionRewriter min_rewriter(&substitutions);
  range.min().Accept(&min_rewriter);
  ExpressionRewriter max_rewriter(&substitutions);
  range.max().Accept(&max_rewriter);
  return Range(RewriteExpression(&range.min(), substitutions),
               RewriteExpression(&range.max(), substitutions));
}

// Rewrites the given action subject to the given identifier substitutions.
std::unique_ptr<const std::string> RewriteAction(
    const std::string* action,
    const std::map<std::string, std::string>& substitutions) {
  if (action) {
    auto i = substitutions.find(*action);
    return (i != substitutions.end())
        ? std::unique_ptr<const std::string>(new std::string(i->second))
        : nullptr;
  } else {
    return nullptr;
  }
}

// Rewrites the given update subject to the given identifier substitutions.
Update RewriteUpdate(const Update& update,
                     const std::map<std::string, std::string>& substitutions) {
  return Update(RewriteName(update.variable(), substitutions),
                RewriteExpression(&update.expr(), substitutions));
}

// Rewrites the given updates subject to the given identifier substitutions.
std::vector<Update> RewriteUpdates(
    const std::vector<Update>& updates,
    const std::map<std::string, std::string>& substitutions) {
  std::vector<Update> new_updates;
  for (const Update& update: updates) {
    new_updates.push_back(RewriteUpdate(update, substitutions));
  }
  return std::move(new_updates);
}

// Rewrites the given outcome subject to the given identifier substitutions.
Outcome RewriteOutcome(
    const Outcome& outcome,
    const std::map<std::string, std::string>& substitutions) {
  return Outcome(RewriteExpression(&outcome.probability(), substitutions),
                 RewriteUpdates(outcome.updates(), substitutions));
}

// Rewrites the given outcomes subject to the given identifier substitutions.
std::vector<Outcome> RewriteOutcomes(
    const std::vector<Outcome>& outcomes,
    const std::map<std::string, std::string>& substitutions) {
  std::vector<Outcome> new_outcomes;
  for (const Outcome& outcome: outcomes) {
    new_outcomes.push_back(RewriteOutcome(outcome, substitutions));
  }
  return std::move(new_outcomes);
}

// Rewrites the given command subject to the given identifier substitutions.
Command RewriteCommand(
    const Command& command,
    const std::map<std::string, std::string>& substitutions) {
  return Command(RewriteAction(command.action(), substitutions),
                 RewriteExpression(&command.guard(), substitutions),
                 RewriteOutcomes(command.outcomes(), substitutions));
}

}  // namespace

bool Model::AddFromModule(
    const std::string& from_module,
    const std::map<std::string, std::string>& substitutions,
    std::string* error) {
  CHECK_NE(current_module_, kNoModule);

  // Find index of from_module.
  int from_module_index;
  if (!GetModuleIndex(from_module, &from_module_index)) {
    if (error) {
      *error = "undefined module '";
      *error += from_module;
      *error += "'";
    }
    return false;
  }

  // Add renamed module variables, maintaining the same order as in
  // from_module.
  for (int from_index: module_variables(from_module_index)) {
    const std::string& from_name = GetVariableName(from_index);
    auto i = substitutions.find(from_name);
    if (i == substitutions.end()) {
      if (error) {
        *error = "missing substitution for module '";
        *error += from_module;
        *error += "' variable '";
        *error += from_name;
        *error += "'";
      }
      return false;
    }
    const std::string& to_name = i->second;
    if (variable_type(from_index) == Type::INT) {
      if (!AddIntVariable(
              std::string(to_name),
              RewriteRange(*CHECK_NOTNULL(GetVariableRange(from_index)),
                           substitutions),
              RewriteExpression(GetVariableInit(from_index), substitutions))) {
        if (error) {
          *error = "duplicate identifier";
        }
        return false;
      }
    } else {
      if (!AddBoolVariable(std::string(to_name),
                           RewriteExpression(GetVariableInit(from_index),
                                             substitutions))) {
        if (error) {
          *error = "duplicate identifier";
        }
        return false;
      }
    }
  }

  for (const Command& command: module_commands(from_module_index)) {
    if (!AddCommand(RewriteCommand(command, substitutions))) {
      *error = "duplicate identifier";
      return false;
    }
  }

  return true;
}

void Model::EndModule() {
  CHECK_NE(current_module_, kNoModule);
  current_module_ = kNoModule;
}

bool Model::AddLabel(const std::string& label,
                     std::unique_ptr<const Expression>&& expr) {
  CHECK(expr);
  return labels_.insert(std::make_pair(label, std::move(expr))).second;
}

const Expression* Model::GetLabelExpr(const std::string& label) const {
  auto i = labels_.find(label);
  return (i != labels_.end()) ? i->second.get() : nullptr;
}

bool Model::SetInit(std::unique_ptr<const Expression>&& init) {
  CHECK(init);
  if (init_) {
    return false;
  }
  init_ = std::move(init);
  return true;
}

bool Model::StartRewards(const std::string* label) {
  CHECK_EQ(current_rewards_, kNoRewards);
  CHECK_EQ(state_rewards_.size(), transition_rewards_.size());
  current_rewards_ = state_rewards_.size();
  state_rewards_.emplace_back();
  transition_rewards_.emplace_back();
  if (label) {
    // Labeled rewards structure -- return true iff no rewards structure with
    // the given label already exists.
    return labeled_rewards_.insert({ *label, current_rewards_ }).second;
  }
  return true;  // Unlabeled rewards structure -- always return true.
}

void Model::StartUnlabeledRewards() {
  StartRewards(nullptr);
}

bool Model::StartLabeledRewards(const std::string& label) {
  return StartRewards(&label);
}

void Model::AddStateReward(StateReward&& state_reward) {
  CHECK_NE(current_rewards_, kNoRewards);
  state_rewards_[current_rewards_].push_back(std::move(state_reward));
}

void Model::AddTransitionReward(TransitionReward&& transition_reward) {
  CHECK_NE(current_rewards_, kNoRewards);
  AddAction(transition_reward.action());
  transition_rewards_[current_rewards_].push_back(std::move(transition_reward));
}

bool Model::GetLabeledRewardsIndex(const std::string& label, int* index) const {
  CHECK(index);
  auto i = labeled_rewards_.find(label);
  if (i != labeled_rewards_.end()) {
    *index = i->second;
    return true;
  } else {
    return false;
  }
}

void Model::EndRewards() {
  CHECK_NE(current_rewards_, kNoRewards);
  current_rewards_ = kNoRewards;
}

bool Model::SetSystem(std::unique_ptr<const ProcessAlgebra>&& system) {
  CHECK(system);
  if (system_) {
    return false;
  }
  system_ = std::move(system);
  return true;
}
