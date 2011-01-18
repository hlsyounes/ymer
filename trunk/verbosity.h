/* -*-C++-*- */
/*
 * Verbosity.
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
#ifndef VERBOSITY_H
#define VERBOSITY_H

#include <config.h>


/* ====================================================================== */
/* Verbosity */

/*
 * The verbosity of output.
 */
struct Verbosity {
  /* Whether to output the model. */
  bool model;
  /* Whether to output the model size. */
  bool model_size;
  /* Whether to output information from the verification engine. */
  bool engine;
  /* Whether to output samples used by the statistical engine. */
  bool sample;
  /* Whether to output the schedule during simulation. */
  bool schedule;
  /* Whether to output the search for a sampling plan. */
  bool sampling_plan;

  /* Default verbosity settings. */
  Verbosity()
    : model(false), model_size(false), engine(false), sample(false),
      schedule(false), sampling_plan(false) {}
};


/* Global verbosity settings. */
extern const Verbosity& verbosity;


#endif /* VERBOSITY_H */
