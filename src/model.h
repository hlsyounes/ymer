// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2014 Google Inc
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
// Classes to represent a PRISM model.

#ifndef MODEL_H_
#define MODEL_H_

#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "distribution.h"
#include "expression.h"
#include "typed-value.h"

// A variable update.
class Update {
 public:
  // Constructs a variable update.
  Update(const std::string& variable, std::unique_ptr<const Expression>&& expr);

  // Returns the variable for this update.
  const std::string& variable() const { return variable_; }

  // Returns the expression for this update.
  const Expression& expr() const { return *expr_; }

 private:
  std::string variable_;
  std::unique_ptr<const Expression> expr_;
};

// Output operator for updates.
std::ostream& operator<<(std::ostream& os, const Update& update);

// A command outcome.
class Outcome {
 public:
  // Constructs an outcome with the given delay and outcomes.
  Outcome(std::unique_ptr<const Distribution>&& delay,
          std::vector<Update>&& updates);

  // Returns the delay for this outcome.
  const Distribution& delay() const { return *delay_; }

  // Returns the updates for this outcome.
  const std::vector<Update>& updates() const { return updates_; }

 private:
  std::unique_ptr<const Distribution> delay_;
  std::vector<Update> updates_;
};

// Output operator for outcomes.
std::ostream& operator<<(std::ostream& os, const Outcome& outcome);

// A module command.
struct Command {
  // Constructs a command with the given action label, guard, and outcomes.
  Command(const std::string& action, std::unique_ptr<const Expression>&& guard,
          std::vector<Outcome>&& outcomes);

  // Returns the action label for this command, or an empty string if the
  // command does not have an action label.
  const std::string& action() const { return action_; }

  // Returns the guard for this command.
  const Expression& guard() const { return *guard_; }

  // Returns the outcomes for this command.
  const std::vector<Outcome>& outcomes() const { return outcomes_; }

 private:
  std::string action_;
  std::unique_ptr<const Expression> guard_;
  std::vector<Outcome> outcomes_;
};

// Outcome operator for commands.
std::ostream& operator<<(std::ostream& os, const Command& command);

// Supported parsed model types.
enum class ModelType { DEFAULT, MDP, DTMC, CTMC, GSMP };

// Output operator for model types.
std::ostream& operator<<(std::ostream& os, ModelType model_type);

// A parsed constant.
class ParsedConstant {
 public:
  // Constructs a parsed constant with the given name, type, and init
  // expression.
  explicit ParsedConstant(const std::string& name, Type type,
                          std::unique_ptr<const Expression>&& init);

  // Returns the name for this parsed constant.
  const std::string& name() const { return name_; }

  // Returns the type for this parsed constant.
  Type type() const { return type_; }

  // Returns a pointer to the init expression for this parsed constant, or null
  // if this parsed constant does not have an init expression.
  const Expression* init() const { return init_.get(); }

 private:
  std::string name_;
  Type type_;
  std::unique_ptr<const Expression> init_;
};

// Resolves the given constants by evaluating their init expressions, and
// populates constant_values.  constant_values may be pre-populated with
// overrides for init expressions, and this is required for constants with no
// init expression.  Populates errors with messages describing any errors that
// are detected during constant resolution.
void ResolveConstants(const std::vector<ParsedConstant>& constants,
                      std::map<std::string, TypedValue>* constant_values,
                      std::vector<std::string>* errors);

// A parsed variable.
class ParsedVariable {
 public:
  // Constructs a parsed variable with the given name, type, and min, max, and
  // init expressions.
  explicit ParsedVariable(const std::string& name, Type type,
                          std::unique_ptr<const Expression>&& min,
                          std::unique_ptr<const Expression>&& max,
                          std::unique_ptr<const Expression>&& init);

  // Returns the name for this parsed variable.
  const std::string& name() const { return name_; }

  // Returns the type for this parsed variable.
  Type type() const { return type_; }

  // Returns the min expression for this parsed variable.
  const Expression&  min() const { return *min_; }

  // Returns the max expression for this parsed variable.
  const Expression&  max() const { return *max_; }

  // Returns true if this parsed variable has an explicit init expression.
  bool has_explicit_init() const { return init_ != nullptr; }

  // Returns the init expression for this parsed variable.  Returns the min
  // expression if this parsed variable does not have an explicit init
  // expression.
  const Expression& init() const { return (init_ == nullptr) ? *min_ : *init_; }

 private:
  std::string name_;
  Type type_;
  std::unique_ptr<const Expression> min_;
  std::unique_ptr<const Expression> max_;
  std::unique_ptr<const Expression> init_;
};

// A parsed module.
class ParsedModule {
 public:
  // Constructs a parsed module with the given name.
  explicit ParsedModule(const std::string& name);

  // Makes the variable with the given index a variable of this parsed module.
  void add_variable(int variable_index) { variables_.insert(variable_index); }

  // Adds the given command to this parsed module.
  void add_command(Command&& command) {
    commands_.push_back(std::move(command));
  }

  // Returns the name for this parsed module.
  const std::string& name() const { return name_; }

  // Returns the indices of variables for this parsed module.
  std::set<int> variables() const { return variables_; }

  // Returns the commands for this parsed module.
  const std::vector<Command>& commands() const { return commands_; }

 private:
  std::string name_;
  std::set<int> variables_;
  std::vector<Command> commands_;
};

#endif  // MODEL_H_
