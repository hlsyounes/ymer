/* -*-C++-*- */
/*
 * States.
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
 * $Id: states.h,v 1.2 2003-11-07 04:26:32 lorens Exp $
 */
#ifndef STATES_H
#define STATES_H

#include <config.h>
#include "expressions.h"

struct Command;
struct Model;


/* ====================================================================== */
/* State */

/*
 * A state.
 */
struct State {
  /* Constructs an initial state for the given model. */
  explicit State(const Model& model);

  /* Constructs a successor state for the given state. */
  State(const State& state, const Command* command, double dt);

  /* Returns the variable values for this state. */
  const ValueMap& values() const { return values_; }

  /* Returns the time spent in the preceding state. */
  double dt() const { return dt_; }

  /* Returns a sampled successor of this state. */
  const State& next(const Model& model) const;

  /* Prints this object on the given stream. */
  void print(std::ostream& os) const;

private:
  /* Variables values for this state. */
  ValueMap values_;
  /* Time spent in the preceding state. */
  double dt_;
};


#endif /* STATES_H */
