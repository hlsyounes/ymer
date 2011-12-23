/* -*-C++-*- */
/*
 * Probability distributions.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011 Google Inc
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
#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <config.h>
#include "expressions.h"
#include "refcount.h"
#include <iostream>
#include <vector>


/* Inverse error function. */
double erfinv(double y);


/* ====================================================================== */
/* Distribution */

/*
 * A probability distribution.
 */
struct Distribution : public RCObject {
  /* Seeds a stream of pseudo-random numbers. */
  static void seed(unsigned long s);

  /* Seeds a process-specific stream of pseudo-random numbers. */
  static void seed(unsigned short id, unsigned long s);

  /* Returns a sample from the semi-open interval [0,1). */
  static double rand01ex();

  /* Returns a sample drawn from this distribution. */
  double sample(const Values& values) const;

  /* Returns a sample drawn from this distribution. */
  double sample(double t0, const Values& values) const;

  /* Tests if this distribution is memoryless. */
  virtual bool memoryless() const { return false; }

  /* Tests if this is a state-invariant distribution. */
  virtual bool state_invariant() const = 0;

  /* Probability density function for this distribution. */
  virtual double pdf(double t, const Values& values) const = 0;

  /* Conditional probability density function for this distribution. */
  double pdf(double t, double t0, const Values& values) const;

  /* Cumulative distribution function for this distribution. */
  virtual double cdf(double t, const Values& values) const = 0;

  /* Conditional cumulative distribution function for this distribution. */
  double cdf(double t, double t0, const Values& values) const;

  /* Inverse cumulative distribution function for this distribution. */
  virtual double inv(double p, const Values& values) const = 0;

  /* Conditional inverse cumulative distribution function for this
     distribution. */
  double inv(double p, double t0, const Values& values) const;

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, int n) const = 0;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Distribution&
  substitution(const Substitutions& subst) const = 0;

protected:
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
struct Exponential : public Distribution {
  /* The standard exponential distribution: Exp(1). */
  static const Exponential STANDARD;
 
 /* Returns an exponential distribution with the given rate. */
  static const Exponential& make(const Expression<double>& rate);

  /* Deletes this exponential distribution. */
  virtual ~Exponential();

  /* Returns the rate of this exponential distribution. */
  const Expression<double>& rate() const { return *rate_; }

  /* Tests if this distribution is memoryless. */
  virtual bool memoryless() const { return true; }

  /* Tests if this is a state-invariant distribution. */
  virtual bool state_invariant() const;

  /* Probability density function for this distribution. */
  virtual double pdf(double t, const Values& values) const;

  /* Cumulative distribution function for this distribution. */
  virtual double cdf(double t, const Values& values) const;

  /* Inverse cumulative distribution function for this distribution. */
  virtual double inv(double p, const Values& values) const;

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, int n) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Exponential& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The rate of this exponential distribution. */
  const Expression<double>* rate_;

  /* Constructs an exponential distribution with rate 1. */
  Exponential();

  /* Constructs an exponential distribution with the given rate. */
  Exponential(const Expression<double>& rate);
};


/* ====================================================================== */
/* Weibull */

/*
 * A Weibull distribution.
 */
struct Weibull : public Distribution {
  /* Returns a Weibull distribution with the given scale and shape. */
  static const Distribution& make(const Expression<double>& scale,
				  const Expression<double>& shape);

  /* Deletes this Weibull distribution. */
  virtual ~Weibull();

  /* Returns the scale of this Weibull distribution. */
  const Expression<double>& scale() const { return *scale_; }

  /* Returns the shape of this Weibull distribution. */
  const Expression<double>& shape() const { return *shape_; }

  /* Tests if this is a state-invariant distribution. */
  virtual bool state_invariant() const;

  /* Probability density function for this distribution. */
  virtual double pdf(double t, const Values& values) const;

  /* Cumulative distribution function for this distribution. */
  virtual double cdf(double t, const Values& values) const;

  /* Inverse cumulative distribution function for this distribution. */
  virtual double inv(double p, const Values& values) const;

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, int n) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Weibull& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The scale of this Weibull distribution. */
  const Expression<double>* scale_;
  /* The shape of this Weibull distribution. */
  const Expression<double>* shape_;

  /* Constructs a Weibull distribution with the given scale and shape. */
  Weibull(const Expression<double>& scale, const Expression<double>& shape);
};


/* ====================================================================== */
/* Lognormal */

/*
 * A lognormal distribution.
 */
struct Lognormal : public Distribution {
  /* Returns a lognormal distribution with the given scale and shape. */
  static const Lognormal& make(const Expression<double>& scale,
			       const Expression<double>& shape);

  /* Deletes this lognormal distribution. */
  virtual ~Lognormal();

  /* Returns the scale of this lognormal distribution. */
  const Expression<double>& scale() const { return *scale_; }

  /* Returns the shape of this lognormal distribution. */
  const Expression<double>& shape() const { return *shape_; }

  /* Tests if this is a state-invariant distribution. */
  virtual bool state_invariant() const;

  /* Probability density function for this distribution. */
  virtual double pdf(double t, const Values& values) const;

  /* Cumulative distribution function for this distribution. */
  virtual double cdf(double t, const Values& values) const;

  /* Inverse cumulative distribution function for this distribution. */
  virtual double inv(double p, const Values& values) const;

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, int n) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Lognormal& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The scale of this lognormal distribution. */
  const Expression<double>* scale_;
  /* The shape of this lognormal distribution. */
  const Expression<double>* shape_;

  /* Constructs a lognormal distribution with the given scale and shape. */
  Lognormal(const Expression<double>& scale, const Expression<double>& shape);
};


/* ====================================================================== */
/* Uniform */

/*
 * A uniform distribution.
 */
struct Uniform : public Distribution {
  /* Returns a uniform distribution with the bounds. */
  static const Uniform& make(const Expression<double>& low,
			     const Expression<double>& high);

  /* Deletes this uniform distribution. */
  virtual ~Uniform();

  /* Returns the lower bound of this uniform distribution. */
  const Expression<double>& low() const { return *low_; }

  /* Returns the upper bound of this uniform distribution. */
  const Expression<double>& high() const { return *high_; }

  /* Tests if this is a state-invariant distribution. */
  virtual bool state_invariant() const;

  /* Probability density function for this distribution. */
  virtual double pdf(double t, const Values& values) const;

  /* Cumulative distribution function for this distribution. */
  virtual double cdf(double t, const Values& values) const;

  /* Inverse cumulative distribution function for this distribution. */
  virtual double inv(double p, const Values& values) const;

  /* Fills the provided list with the first n moments of this distribution. */
  virtual void moments(std::vector<double>& m, int n) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Uniform& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The lower bound of this uniform distribution. */
  const Expression<double>* low_;
  /* The upper bound of this uniform distribution. */
  const Expression<double>* high_;

  /* Constructs a uniform distribution with the given bounds. */
  Uniform(const Expression<double>& low, const Expression<double>& high);
};


#endif /* DISTRIBUTIONS_H */
