// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2015 Google Inc
//
// This file is part of Ymer.
//
// Ymer is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Ymer is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ymer; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Model checking parameters.

#ifndef MODEL_CHECKING_PARAMS_H_
#define MODEL_CHECKING_PARAMS_H_

// Model checking eninges.
enum class ModelCheckingEngine { SAMPLING, HYBRID, MIXED };

// Hypothesis testing algorithms.
enum class ThresholdAlgorithm { FIXED, SSP, SPRT, CHOW_ROBBINS };

// Estimation algorithm.
enum class EstimationAlgorithm { FIXED, CHOW_ROBBINS };

// Model checking parameters.
struct ModelCheckingParams {
  double alpha;
  double beta;
  double delta;
  double epsilon;
  double termination_probability;
  ModelCheckingEngine engine;
  ThresholdAlgorithm threshold_algorithm;
  EstimationAlgorithm estimation_algorithm;
  int fixed_sample_size;
  int max_path_length;
  double nested_error;
  bool memoization;
};

#endif  // MODEL_CHECKING_PARAMS_H_
