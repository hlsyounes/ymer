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

#include "command.h"

#include <memory>
#include <vector>

#include "glog/logging.h"

#include "expression.h"

template <typename IdType>
Update<IdType>::Update(const IdType& variable,
                       std::unique_ptr<const UpdateExpression>&& expr)
    : variable_(variable), expr_(std::move(expr)) {
  CHECK(expr_);
}

template <typename IdType>
Update<IdType>::Update(Update<IdType>&& update)
    : variable_(std::move(update.variable_)), expr_(std::move(update.expr_)) {
}

template <typename IdType>
Update<IdType>& Update<IdType>::operator=(Update<IdType>&& update) {
  variable_ = std::move(update.variable_);
  expr_ = std::move(update.expr_);
  return *this;
}

template class Update<std::string>;
template class Update<int>;

template <typename IdType>
Outcome<IdType>::Outcome(
    std::unique_ptr<const ProbabilityExpression>&& probability,
    std::vector<Update<IdType> >&& updates)
    : probability_(std::move(probability)), updates_(std::move(updates)) {
  CHECK(probability_);
}

template <typename IdType>
Outcome<IdType>::Outcome(Outcome<IdType>&& outcome)
    : probability_(std::move(outcome.probability_)),
      updates_(std::move(outcome.updates_)) {
}

template <typename IdType>
Outcome<IdType>& Outcome<IdType>::operator=(Outcome<IdType>&& outcome) {
  probability_ = std::move(outcome.probability_);
  updates_ = std::move(outcome.updates_);
  return *this;
}

template class Outcome<std::string>;
template class Outcome<int>;

template <typename IdType>
Command<IdType>::Command(const Optional<IdType>& action,
                         std::unique_ptr<const GuardExpression>&& guard,
                         std::vector<Outcome<IdType> >&& outcomes)
    : action_(action),
      guard_(std::move(guard)),
      outcomes_(std::move(outcomes)) {
  CHECK(guard_);
}

template <typename IdType>
Command<IdType>::Command(Command<IdType>&& command)
    : action_(std::move(command.action_)), guard_(std::move(command.guard_)),
      outcomes_(std::move(command.outcomes_)) {
}

template <typename IdType>
Command<IdType>& Command<IdType>::operator=(Command<IdType>&& command) {
  action_ = std::move(command.action_);
  guard_ = std::move(command.guard_);
  outcomes_ = std::move(command.outcomes_);
  return *this;
}

template class Command<std::string>;
template class Command<int>;
