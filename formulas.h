/* -*-C++-*- */
/*
 * Formulas.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011--2012 Google Inc
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
#ifndef FORMULAS_H
#define FORMULAS_H

#include <config.h>
#include "expressions.h"
#include <cudd.h>
#include <deque>

struct Model;
struct State;


/* Delta function determining half-width of indifference region. */
typedef double (*DeltaFun)(double theta);

/* Sampling algorithm. */
enum SamplingAlgorithm { ESTIMATE, SEQUENTIAL, SPRT, FIXED };


/* ====================================================================== */
/* StateFormula */

/*
 * A state formula.
 */
struct StateFormula {
  /* Increases the reference count for the given state formula. */
  static void ref(const StateFormula* f) {
    if (f != NULL) {
      f->ref_count_++;
    }
  }

  /* Decreases the reference count for the given state formula. */
  static void deref(const StateFormula* f) {
    if (f != NULL) {
      f->ref_count_--;
    }
  }

  /* Decreases the reference count for the given state formula and
     deletes it if the the reference count becomes zero. */
  static void destructive_deref(const StateFormula* f) {
    if (f != NULL) {
      f->ref_count_--;
      if (f->ref_count_ == 0) {
	delete f;
      }
    }
  }

  /* Returns the current formula level. */
  static size_t formula_level() { return formula_level_; }

  /* Deletes this state formula. */
  virtual ~StateFormula() {}

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const = 0;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const = 0;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const = 0;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula&
  substitution(const SubstitutionMap& subst) const = 0;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const = 0;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const = 0;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const = 0;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const = 0;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, DeltaFun delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon) const = 0;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 double epsilon, bool estimate) const = 0;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const = 0;

protected:
  /* Nesting level of formula just being verified. */
  static size_t formula_level_;

  /* Constructs a state formula. */
  StateFormula() : ref_count_(0) {}

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

private:
  /* Reference counter. */
  mutable size_t ref_count_;

  friend std::ostream& operator<<(std::ostream& os, const StateFormula& f);
};

/* Output operator for state formulas. */
std::ostream& operator<<(std::ostream& os, const StateFormula& f);


/* ====================================================================== */
/* PathFormula */

/*
 * A path formula.
 */
struct PathFormula {
  /* Increases the reference count for the given path formula. */
  static void ref(const PathFormula* f) {
    if (f != NULL) {
      f->ref_count_++;
    }
  }

  /* Decreases the reference count for the given path formula. */
  static void deref(const PathFormula* f) {
    if (f != NULL) {
      f->ref_count_--;
    }
  }

  /* Decreases the reference count for the given path formula and
     deletes it if the the reference count becomes zero. */
  static void destructive_deref(const PathFormula* f) {
    if (f != NULL) {
      f->ref_count_--;
      if (f->ref_count_ == 0) {
	delete f;
      }
    }
  }

  /* Deletes this path formula. */
  virtual ~PathFormula() {}

  /* Tests if this path formula contains probabilistic elements. */
  virtual bool probabilistic() const = 0;

  /* Returns this path formula subject to the given substitutions. */
  virtual const PathFormula& substitution(const ValueMap& values) const = 0;

  /* Returns this path formula subject to the given substitutions. */
  virtual const PathFormula&
  substitution(const SubstitutionMap& subst) const = 0;

  /* Estimated effort for generating a sample for this path formula. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const = 0;

  /* Generates a sample for this path formula. */
  virtual bool sample(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const = 0;

  /* Generates a sample for this path formula. */
  virtual bool sample(DdManager* dd_man, const Model& model,
		      const State& state, double epsilon,
		      DdNode* dd1, DdNode* dd2) const = 0;

  /* Verifies this path formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, const TypedValue& p, bool strict,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
		      double epsilon) const = 0;

  /* Verifies this path formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 const TypedValue& p, bool strict,
			 double epsilon, bool estimate) const = 0;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const = 0;

protected:
  /* Constructs a path formula. */
  PathFormula() : ref_count_(0) {}

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

private:
  /* Reference counter. */
  mutable size_t ref_count_;

  friend std::ostream& operator<<(std::ostream& os, const PathFormula& f);
};

/* Output operator for path formulas. */
std::ostream& operator<<(std::ostream& os, const PathFormula& f);


/* ====================================================================== */
/* FormulaList */

/*
 * List of formulas.
 */
struct FormulaList : public std::deque<const StateFormula*> {
};


/* ====================================================================== */
/* Conjunction */

struct Conjunction : public StateFormula {
  /* Deletes this conjunction. */
  virtual ~Conjunction();

  /* Adds a conjunct to this conjunction. */
  void add_conjunct(const StateFormula& conjunct);

  /* Returns the conjuncts for this conjunction. */
  const FormulaList& conjuncts() const { return conjuncts_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Conjunction& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, DeltaFun delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The conjuncts. */
  FormulaList conjuncts_;
};


/* ====================================================================== */
/* Disjunction */

struct Disjunction : public StateFormula {
  /* Deletes this disjunction. */
  virtual ~Disjunction();

  /* Adds a disjunct to this disjunction. */
  void add_disjunct(const StateFormula& disjunct);

  /* Returns the disjuncts for this disjunction. */
  const FormulaList& disjuncts() const { return disjuncts_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Disjunction& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, DeltaFun delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The disjuncts. */
  FormulaList disjuncts_;
};


/* ====================================================================== */
/* Negation */

/*
 * A negated state formula.
 */
struct Negation : public StateFormula {
  /* Constructs a negation. */
  Negation(const StateFormula& negand);

  /* Deletes this negation. */
  virtual ~Negation();

  /* Returns the negated state formula. */
  const StateFormula& negand() const { return *negand_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Negation& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, DeltaFun delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The negated state formula. */
  const StateFormula* negand_;
};


/* ====================================================================== */
/* Implication */

/*
 * An implication.
 */
struct Implication : public StateFormula {
  /* Constructs an implication. */
  Implication(const StateFormula& antecedent, const StateFormula& consequent);

  /* Deletes this implication. */
  virtual ~Implication();

  /* Returns the antecedent of this implication. */
  const StateFormula& antecedent() const { return *antecedent_; }

  /* Returns the consequent of this implication. */
  const StateFormula& consequent() const { return *consequent_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Implication& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, DeltaFun delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The antecedent of this implication. */
  const StateFormula* antecedent_;
  /* The consequent of this implication. */
  const StateFormula* consequent_;
};


/* ====================================================================== */
/* Probabilistic */

/*
 * Probabilistic path quantification.
 */
struct Probabilistic : public StateFormula {
  /* Constructs a probabilistic path quantification. */
  Probabilistic(const TypedValue& threshold, bool strict,
		const PathFormula& formula);

  /* Deletes this probabilistic path quantification. */
  virtual ~Probabilistic();

  /* Returns the probability threshold. */
  const TypedValue& threshold() const { return threshold_; }

  /* Tests if the threshold is strict. */
  bool strict() const { return strict_; }

  /* Returns the path formula. */
  const PathFormula& formula() const { return *formula_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Probabilistic&
  substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, DeltaFun delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The probability threshold. */
  TypedValue threshold_;
  /* Whether the threshold is strict. */
  bool strict_;
  /* The path formula. */
  const PathFormula* formula_;
  /* Cached acceptance sampling results. */
  mutable std::map<ValueMap, std::pair<size_t, double> > cache_;
};


/* ====================================================================== */
/* Comparison */

/*
 * A comparison state formula.
 */
struct Comparison : public StateFormula {
  /* Deletes this comparison. */
  virtual ~Comparison();

  /* Returns the first expression of this comparison. */
  const Expression& expr1() const { return *expr1_; }

  /* Returns the second expression of this comparison. */
  const Expression& expr2() const { return *expr2_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, DeltaFun delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

protected:
  /* Constructs a comparison. */
  Comparison(const Expression& expr1, const Expression& expr2);

private:
  /* The first expression. */
  const Expression* expr1_;
  /* The second expression. */
  const Expression* expr2_;
};


/* ====================================================================== */
/* LessThan */

/*
 * A less-than comparison state formula.
 */
struct LessThan : public Comparison {
  /* Constructs a less-than comparison. */
  LessThan(const Expression& expr1, const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const LessThan& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* LessThanOrEqual */

/*
 * A less-than-or-equal comparison state formula.
 */
struct LessThanOrEqual : public Comparison {
  /* Constructs a less-than-or-equal comparison. */
  LessThanOrEqual(const Expression& expr1, const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const LessThanOrEqual&
  substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* GreaterThanOrEqual */

/*
 * A greater-than-or-equal comparison state formula.
 */
struct GreaterThanOrEqual : public Comparison {
  /* Constructs a greater-than-or-equal comparison. */
  GreaterThanOrEqual(const Expression& expr1, const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const GreaterThanOrEqual&
  substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* GreaterThan */

/*
 * A greater-than comparison state formula.
 */
struct GreaterThan : public Comparison {
  /* Constructs a greater-than comparison. */
  GreaterThan(const Expression& expr1, const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const GreaterThan& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Equality */

/*
 * An equality comparison state formula.
 */
struct Equality : public Comparison {
  /* Constructs an equality comparison. */
  Equality(const Expression& expr1, const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Equality& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Inequality */

/*
 * An inequality comparison state formula.
 */
struct Inequality : public Comparison {
  /* Constructs an inequality comparison. */
  Inequality(const Expression& expr1, const Expression& expr2);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula& substitution(const ValueMap& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Inequality& substitution(const SubstitutionMap& subst) const;

  /* Returns the `current state' BDD representation for this state formula. */
  virtual DdNode* bdd(DdManager* dd_man) const;

  /* Returns the `next state' BDD representation for this state formula. */
  virtual DdNode* primed_bdd(DdManager* dd_man) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Until */

/*
 * An until path formula.
 */
struct Until : public PathFormula {
  /* Constructs an until formula. */
  Until(const StateFormula& pre, const StateFormula& post,
	const TypedValue& min_time, const TypedValue& max_time);

  /* Deletes this until formula. */
  virtual ~Until();

  /* Returns the precondition formula. */
  const StateFormula& pre() const { return *pre_; }

  /* Returns the postcondition formula. */
  const StateFormula& post() const { return *post_; }

  /* Returns the lower time bound. */
  const TypedValue& min_time() const { return min_time_; }

  /* Returns the upper time bound. */
  const TypedValue& max_time() const { return max_time_; }

  /* Tests if this path formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Returns this path formula subject to the given substitutions. */
  virtual const PathFormula& substitution(const ValueMap& values) const;

  /* Returns this path formula subject to the given substitutions. */
  virtual const Until& substitution(const SubstitutionMap& subst) const;

  /* Estimated effort for generating a sample for this path formula. */
  virtual double effort(const Model& model, const State& state,
			double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Generates a sample for this path formula. */
  virtual bool sample(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const;

  /* Generates a sample for this path formula. */
  virtual bool sample(DdManager* dd_man, const Model& model,
		      const State& state, double epsilon,
		      DdNode* dd1, DdNode* dd2) const;

  /* Verifies this path formula using the mixed engine. */
  virtual bool verify(DdManager* dd_man, const Model& model,
		      const State& state, const TypedValue& p, bool strict,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
		      double epsilon) const;

  /* Verifies this path formula using the hybrid engine. */
  virtual DdNode* verify(DdManager* dd_man, const Model& model,
			 const TypedValue& p, bool strict,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The precondition formula. */
  const StateFormula* pre_;
  /* The postcondition formula. */
  const StateFormula* post_;
  /* The lower time bound. */
  TypedValue min_time_;
  /* The upper time bound. */
  TypedValue max_time_;
};


#endif /* FORMULAS_H */
