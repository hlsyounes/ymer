/*
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: distributions.cc,v 1.1 2003-11-12 03:59:18 lorens Exp $
 */
#include "distributions.h"
#include "expressions.h"
#include "rng.h"


/* ====================================================================== */
/* Distribution */

/* The standard exponential distribution: Exp(1). */
const Distribution& Distribution::EXP1 = Exponential::EXP1_;

/* Output operator for distributions. */
std::ostream& operator<<(std::ostream& os, const Distribution& d) {
  d.print(os);
  return os;
}


/* ====================================================================== */
/* Exponential */

/* The standard exponential distribution: Exp(1). */
const Exponential Exponential::EXP1_;


/* Returns an exponential distribution with the given rate. */
const Exponential& Exponential::make(const Expression& rate) {
  const Value* value = dynamic_cast<const Value*>(&rate);
  if (value != NULL && value->value() == 1) {
    Expression::register_use(value);
    Expression::unregister_use(value);
    return EXP1_;
  } else {
    return *new Exponential(rate);
  }
}


/* Constructs an exponential distribution with rate 1. */
Exponential::Exponential()
  : rate_(new Value(1)) {
  Expression::register_use(rate_);
  register_use(this);
}


/* Constructs an exponential distribution with the given rate. */
Exponential::Exponential(const Expression& rate)
  : rate_(&rate) {
  Expression::register_use(rate_);
}


/* Deletes this exponential distribution. */
Exponential::~Exponential() {
  Expression::unregister_use(rate_);
}


/* Returns a sample drawn from this distribution. */
double Exponential::sample(const ValueMap& values) const {
  double lambda = rate().value(values).double_value();
  return -log(genrand_real3())/lambda;
}


/* Returns this distribution subject to the given substitutions. */
const Exponential& Exponential::substitution(const ValueMap& values) const {
  const Expression& e = rate().substitution(values);
  if (&e != &rate()) {
    return make(e);
  } else {
    return *this;
  }
}


/* Returns this distribution subject to the given substitutions. */
const Exponential&
Exponential::substitution(const SubstitutionMap& subst) const {
  const Expression& e = rate().substitution(subst);
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
const Distribution& Weibull::make(const Expression& scale,
				  const Expression& shape) {
  const Value* value = dynamic_cast<const Value*>(&shape);
  if (value != NULL && value->value() == 1) {
    Expression::register_use(value);
    Expression::unregister_use(value);
    return Exponential::make(Division::make(*new Value(1), scale));
  } else {
    return *new Weibull(scale, shape);
  }
}


/* Constructs a Weibull distribution with the given scale and shape. */
Weibull::Weibull(const Expression& scale, const Expression& shape)
  : scale_(&scale), shape_(&shape) {
  Expression::register_use(scale_);
  Expression::register_use(shape_);
}


/* Deletes this Weibull distribution. */
Weibull::~Weibull() {
  Expression::unregister_use(scale_);
  Expression::unregister_use(shape_);
}


/* Returns a sample drawn from this distribution. */
double Weibull::sample(const ValueMap& values) const {
  double eta = scale().value(values).double_value();
  double beta = shape().value(values).double_value();
  return eta*pow(-log(genrand_real3()), 1.0/beta);
}


/* Returns this distribution subject to the given substitutions. */
const Distribution& Weibull::substitution(const ValueMap& values) const {
  const Expression& e1 = scale().substitution(values);
  const Expression& e2 = shape().substitution(values);
  if (&e1 != &scale() || &e2 != &shape()) {
    return make(e1, e2);
  } else {
    return *this;
  }
}


/* Returns this distribution subject to the given substitutions. */
const Weibull& Weibull::substitution(const SubstitutionMap& subst) const {
  const Expression& e1 = scale().substitution(subst);
  const Expression& e2 = shape().substitution(subst);
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
