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

#include <initializer_list>
#include <vector>

#include "compiled-expression.h"

// Supported distribution types.
enum class DistributionType {
  MEMORYLESS, WEIBULL, LOGNORMAL, UNIFORM
};

// A compiled distribution.
class CompiledDistribution {
 public:
  // Returns a memoryless distribution with parameter p.
  static CompiledDistribution MakeMemoryless(const CompiledExpression& p);
  // Returns a Weibull distribution with the given scale and shape parameters.
  static CompiledDistribution MakeWeibull(
      const CompiledExpression& scale, const CompiledExpression& shape);
  // Returns a lognormal distribution with the given scale and shape parameters.
  static CompiledDistribution MakeLognormal(
      const CompiledExpression& scale, const CompiledExpression& shape);
  // Returns a uniform distribution over the interval [low, high).
  static CompiledDistribution MakeUniform(
      const CompiledExpression& low, const CompiledExpression& high);

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

#endif  // COMPILED_DISTRIBUTION_H_
