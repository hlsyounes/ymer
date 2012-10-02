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

  /* Adds a global variable to this model. */
  void add_variable(const Variable& variable);

  /* Adds a module to this model. */
  void add_module(const Module& module);

  /* Compiles the commands of this model. */
  void compile();

  // Returns the variables for this model.
  const std::vector<ParsedVariable>& variables() const { return variables_; }

  /* Returns the global variables for this model. */
  const std::vector<const Variable*>& global_variables() const {
    return global_variables_;
  }

  /* Returns the name of the variable with index i. */
  const std::string& variable_name(int i) const { return variable_names_[i]; }

  /* Returns the modules for this model */
  const ModuleList& modules() const { return modules_; }

  /* Returns all commands for this model. */
  const CommandList& commands() const { return commands_; }

  /* Caches DDs for this model. */
  void cache_dds(const DecisionDiagramManager& dd_man, size_t moments) const;

  /* Returns an MTBDD representing the rate matrix for this model. */
  DdNode* rate_mtbdd(const DecisionDiagramManager& dd_man) const;

  /* Returns a reachability BDD for this model. */
  DdNode* reachability_bdd(const DecisionDiagramManager& dd_man) const;

  /* Returns an ODD for this model. */
  ODDNode* odd(const DecisionDiagramManager& dd_man) const;

  /* Returns a BDD representing the initial state for this model. */
  DdNode* init_bdd(const DecisionDiagramManager& dd_man) const;

  /* Returns the index associated with the initial state for this model. */
  int init_index(const DecisionDiagramManager& dd_man) const;

  /* Returns a BDD representing the given state. */
  BDD state_bdd(const DecisionDiagramManager& dd_man,
                const std::vector<int>& state) const;

  /* Returns the row variables for this model. */
  DdNode** row_variables(const DecisionDiagramManager& dd_man) const;

  /* Returns the column variables for this model. */
  DdNode** column_variables(const DecisionDiagramManager& dd_man) const;

  /* Returns the number of states for this model. */
  double num_states(const DecisionDiagramManager& dd_man) const;

  /* Returns the number of transitions for this model. */
  double num_transitions(const DecisionDiagramManager& dd_man) const;

  /* Releases all DDs cached for this model. */
  void uncache_dds(const DecisionDiagramManager& dd_man) const;

private:
  std::vector<ParsedVariable> variables_;
  /* The global variables for this model. */
  std::vector<const Variable*> global_variables_;
  /* Variable names. */
  std::vector<std::string> variable_names_;
  /* The modules for this model */
  ModuleList modules_;
  /* Compiled commands for this model. */
  CommandList commands_;
  /* Modules that the above commands are associated with. */
  std::vector<ModuleSet> command_modules_;
  /* Cached MTBDD representing rate matrix. */
  mutable DdNode* rate_mtbdd_;
  /* Cached reachability BDD. */
  mutable DdNode* reach_bdd_;
  /* Cached ODD. */
  mutable ODDNode* odd_;
  /* Cached BDD representing the initial state. */
  mutable DdNode* init_bdd_;
  /* Cached index associated with the initial state. */
  mutable int init_index_;
  /* Cached row variables. */
  mutable DdNode** row_variables_;
  /* Cached column variables. */
  mutable DdNode** column_variables_;
};

/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m);

#endif  // MODELS_H_
