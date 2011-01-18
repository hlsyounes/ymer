/* -*-C++-*- */
/*
 * Sampling-based model-checking engine.
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
#ifndef SAMPLING_H
#define SAMPLING_H

#include <config.h>
#include "verify.h"
#include "simulator.h"
#include "models.h"
#include "formulas.h"

namespace Sampling {

  /* Delta function determining half-width of indifference region. */
  typedef double (*DeltaFun)(double theta);

  /* Sampling algorithm. */
  enum Algorithm { HOEFFDING, SSSP, SPRT, NONPARAMETRIC };


  /* Sets engine parameters. */
  void set_parameters(DeltaFun delta, Algorithm algorithm, bool estimate,
		      double nested_error, bool memoization,
		      bool record_trajectories, double termination_prob);

  /* Verifies the given state formula using the statistical engine. */
  VerificationResult verify(const StateFormula& formula, const Model& model,
			    double alpha, double beta, double gamma);

  /* Generates a sample for the given path formula. */
  VerificationResult sample(const PathFormula& formula, const Model& model,
			    double alpha, double beta, double gamma);

  /* Clears memoized verification results. */
  int clear_memo();

  /* Gets the current statistics. */
  void get_statistics(double& sample_avg, double& sample_var,
		      double& path_avg, double& path_var);

  /* Clears the current statistics. */
  void clear_statistics();

} /* namespace Sampling */

#endif /* SAMPLING_H */
