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
// A parsed model.

#ifndef MODELS_H_
#define MODELS_H_

#include <set>
#include <string>
#include <vector>

#include "modules.h"
#include "odd.h"
#include "src/ddutil.h"

struct PHData;

// A parsed variable.
class ParsedVariable {
 public:
  // Constructs a parsed variable with the given name, value range and initial
  // value.
  ParsedVariable(
      const std::string& name, int min_value, int max_value, int init_range);

  // Returns the name for this parsed variable.
  const std::string& name() const { return name_; }

  // Returns the minimum value for this parsed variable.
  int min_value() const { return min_value_; }

  // Returns the maximum value for this parsed variable.
  int max_value() const { return max_value_; }

  // Returns the initial value for this parsed variable.
  int init_value() const { return init_value_; }

 private:
  std::string name_;
  int min_value_;
  int max_value_;
  int init_value_;
};

// A parsed model.
class Model {
 public:
  /* Constructs a model. */
  Model();

  /* Deletes this model. */
  ~Model();

  // Adds an int variable with the given name.
  void AddIntVariable(
      const std::string& name, int min_value, int max_value, int init_value);

  // Opens a new module scope.  Requires that no module scope is currently open.
  void OpenModuleScope();

  // Closes the current module scope.  Requires that a module scope is
  // currently open.
  void CloseModuleScope();

  /* Adds a module to this model. */
  void add_module(const Module& module);

  /* Compiles the commands of this model. */
  void compile();

  // Returns the variables for this model.
  const std::vector<ParsedVariable>& variables() const { return variables_; }

  // Returns the global variables (just indices) for this model.
  const std::set<int>& global_variables() const { return global_variables_; }

  // Returns the module variables (just indices) for the ith module.
  const std::set<int>& module_variables(int i) const {
    return module_variables_[i];
  }

  /* Returns the name of the variable with index i. */
  const std::string& variable_name(int i) const { return variables_[i].name(); }

  /* Returns the modules for this model */
  const std::vector<const Module*>& modules() const { return modules_; }

  /* Returns all commands for this model. */
  const std::vector<const Command*>& commands() const { return commands_; }

  const std::vector<std::set<const Module*>>& command_modules() const {
    return command_modules_;
  }

private:
  std::vector<ParsedVariable> variables_;
  std::set<int> global_variables_;
  std::vector<std::set<int>> module_variables_;
  int current_module_;
  /* The modules for this model */
  std::vector<const Module*> modules_;
  /* Compiled commands for this model. */
  std::vector<const Command*> commands_;
  /* Modules that the above commands are associated with. */
  std::vector<std::set<const Module*>> command_modules_;
};

/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m);

class VariableProperties {
 public:
  VariableProperties(int min_value, int low_bit, int high_bit);

  int min_value() const { return min_value_; }

  int low_bit() const { return low_bit_; }

  int high_bit() const { return high_bit_; }

 private:
  int min_value_;
  int low_bit_;
  int high_bit_;
};

// A model compiled into decision diagrams.
class DecisionDiagramModel {
 public:
  ~DecisionDiagramModel();

  static DecisionDiagramModel Create(const DecisionDiagramManager* manager,
                                     size_t moments, const Model& model);

  const DecisionDiagramManager& manager() const { return *manager_; }

  const ADD& rate_matrix() const { return rate_matrix_; }
  const BDD& reachable_states() const { return reachable_states_; }
  const BDD& initial_state() const { return initial_state_; }
  int initial_state_index() const { return initial_state_index_; }
  ODDNode* odd() const { return odd_; }

 private:
  DecisionDiagramModel(
      const DecisionDiagramManager* manager,
      const ADD& rate_matrix, const BDD& reachable_states,
      const BDD& initial_state, int initial_state_index, ODDNode* odd);

  const DecisionDiagramManager* manager_;
  ADD rate_matrix_;
  BDD reachable_states_;
  BDD initial_state_;
  int initial_state_index_;
  ODDNode* odd_;
};

// Returns the 'current state' MTBDD representation for an expression.
ADD mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Expression& e);

// Returns the 'next state' MTBDD representation for an expression.
ADD primed_mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Expression& e);

// Returns the 'current state' MTBDD representation for a variable.
ADD variable_mtbdd(const DecisionDiagramManager& manager,
                   int low, int low_bit, int high_bit);

// Returns the 'next state' MTBDD representation for a variable.
ADD variable_primed_mtbdd(const DecisionDiagramManager& manager,
                          int low, int low_bit, int high_bit);

#endif  // MODELS_H_
