/* -*-C++-*- */
/*
 * Discrete-event simulator.
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
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <config.h>
#include "models.h"
#include "expressions.h"
#include <iostream>


/* ====================================================================== */
/* State */

/*
 * A simulation state.
 */
struct State {
  /* Constructs an initial state for the given model. */
  explicit State(const Model& model);

  /* Constructs a state. */
  explicit State(const Values& values) : values_(values) {}

  /* Deletes this state. */
  virtual ~State() {}

  /* Returns the variable values for this state. */
  const Values& values() const { return values_; }

  /* Returns the variable values for this state. */
  Values& values() { return values_; }

  /* Returns a sampled successor of this state. */
  virtual const State& next(const Command*& trigger, double& time,
			    const Model& model) const;

  /* Returns a copy of this state with resampled trigger times. */
  virtual const State& resampled(const Model& model) const;

private:
  /* Variable values for this state. */
  Values values_;
};

/* Output operator for states. */
std::ostream& operator<<(std::ostream& os, const State& s);


#endif /* SIMULATOR_H */
