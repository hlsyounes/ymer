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
  CompiledUpdate(int variable, const CompiledExpression& expr);

  // Returns the variable index for this update.
  int variable() const { return variable_; }

  // Returns the expression for this update.
  const CompiledExpression& expr() const { return expr_; }

 private:
  int variable_;
  CompiledExpression expr_;
};

// A compiled outcome.
class CompiledOutcome {
 public:
  // Constructs a compiled outcome with the given delay distribution and
  // updates.
  CompiledOutcome(const CompiledDistribution& delay,
                  const std::vector<CompiledUpdate>& updates, int first_index);

  // Returns the delay distribution for this outcome.
  const CompiledDistribution& delay() const { return delay_; }

  // Returns the updates for this outcome.
  const std::vector<CompiledUpdate>& updates() const { return updates_; }

  // Returns the first index for the range of composite outcomes rooted at this
  // outcome.  Relevant only if the delay distribution is not memoryless.  The
  // index is used to access the stored trigger time of an enabled event during
  // model simulation.
  int first_index() const { return first_index_; }

 private:
  CompiledDistribution delay_;
  std::vector<CompiledUpdate> updates_;
  int first_index_;
};

// A compiled command.
class CompiledCommand {
 public:
  // Constructs a compiled command with the given guard expression and outcomes.
  CompiledCommand(const CompiledExpression& guard,
                  const std::vector<CompiledOutcome>& outcomes);

  // Returns the guard expression for this command.
  const CompiledExpression& guard() const { return guard_; }

  // Returns the outcomes for this command.
  const std::vector<CompiledOutcome>& outcomes() const { return outcomes_; }

 private:
  CompiledExpression guard_;
  std::vector<CompiledOutcome> outcomes_;
};

// A compiled outcome for a Markov command.
class CompiledMarkovOutcome {
 public:
  // Constructs a compiled outcome for a Markov command with the given weight
  // and updates.
  CompiledMarkovOutcome(const CompiledExpression& weight,
                        const std::vector<CompiledUpdate>& updates);

  // Returns the weight for this outcome.
  const CompiledExpression& weight() const { return weight_; }

  // Returns the updates for this outcome.
  const std::vector<CompiledUpdate>& updates() const { return updates_; }

 private:
  CompiledExpression weight_;
  std::vector<CompiledUpdate> updates_;
};

// A compiled Markov command.
class CompiledMarkovCommand {
 public:
  // Constructs a compiled Markov command with the given guard and outcomes.
  CompiledMarkovCommand(const CompiledExpression& guard,
                        const std::vector<CompiledMarkovOutcome>& outcomes);

  // Returns the guard for this command.
  const CompiledExpression& guard() const { return guard_; }

  // Returns the outcomes for this command.
  const std::vector<CompiledMarkovOutcome>& outcomes() const {
    return outcomes_;
  }

 private:
  CompiledExpression guard_;
  std::vector<CompiledMarkovOutcome> outcomes_;
};

// A compiled GSMP command.
class CompiledGsmpCommand {
 public:
  // Constructs a compiled GSMP command with the given guard, delay
  // distribution, and updates.
  CompiledGsmpCommand(const CompiledExpression& guard,
                      const CompiledDistribution& delay,
                      const std::vector<CompiledUpdate>& updates,
                      int first_index);

  // Returns the guard for this command.
  const CompiledExpression& guard() const { return guard_; }

  // Returns the delay distribution for this command.
  const CompiledDistribution& delay() const { return delay_; }

  // Returns the updates for this command.
  const std::vector<CompiledUpdate>& updates() const { return updates_; }

  // Returns the first index for the range of composite outcomes rooted at this
  // compiled GSMP command.  The index is used to access the stored trigger time
  // of an enabled event during model simulation.
  int first_index() const { return first_index_; }

 private:
  CompiledExpression guard_;
  CompiledDistribution delay_;
  std::vector<CompiledUpdate> updates_;
  int first_index_;
};

struct FactoredGsmpCommands {
  std::vector<CompiledGsmpCommand> gsmp_commands;
  std::vector<int> offsets;
};

// Supported compiled model types.
enum class CompiledModelType { DTMC, CTMC, GSMP };

// A compiled model.
class CompiledModel {
 public:
  // Adds a variable to this compiled model.
  void AddVariable(const std::string& name, int min_value, int max_value,
                   int init_value);

  // Adds a command to this compiled model.
  void AddCommand(const CompiledCommand& command);

  // Sets the total number of events for which we may need to store a trigger
  // time during model simulation.
  void SetTriggerTimeCount(int trigger_time_count);

  // Returns the type of this compiled model.
  CompiledModelType model_type() const { return model_type_; }

  // Returns the variables for this compiled model.
  const std::vector<StateVariableInfo>& variables() const { return variables_; }

  // Returns the initial values for the variables of this compiled model.
  const std::vector<int>& init_values() const { return init_values_; }

  // Returns the commands for this compiled model.
  const std::vector<CompiledCommand>& commands() const { return commands_; }

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
  const std::vector<FactoredGsmpCommands>& factored_gsmp_commands() const {
    return factored_gsmp_commands_;
  }

  // Returns the total number of events for which we may need to store a trigger
  // time during model simulation.
  int trigger_time_count() const { return trigger_time_count_; }

  // Returns the number of integer and double registers referenced by this
  // compiled model.
  std::pair<int, int> GetRegisterCounts() const;

  // Returns the number of bits needed to represent the variables of this
  // compiled model.
  int BitCount() const;

 private:
  CompiledModelType model_type_;
  std::vector<StateVariableInfo> variables_;
  std::vector<int> init_values_;
  std::vector<CompiledCommand> commands_;
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
  std::vector<FactoredGsmpCommands> factored_gsmp_commands_;
  int trigger_time_count_;
};

#endif  // COMPILED_MODEL_H_
