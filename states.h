/* -*-C++-*- */
/*
 * States.
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
 * $Id: states.h,v 1.1 2003-08-13 18:48:39 lorens Exp $
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
