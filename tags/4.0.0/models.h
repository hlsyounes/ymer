/* -*-C++-*- */
/*
 * Models.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011 Google Inc
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
#ifndef MODELS_H
#define MODELS_H

#include <config.h>
#include "modules.h"
#include "expressions.h"
#include <iostream>


/* ====================================================================== */
/* Model */

/*
 * A model.
 */
struct Model {
  /* Deletes this model. */
  ~Model();

  /* Adds a global integer-valued constant to this model. */
  void add_constant(const Constant<int>& constant);

  /* Adds a global double-valued constant to this model. */
  void add_constant(const Constant<double>& constant);

  /* Adds a global integer-valued variable to this model. */
  void add_variable(const Variable<int>& variable);

  /* Adds a global double-valued variable to this model. */
  void add_variable(const Variable<double>& variable);

  /* Adds a module to this model. */
  void add_module(const Module& module);

  /* Compiles the commands of this model. */
  void compile() const;

  /* Returns the global integer-valued constants for this model. */
  const std::vector<const Constant<int>*>& int_constants() const {
    return int_constants_;
  }

  /* Returns the global double-valued constants for this model. */
  const std::vector<const Constant<double>*>& double_constants() const {
    return double_constants_;
  }

  /* Returns the global integer-valued variables for this model. */
  const VariableList<int>& int_variables() const { return int_variables_; }

  /* Returns the global double-valued variables for this model. */
  const VariableList<double>& double_variables() const {
    return double_variables_;
  }

  /* Returns the modules for this model */
  const ModuleList& modules() const { return modules_; }

  /* Returns all commands for this model. */
  const CommandList& commands() const { return commands_; }

private:
  /* The global integer-valued constants for this model. */
  std::vector<const Constant<int>*> int_constants_;
  /* The global double-valued constants for this model. */
  std::vector<const Constant<double>*> double_constants_;
  /* The global integer-valued variables for this model. */
  VariableList<int> int_variables_;
  /* The global double-valued variables for this model. */
  VariableList<double> double_variables_;
  /* The modules for this model */
  ModuleList modules_;
  /* Compiled commands for this model. */
  mutable CommandList commands_;
};

/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m);


#endif /* MODELS_H */
