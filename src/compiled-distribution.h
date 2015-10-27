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
#include <random>
#include <vector>

#include "glog/logging.h"

// Supported compiled GSMP distribution types.
enum class CompiledGsmpDistributionType { WEIBULL, LOGNORMAL, UNIFORM };

// A compiled GSMP distribution.
class CompiledGsmpDistribution {
 public:
  // Returns a Weibull distribution with the given scale and shape parameters.
  static CompiledGsmpDistribution MakeWeibull(double scale, double shape);
  // Returns a lognormal distribution with the given scale and shape parameters.
  static CompiledGsmpDistribution MakeLognormal(double scale, double shape);
  // Returns a uniform distribution over the interval [low, high).
  static CompiledGsmpDistribution MakeUniform(double low, double high);

  // Returns the type for this distribution.
  CompiledGsmpDistributionType type() const { return type_; }

  // Returns the parameters for this distribution.
  const std::vector<double>& parameters() const { return parameters_; }

 private:
  // Constructs a compiled GSMP distribution with the given type and parameters.
  CompiledGsmpDistribution(CompiledGsmpDistributionType type,
                           std::initializer_list<double> parameters);

  CompiledGsmpDistributionType type_;
  std::vector<double> parameters_;
};

// A sampler for compiled distributions.
template <typename Engine>
class CompiledDistributionSampler {
 public:
  // Constructs a sampler for compiled distributions.
  explicit CompiledDistributionSampler(Engine* engine);

  // Generates a sample for the given compiled distribution.
  double Sample(const CompiledGsmpDistribution& dist,
                const std::vector<int>& state);

  double StandardUniform();
  double Exponential(double lambda);

 private:
  std::uniform_real_distribution<> standard_uniform_;
  Engine* engine_;
  bool has_unused_lognormal_;
  double unused_lognormal_;
};

template <typename Engine>
CompiledDistributionSampler<Engine>::CompiledDistributionSampler(Engine* engine)
    : engine_(engine), has_unused_lognormal_(false) {}

template <typename Engine>
double CompiledDistributionSampler<Engine>::Sample(
    const CompiledGsmpDistribution& dist, const std::vector<int>& state) {
  switch (dist.type()) {
    case CompiledGsmpDistributionType::WEIBULL: {
      double eta = dist.parameters()[0];
      double beta = dist.parameters()[1];
      return eta * pow(-log(1.0 - StandardUniform()), 1.0 / beta);
    }
    case CompiledGsmpDistributionType::LOGNORMAL: {
      if (has_unused_lognormal_) {
        has_unused_lognormal_ = false;
        return unused_lognormal_;
      }
      // Generate two N(0,1) samples using the Box-Muller transform.
      double mu = dist.parameters()[0];
      double sigma = dist.parameters()[1];
      double mean = log(mu) - 0.5 * sigma * sigma;
      double u1 = 1.0 - StandardUniform();
      double u2 = 1.0 - StandardUniform();
      double tmp = sqrt(-2.0 * log(u2));
      double x1 = tmp * cos(2 * M_PI * u1);
      double x2 = tmp * sin(2 * M_PI * u1);
      unused_lognormal_ = exp(x2 * sigma + mean);
      has_unused_lognormal_ = true;
      return exp(x1 * sigma + mean);
    }
    case CompiledGsmpDistributionType::UNIFORM: {
      double a = dist.parameters()[0];
      double b = dist.parameters()[1];
      return (b - a) * StandardUniform() + a;
    }
  }
  LOG(FATAL) << "bad distribution type";
}

template <typename Engine>
double CompiledDistributionSampler<Engine>::StandardUniform() {
  return standard_uniform_(*engine_);
}

template <typename Engine>
double CompiledDistributionSampler<Engine>::Exponential(double lambda) {
  return -log(1.0 - StandardUniform()) / lambda;
}

#endif  // COMPILED_DISTRIBUTION_H_
