/*
 * Formulas.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011--2012 Google Inc
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

#ifndef FORMULAS_H
#define FORMULAS_H

#include "models.h"
#include "states.h"
#include "src/compiled-expression.h"
#include "src/compiled-model.h"
#include "src/compiled-property.h"
#include "src/ddutil.h"
#include "src/rng.h"
#include "src/statistics.h"

/* Sampling algorithm. */
enum SamplingAlgorithm { ESTIMATE, SSP, SPRT, FIXED };

struct ModelCheckingParams {
  ModelCheckingParams();

  double alpha;
  double beta;
  double delta;
  double epsilon;
  SamplingAlgorithm algorithm;
  int fixed_sample_size;
  size_t max_path_length;
  double nested_error;
  bool memoization;
};

struct ModelCheckingStats {
  Sample<double> time;
  Sample<int> sample_size;
  Sample<int> sample_cache_size;
  Sample<size_t> path_length;
};

bool Verify(const CompiledProperty& property, const CompiledModel& model,
            const DecisionDiagramModel* dd_model,
            const ModelCheckingParams& params,
            CompiledExpressionEvaluator* evaluator,
            CompiledDistributionSampler<DCEngine>* sampler, const State& state,
            ModelCheckingStats* stats);

BDD Verify(const CompiledProperty& property,
           const DecisionDiagramModel& dd_model, bool estimate,
           bool top_level_property, double epsilon);

bool GetObservation(const CompiledPathProperty& property,
                    const CompiledModel& model,
                    const DecisionDiagramModel* dd_model,
                    const ModelCheckingParams& params,
                    CompiledExpressionEvaluator* evaluator,
                    CompiledDistributionSampler<DCEngine>* sampler,
                    const State& state, ModelCheckingStats* stats);

#endif /* FORMULAS_H */
