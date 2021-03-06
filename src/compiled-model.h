// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2012 Google Inc
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
// A compiled model.

#ifndef COMPILED_MODEL_H_
#define COMPILED_MODEL_H_

#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "compiled-distribution.h"
#include "compiled-expression.h"
#include "ddutil.h"

// A compiled update.
class CompiledUpdate {
 public:
  // Constructs a compiled update with the given variable index and expression.
  explicit CompiledUpdate(int variable, const CompiledExpression& expr);

  // Returns the variable index for this update.
  int variable() const { return variable_; }

  // Returns the expression for this update.
  const CompiledExpression& expr() const { return expr_; }

 private:
  int variable_;
  CompiledExpression expr_;
};

// A compiled outcome for a Markov command.
class CompiledMarkovOutcome {
 public:
  // Constructs a compiled outcome for a Markov command with the given
  // probability and updates.
  explicit CompiledMarkovOutcome(const CompiledExpression& probability,
                                 const std::vector<CompiledUpdate>& updates);

  // Returns the probability for this outcome.
  const CompiledExpression& probability() const { return probability_; }

  // Returns the updates for this outcome.
  const std::vector<CompiledUpdate>& updates() const { return updates_; }

 private:
  CompiledExpression probability_;
  std::vector<CompiledUpdate> updates_;
};

// A compiled Markov command.
class CompiledMarkovCommand {
 public:
  // Constructs a compiled Markov command with the given guard and outcomes.
  explicit CompiledMarkovCommand(
      const std::optional<int>& module, const CompiledExpression& guard,
      const CompiledExpression& weight,
      const std::vector<CompiledMarkovOutcome>& outcomes);

  // Returns the module for this command, or none if the compiled command is not
  // associated with a module.
  const std::optional<int>& module() const { return module_; }

  // Returns the guard for this command.
  const CompiledExpression& guard() const { return guard_; }

  // Returns the weight for this command.
  const CompiledExpression& weight() const { return weight_; }

  // Returns the outcomes for this command.
  const std::vector<CompiledMarkovOutcome>& outcomes() const {
    return outcomes_;
  }

 private:
  std::optional<int> module_;
  CompiledExpression guard_;
  CompiledExpression weight_;
  std::vector<CompiledMarkovOutcome> outcomes_;
};

// A compiled GSMP command.
class CompiledGsmpCommand {
 public:
  // Constructs a compiled GSMP command with the given guard, delay
  // distribution, and updates.
  explicit CompiledGsmpCommand(const std::optional<int>& module,
                               const CompiledExpression& guard,
                               const CompiledGsmpDistribution& delay,
                               const std::vector<CompiledUpdate>& updates,
                               int first_index);

  // Returns the module for this command, or none if the compiled command is not
  // associated with a module.
  const std::optional<int>& module() const { return module_; }

  // Returns the guard for this command.
  const CompiledExpression& guard() const { return guard_; }

  // Returns the delay distribution for this command.
  const CompiledGsmpDistribution& delay() const { return delay_; }

  // Returns the updates for this command.
  const std::vector<CompiledUpdate>& updates() const { return updates_; }

  // Returns the first index for the range of composite outcomes rooted at this
  // compiled GSMP command.  The index is used to access the stored trigger time
  // of an enabled event during model simulation.
  int first_index() const { return first_index_; }

 private:
  std::optional<int> module_;
  CompiledExpression guard_;
  CompiledGsmpDistribution delay_;
  std::vector<CompiledUpdate> updates_;
  int first_index_;
};

struct CompiledGsmpCommandFactors {
  std::vector<CompiledGsmpCommand> gsmp_commands;
  std::vector<int> offsets;
};

// Supported compiled model types.
enum class CompiledModelType { DTMC, CTMC, GSMP };

// A compiled model.
class CompiledModel {
 public:
  // Constructs a compiled model with the given type, variables, and initial
  // variable values.
  explicit CompiledModel(CompiledModelType type,
                         const std::vector<StateVariableInfo>& variables,
                         const std::vector<std::set<int>>& module_variables,
                         const std::vector<int>& init_values,
                         const std::optional<CompiledExpression>& init_expr);

  // Sets the single (non-factored) Markov commands for this compiled model.
  void set_single_markov_commands(
      const std::vector<CompiledMarkovCommand>& single_markov_commands) {
    single_markov_commands_ = single_markov_commands;
  }

  // Sets the factored Markov commands for this compiled model.
  void set_factored_markov_commands(
      const std::vector<std::vector<std::vector<CompiledMarkovCommand>>>&
          factored_markov_commands) {
    factored_markov_commands_ = factored_markov_commands;
  }

  // Sets the single (non-factored) GSMP commands for this compiled model.
  void set_single_gsmp_commands(
      const std::vector<CompiledGsmpCommand>& single_gsmp_commands) {
    single_gsmp_commands_ = single_gsmp_commands;
    for (const auto& command : single_gsmp_commands) {
      gsmp_event_count_ =
          std::max(gsmp_event_count_, command.first_index() + 1);
    }
  }

  // Sets the factored GSMP commands for this compiled model.
  void set_factored_gsmp_commands(
      const std::vector<CompiledGsmpCommandFactors>& factored_gsmp_commands) {
    factored_gsmp_commands_ = factored_gsmp_commands;
    for (const auto& factors : factored_gsmp_commands) {
      if (!factors.gsmp_commands.empty()) {
        const int count = factors.offsets[0];
        for (const auto& command : factors.gsmp_commands) {
          gsmp_event_count_ =
              std::max(gsmp_event_count_, command.first_index() + count);
        }
      }
    }
  }

  // Sets the pivoted single (non-factored) Markov commands for this compiled
  // model.
  void set_pivoted_single_markov_commands(
      int pivot_variable, const std::vector<std::vector<CompiledMarkovCommand>>&
                              pivoted_single_markov_commands) {
    pivot_variable_ = pivot_variable;
    pivoted_single_markov_commands_ = pivoted_single_markov_commands;
  }

  // Returns the type of this compiled model.
  CompiledModelType type() const { return type_; }

  // Returns the variables for this compiled model.
  const std::vector<StateVariableInfo>& variables() const { return variables_; }

  // Returns the module variables for this compiled model.
  const std::vector<std::set<int>>& module_variables() const {
    return module_variables_;
  }

  // Returns the initial values for the variables of this compiled model.
  const std::vector<int>& init_values() const { return init_values_; }

  // Returns the optional init expression for this compiled model.
  const std::optional<CompiledExpression>& init_expr() const {
    return init_expr_;
  }

  // Returns the single Markov commands for this compiled model.
  const std::vector<CompiledMarkovCommand>& single_markov_commands() const {
    return single_markov_commands_;
  }

  // Returns the factored Markov commands for this compiled model.
  const std::vector<std::vector<std::vector<CompiledMarkovCommand>>>&
  factored_markov_commands() const {
    return factored_markov_commands_;
  }

  // Returns the single GSMP commands for this compiled model.
  const std::vector<CompiledGsmpCommand>& single_gsmp_commands() const {
    return single_gsmp_commands_;
  }

  // Returns the factored GSMP commands for this compiled model.
  const std::vector<CompiledGsmpCommandFactors>& factored_gsmp_commands()
      const {
    return factored_gsmp_commands_;
  }

  const std::optional<int>& pivot_variable() const { return pivot_variable_; }

  const std::vector<std::vector<CompiledMarkovCommand>>&
  pivoted_single_markov_commands() const {
    return pivoted_single_markov_commands_;
  }

  // Returns the total number of GSMP events for which we may need to store a
  // trigger time during model simulation.
  int gsmp_event_count() const { return gsmp_event_count_; }

  // Returns the total event count for this compiled model.
  int EventCount() const;

  // Returns the number of integer and double registers referenced by this
  // compiled model.
  std::pair<int, int> GetRegisterCounts() const;

 private:
  CompiledModelType type_;
  std::vector<StateVariableInfo> variables_;
  std::vector<std::set<int>> module_variables_;
  std::vector<int> init_values_;
  std::optional<CompiledExpression> init_expr_;
  // The single (non-factored) commands with memoryless distributions.
  std::vector<CompiledMarkovCommand> single_markov_commands_;
  // For every action, and every module where the action occurs, the factored
  // commands with memoryless distributions.
  std::vector<std::vector<std::vector<CompiledMarkovCommand>>>
      factored_markov_commands_;
  // The single (non-factored) commands with general distributions.
  std::vector<CompiledGsmpCommand> single_gsmp_commands_;
  // For every action, the first component of factored commands with general
  // distributions.  Empty if no action has general distributions, otherwise
  // has the same number of elements as factored_markov_commands_ even if some
  // actions do not have any general-distribution commands.  Should be composed
  // with factored_markov_commands_, ignoring the first module, to get the
  // composite commands.
  std::vector<CompiledGsmpCommandFactors> factored_gsmp_commands_;
  std::optional<int> pivot_variable_;
  std::vector<std::vector<CompiledMarkovCommand>>
      pivoted_single_markov_commands_;
  int gsmp_event_count_;
};

#endif  // COMPILED_MODEL_H_
