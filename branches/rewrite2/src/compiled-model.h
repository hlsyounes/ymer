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
// A compiled model.

#ifndef COMPILED_MODEL_H_
#define COMPILED_MODEL_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "expression.h"
#include "model.h"
#include "optional.h"
#include "type.h"

struct VariableInfo {
  std::string name;
  TypedValue min_value;
  TypedValue max_value;
  TypedValue init_value;
};

using ActionToCommandsMap = std::map<int, std::vector<CompiledCommand> >;

class CompiledModel {
 public:
  CompiledModel(CompiledModel&& compiled_model);

  // Disallow copy and assign.
  CompiledModel(const CompiledModel&) = delete;
  CompiledModel& operator=(const CompiledModel&) = delete;

  static CompiledModel Make(
      const Model& parsed_model,
      const std::map<std::string, const ParsedExpression*>& const_overrides,
      std::string* error);

  // Returns true if the compiled model is valid.
  bool is_valid() const { return is_valid_; }

  // Returns the model type.
  ModelType model_type() const { return model_type_; }

  // Returns the number of variables for this compiled model.
  int num_variables() const { return variables_.size(); }

  // Returns the information for the ith variable.
  const VariableInfo& variable_info(int i) const { return variables_[i]; }

  // Returns the number of actions for this compiled model.
  int num_actions() const { return action_names_.size(); }

  // Returns the init expression for this compiled model.
  const CompiledExpression<double>* init() const { return init_.get(); }

  // Returns the commands without action for this compiled model.
  const std::vector<CompiledCommand>& commands_without_action() const {
    return commands_without_action_;
  }

  // Returns the number of modules for this compiled model.
  int num_modules() const { return module_commands_.size(); }

  // Returns the commands for the ith module.
  const ActionToCommandsMap& commands(int i) const {
    return module_commands_[i];
  }

 private:
  CompiledModel();
  CompiledModel(ModelType model_type,
                std::vector<VariableInfo>&& variables,
                std::vector<std::string>&& action_names,
                std::unique_ptr<const CompiledExpression<double> >&& init,
                std::vector<CompiledCommand>&& commands_without_action,
                std::vector<ActionToCommandsMap>&& module_commands);

  // Model type.
  ModelType model_type_;
  // Variables.
  std::vector<VariableInfo> variables_;
  // Actions.
  std::vector<std::string> action_names_;
  // Init expression.
  std::unique_ptr<const CompiledExpression<double> > init_;
  // Commands without action.
  std::vector<CompiledCommand> commands_without_action_;
  // Module commands.
  std::vector<ActionToCommandsMap> module_commands_;
  // Whether this system is valid.
  bool is_valid_;
};

#endif  // COMPILED_MODEL_H_
