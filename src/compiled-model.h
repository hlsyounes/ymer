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
                  const std::vector<CompiledUpdate>& updates);

  // Returns the delay distribution for this outcome.
  const CompiledDistribution& delay() const { return delay_; }

  // Returns the updates for this outcome.
  const std::vector<CompiledUpdate>& updates() const { return updates_; }

 private:
  CompiledDistribution delay_;
  std::vector<CompiledUpdate> updates_;
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

// A compiled model.
class CompiledModel {
 public:
  // Adds a variable to this compiled model.
  void AddVariable(const std::string& name, int min_value, int max_value,
                   int init_value);

  // Adds a command to this compiled model.
  void AddCommand(const CompiledCommand& command);

  // Returns the variables for this compiled model.
  const std::vector<StateVariableInfo>& variables() const { return variables_; }

  // Returns the initial values for the variables of this compiled model.
  const std::vector<int>& init_values() const { return init_values_; }

  // Returns the commands for this compiled model.
  const std::vector<CompiledCommand>& commands() const { return commands_; }

  // Returns the number of integer and double registers referenced by this
  // compiled model.
  std::pair<int, int> GetNumRegisters() const;

  // Returns the number of bits needed to represent the variables of this
  // compiled model.
  int NumBits() const;

 private:
  std::vector<StateVariableInfo> variables_;
  std::vector<int> init_values_;
  std::vector<CompiledCommand> commands_;
};

#endif  // COMPILED_MODEL_H_
