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
//
// Classes for representing module commands.

#ifndef COMMAND_H_
#define COMMAND_H_

#include <memory>
#include <vector>

#include "expression.h"
#include "optional.h"

template <typename IdType> class CommandValueTypeInfo;

template <>
class CommandValueTypeInfo<std::string> {
 public:
  using GuardValueType = TypedValue;
  using ProbabilityValueType = TypedValue;
  using UpdateValueType = TypedValue;

  CommandValueTypeInfo() = delete;
};

template <>
class CommandValueTypeInfo<int> {
 public:
  using GuardValueType = double;
  using ProbabilityValueType = double;
  using UpdateValueType = double;

  CommandValueTypeInfo() = delete;
};

template <typename IdType>
using GuardValueType = typename CommandValueTypeInfo<IdType>::GuardValueType;

template <typename IdType>
using ProbabilityValueType =
    typename CommandValueTypeInfo<IdType>::ProbabilityValueType;

template <typename IdType>
using UpdateValueType = typename CommandValueTypeInfo<IdType>::UpdateValueType;

// A variable update for a module command.
template <typename IdType>
class Update {
 public:
  using UpdateExpression = Expression<IdType, UpdateValueType<IdType> >;

  // Constructs an update for the given variable and with the given
  // expression.
  Update(const IdType& variable,
         std::unique_ptr<const UpdateExpression>&& expr);
  Update(Update<IdType>&& update);

  // Disallow copy and assign.
  Update(const Update<IdType>&) = delete;
  Update& operator=(const Update<IdType>&) = delete;

  // Move assignment.
  Update<IdType>& operator=(Update<IdType>&& update);

  // Returns the variable for this update.
  const IdType& variable() const { return variable_; }

  // Returns the expression for this update.
  const UpdateExpression& expr() const { return *expr_; }

 private:
  // The variable for this update.
  IdType variable_;
  // The expression for this update.
  std::unique_ptr<const UpdateExpression> expr_;
};

using ParsedUpdate = Update<std::string>;
using CompiledUpdate = Update<int>;

// A possible outcome of a module command, consisting of a probability and a
// list of variable updates.
template <typename IdType>
class Outcome {
 public:
  using ProbabilityExpression =
      Expression<IdType, ProbabilityValueType<IdType> >;

  // Constructs an outcome.
  Outcome(std::unique_ptr<const ProbabilityExpression>&& probability,
          std::vector<Update<IdType> >&& updates);
  Outcome(Outcome<IdType>&& outcome);

  // Disallow copy and assign.
  Outcome(const Outcome<IdType>&) = delete;
  Outcome<IdType>& operator=(const Outcome<IdType>&) = delete;

  // Move assignment.
  Outcome<IdType>& operator=(Outcome<IdType>&& outcome);

  // Returns the probability for this outcome.
  const ProbabilityExpression& probability() const { return *probability_; }

  // Returns the list of updates for this outcome.
  const std::vector<Update<IdType> >& updates() const {
    return updates_;
  }

 private:
  // The probability for this outcome.
  std::unique_ptr<const ProbabilityExpression> probability_;
  // The list of updates for this outcome.
  std::vector<Update<IdType> > updates_;
};

using ParsedOutcome = Outcome<std::string>;
using CompiledOutcome = Outcome<int>;

// A module command.
template <typename IdType>
class Command {
 public:
  using GuardExpression = Expression<IdType, GuardValueType<IdType> >;

  // Constructs a command with the given action, guard, and outcomes.  The
  // action is optional and may be kNoId.
  Command(const Optional<IdType>& action,
          std::unique_ptr<const GuardExpression>&& guard,
          std::vector<Outcome<IdType> >&& outcomes);
  Command(Command<IdType>&& command);

  // Disallow copy and assign.
  Command(const Command<IdType>&) = delete;
  Command<IdType>& operator=(const Command<IdType>&) = delete;

  // Move assignment.
  Command<IdType>& operator=(Command<IdType>&& command);

  // Returns the action for this command; kNoId if no action is associated with
  // this command.
  const Optional<IdType>& action() const {
    return action_;
  }

  // Returns the guard for this command.
  const GuardExpression& guard() const { return *guard_; }

  // Returns the outcomes for this command.
  const std::vector<Outcome<IdType> >& outcomes() const { return outcomes_; }

 private:
  // The action for this command; kNoId if no action is associated with this
  // command.
  Optional<IdType> action_;
  // The guard for this command.
  std::unique_ptr<const GuardExpression> guard_;
  // The outcomes for this command.
  std::vector<Outcome<IdType> > outcomes_;
};

using ParsedCommand = Command<std::string>;
using CompiledCommand = Command<int>;

#endif  // COMMAND_H_
