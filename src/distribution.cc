// Copyright (C) 2003--2005 Carnegie Mellon University
// Copyright (C) 2011--2014 Google Inc
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

#include "distribution.h"

Distribution::~Distribution() = default;

void Distribution::Accept(DistributionVisitor* visitor) const {
  DoAccept(visitor);
}

namespace {

// A distribution visitor that prints a distribution to an output stream.
class DistributionPrinter : public DistributionVisitor {
 public:
  explicit DistributionPrinter(std::ostream* os);

 private:
  void DoVisitMemoryless(const Memoryless& distribution) override;
  void DoVisitWeibull(const Weibull& distribution) override;
  void DoVisitLognormal(const Lognormal& distribution) override;
  void DoVisitUniform(const Uniform& distribution) override;

  std::ostream* os_;
};

DistributionPrinter::DistributionPrinter(std::ostream* os) : os_(os) {}

void DistributionPrinter::DoVisitMemoryless(const Memoryless& distribution) {
  *os_ << distribution.weight();
}

void DistributionPrinter::DoVisitWeibull(const Weibull& distribution) {
  *os_ << "W(" << distribution.scale() << ", " << distribution.shape() << ")";
}

void DistributionPrinter::DoVisitLognormal(const Lognormal& distribution) {
  *os_ << "L(" << distribution.scale() << ", " << distribution.shape() << ")";
}

void DistributionPrinter::DoVisitUniform(const Uniform& distribution) {
  *os_ << "U(" << distribution.low() << ", " << distribution.high() << ")";
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const Distribution& d) {
  DistributionPrinter printer(&os);
  d.Accept(&printer);
  return os;
}

Memoryless::Memoryless(std::unique_ptr<const Expression>&& weight)
    : weight_(std::move(weight)) {}

void Memoryless::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitMemoryless(*this);
}

Weibull::Weibull(std::unique_ptr<const Expression>&& scale,
                 std::unique_ptr<const Expression>&& shape)
    : scale_(std::move(scale)), shape_(std::move(shape)) {}

void Weibull::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitWeibull(*this);
}

Lognormal::Lognormal(std::unique_ptr<const Expression>&& scale,
                     std::unique_ptr<const Expression>&& shape)
    : scale_(std::move(scale)), shape_(std::move(shape)) {}

void Lognormal::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitLognormal(*this);
}

Uniform::Uniform(std::unique_ptr<const Expression>&& low,
                 std::unique_ptr<const Expression>&& high)
    : low_(std::move(low)), high_(std::move(high)) {}

void Uniform::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitUniform(*this);
}

DistributionVisitor::~DistributionVisitor() = default;

void DistributionVisitor::VisitMemoryless(const Memoryless& dist) {
  DoVisitMemoryless(dist);
}

void DistributionVisitor::VisitWeibull(const Weibull& dist) {
  DoVisitWeibull(dist);
}

void DistributionVisitor::VisitLognormal(const Lognormal& dist) {
  DoVisitLognormal(dist);
}

void DistributionVisitor::VisitUniform(const Uniform& dist) {
  DoVisitUniform(dist);
}
