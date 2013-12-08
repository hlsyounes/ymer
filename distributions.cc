/*
 * Copyright (C) 2003, 2004 Carnegie Mellon University
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

#include <cmath>
#include <iostream>

#include "config.h"
#include "src/expression.h"

namespace {

class ConstantExpressionEvaluator : public ExpressionVisitor {
 public:
  ConstantExpressionEvaluator();

  TypedValue value() const { return value_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitComputation(const Computation& expr);

  TypedValue value_;
};

ConstantExpressionEvaluator::ConstantExpressionEvaluator()
    : value_(0) {
}

void ConstantExpressionEvaluator::DoVisitLiteral(const Literal& expr) {
  value_ = expr.value();
}

void ConstantExpressionEvaluator::DoVisitIdentifier(const Identifier& expr) {
  LOG(FATAL) << "expecting constant expression";
}

void ConstantExpressionEvaluator::DoVisitComputation(const Computation& expr) {
  expr.operand1().Accept(this);
  TypedValue operand1 = value_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case Computation::PLUS:
      value_ = operand1 + value_;
      break;
    case Computation::MINUS:
      value_ = operand1 - value_;
      break;
    case Computation::MULTIPLY:
      value_ = operand1 * value_;
      break;
    case Computation::DIVIDE:
      value_ = operand1 / value_;
      break;
  }
}

TypedValue EvaluateConstantExpression(const Expression& expr) {
  ConstantExpressionEvaluator evaluator;
  expr.Accept(&evaluator);
  return evaluator.value();
}

}  // namespace

/* ====================================================================== */
/* Distribution */

Distribution::Distribution() {
}

Distribution::~Distribution() {
}

void Distribution::Accept(DistributionVisitor* visitor) const {
  DoAccept(visitor);
}

/* Provides the parameters for an acyclic continuous phase-type
   (ACPH) distribution in the class of EC distributions matching the
   first three moments of this distribution. */
void Distribution::acph(ECParameters& params) const {
  /*
   * Implements the moment matching algorithm described in:
   *
   *   Osogami, Takayuki, and Mor Harchol-Balter.  2003.  "A
   *     closed-form solution for mapping general distributions to
   *     minimal PH distributions".  In Proceedings of the 13th
   *     International Conference on Tools and Algorithms for the
   *     Construnction and Analysis of Systems.
   *
   * This is a slightly modified version avoiding solutions with p<1,
   * at the price that some positive distributions cannot be properly
   * represented.  For a distribution that the original algorithm
   * would map to an EC distribution with p<1, this algorithm modifies
   * the second and third moments so that the modified distribution
   * can be matched to an EC distribution with p=1.  This is to avoid
   * EC distributions with mass probability at zero, which do not fit
   * well with the model of discrete event systems that we are using.
   */
  std::vector<double> m;
  moments(m, 3);
  double mu1G = m[0];
  double mu2G = m[1];
  double mu3G = m[2];
  double m2G = mu2G/(mu1G*mu1G);
  double m3G = mu3G/(mu1G*mu2G);
  double x = 1.0/(m2G - 1.0);
  if (fabs(rint(x) - x) < 1e-10) {
    /*
     * Adjust second moment so that 1/(m2G - 1) is not an integer.
     */
    m2G *= (1.0 - 1e-5);
    std::cerr << std::endl
	      << PACKAGE ": second moment for " << *this << " is changed from "
	      << mu2G << " to " << m2G*mu1G*mu1G << std::endl;
  }
  if (m3G < 2.0*m2G - 1.0) {
    /*
     * Adjust third moment so that the resulting EC distribution has
     * no mass probability at zero (p=1).
     */
    m3G = 2.0*m2G - 1.0;
    std::cerr << std::endl
	      << PACKAGE ": third moment for " << *this << " is changed from "
	      << mu3G << " to " << m3G*mu1G*mu2G << std::endl;
  }
  double n;
  if (m2G < 2.0 && fabs(m3G - (2.0*m2G - 1.0)) < 1e-10) {
    n = floor(m2G/(m2G - 1.0));
  } else {
    n = floor(m2G/(m2G - 1.0) + 1.0);
  }
  double m2X = ((n - 3.0)*m2G - (n - 2.0))/((n - 2.0)*m2G - (n - 1.0));
  double mu1X = mu1G/((n - 2.0)*m2X - (n - 3.0));
  double alpha = (n - 2.0)*(m2X - 1.0)*(n*(n - 1.0)*m2X*m2X
					- n*(2.0*n - 5.0)*m2X
					+ (n - 1.0)*(n - 3.0));
  double tmp = (n - 2.0)*m2X - (n - 3.0);
  double beta = ((n - 1.0)*m2X - (n - 2.0))*tmp*tmp;
  double m3X = (beta*m3G - alpha)/m2X;
  double u, v;
  if (fabs(3.0*m2X - 2.0*m3X) < 1e-10) {
    u = 1.0;
    v = 0.0;
  } else {
    u = (6.0 - 2.0*m3X)/(3.0*m2X - 2.0*m3X);
    v = (12.0 - 6.0*m2X)/(m2X*(3.0*m2X - 2.0*m3X));
  }
  params.n = int(n + 0.5);
  params.re = 1.0/((m2X - 1.0)*mu1X);
  tmp = sqrt(u*u - 4.0*v);
  params.rc1 = (u + tmp)/(2.0*mu1X);
  if (fabs(params.rc1*mu1X - 1.0) < 1e-10) {
    params.pc = 0.0;
  } else {
    params.rc2 = (u - tmp)/(2.0*mu1X);
    params.pc = params.rc2*(params.rc1*mu1X - 1.0)/params.rc1;
  }
}


/* Provides the parameters for an acyclic continuous phase-type
   (ACPH) distribution matching the first two moments of this
   distribution. */
void Distribution::acph2(ACPH2Parameters& params) const {
  std::vector<double> m;
  moments(m, 2);
  double mu = m[0];
  double cv2 = m[1]/(mu*mu) - 1.0;
  if (cv2 >= 0.5) {
    params.n = 2;
    params.p = 1/(2.0*cv2);
    params.r1 = 2.0/mu;
    params.r2 = 1.0/(mu*cv2);
  } else {
    double n = 1.0/cv2;
    if (fabs(rint(n) - n) < 1e-10) {
      n = rint(n);
    } else {
      n = ceil(n);
    }
    params.n = int(n + 0.5);
    params.p = 1.0 - ((2.0*n*cv2 + n - 2.0 - sqrt(n*n + 4.0 - 4.0*n*cv2))
		      /2.0/(n - 1.0)/(cv2 + 1.0));
    params.r1 = (1.0 - params.p + n*params.p)/mu;
    params.r2 = params.r1;
  }
}

namespace {

// A distribution visitor that prints a distribution to an output stream.
class DistributionPrinter : public DistributionVisitor {
 public:
  explicit DistributionPrinter(std::ostream* os);

 private:
  virtual void DoVisitExponential(const Exponential& distribution);
  virtual void DoVisitWeibull(const Weibull& distribution);
  virtual void DoVisitLognormal(const Lognormal& distribution);
  virtual void DoVisitUniform(const Uniform& distribution);

  std::ostream* os_;
};

DistributionPrinter::DistributionPrinter(std::ostream* os)
    : os_(os) {
}

void DistributionPrinter::DoVisitExponential(const Exponential& distribution) {
  *os_ << "Exp(" << distribution.rate() << ")";
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


/* ====================================================================== */
/* Exponential */

/* Returns an exponential distribution with the given rate. */
const Exponential* Exponential::make(std::unique_ptr<const Expression>&& rate) {
  return new Exponential(std::move(rate));
}


/* Constructs an exponential distribution with the given rate. */
Exponential::Exponential(std::unique_ptr<const Expression>&& rate)
    : rate_(std::move(rate)) {
}


/* Deletes this exponential distribution. */
Exponential::~Exponential() = default;

void Exponential::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitExponential(*this);
}

/* Fills the provided list with the first n moments of this distribution. */
void Exponential::moments(std::vector<double>& m, size_t n) const {
  /* N.B. this function should never be called for a distribution with
     non-constant parameters. */
  double lambda_inv = 1.0/EvaluateConstantExpression(rate()).value<double>();
  double mi = 1.0;
  for (size_t i = 1; i <= n; i++) {
    mi *= i*lambda_inv;
    m.push_back(mi);
  }
}


/* ===================================================================== */
/* Weibull */

/* Returns a Weibull distribution with the given scale and shape. */
const Distribution* Weibull::make(std::unique_ptr<const Expression>&& scale,
				  std::unique_ptr<const Expression>&& shape) {
  const Literal* value = dynamic_cast<const Literal*>(shape.get());
  if (value != nullptr && value->value() == 1) {
    return Exponential::make(Computation::Create(
        Computation::DIVIDE, std::move(shape), std::move(scale)));
  } else {
    return new Weibull(std::move(scale), std::move(shape));
  }
}


/* Constructs a Weibull distribution with the given scale and shape. */
Weibull::Weibull(std::unique_ptr<const Expression>&& scale,
                 std::unique_ptr<const Expression>&& shape)
    : scale_(std::move(scale)), shape_(std::move(shape)) {
}


/* Deletes this Weibull distribution. */
Weibull::~Weibull() = default;

void Weibull::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitWeibull(*this);
}

/* Fills the provided list with the first n moments of this distribution. */
void Weibull::moments(std::vector<double>& m, size_t n) const {
  /* N.B. this function should never be called for a distribution with
     non-constant parameters. */
  double eta = EvaluateConstantExpression(scale()).value<double>();
  double beta_inv = 1.0/EvaluateConstantExpression(shape()).value<double>();
  double ei = 1.0;
  double bi = 1.0;
  for (size_t i = 1; i <= n; i++) {
    ei *= eta;
    bi += beta_inv;
    m.push_back(ei*tgamma(bi));
  }
}


/* ===================================================================== */
/* Lognormal */

/* Returns a lognormal distribution with the given scale and shape. */
const Lognormal* Lognormal::make(std::unique_ptr<const Expression>&& scale,
				 std::unique_ptr<const Expression>&& shape) {
  return new Lognormal(std::move(scale), std::move(shape));
}


/* Constructs a lognormal distribution with the given scale and shape. */
Lognormal::Lognormal(std::unique_ptr<const Expression>&& scale,
                     std::unique_ptr<const Expression>&& shape)
    : scale_(std::move(scale)), shape_(std::move(shape)) {
}


/* Deletes this lognormal distribution. */
Lognormal::~Lognormal() = default;

void Lognormal::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitLognormal(*this);
}

/* Fills the provided list with the first n moments of this distribution. */
void Lognormal::moments(std::vector<double>& m, size_t n) const {
  /* N.B. this function should never be called for a distribution with
     non-constant parameters. */
  double mu = EvaluateConstantExpression(scale()).value<double>();
  double sigma = EvaluateConstantExpression(shape()).value<double>();
  double mean = log(mu) - sigma*sigma/2.0;
  for (size_t i = 1; i <= n; i++) {
    m.push_back(exp(i*mean + i*i*sigma*sigma/2.0));
  }
}


/* ===================================================================== */
/* Uniform */

/* Returns a uniform distribution with the given bounds. */
const Uniform* Uniform::make(std::unique_ptr<const Expression>&& low,
                             std::unique_ptr<const Expression>&& high) {
  return new Uniform(std::move(low), std::move(high));
}


/* Constructs a uniform distribution with the given bounds. */
Uniform::Uniform(std::unique_ptr<const Expression>&& low,
                 std::unique_ptr<const Expression>&& high)
    : low_(std::move(low)), high_(std::move(high)) {
}


/* Deletes this uniform distribution. */
Uniform::~Uniform() = default;

void Uniform::DoAccept(DistributionVisitor* visitor) const {
  visitor->VisitUniform(*this);
}

/* Fills the provided list with the first n moments of this distribution. */
void Uniform::moments(std::vector<double>& m, size_t n) const {
  /* N.B. this function should never be called for a distribution with
     non-constant parameters. */
  double a = EvaluateConstantExpression(low()).value<double>();
  double b = EvaluateConstantExpression(high()).value<double>();
  double ai = a;
  double bi = b;
  for (size_t i = 1; i <= n; i++) {
    ai *= a;
    bi *= b;
    m.push_back((bi - ai)/((i + 1)*(b - a)));
  }
}

DistributionVisitor::DistributionVisitor() {
}

DistributionVisitor::DistributionVisitor(const DistributionVisitor&) {
}

DistributionVisitor& DistributionVisitor::operator=(
    const DistributionVisitor&) {
  return *this;
}

DistributionVisitor::~DistributionVisitor() {
}

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
