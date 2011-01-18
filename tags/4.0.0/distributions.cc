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
#include "distributions.h"
#include "rng.h"
#include "gsl/gsl_cdf.h"
#include <cmath>
#include <cstdlib>
#include <stdexcept>


/* ====================================================================== */
/* Distribution */

/* A process-specific pseudo-random number generator, or 0. */
static mt_struct* mts = 0;
/* Whether mts ever has been set. */
bool mts_set = false;


/* Cleanup of mts on program exit. */
static void free_mts() {
  if (mts != 0) {
    free_mt_struct(mts);
    mts = 0;
  }
}


/* Seeds a stream of pseudo-random numbers. */
void Distribution::seed(unsigned long s) {
  if (mts != 0) {
    free_mt_struct(mts);
    mts = 0;
  }
  init_genrand(s);
}


/* Seeds a process-specific stream of pseudo-random numbers. */
void Distribution::seed(unsigned short id, unsigned long s) {
  if (!mts_set) {
    atexit(free_mts);
    mts_set = true;
  } else if (mts != 0) {
    free_mt_struct(mts);
  }
  mts = get_mt_parameter_id(id);
  init_genrand_id(s, mts);
}


/* Returns a sample from the semi-open interval [0,1). */
double Distribution::rand01ex() {
  return genrand_real2_id(mts);
}


/* Returns a sample drawn from this distribution. */
double Distribution::sample(const Values& values) const {
  return inv(genrand_real3_id(mts), values);
}


/* Returns a sample drawn from this distribution. */
double Distribution::sample(double t0, const Values& values) const {
  return inv(genrand_real3_id(mts), t0, values);
}


/* Conditional probability density function for this distribution. */
double Distribution::pdf(double t, double t0, const Values& values) const {
  double p0 = cdf(t0, values);
  if (p0 >= 1.0) {
    throw std::out_of_range("cdf(t0) is 1 in pdf(t, t0)");
  } else if (t <= 0.0) {
    return 0.0;
  } else {
    return pdf(t + t0, values)/(1.0 - p0);
  }
}


/* Conditional cumulative distribution function for this distribution. */
double Distribution::cdf(double t, double t0, const Values& values) const {
  double p0 = cdf(t0, values);
  if (p0 >= 1.0) {
    throw std::out_of_range("cdf(t0) is 1 in cdf(t, t0)");
  } else if (t <= 0) {
    return 0.0;
  } else {
    return 1.0 - (1.0 - cdf(t + t0, values))/(1.0 - p0);
  }
}


/* Conditional inverse cumulative distribution function for this
   distribution. */
double Distribution::inv(double p, double t0, const Values& values) const {
  double p0 = cdf(t0, values);
  if (p0 >= 1.0) {
    throw std::out_of_range("cdf(t0) is 1 in inv(t, t0)");
  } else if (p < 0.0 || p > 1.0) {
    throw std::out_of_range("p < 0 or p > 1 in inv(p, t0)");
  } else {
    return inv(1.0 - (1.0 - p)*(1.0 - p0), values) - t0;
  }
}


/* Output operator for distributions. */
std::ostream& operator<<(std::ostream& os, const Distribution& d) {
  d.print(os);
  return os;
}


/* ====================================================================== */
/* Exponential */

/* The standard exponential distribution: Exp(1). */
const Exponential Exponential::STANDARD;


/* Returns an exponential distribution with the given rate. */
const Exponential& Exponential::make(const Expression<double>& rate) {
  const Value<double>* value = dynamic_cast<const Value<double>*>(&rate);
  if (value != 0 && value->value() == 1.0) {
    ref(value);
    deref(value);
    return STANDARD;
  } else {
    return *new Exponential(rate);
  }
}


/* Constructs an exponential distribution with rate 1. */
Exponential::Exponential()
  : rate_(new Value<double>(1.0)) {
  ref(rate_);
  ref(this);
}


/* Constructs an exponential distribution with the given rate. */
Exponential::Exponential(const Expression<double>& rate)
  : rate_(&rate) {
  ref(rate_);
}


/* Deletes this exponential distribution. */
Exponential::~Exponential() {
  deref(rate_);
}


/* Tests if this is a state-invariant distribution. */
bool Exponential::state_invariant() const {
  return rate().state_invariant();
}


/* Probability density function for this distribution. */
double Exponential::pdf(double t, const Values& values) const {
  if (t < 0.0) {
    return 0.0;
  } else {
    double r = rate().value(values);
    return r*exp(-r*t);
  }
}


/* Cumulative distribution function for this distribution. */
double Exponential::cdf(double t, const Values& values) const {
  if (t <= 0.0) {
    return 0.0;
  } else {
    return 1.0 - exp(-rate().value(values)*t);
  }
}


/* Inverse cumulative distribution function for this distribution. */
double Exponential::inv(double p, const Values& values) const {
  if (p <= 0.0) {
    return 0.0;
  } else if (p >= 1.0) {
    return HUGE_VAL;
  } else {
    return -log(1.0 - p)/rate().value(values);
  }
}


/* Fills the provided list with the first n moments of this distribution. */
void Exponential::moments(std::vector<double>& m, int n) const {
  /* N.B. this function should never be called for a distribution with
     non-constant parameters. */
  double lambda_inv = 1.0/rate().value(Values());
  double mi = 1.0;
  for (int i = 1; i <= n; i++) {
    mi *= i*lambda_inv;
    m.push_back(mi);
  }
}


/* Returns this distribution subject to the given substitutions. */
const Exponential&
Exponential::substitution(const Substitutions& subst) const {
  const Expression<double>& e = rate().substitution(subst);
  if (&e != &rate()) {
    return make(e);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
void Exponential::print(std::ostream& os) const {
  os << "Exp(" << rate() << ")";
}


/* ===================================================================== */
/* Weibull */

/* Returns a Weibull distribution with the given scale and shape. */
const Distribution& Weibull::make(const Expression<double>& scale,
				  const Expression<double>& shape) {
  const Value<double>* value = dynamic_cast<const Value<double>*>(&shape);
  if (value != 0 && value->value() == 1.0) {
    ref(value);
    deref(value);
    return Exponential::make(Division::make(*new Value<double>(1.0), scale));
  } else {
    return *new Weibull(scale, shape);
  }
}


/* Constructs a Weibull distribution with the given scale and shape. */
Weibull::Weibull(const Expression<double>& scale,
		 const Expression<double>& shape)
  : scale_(&scale), shape_(&shape) {
  ref(scale_);
  ref(shape_);
}


/* Deletes this Weibull distribution. */
Weibull::~Weibull() {
  deref(scale_);
  deref(shape_);
}


/* Tests if this is a state-invariant distribution. */
bool Weibull::state_invariant() const {
  return scale().state_invariant() && shape().state_invariant();
}


/* Probability density function for this distribution. */
double Weibull::pdf(double t, const Values& values) const {
  if (t < 0.0) {
    return 0.0;
  } else {
    double sh = shape().value(values);
    double x = pow(t/scale().value(values), sh);
    return sh*x/t*exp(-x);
  }
}


/* Cumulative distribution function for this distribution. */
double Weibull::cdf(double t, const Values& values) const {
  if (t <= 0.0) {
    return 0.0;
  } else {
    return 1.0 - exp(-pow(t/scale().value(values), shape().value(values)));
  }
}


/* Inverse cumulative distribution function for this distribution. */
double Weibull::inv(double p, const Values& values) const {
  if (p <= 0.0) {
    return 0.0;
  } else if (p >= 1.0) {
    return HUGE_VAL;
  } else {
    return scale().value(values)*pow(-log(1.0 - p), 1.0/shape().value(values));
  }
}


/* Fills the provided list with the first n moments of this distribution. */
void Weibull::moments(std::vector<double>& m, int n) const {
  /* N.B. this function should never be called for a distribution with
     non-constant parameters. */
  double eta = scale().value(Values());
  double beta_inv = 1.0/shape().value(Values());
  double ei = 1.0;
  double bi = 1.0;
  for (int i = 1; i <= n; i++) {
    ei *= eta;
    bi += beta_inv;
    m.push_back(ei*tgamma(bi));
  }
}


/* Returns this distribution subject to the given substitutions. */
const Weibull& Weibull::substitution(const Substitutions& subst) const {
  const Expression<double>& e1 = scale().substitution(subst);
  const Expression<double>& e2 = shape().substitution(subst);
  if (&e1 != &scale() || &e2 != &shape()) {
    return *new Weibull(e1, e2);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
void Weibull::print(std::ostream& os) const {
  os << "W(" << scale() << ',' << shape() << ")";
}


/* ===================================================================== */
/* Lognormal */

/* Returns a lognormal distribution with the given scale and shape. */
const Lognormal& Lognormal::make(const Expression<double>& scale,
				 const Expression<double>& shape) {
  return *new Lognormal(scale, shape);
}


/* Constructs a lognormal distribution with the given scale and shape. */
Lognormal::Lognormal(const Expression<double>& scale,
		     const Expression<double>& shape)
  : scale_(&scale), shape_(&shape) {
  ref(scale_);
  ref(shape_);
}


/* Deletes this lognormal distribution. */
Lognormal::~Lognormal() {
  deref(scale_);
  deref(shape_);
}


/* Tests if this is a state-invariant distribution. */
bool Lognormal::state_invariant() const {
  return scale().state_invariant() && shape().state_invariant();
}


/* Probability density function for this distribution. */
double Lognormal::pdf(double t, const Values& values) const {
  if (t < 0.0) {
    return 0.0;
  } else {
    double sh = shape().value(values);
    double x = log(t/scale().value(values)) + 0.5*sh*sh;
    return 1.0/sh/sqrt(2.0*M_PI)/t*exp(-0.5*x*x/sh/sh);
  }
}


/* Cumulative distribution function for this distribution. */
double Lognormal::cdf(double t, const Values& values) const {
  if (t <= 0.0) {
    return 0.0;
  } else {
    double sh = shape().value(values);
    return 0.5*(1.0
		+ erf((log(t/scale().value(values))/sh + 0.5*sh)*M_SQRT1_2));
  }
}


/* Inverse cumulative distribution function for this distribution. */
double Lognormal::inv(double p, const Values& values) const {
  if (p <= 0.0) {
    return 0.0;
  } else if (p >= 1.0) {
    return HUGE_VAL;
  } else {
    double sh = shape().value(values);
    return (scale().value(values)
	    *exp(sh*(gsl_cdf_ugaussian_Pinv(p) - 0.5*sh)));
  }
}


/* Fills the provided list with the first n moments of this distribution. */
void Lognormal::moments(std::vector<double>& m, int n) const {
  /* N.B. this function should never be called for a distribution with
     non-constant parameters. */
  double mu = scale().value(Values());
  double sigma = shape().value(Values());
  double mean = log(mu) - sigma*sigma/2.0;
  for (int i = 1; i <= n; i++) {
    m.push_back(exp(i*mean + i*i*sigma*sigma/2.0));
  }
}


/* Returns this distribution subject to the given substitutions. */
const Lognormal& Lognormal::substitution(const Substitutions& subst) const {
  const Expression<double>& e1 = scale().substitution(subst);
  const Expression<double>& e2 = shape().substitution(subst);
  if (&e1 != &scale() || &e2 != &shape()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
void Lognormal::print(std::ostream& os) const {
  os << "L(" << scale() << ',' << shape() << ")";
}


/* ===================================================================== */
/* Uniform */

/* Returns a uniform distribution with the given bounds. */
const Uniform& Uniform::make(const Expression<double>& low,
			     const Expression<double>& high) {
  return *new Uniform(low, high);
}


/* Constructs a uniform distribution with the given bounds. */
Uniform::Uniform(const Expression<double>& low, const Expression<double>& high)
  : low_(&low), high_(&high) {
  ref(low_);
  ref(high_);
}


/* Deletes this uniform distribution. */
Uniform::~Uniform() {
  deref(low_);
  deref(high_);
}


/* Tests if this is a state-invariant distribution. */
bool Uniform::state_invariant() const {
  return low().state_invariant() && high().state_invariant();
}


/* Probability density function for this distribution. */
double Uniform::pdf(double t, const Values& values) const {
  double l = low().value(values);
  double h = high().value(values);
  if (t < l || t > h) {
    return 0.0;
  } else {
    return 1.0/(h - l);
  }
}


/* Cumulative distribution function for this distribution. */
double Uniform::cdf(double t, const Values& values) const {
  double l = low().value(values);
  double h = high().value(values);
  if (t <= l) {
    return 0.0;
  } else if (t >= h) {
    return 1.0;
  } else {
    return (t - l)/(h - l);
  }
}


/* Inverse cumulative distribution function for this distribution. */
double Uniform::inv(double p, const Values& values) const {
  if (p <= 0.0) {
    return low().value(values);
  } else if (p >= 1.0) {
    return high().value(values);
  } else {
    double l = low().value(values);
    return (high().value(values) - l)*p + l;
  }
}


/* Fills the provided list with the first n moments of this distribution. */
void Uniform::moments(std::vector<double>& m, int n) const {
  /* N.B. this function should never be called for a distribution with
     non-constant parameters. */
  double a = low().value(Values());
  double b = high().value(Values());
  double ai = a;
  double bi = b;
  for (int i = 1; i <= n; i++) {
    ai *= a;
    bi *= b;
    m.push_back((bi - ai)/((i + 1)*(b - a)));
  }
}


/* Returns this distribution subject to the given substitutions. */
const Uniform& Uniform::substitution(const Substitutions& subst) const {
  const Expression<double>& e1 = low().substitution(subst);
  const Expression<double>& e2 = high().substitution(subst);
  if (&e1 != &low() || &e2 != &high()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}


/* Prints this object on the given stream. */
void Uniform::print(std::ostream& os) const {
  os << "U(" << low() << ',' << high() << ")";
}
