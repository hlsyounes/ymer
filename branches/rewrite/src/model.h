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
// Classes for representing a model.

#ifndef MODEL_H_
#define MODEL_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

class Expression;
class ProcessAlgebra;

// Supported expression types.
enum class Type { INT, DOUBLE, BOOL };

// Returns the name of the given expression type.
std::string Type_Name(Type type);

// Range for int variables.
class Range {
 public:
  // Constucts a range.
  Range(std::unique_ptr<const Expression>&& min,
        std::unique_ptr<const Expression>&& max);
  Range(Range&& range);

  // Disallow copy and assign.
  Range(const Range&) = delete;
  Range& operator=(const Range&) = delete;

  // Returns the min expression for this range.
  const Expression& min() const { return *min_; }

  // Returns the max expression for this range.
  const Expression& max() const { return *max_; }

 private:
  // The min expression for this range.
  std::unique_ptr<const Expression> min_;
  // The max expression for this range.
  std::unique_ptr<const Expression> max_;
};

// A variable update for a module command.
class Update {
 public:
  // Constructs an update for the given variable and with the given
  // expression.
  Update(const std::string& variable, std::unique_ptr<const Expression>&& expr);
  Update(Update&& update);

  // Disallow copy and assign.
  Update(const Update&) = delete;
  Update& operator=(const Update&) = delete;

  // Move assignment.
  Update& operator=(Update&& update);

  // Returns the variable for this update.
  const std::string& variable() const { return variable_; }

  // Returns the expression for this update.
  const Expression& expr() const { return *expr_; }

 private:
  // The variable for this update.
  std::string variable_;
  // The expression for this update.
  std::unique_ptr<const Expression> expr_;
};

// A possible outcome of a module command, consisting of a probability and a
// list of variable updates.
class Outcome {
 public:
  // Constructs an outcome.
  Outcome(std::unique_ptr<const Expression>&& probability,
          std::vector<Update>&& updates);
  Outcome(Outcome&& outcome);

  // Disallow copy and assign.
  Outcome(const Outcome&) = delete;
  Outcome& operator=(const Outcome&) = delete;

  // Move assignment.
  Outcome& operator=(Outcome&& outcome);

  // Returns the probability for this outcome.
  const Expression& probability() const { return *probability_; }

  // Returns the list of updates for this outcome.
  const std::vector<Update>& updates() const { return updates_; }

 private:
  // The probability for this outcome.
  std::unique_ptr<const Expression> probability_;
  // The list of updates for this outcome.
  std::vector<Update> updates_;
};

// A module command.
class Command {
 public:
  // Constructs a command with the given action, guard, and outcomes.  The
  // action is optional and may be null.
  Command(std::unique_ptr<const std::string>&& action,
          std::unique_ptr<const Expression>&& guard,
          std::vector<Outcome>&& outcomes);
  Command(Command&& command);

  // Disallow copy and assign.
  Command(const Command&) = delete;
  Command& operator=(const Command&) = delete;

  // Move assignment.
  Command& operator=(Command&& command);

  // Returns the action for this command; null if no action is associated with
  // this command.
  const std::string* action() const { return action_.get(); }

  // Returns the guard for this command.
  const Expression& guard() const { return *guard_; }

  // Returns the outcomes for this command.
  const std::vector<Outcome>& outcomes() const { return outcomes_; }

 private:
  // The action for this command; null if no action is associated with this
  // command.
  std::unique_ptr<const std::string> action_;
  // The guard for this command.
  std::unique_ptr<const Expression> guard_;
  // The outcomes for this command.
  std::vector<Outcome> outcomes_;
};

// A state reward specification.
class StateReward {
 public:
  // Constructs a state reward specification.
  StateReward(std::unique_ptr<const Expression>&& guard,
              std::unique_ptr<const Expression>&& reward);
  StateReward(StateReward&& state_reward);

  // Disallow copy and assign.
  StateReward(const StateReward&) = delete;
  StateReward& operator=(const StateReward&) = delete;

  // Move assignment.
  StateReward& operator=(StateReward&& state_reward);

  // Returns the guard for this state reward specification.
  const Expression& guard() const { return *guard_; }

  // Returns the reward for this state reward specification.
  const Expression& reward() const { return *reward_; }

 private:
  // The guard for this state reward specification.
  std::unique_ptr<const Expression> guard_;
  // The reward for this state reward specification.
  std::unique_ptr<const Expression> reward_;
};

// A transition reward specification.
class TransitionReward {
 public:
  // Constructs a transition reward specification.  The action is optional and
  // may be null.
  TransitionReward(std::unique_ptr<const std::string>&& action,
                   std::unique_ptr<const Expression>&& guard,
                   std::unique_ptr<const Expression>&& reward);
  TransitionReward(TransitionReward&& transition_reward);

  // Disallow copy and assign.
  TransitionReward(const TransitionReward&) = delete;
  TransitionReward& operator=(const TransitionReward&) = delete;

  // Move assignment.
  TransitionReward& operator=(TransitionReward&& transition_reward);

  // Returns the action for this transition reward specification; null if no
  // action is associated with this transition reward specification.
  const std::string* action() const { return action_.get(); }

  // Returns the guard for this transition reward specification.
  const Expression& guard() const { return *guard_; }

  // Returns the reward for this transition reward specification.
  const Expression& reward() const { return *reward_; }

 private:
  // The action for this transition reward specification; null if no action is
  // associated with this transition reward specification.
  std::unique_ptr<const std::string> action_;
  // The guard for this transition reward specification.
  std::unique_ptr<const Expression> guard_;
  // The reward for this transition reward specification.
  std::unique_ptr<const Expression> reward_;
};

// Supported model types.  Note: NONE is not a valid model type.
enum class ModelType { NONE, DTMC, CTMC, MDP };

// A model.
class Model {
 public:
  // Constructs an empty model.
  Model();

  ~Model();

  // Disallow copy and assign.
  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  // Sets the model type.  Returns false if the type has already been set for
  // this model.
  bool SetType(ModelType type);

  // Returns the model type.
  ModelType type() const { return type_; }

  // Adds a constant with the given name, type, and init expression to this
  // model.  The init expression is optional and may be null.  Returns false
  // if an identifier with the given name has already been added.
  bool AddConstant(const std::string& name, Type type,
                   std::unique_ptr<const Expression>&& init);

  // Returns the number of constants defined for this model.
  int num_constants() const { return constant_types_.size(); }

  // Returns the type of the ith constant.
  Type constant_type(int i) const { return constant_types_[i]; }

  // Returns the init expression for the ith constant; null if the ith
  // constant does not have an init expression.
  const Expression* GetConstantInit(int i) const;

  // Returns the name for the ith constant.
  const std::string& GetConstantName(int i) const;

  // Adds an int variable with the given name, range, and init expression to
  // this model.  The init expression is optional and may be null.  Returns
  // false if an identifier with the given name has already been added.  The
  // variable becomes a module variable if this method is called between calls
  // to StartModule() and EndModule().
  bool AddIntVariable(const std::string& name, Range&& range,
                      std::unique_ptr<const Expression>&& init);

  // Adds a bool variable with the given name and init expression to this
  // model.  The init expression is optional and may be null.  Returns false
  // if an identifier with the given name has already been added.  The
  // variable becomes a module variable if this method is called between calls
  // to StartModule() and EndModule().
  bool AddBoolVariable(const std::string& name,
                       std::unique_ptr<const Expression>&& init);

  // Returns the number of variables defined for this model.
  int num_variables() const { return variable_types_.size(); }

  // Returns the type of the ith variable.
  Type variable_type(int i) const { return variable_types_[i]; }

  // Returns the range for the ith variable; null if the ith variable is of
  // type bool.
  const Range* GetVariableRange(int i) const;

  // Returns the init expression for the ith variable; null if the ith
  // variable does not have an init expression.
  const Expression* GetVariableInit(int i) const;

  // Returns the name for the ith variable.
  const std::string& GetVariableName(int i) const;

  // Adds a formula with the given name to this model.  Returns false if an
  // identifier with the given name has already been added.
  bool AddFormula(const std::string& name,
                  std::unique_ptr<const Expression>&& expr);

  // Returns the number of formulas defined for this model.
  int num_formulas() const { return formula_exprs_.size(); }

  // Returns the expression for the ith formula.
  const Expression& formula_expr(int i) const { return *formula_exprs_[i]; }

  // Returns the name for the ith formula.
  const std::string& GetFormulaName(int i) const;

  // Marks the start of a new module with the given name.  Returns false if a
  // module with the given name already exists.  Requires that no module is
  // currently open.
  bool StartModule(const std::string& name);

  // Returns the number of modules for this model.
  int num_modules() const { return modules_.size(); }

  // Returns the name for the ith module.
  const std::string& GetModuleName(int i) const;

  // Returns the module variables (indices) for the ith module.
  const std::set<int>& module_variables(int i) const {
    return module_variables_[i];
  }

  // Adds a command to the current module.  Requires that a module is
  // currently open.
  bool AddCommand(Command&& command);

  // Returns the commands for the ith module.
  const std::vector<Command>& module_commands(int i) const {
    return module_commands_[i];
  }

  // Adds commands and variables from an existing module to the current
  // module, applying the given substitutions to identifiers.  All variables
  // for the existing module must be renamed to unused named.  Returns true on
  // success.  Returns false if the source module does not exist, or if the
  // substitutions for module variables are incomplete or would result in
  // duplicate variables.  If error is non-null, then it is populated with an
  // error message in case of failure.  Requires that a module is currently
  // open.
  bool AddFromModule(const std::string& from_module,
                     const std::map<std::string, std::string>& substitutions,
                     std::string* error);

  // Ends the current module.  Requires that a module is currently open.
  void EndModule();

  // Adds a label for the given expression.  Returns false if a label with the
  // given name has already been added.
  bool AddLabel(const std::string& name,
                std::unique_ptr<const Expression>&& expr);

  // Sets the init expression for this model.  Returns false if this model
  // already has an init expression.
  bool SetInit(std::unique_ptr<const Expression>&& init);

  // Returns the init expression for this model; null if no init expression
  // has been set for this model.
  const Expression* init() const { return init_.get(); }

  // Marks the start of a new rewards structure without a label.  Requires
  // that no rewards structure is currently open.
  void StartUnlabeledRewards();

  // Marks the start of a new rewards structure with the given label.  Returns
  // false if a rewards structure with the given label already exists.
  // Requires that no rewards structure is currently open.
  bool StartLabeledRewards(const std::string& label);

  // Returns the number of rewards structures for this model.
  int num_rewards() const { return state_rewards_.size(); }

  // Adds a state reward to the current rewards structure.  Requires that a
  // rewards structure is currently open.
  void AddStateReward(StateReward&& state_reward);

  // Adds a transition reward to the current rewards structure.  Requires that
  // a rewards structure is currently open.
  void AddTransitionReward(TransitionReward&& transition_reward);

  // Returns the state rewards for the ith rewards structure.
  const std::vector<StateReward>& state_rewards(int i) const {
    return state_rewards_[i];
  }

  // Returns the transition rewards for the ith rewards structure.
  const std::vector<TransitionReward>& transition_rewards(int i) const {
    return transition_rewards_[i];
  }

  // Finds the index for a labeled rewards structure.  Returns true if a
  // rewards structure with the given label was found.
  bool GetLabeledRewardsIndex(const std::string& label, int* index) const;

  // Ends the current rewards structure.  Requires that a rewards structure is
  // currently open.
  void EndRewards();

  // Sets the system process algebra expression for this model.  Returns false
  // if this model already has a system process algebra expression.
  bool SetSystem(std::unique_ptr<const ProcessAlgebra>&& system);

  // Returns the system process algebra expression for this model; null if no
  // system process algebra expression has been set for this model.
  const ProcessAlgebra* system() const { return system_.get(); }

 private:
  // Helper method for Add*Variable().  Requires that range is non-null iff
  // type is int.
  bool AddVariable(const std::string& name, Type type, Range* range,
                   std::unique_ptr<const Expression>&& init);

  // Helper method for AddCommand().  Returns true if name is null, or if name
  // is a permissible action identifier (either already defined to be an
  // action identifier, or an undefined identifer).  Actions for commands must
  // not clash with variable, constant, and formula identifiers, since that
  // could result in ambiguous substitutions when using AddFromModule().
  bool AddAction(const std::string* name);

  // Helper method for Start*Rewards().
  bool StartRewards(const std::string* label);

  // Find the index for the variable with the given name.  Returns true if a
  // variable with the given name was found.
  bool GetVariableIndex(const std::string& name, int* index) const;

  // Finds the index for the module with the given name.  Returns true if a
  // module with the given name was found.
  bool GetModuleIndex(const std::string& name, int* index) const;

  // The model type.
  ModelType type_;
  // Map from identifier name to a unique integer ID.  The ID encodes the
  // identifier kind (constant, variable, or formula) and the order in which
  // it was added relative to other identifiers of the the same kind.
  std::map<std::string, int> identifiers_;
  // Type of constants in the order they were added to this model.
  std::vector<Type> constant_types_;
  // Map from an index into constant_types_ to the init expression for that
  // constant.  The map is sparse, meaning that the index for a constant with
  // no init expression is not present in the map.
  std::map<int, std::unique_ptr<const Expression> > constant_inits_;
  // Type of variables in the order they were added to this model.
  std::vector<Type> variable_types_;
  // Map from an index into variable_types_ to the range for that variable.
  // The map is sparse, meaning that the index for a variable of type bool
  // (which has no range) is not present in the map.
  std::map<int, Range> variable_ranges_;
  // Map from an index into variable_types_ to the init expression for that
  // variable.  The map is sparse, meaning that the index for a variable with
  // no init expression is not present in the map.
  std::map<int, std::unique_ptr<const Expression> > variable_inits_;
  // Formula expressions in the order they were added to this model.
  std::vector<std::unique_ptr<const Expression> > formula_exprs_;
  // Map from module name to module index.
  std::map<std::string, int> modules_;
  // The index of the current module, or an invalid index if no module is
  // currently open.
  int current_module_;
  // For each module, the set of indices for variables that belong to that
  // module.
  std::vector<std::set<int> > module_variables_;
  // For each module, the list of commands for that module.
  std::vector<std::vector<Command> > module_commands_;
  // Map from label name to labeled expression.
  std::map<std::string, std::unique_ptr<const Expression> > labels_;
  // The init expression for this model; null if no init expression has been
  // set for this model.
  std::unique_ptr<const Expression> init_;
  // The state rewards for each rewards structure of this model.
  std::vector<std::vector<StateReward> > state_rewards_;
  // The transition rewards for each rewards structure of this model.
  std::vector<std::vector<TransitionReward> > transition_rewards_;
  // The index of the current rewards structure, or an invalid index if no
  // rewards structure is currently open.
  int current_rewards_;
  // Map from rewards label to rewards structure index.  Not all rewards
  // structures have a label.  Only indices of labeled rewards structures will
  // be present as values in this map.
  std::map<std::string, int> labeled_rewards_;
  // The system process algebra expression for this model; null if no system
  // process algebra expression has been set for this model.
  std::unique_ptr<const ProcessAlgebra> system_;
};

#endif  // MODEL_H_
