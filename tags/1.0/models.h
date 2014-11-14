/* -*-C++-*- */
/*
 * Models.
 *
 * Copyright (C) 2003 Carnegie Mellon University
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
 *
 * $Id: models.h,v 1.3 2003-11-07 04:25:32 lorens Exp $
 */
#ifndef MODELS_H
#define MODELS_H

#include <config.h>
#include "modules.h"
#include "odd.h"
#include <map>


/* ====================================================================== */
/* SynchronizationMap */

/*
 * A synchronization map.
 */
struct SynchronizationMap
  : public std::multimap<std::pair<const Module*, size_t>, const Command*> {
};

/* Range for synchronization map. */
typedef std::pair<SynchronizationMap::const_iterator,
		  SynchronizationMap::const_iterator> SynchronizationMapRange;


/* ====================================================================== */
/* Model */

/*
 * A model.
 */
struct Model {
  /* Constructs a model. */
  Model();

  /* Deletes this model. */
  ~Model();

  /* Adds a global variable to this model. */
  void add_variable(const Variable& variable);

  /* Adds a module to this model. */
  void add_module(const Module& module);

  /* Returns the global variables for this model. */
  const VariableList& variables() const { return variables_; }

  /* Returns the modules for this model */
  const ModuleList& modules() const { return modules_; }

  /* Caches commands for this model. */
  void cache_commands() const;

  /* Returns all commands for this model. */
  const CommandList& commands() const { return commands_; }

  /* Caches DDs for this model. */
  void cache_dds(DdManager* dd_man) const;

  /* Returns an MTBDD representing the rate matrix for this model. */
  DdNode* rate_mtbdd(DdManager* dd_man) const;

  /* Returns a reachability BDD for this model. */
  DdNode* reachability_bdd(DdManager* dd_man) const;

  /* Returns an ODD for this model. */
  ODDNode* odd(DdManager* dd_man) const;

  /* Returns a BDD representing the initial state for this model. */
  DdNode* init_bdd(DdManager* dd_man) const;

  /* Returns the index associated with the initial state for this model. */
  int init_index(DdManager* dd_man) const;

  /* Returns the row variables for this model. */
  DdNode** row_variables(DdManager* dd_man) const;

  /* Returns the column variables for this model. */
  DdNode** column_variables(DdManager* dd_man) const;

  /* Returns the number of states for this model. */
  double num_states(DdManager* dd_man) const;

  /* Returns the number of transitions for this model. */
  double num_transitions(DdManager* dd_man) const;

  /* Releases all DDs cached for this model. */
  void uncache_dds(DdManager* dd_man) const;

private:
  /* The global variables for this model. */
  VariableList variables_;
  /* The modules for this model */
  ModuleList modules_;
  /* Cached commands for this model. */
  mutable CommandList commands_;
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

  /* Returns a BDD representing the range for all model variables. */
  DdNode* range_bdd(DdManager* dd_man) const;
};

/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m);


#endif /* MODELS_H */
