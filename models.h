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

#include "odd.h"
#include "src/compiled-model.h"
#include "src/ddutil.h"
#include "src/model.h"

// A parsed model.
class Model {
 public:
  // Constructs an empty model.
  Model();

  /* Deletes this model. */
  ~Model();

  // Sets the type of this model.  Returns false if the type has already been
  // set for this model.
  bool SetType(ModelType type);

  // Adds a constant with the given name, type, and init expression to this
  // model.  The init expression is optional and may be null.  Returns false if
  // an identifier with the given name has already been added and adds a reason
  // for the failure to errors.
  bool AddConstant(const std::string& name, Type type,
                   std::unique_ptr<const Expression>&& init,
                   std::vector<std::string>* errors);

  // Adds an int variable with the given name, and min, max, and init
  // expressions.  The init expression is optional and may be null.  Returns
  // false if an identifier with the given name has already been added and adds
  // a reason for the failure to errors.  The variable becomes a module variable
  // if this method is called between calls to StartModule() and EndModule().
  bool AddIntVariable(const std::string& name,
                      std::unique_ptr<const Expression>&& min,
                      std::unique_ptr<const Expression>&& max,
                      std::unique_ptr<const Expression>&& init,
                      std::vector<std::string>* errors);

  // Adds a bool variable with the given name and init expression.  The init
  // expression is optional and may be null.  Returns false if an identifier
  // with the given name has already been added and adds a reason for the
  // failure to errors.  The variable becomes a module variable if this method
  // is called between calls to StartModule() and EndModule().
  bool AddBoolVariable(const std::string& name,
                       std::unique_ptr<const Expression>&& init,
                       std::vector<std::string>* errors);

  // Marks the start of a new module with the given.  Returns false if a module
  // with the given name already exists.  Requires that no module is currently
  // open.
  bool StartModule(const std::string& name);

  // Adds a command to the current module.  Returns false if the command could
  // not be added and adds a reason for the failure to errors.  Requires that a
  // module is currently open.
  bool AddCommand(Command&& command, std::vector<std::string>* errors);

  // Adds commands and variables from an existing module to the current module,
  // applying the given substitutions to identifiers.  All variables for the
  // existing module must be renamed to unused names.  Returns false on failure
  // and adds a reason for the failure to errors.  Requires that a module is
  // currently open.
  bool AddFromModule(const std::string& from_name,
                     const std::map<std::string, std::string>& substitutions,
                     std::vector<std::string>* errors);

  // Marks the end of the current module.  Requires that a module is currently
  // open.
  void EndModule();

  /* Compiles the commands of this model. */
  void compile();

  // Returns the type of this model.
  ModelType type() const { return type_; }

  // Returns the constants for this model.
  const std::vector<ParsedConstant>& constants() const { return constants_; }

  // Returns the variables for this model.
  const std::vector<ParsedVariable>& variables() const { return variables_; }

  // Returns the global variables (just indices) for this model.
  const std::set<int>& global_variables() const { return global_variables_; }

  // Returns the modules for this model.
  const std::vector<ParsedModule>& modules() const { return modules_; }

  /* Returns all commands for this model. */
  const std::vector<const Command*>& commands() const { return commands_; }

private:
  struct IdentifierIndex {
    enum Type { kConstant, kVariable, kAction } type;
    size_t index;
  };

  static std::string IdentifierIndexTypeToString(IdentifierIndex::Type type);

  bool AddVariable(const std::string& name, Type type,
                   std::unique_ptr<const Expression>&& min,
                   std::unique_ptr<const Expression>&& max,
                   std::unique_ptr<const Expression>&& init,
                   std::vector<std::string>* errors);

  bool AddAction(const std::string& name, std::vector<std::string>* errors);

  ModelType type_;
  std::map<std::string, IdentifierIndex> identifier_indices_;
  std::vector<ParsedConstant> constants_;
  std::vector<ParsedVariable> variables_;
  std::set<int> global_variables_;
  int current_module_;
  std::map<std::string, int> module_indices_;
  std::vector<ParsedModule> modules_;
  /* Compiled commands for this model. */
  std::vector<const Command*> commands_;
};

// Output operator for models.
std::ostream& operator<<(std::ostream& os, const Model& m);

// A model compiled into decision diagrams.
class DecisionDiagramModel {
 public:
  ~DecisionDiagramModel();

  static DecisionDiagramModel Create(
      const DecisionDiagramManager* manager, size_t moments, const Model& model,
      const CompiledModel& compiled_model,
      const std::map<std::string, IdentifierInfo>& identifiers_by_name);

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

#endif  // MODELS_H_
