/* -*-C++-*- */
/*
 * Trajectories.
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
#ifndef TRAJECTORIES_H
#define TRAJECTORIES_H

#include "modules.h"
#include "expressions.h"


struct Trajectory {
  /* Constructs a trajectory with the given initial state. */
  explicit Trajectory(const Values& state)
    : state_(state), time_(0.0), trigger_(0), next_(0) {}

  /* Deletes this trajectory. */
  ~Trajectory() {
    if (next_ != 0) {
      delete next_;
    }
  }

  /* Sets the triggering event. */
  void set_trigger(const Command* trigger, double time) {
    trigger_ = trigger;
    time_ = time;
  }

  /* Sets the continuation. */
  void set_next(const Trajectory* next) {
    next_ = next;
  }

  /* Returns the initial state of this trajectory. */
  const Values& state() const { return state_; }

  /* Returns the time spent in the initial state. */
  double time() const { return time_; }

  /* Returns the event triggering in the initial state. */
  const Command* trigger() const { return trigger_; }

  /* Returns the continuation of this trajectory. */
  const Trajectory* next() const { return next_; }

private:
  /* Initial state of this trajectory. */
  Values state_;
  /* Time spent in the initial state. */
  double time_;
  /* Event triggering in the initial state. */
  const Command* trigger_;
  /* Continuation of this trajectory. */
  const Trajectory* next_;
};


#endif /* TRAJECTORIES_H */
