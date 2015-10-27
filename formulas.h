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

#include "src/compiled-expression.h"
#include "src/compiled-model.h"
#include "src/compiled-property.h"
#include "src/ddmodel.h"
#include "src/ddutil.h"
#include "src/model-checking-params.h"
#include "src/simulator.h"
#include "src/statistics.h"

struct ModelCheckingStats {
  explicit ModelCheckingStats(bool populate_distribution)
      : time(populate_distribution),
        sample_size(populate_distribution),
        sample_cache_size(populate_distribution),
        path_length(populate_distribution),
        path_length_accept(populate_distribution),
        path_length_reject(populate_distribution),
        path_length_terminate(populate_distribution) {}

  Sample<double> time;
  Sample<int> sample_size;
  Sample<int> sample_cache_size;
  Sample<int> path_length;
  Sample<int> path_length_accept;
  Sample<int> path_length_reject;
  Sample<int> path_length_terminate;
};

bool Verify(const CompiledProperty& property, const CompiledModel& model,
            const DecisionDiagramModel* dd_model,
            const ModelCheckingParams& params, const State& state,
            std::vector<CompiledExpressionEvaluator>* evaluators,
            std::vector<CompiledDistributionSampler<std::mt19937_64>>* samplers,
            ModelCheckingStats* stats);

BDD Verify(const CompiledProperty& property,
           const DecisionDiagramModel& dd_model, bool top_level_property,
           double epsilon);

BDD VerifyExistsUntil(const DecisionDiagramModel& dd_model, const BDD& pre,
                      const BDD& post);

#endif /* FORMULAS_H */
