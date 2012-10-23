/* -*-C++-*- */
/*
 * States.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
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
 * $Id: states.h,v 4.1 2005-02-01 14:19:47 lorens Exp $
 */
#ifndef STATES_H
#define STATES_H

#include <config.h>
#include "src/compiled-model.h"
#include "src/expression.h"
#include "src/rng.h"

struct Command;
struct Model;


/* ====================================================================== */
/* State */

/*
 * A state.
 */
struct State {
  /* Constructs an initial state for the given model. */
  State(const Model* model, const CompiledModel& compiled_model,
        DCEngine* engine);

  /* Deletes this state. */
  virtual ~State() {}

  /* Returns the model associated with this state. */
  const Model* model() const { return model_; }

  /* Returns the variable values for this state. */
  const std::vector<int>& values() const { return values_; }

  /* Returns the time spent in the preceding state. */
  virtual double dt() const { return 0.0; }

  /* Returns a sampled successor of this state. */
  virtual const State& next() const;

  /* Returns a copy of this state with resampled trigger times. */
  virtual const State& resampled() const;

  /* Prints this object on the given stream. */
  void print(std::ostream& os) const;

protected:
  /* Constructs a state. */
  State(const Model* model, const std::vector<int>& values, DCEngine* engine)
      : model_(model), values_(values), engine_(engine) {}

  /* Returns the variable values for this state. */
  std::vector<int>& values() { return values_; }

  DCEngine* engine() const { return engine_; }

private:
  /* The model associated with this state. */
  const Model* model_;
  /* Variables values for this state. */
  std::vector<int> values_;
  DCEngine* engine_;
};


#endif /* STATES_H */
