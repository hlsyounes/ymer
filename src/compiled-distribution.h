// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2012 Google Inc
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
// A compiled distribution.

#ifndef COMPILED_DISTRIBUTION_H_
#define COMPILED_DISTRIBUTION_H_

#include <cmath>
#include <initializer_list>
#include <vector>

#include "compiled-expression.h"
#include "rng.h"

// Supported distribution types.
enum class DistributionType { MEMORYLESS, WEIBULL, LOGNORMAL, UNIFORM };

// A compiled distribution.
class CompiledDistribution {
 public:
  // Returns a memoryless distribution with parameter p.
  static CompiledDistribution MakeMemoryless(const CompiledExpression& p);
  // Returns a Weibull distribution with the given scale and shape parameters.
  static CompiledDistribution MakeWeibull(const CompiledExpression& scale,
                                          const CompiledExpression& shape);
  // Returns a lognormal distribution with the given scale and shape parameters.
  static CompiledDistribution MakeLognormal(const CompiledExpression& scale,
                                            const CompiledExpression& shape);
  // Returns a uniform distribution over the interval [low, high).
  static CompiledDistribution MakeUniform(const CompiledExpression& low,
                                          const CompiledExpression& high);

  // Returns the type for this distribution.
  DistributionType type() const { return type_; }

  // Returns the parameters for this distribution.
  const std::vector<CompiledExpression>& parameters() const {
    return parameters_;
  }

 private:
  // Constructs a compiled distribution with the given type and parameters.
  CompiledDistribution(DistributionType type,
                       std::initializer_list<CompiledExpression> parameters);

  DistributionType type_;
  std::vector<CompiledExpression> parameters_;
};

// A sampler for compiled distributions.
template <typename Engine>
class CompiledDistributionSampler {
 public:
  // Constructs a sampler for compiled distributions.
  CompiledDistributionSampler(CompiledExpressionEvaluator* evaluator,
                              Engine* engine);

  // Generates a sample for the given compiled distribution.
  double Sample(const CompiledDistribution& dist,
                const std::vector<int>& state);

  double StandardUniform();
  double Exponential(double lambda);

 private:
  CompiledExpressionEvaluator* evaluator_;
  Engine* engine_;
  bool has_unused_lognormal_;
  double unused_lognormal_;
};

template <typename Engine>
CompiledDistributionSampler<Engine>::CompiledDistributionSampler(
    CompiledExpressionEvaluator* evaluator, Engine* engine)
    : evaluator_(evaluator), engine_(engine), has_unused_lognormal_(false) {}

template <typename Engine>
double CompiledDistributionSampler<Engine>::Sample(
    const CompiledDistribution& dist, const std::vector<int>& state) {
  double sample = 0.0;
  switch (dist.type()) {
    case DistributionType::MEMORYLESS: {
      sample = Exponential(
          evaluator_->EvaluateDoubleExpression(dist.parameters()[0], state));
      break;
    }
    case DistributionType::WEIBULL: {
      double eta =
          evaluator_->EvaluateDoubleExpression(dist.parameters()[0], state);
      double beta =
          evaluator_->EvaluateDoubleExpression(dist.parameters()[1], state);
      sample = eta * pow(-log(1.0 - StandardUniform()), 1.0 / beta);
      break;
    }
    case DistributionType::LOGNORMAL: {
      if (has_unused_lognormal_) {
        has_unused_lognormal_ = false;
        return unused_lognormal_;
      }
      // Generate two N(0,1) samples using the Box-Muller transform.
      double mu =
          evaluator_->EvaluateDoubleExpression(dist.parameters()[0], state);
      double sigma =
          evaluator_->EvaluateDoubleExpression(dist.parameters()[1], state);
      double mean = log(mu) - 0.5 * sigma * sigma;
      double u1 = 1.0 - StandardUniform();
      double u2 = 1.0 - StandardUniform();
      double tmp = sqrt(-2.0 * log(u2));
      double x1 = tmp * cos(2 * M_PI * u1);
      double x2 = tmp * sin(2 * M_PI * u1);
      unused_lognormal_ = exp(x2 * sigma + mean);
      has_unused_lognormal_ = true;
      sample = exp(x1 * sigma + mean);
      break;
    }
    case DistributionType::UNIFORM: {
      double a =
          evaluator_->EvaluateDoubleExpression(dist.parameters()[0], state);
      double b =
          evaluator_->EvaluateDoubleExpression(dist.parameters()[1], state);
      sample = (b - a) * StandardUniform() + a;
      break;
    }
  }
  return sample;
}

template <typename Engine>
double CompiledDistributionSampler<Engine>::StandardUniform() {
  return ::StandardUniform(*engine_);
}

template <typename Engine>
double CompiledDistributionSampler<Engine>::Exponential(double lambda) {
  return -log(1.0 - StandardUniform()) / lambda;
}

#endif  // COMPILED_DISTRIBUTION_H_
