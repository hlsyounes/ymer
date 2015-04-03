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
// A model compiled into decision diagrams.

#ifndef DD_MODEL_H_
#define DD_MODEL_H_

#include <map>
#include <string>

#include "compiled-model.h"
#include "ddutil.h"

// A model compiled into decision diagrams.
class DecisionDiagramModel {
 public:
  static DecisionDiagramModel Make(
      const DecisionDiagramManager* manager, const CompiledModel& model,
      size_t moments,
      const std::map<std::string, IdentifierInfo>& identifiers_by_name);

  const DecisionDiagramManager& manager() const { return *manager_; }

  const BDD& initial_states() const { return initial_states_; }
  const ADD& rate_matrix() const { return rate_matrix_; }
  const BDD& reachable_states() const { return reachable_states_; }
  const ODD& odd() const { return odd_; }
  Optional<int> initial_state_index() const { return initial_state_index_; }

 private:
  DecisionDiagramModel(const DecisionDiagramManager* manager,
                       const BDD& initial_states, const ADD& rate_matrix,
                       const BDD& reachable_states);

  const DecisionDiagramManager* manager_;
  BDD initial_states_;
  ADD rate_matrix_;
  BDD reachable_states_;
  ODD odd_;
  Optional<int> initial_state_index_;
};

#endif  // DDMODEL_H_
