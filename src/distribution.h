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
//
// A class hierarchy for representing distributions that supports the visitor
// pattern.

#ifndef DISTRIBUTIONS_H_
#define DISTRIBUTIONS_H_

#include <memory>
#include <ostream>

#include "src/expression.h"

class DistributionVisitor;

// Abstract base class for probability distributions.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcreteDistributionVisitor final : public DistributionVisitor {
//     ...
//   };
//
//   Distribution* dist = ...;
//   ConcreteDistributionVisitor visitor;
//   dist->Accept(&visitor);
//
class Distribution {
 public:
  virtual ~Distribution();

  void Accept(DistributionVisitor* visitor) const;

 private:
  virtual void DoAccept(DistributionVisitor* visitor) const = 0;
};

// Output operator for distributions.
std::ostream& operator<<(std::ostream& os, const Distribution& d);

// A memoryless distribution.
class Memoryless : public Distribution {
 public:
  // Constructs a memoryless distribution with the given weight (probability for
  // discrete-time model; rate for continuous-time model).
  explicit Memoryless(std::unique_ptr<const Expression>&& weight);

  // Returns the weight of this memoryless distribution.
  const Expression& weight() const { return *weight_; }

 private:
  virtual void DoAccept(DistributionVisitor* visitor) const;

  std::unique_ptr<const Expression> weight_;
};

// A Weibull distribution.
class Weibull : public Distribution {
 public:
  // Constructs a Weibull distribution with the given scale and shape.
  explicit Weibull(std::unique_ptr<const Expression>&& scale,
                   std::unique_ptr<const Expression>&& shape);

  // Returns the scale of this Weibull distribution.
  const Expression& scale() const { return *scale_; }

  // Returns the shape of this Weibull distribution.
  const Expression& shape() const { return *shape_; }

 private:
  void DoAccept(DistributionVisitor* visitor) const override;

  std::unique_ptr<const Expression> scale_;
  std::unique_ptr<const Expression> shape_;
};

// A lognormal distribution.
class Lognormal : public Distribution {
 public:
  // Constructs a lognormal distribution with the given scale and shape.
  explicit Lognormal(std::unique_ptr<const Expression>&& scale,
                     std::unique_ptr<const Expression>&& shape);

  // Returns the scale of this lognormal distribution.
  const Expression& scale() const { return *scale_; }

  // Returns the shape of this lognormal distribution.
  const Expression& shape() const { return *shape_; }

 private:
  void DoAccept(DistributionVisitor* visitor) const override;

  std::unique_ptr<const Expression> scale_;
  std::unique_ptr<const Expression> shape_;
};

// A uniform distribution.
class Uniform : public Distribution {
 public:
  // Constructs a uniform distribution with the given bounds.
  explicit Uniform(std::unique_ptr<const Expression>&& low,
                   std::unique_ptr<const Expression>&& high);

  // Returns the lower bound of this uniform distribution.
  const Expression& low() const { return *low_; }

  // Returns the upper bound of this uniform distribution.
  const Expression& high() const { return *high_; }

 private:
  void DoAccept(DistributionVisitor* visitor) const override;

  std::unique_ptr<const Expression> low_;
  std::unique_ptr<const Expression> high_;
};

// Abstract base class for distribution visitors.
class DistributionVisitor {
 public:
  void VisitMemoryless(const Memoryless& dist);
  void VisitWeibull(const Weibull& dist);
  void VisitLognormal(const Lognormal& dist);
  void VisitUniform(const Uniform& dist);

 protected:
  ~DistributionVisitor();

 private:
  virtual void DoVisitMemoryless(const Memoryless& dist) = 0;
  virtual void DoVisitWeibull(const Weibull& dist) = 0;
  virtual void DoVisitLognormal(const Lognormal& dist) = 0;
  virtual void DoVisitUniform(const Uniform& dist) = 0;
};

#endif  // DISTRIBUTIONS_H_
