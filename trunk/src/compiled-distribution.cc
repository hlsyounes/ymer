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

#include "compiled-distribution.h"

#include <vector>

#include "compiled-expression.h"

CompiledDistribution CompiledDistribution::MakeMemoryless(
    const CompiledExpression& p) {
  return CompiledDistribution(DistributionType::MEMORYLESS, { p });
}

CompiledDistribution CompiledDistribution::MakeWeibull(
    const CompiledExpression& scale, const CompiledExpression& shape) {
  return CompiledDistribution(DistributionType::WEIBULL, { scale, shape });
}

CompiledDistribution CompiledDistribution::MakeLognormal(
    const CompiledExpression& scale, const CompiledExpression& shape) {
  return CompiledDistribution(DistributionType::LOGNORMAL, { scale, shape });
}

CompiledDistribution CompiledDistribution::MakeUniform(
    const CompiledExpression& low, const CompiledExpression& high) {
  return CompiledDistribution(DistributionType::UNIFORM, { low, high });
}

CompiledDistribution::CompiledDistribution(
    DistributionType type,
    std::initializer_list<CompiledExpression> parameters)
    : type_(type), parameters_(parameters) {
}
