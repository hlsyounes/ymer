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
 * $Id: models.h,v 1.1 2003-08-10 01:52:50 lorens Exp $
 */
#ifndef MODELS_H
#define MODELS_H

#include <config.h>
#include "modules.h"
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
  Model() {}

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

  /* Returns an MTBDD representing the rate matrix for this model. */
  DdNode* mtbdd(DdManager* dd_man) const;

private:
  /* The global variables for this model. */
  VariableList variables_;
  /* The modules for this model */
  ModuleList modules_;

  /* Returns a BDD representing the range for all model variables. */
  DdNode* range_bdd(DdManager* dd_man) const;
};

/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m);


#endif /* MODELS_H */
