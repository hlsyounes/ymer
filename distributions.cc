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

#include "distributions.h"

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
  void DoVisitExponential(const Exponential& distribution) override;
  void DoVisitWeibull(const Weibull& distribution) override;
  void DoVisitLognormal(const Lognormal& distribution) override;
  void DoVisitUniform(const Uniform& distribution) override;

  std::ostream* os_;
};

DistributionPrinter::DistributionPrinter(std::ostream* os) : os_(os) {}

void DistributionPrinter::DoVisitExponential(const Exponential& distribution) {
  *os_ << distribution.rate();
}

void DistributionPrinter::DoVisitWeibull(const Weibull& distribution) {
  *os_ << "W(" << distribution.scale() << "," << distribution.shape() << ")";
}

void DistributionPrinter::DoVisitLognormal(const Lognormal& distribution) {
  *os_ << "L(" << distribution.scale() << "," << distribution.shape() << ")";
}

void DistributionPrinter::DoVisitUniform(const Uniform& distribution) {
  *os_ << "U(" << distribution.low() << "," << distribution.high() << ")";
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const Distribution& d) {
  DistributionPrinter printer(&os);
  d.Accept(&printer);
  return os;
}

Exponential::Exponential(std::unique_ptr<const Expression>&& rate)
    : rate_(std::move(rate)) {}

std::unique_ptr<const Exponential> Exponential::New(
    std::unique_ptr<const Expression>&& rate) {
  return std::unique_ptr<const Exponential>(new Exponential(std::move(rate)));
}

void Exponential::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitExponential(*this);
}

Weibull::Weibull(std::unique_ptr<const Expression>&& scale,
                 std::unique_ptr<const Expression>&& shape)
    : scale_(std::move(scale)), shape_(std::move(shape)) {}

std::unique_ptr<const Weibull> Weibull::New(
    std::unique_ptr<const Expression>&& scale,
    std::unique_ptr<const Expression>&& shape) {
  return std::unique_ptr<const Weibull>(
      new Weibull(std::move(scale), std::move(shape)));
}

void Weibull::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitWeibull(*this);
}

Lognormal::Lognormal(std::unique_ptr<const Expression>&& scale,
                     std::unique_ptr<const Expression>&& shape)
    : scale_(std::move(scale)), shape_(std::move(shape)) {}

std::unique_ptr<const Lognormal> Lognormal::New(
    std::unique_ptr<const Expression>&& scale,
    std::unique_ptr<const Expression>&& shape) {
  return std::unique_ptr<const Lognormal>(
      new Lognormal(std::move(scale), std::move(shape)));
}

void Lognormal::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitLognormal(*this);
}

Uniform::Uniform(std::unique_ptr<const Expression>&& low,
                 std::unique_ptr<const Expression>&& high)
    : low_(std::move(low)), high_(std::move(high)) {}

std::unique_ptr<const Uniform> Uniform::New(
    std::unique_ptr<const Expression>&& low,
    std::unique_ptr<const Expression>&& high) {
  return std::unique_ptr<const Uniform>(
      new Uniform(std::move(low), std::move(high)));
}

void Uniform::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitUniform(*this);
}

DistributionVisitor::~DistributionVisitor() = default;

void DistributionVisitor::VisitExponential(const Exponential& dist) {
  DoVisitExponential(dist);
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
