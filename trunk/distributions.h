/* -*-C++-*- */
/*
 * Probability distributions.
 *
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
 * $Id: distributions.h,v 1.1 2003-11-12 03:59:15 lorens Exp $
 */
#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <iostream>

struct Expression;
struct ValueMap;
struct SubstitutionMap;


/* ====================================================================== */
/* Distribution */

/*
 * A probability distribution.
 */
struct Distribution {
  /* The standard exponential distribution: Exp(1). */
  static const Distribution& EXP1;

  /* Register use of the given distribution. */
  static void register_use(const Distribution* d) {
    if (d != NULL) {
      d->ref_count_++;
    }
  }

  /* Unregister use of the given distribution. */
  static void unregister_use(const Distribution* d) {
    if (d != NULL) {
      d->ref_count_--;
      if (d->ref_count_ == 0) {
	delete d;
      }
    }
  }

  /* Deletes this distribution. */
  virtual ~Distribution() {}

  /* Returns a sample drawn from this distribution. */
  virtual double sample(const ValueMap& values) const = 0;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Distribution& substitution(const ValueMap& values) const = 0;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Distribution&
  substitution(const SubstitutionMap& subst) const = 0;

protected:
  /* Constructs a distribution. */
  Distribution() : ref_count_(0) {}

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

private:
  /* Reference counter. */
  mutable size_t ref_count_;

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
  /* Returns an exponential distribution with the given rate. */
  static const Exponential& make(const Expression& rate);

  /* Deletes this exponential distribution. */
  virtual ~Exponential();

  /* Returns the rate of this exponential distribution. */
  const Expression& rate() const { return *rate_; }

  /* Returns a sample drawn from this distribution. */
  virtual double sample(const ValueMap& values) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Exponential& substitution(const ValueMap& values) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Exponential& substitution(const SubstitutionMap& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The standard exponential distribution: Exp(1). */
  static const Exponential EXP1_;

  /* The rate of this exponential distribution. */
  const Expression* rate_;

  /* Constructs an exponential distribution with rate 1. */
  Exponential();

  /* Constructs an exponential distribution with the given rate. */
  Exponential(const Expression& rate);

  friend struct Distribution;
};


/* ====================================================================== */
/* Weibull */

/*
 * A Weibull distribution.
 */
struct Weibull : public Distribution {
  /* Returns a Weibull distribution with the given scale and shape. */
  static const Distribution& make(const Expression& scale,
				  const Expression& shape);

  /* Deletes this Weibull distribution. */
  virtual ~Weibull();

  /* Returns the scale of this Weibull distribution. */
  const Expression& scale() const { return *scale_; }

  /* Returns the shape of this Weibull distribution. */
  const Expression& shape() const { return *shape_; }

  /* Returns a sample drawn from this distribution. */
  virtual double sample(const ValueMap& values) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Distribution& substitution(const ValueMap& values) const;

  /* Returns this distribution subject to the given substitutions. */
  virtual const Weibull& substitution(const SubstitutionMap& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The scale of this Weibull distribution. */
  const Expression* scale_;
  /* The shape of this Weibull distribution. */
  const Expression* shape_;

  /* Constructs a Weibull distribution with the given scale and shape. */
  Weibull(const Expression& scale, const Expression& shape);
};


#endif /* DISTRIBUTIONS_H */
