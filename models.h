/* -*-C++-*- */
/*
 * Models.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: models.h,v 1.2 2003-08-13 18:47:48 lorens Exp $
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
