/*
 * Copyright (C) 2003, 2004 Carnegie Mellon University
 *
 * This file is part of Ymer.
 *
 * Ymer is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Ymer is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ymer; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "ddmodel.h"

#include <iostream>
#include <set>
#include <vector>

#include "glog/logging.h"

DecisionDiagramModel::DecisionDiagramModel(
    const DecisionDiagramManager* manager, const BDD& initial_states,
    const ADD& rate_matrix, const BDD& reachable_states)
    : manager_(manager),
      initial_states_(initial_states),
      rate_matrix_(rate_matrix),
      reachable_states_(reachable_states),
      odd_(ODD::Make(reachable_states)),
      initial_state_index_(odd_.StateIndex(initial_states)) {}

namespace {

std::vector<IdentifierInfo> GetVariables(
    const CompiledModel& model,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name) {
  std::vector<IdentifierInfo> variables;
  variables.reserve(model.variables().size());
  for (const auto& v : model.variables()) {
    auto i = identifiers_by_name.find(v.name());
    CHECK(i != identifiers_by_name.end());
    variables.push_back(i->second);
  }
  return variables;
}

BDD VariableIdentityBdd(const DecisionDiagramManager& dd_manager,
                        const IdentifierInfo& variable) {
  return IdentifierToAdd(dd_manager, variable) ==
         PrimedIdentifierToAdd(dd_manager, variable);
}

BDD UntouchedVariablesBdd(const DecisionDiagramManager& dd_manager,
                          const std::vector<IdentifierInfo>& variables,
                          const std::set<int>& excluded_variables) {
  BDD updates = dd_manager.GetConstant(true);
  for (size_t i = variables.size(); i > 0; --i) {
    if (excluded_variables.find(i - 1) == excluded_variables.end()) {
      updates = VariableIdentityBdd(dd_manager, variables[i - 1]) && updates;
    }
  }
  return updates;
}

BDD CompiledUpdateToBdd(const DecisionDiagramManager& dd_manager,
                        const std::vector<IdentifierInfo>& variables,
                        const CompiledUpdate& update) {
  return PrimedIdentifierToAdd(dd_manager, variables[update.variable()]) ==
         update.expr().dd().value();
}

ADD CompiledMarkovOutcomeToAdd(const DecisionDiagramManager& dd_manager,
                               const std::vector<IdentifierInfo>& variables,
                               const std::set<int>* module_variables,
                               const CompiledMarkovOutcome& outcome) {
  BDD updates = dd_manager.GetConstant(true);
  std::set<int> touched_variables;
  for (const auto& update : outcome.updates()) {
    updates = CompiledUpdateToBdd(dd_manager, variables, update) && updates;
    touched_variables.insert(update.variable());
  }
  for (size_t i = variables.size(); i > 0; --i) {
    // If this is not a touched variable, we need to include a variable
    // identity in the update if:
    //
    //   * the outcome is not associated with any module, or
    //   * the variable belongs to the associated module.
    //
    if (touched_variables.find(i - 1) == touched_variables.end() &&
        (module_variables == nullptr ||
         module_variables->find(i - 1) != module_variables->end())) {
      updates = VariableIdentityBdd(dd_manager, variables[i - 1]) && updates;
    }
  }
  return outcome.probability().dd().value() * ADD(updates);
}

ADD CompiledMarkovCommandToAdd(
    const DecisionDiagramManager& dd_manager,
    const std::vector<IdentifierInfo>& variables,
    const std::vector<std::set<int>>& module_variables,
    bool with_all_untouched_variables, const CompiledMarkovCommand& command) {
  ADD outcomes = dd_manager.GetConstant(0);
  const std::set<int>* command_module_variables =
      command.module().has_value() ? &module_variables[command.module().value()]
                                   : nullptr;
  for (const auto& outcome : command.outcomes()) {
    outcomes = CompiledMarkovOutcomeToAdd(dd_manager, variables,
                                          command_module_variables, outcome) +
               outcomes;
  }
  if (with_all_untouched_variables && command_module_variables != nullptr) {
    outcomes = ADD(UntouchedVariablesBdd(dd_manager, variables,
                                         *command_module_variables)) *
               outcomes;
  }
  return command.guard().dd().value() * command.weight().dd().value() *
         outcomes;
}

ADD FactoredMarkovCommandsToAdd(
    const DecisionDiagramManager& dd_manager,
    const std::vector<IdentifierInfo>& variables,
    const std::vector<std::set<int>>& module_variables,
    const std::vector<std::vector<CompiledMarkovCommand>>& command_factors) {
  ADD action = dd_manager.GetConstant(1);
  std::set<int> touched_variables;
  for (size_t i = command_factors.size(); i > 0; --i) {
    ADD module = dd_manager.GetConstant(0);
    const auto& module_commands = command_factors[i - 1];
    for (size_t j = module_commands.size(); j > 0; --j) {
      const auto& command = module_commands[j - 1];
      CHECK(command.module().has_value());
      module = CompiledMarkovCommandToAdd(dd_manager, variables,
                                          module_variables, false, command) +
               module;
      const auto& variables_for_module =
          module_variables[command.module().value()];
      touched_variables.insert(variables_for_module.begin(),
                               variables_for_module.end());
    }
    action = module * action;
  }
  action =
      ADD(UntouchedVariablesBdd(dd_manager, variables, touched_variables)) *
      action;
  return action;
}

BDD ReachabilityBdd(const DecisionDiagramManager& dd_manager, const BDD& init,
                    const ADD& rates) {
  std::cout << "Computing reachable states";
  // Precompute variable permutations and cubes.
  size_t nvars = dd_manager.GetVariableCount() / 2;
  std::vector<int> row_to_col(2 * nvars);
  std::vector<int> col_to_row(2 * nvars);
  for (size_t i = 0; i < nvars; i++) {
    row_to_col[2 * i] = 2 * i + 1;
    row_to_col[2 * i + 1] = 2 * i + 1;
    col_to_row[2 * i] = 2 * i;
    col_to_row[2 * i + 1] = 2 * i;
  }
  BDD row_cube =
      dd_manager.GetCube(dd_manager.GetBddVariableArray(0, 2, 2 * nvars));

  // Fixpoint computation of reachability.
  BDD trans = BDD(rates);
  BDD solr = init;
  BDD solc = solr.Permutation(row_to_col);
  bool done = false;
  size_t iters = 0;
  while (!done) {
    iters++;
    if (iters % 1000 == 0) {
      std::cout << ':';
    } else if (iters % 100 == 0) {
      std::cout << '.';
    }
    BDD next_solc = solc || (trans && solr).ExistAbstract(row_cube);
    if (next_solc.is_same(solc)) {
      done = true;
    } else {
      solc = next_solc;
    }
    solr = solc.Permutation(col_to_row);
  }
  std::cout << ' ' << iters << " iterations." << std::endl;
  return solr;
}

}  // namespace

DecisionDiagramModel DecisionDiagramModel::Make(
    const DecisionDiagramManager* manager, const CompiledModel& model,
    size_t moments,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name) {
  CHECK(manager);
  std::cout <<"Building model...";

  const std::vector<IdentifierInfo> variables =
      GetVariables(model, identifiers_by_name);

  // BDD for initial state.
  BDD init_bdd = manager->GetConstant(true);
  for (size_t i = model.variables().size(); i > 0; --i) {
    const auto& v = model.variables()[i - 1];
    const int init_value = model.init_values()[i - 1];
    auto j = identifiers_by_name.find(v.name());
    CHECK(j != identifiers_by_name.end());
    init_bdd = IdentifierToAdd(*manager, variables[i - 1])
                   .Interval(init_value, init_value) &&
               init_bdd;
  }

  // TODO(hlsyounes): Add phase variables for GSMP commands.

  // Compute rate matrix for all commands.
  ADD rate_matrix = manager->GetConstant(0);
  if (model.pivot_variable().has_value()) {
    const int variable = model.pivot_variable().value();
    const int min_value = model.variables()[variable].min_value();
    const auto& pivoted_commands = model.pivoted_single_markov_commands();
    for (size_t i = pivoted_commands.size(); i > 0; --i) {
      const int value = min_value + i - 1;
      ADD variable_assignment =
          ADD(IdentifierToAdd(*manager, variables[variable])
                  .Interval(value, value));
      const auto& commands = pivoted_commands[i - 1];
      for (size_t j = commands.size(); j > 0; --j) {
        const auto& command = commands[j - 1];
        rate_matrix = variable_assignment *
                          CompiledMarkovCommandToAdd(*manager, variables,
                                                     model.module_variables(),
                                                     true, command) +
                      rate_matrix;
      }
    }
  }
  for (size_t i = model.single_markov_commands().size(); i > 0; --i) {
    const auto& command = model.single_markov_commands()[i - 1];
    rate_matrix =
        CompiledMarkovCommandToAdd(*manager, variables,
                                   model.module_variables(), true, command) +
        rate_matrix;
  }
  for (size_t i = model.factored_markov_commands().size(); i > 0; --i) {
    rate_matrix = FactoredMarkovCommandsToAdd(
                      *manager, variables, model.module_variables(),
                      model.factored_markov_commands()[i - 1]) +
                  rate_matrix;
  }
  if (model.type() == CompiledModelType::DTMC) {
    // Normalize rate matrix.
    ADD col_cube = manager->GetCube(
        manager->GetAddVariableArray(1, 2, manager->GetVariableCount()));
    rate_matrix = rate_matrix / rate_matrix.ExistAbstract(col_cube);
  }

  std::cout << manager->GetVariableCount() << " variables." << std::endl;

  // Reachability analysis.
  BDD reach_bdd = ReachabilityBdd(*manager, init_bdd, rate_matrix);
  rate_matrix = ADD(reach_bdd) * rate_matrix;

  return DecisionDiagramModel(manager, init_bdd, rate_matrix, reach_bdd);
}
