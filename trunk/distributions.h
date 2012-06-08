/* -*-C++-*- */
/*
 * Probability distributions.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
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
 *
 * $Id: distributions.h,v 4.1 2005-02-01 14:01:41 lorens Exp $
 */
#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <config.h>
#include "src/expression.h"
#include "rng.h"
#include <iostream>
#include <vector>


/* ====================================================================== */
/* ECParameters */

/*
 * Parameters defining an acyclic continuous phase-type (ACPH)
 * distribution in the class of EC distributions with p=1.
 */
struct ECParameters {
  /* Number of phases of this EC distribution. */
  size_t n;
  /* Rate associated with phases of the Erlang part of this EC
     distribution. */
  double re;
  /* Rate associated with the first phase of the two-phase Coxian part
     of this EC distribution. */
  double rc1;
  /* Rate associated with the second phase of the two-phase Coxian
     part of this EC distribution. */
  double rc2;
  /* Probability of transitioning to from the first phase to the
     second phase of the two-phase Coxian part of this EC distribution
     (1-pc is the probability of bypassing the second phase). */
  double pc;
};


/* ====================================================================== */
/* ACPH2Parameters */

/*
 * Parameters defining an acyclic continuous phase-type (ACPH)
 * distribution that is either a Cox-2 distribution or a generalized
 * Erlang distribution.
 */
struct ACPH2Parameters {
  /* Number of phases of this distribution. */
  size_t n;
  /* Rate associated with either the first phase (Cox-2) or all phases
     (Erlang). */
  double r1;
  /* Rate associated with the second phase. */
  double r2;
  /* Probability of transitioning from the first phase to the second
     phase. */
  double p;
};


/* ====================================================================== */
/* Distribution */

class DistributionVisitor;

// Abstract base class for probability distributions.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcreteDistributionVisitor : public DistributionVisitor {
//     ...
//   };
//
//   Distribution* dist = ...;
//   ConcreteDistributionVisitor visitor;
//   dist->Accept(&visitor);
//
class Distribution {
 public:
  /* An id-specific random number generator, or NULL. */
  static mt_struct* mts;

  /* Deletes this distribution. */
  virtual ~Distribution();

  void Accept(DistributionVisitor* visitor) const;

  /* Provides the parameters for an acyclic continuous phase-type
     (ACPH) distribution in the class of EC distributions matching the
     first three moments of this distribution. */
  void acph(ECParameters& params) const;

  /* Provides the parameters for an acyclic continuous phase-type
     (ACPH) distribution matching the first two moments of this
     distribution. */
  void acph2(ACPH2Parameters& params) const;

  /* Tests if this a memoryless distribution. */
  virtual bool memoryless() const { return false; }

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, size_t n) const = 0;

  /* Returns a sample drawn from this distribution. */
  virtual double sample(const std::vector<int>& state) const = 0;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Distribution* substitution(
      const std::map<std::string, const Variable*>& substitutions) const = 0;

protected:
  /* Constructs a distribution. */
  Distribution();

private:
  // Disallow copy and assign.
  Distribution(const Distribution&);
  Distribution& operator=(const Distribution&);

  virtual void DoAccept(DistributionVisitor* visitor) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const Distribution& d);
};

/* Output operator for distributions. */
std::ostream& operator<<(std::ostream& os, const Distribution& d);


/* ====================================================================== */
/* Exponential */

/*
 * An exponential distribution.
 */
class Exponential : public Distribution {
 public:
  /* Returns an exponential distribution with the given rate. */
  static const Exponential* make(const Expression& rate);

  /* Deletes this exponential distribution. */
  virtual ~Exponential();

  /* Returns the rate of this exponential distribution. */
  const Expression& rate() const { return *rate_; }

  /* Tests if this a memoryless distribution. */
  virtual bool memoryless() const { return true; }

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, size_t n) const;

  /* Returns a sample drawn from this distribution. */
  virtual double sample(const std::vector<int>& state) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Exponential* substitution(
      const std::map<std::string, const Variable*>& substitutions) const;

private:
  /* Constructs an exponential distribution with the given rate. */
  Exponential(const Expression& rate);

  virtual void DoAccept(DistributionVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The rate of this exponential distribution. */
  const Expression* rate_;
};


/* ====================================================================== */
/* Weibull */

/*
 * A Weibull distribution.
 */
class Weibull : public Distribution {
 public:
  /* Returns a Weibull distribution with the given scale and shape. */
  static const Distribution* make(const Expression& scale,
				  const Expression& shape);

  /* Deletes this Weibull distribution. */
  virtual ~Weibull();

  /* Returns the scale of this Weibull distribution. */
  const Expression& scale() const { return *scale_; }

  /* Returns the shape of this Weibull distribution. */
  const Expression& shape() const { return *shape_; }

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, size_t n) const;

  /* Returns a sample drawn from this distribution. */
  virtual double sample(const std::vector<int>& state) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Distribution* substitution(
      const std::map<std::string, const Variable*>& substitutions) const;

private:
  /* Constructs a Weibull distribution with the given scale and shape. */
  Weibull(const Expression& scale, const Expression& shape);

  virtual void DoAccept(DistributionVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The scale of this Weibull distribution. */
  const Expression* scale_;
  /* The shape of this Weibull distribution. */
  const Expression* shape_;
};


/* ====================================================================== */
/* Lognormal */

/*
 * A lognormal distribution.
 */
class Lognormal : public Distribution {
 public:
  /* Returns a lognormal distribution with the given scale and shape. */
  static const Lognormal* make(const Expression& scale,
			       const Expression& shape);

  /* Deletes this lognormal distribution. */
  virtual ~Lognormal();

  /* Returns the scale of this lognormal distribution. */
  const Expression& scale() const { return *scale_; }

  /* Returns the shape of this lognormal distribution. */
  const Expression& shape() const { return *shape_; }

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, size_t n) const;

  /* Returns a sample drawn from this distribution. */
  virtual double sample(const std::vector<int>& state) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Lognormal* substitution(
      const std::map<std::string, const Variable*>& substitutions) const;

private:
  /* Constructs a lognormal distribution with the given scale and shape. */
  Lognormal(const Expression& scale, const Expression& shape);

  virtual void DoAccept(DistributionVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The scale of this lognormal distribution. */
  const Expression* scale_;
  /* The shape of this lognormal distribution. */
  const Expression* shape_;
  /* Whether there is an unused sample available. */
  mutable bool have_unused_;
  /* An unused sample. */
  mutable double unused_;
};


/* ====================================================================== */
/* Uniform */

/*
 * A uniform distribution.
 */
class Uniform : public Distribution {
 public:
  /* Returns a uniform distribution with the bounds. */
  static const Uniform* make(const Expression& low, const Expression& high);

  /* Deletes this uniform distribution. */
  virtual ~Uniform();

  /* Returns the lower bound of this uniform distribution. */
  const Expression& low() const { return *low_; }

  /* Returns the upper bound of this uniform distribution. */
  const Expression& high() const { return *high_; }

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, size_t n) const;

  /* Returns a sample drawn from this distribution. */
  virtual double sample(const std::vector<int>& state) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Uniform* substitution(
      const std::map<std::string, const Variable*>& substitutions) const;

private:
  /* Constructs a uniform distribution with the given bounds. */
  Uniform(const Expression& low, const Expression& high);

  virtual void DoAccept(DistributionVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The lower bound of this uniform distribution. */
  const Expression* low_;
  /* The upper bound of this uniform distribution. */
  const Expression* high_;
};

// Abstract base class for distribution visitors.
class DistributionVisitor {
 public:
  void VisitExponential(const Exponential& dist);
  void VisitWeibull(const Weibull& dist);
  void VisitLognormal(const Lognormal& dist);
  void VisitUniform(const Uniform& dist);

 protected:
  DistributionVisitor();
  DistributionVisitor(const DistributionVisitor&);
  DistributionVisitor& operator=(const DistributionVisitor&);
  ~DistributionVisitor();

 private:
  virtual void DoVisitExponential(const Exponential& dist) = 0;
  virtual void DoVisitWeibull(const Weibull& dist) = 0;
  virtual void DoVisitLognormal(const Lognormal& dist) = 0;
  virtual void DoVisitUniform(const Uniform& dist) = 0;
};

#endif /* DISTRIBUTIONS_H */
