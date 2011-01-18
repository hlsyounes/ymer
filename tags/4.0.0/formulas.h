/* -*-C++-*- */
/*
 * Formulas.
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
#ifndef FORMULAS_H
#define FORMULAS_H

#include <config.h>
#include "refcount.h"
#include "expressions.h"
#include <iostream>
#include <vector>


/* ====================================================================== */
/* StateFormula */

/*
 * A state formula.
 */
struct StateFormula : public RCObject {
  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const = 0;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const Values& values) const = 0;

  /* Returns this state formula subject to the given substitutions. */
  virtual const StateFormula&
  substitution(const Substitutions& subst) const = 0;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const StateFormula& f);
};

/* Output operator for state formulas. */
std::ostream& operator<<(std::ostream& os, const StateFormula& f);


/* ====================================================================== */
/* PathFormula */

/*
 * A path formula.
 */
struct PathFormula : public RCObject {
  /* Tests if this path formula contains probabilistic elements. */
  virtual bool probabilistic() const = 0;

  /* Returns this path formula subject to the given substitutions. */
  virtual const PathFormula&
  substitution(const Substitutions& subst) const = 0;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const PathFormula& f);
};

/* Output operator for path formulas. */
std::ostream& operator<<(std::ostream& os, const PathFormula& f);


/* ====================================================================== */
/* FormulaList */

/*
 * List of formulas.
 */
struct FormulaList : public std::vector<const StateFormula*> {
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
  virtual bool holds(const Values& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Conjunction& substitution(const Substitutions& subst) const;

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
  virtual bool holds(const Values& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Disjunction& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The disjuncts. */
  FormulaList disjuncts_;
};


/* ====================================================================== */
/* Probabilistic */

/*
 * Probabilistic path quantification.
 */
struct Probabilistic : public StateFormula {
  /* Constructs a probabilistic path quantification. */
  Probabilistic(bool negated, double threshold, bool strict,
		const PathFormula& formula);

  /* Deletes this probabilistic path quantification. */
  virtual ~Probabilistic();

  /* Tests if comparison is negated. */
  bool negated() const { return negated_; }

  /* Returns the probability threshold. */
  double threshold() const { return threshold_; }

  /* Tests if the threshold is strict. */
  bool strict() const { return strict_; }

  /* Returns the path formula. */
  const PathFormula& formula() const { return *formula_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const Values& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Probabilistic& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Whether the comparison is negated. */
  bool negated_;
  /* The probability threshold. */
  double threshold_;
  /* Whether the threshold is strict. */
  bool strict_;
  /* The path formula. */
  const PathFormula* formula_;
};


/* ====================================================================== */
/* Comparison */

/*
 * A comparison state formula.
 */
template<typename T>
struct Comparison : public StateFormula {
  /* Deletes this comparison. */
  virtual ~Comparison();

  /* Returns the first expression of this comparison. */
  const Expression<T>& expr1() const { return *expr1_; }

  /* Returns the second expression of this comparison. */
  const Expression<T>& expr2() const { return *expr2_; }

  /* Tests if comparison is negated. */
  bool negated() const { return negated_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

protected:
  /* Constructs a comparison. */
  Comparison(const Expression<T>& expr1, const Expression<T>& expr2,
	     bool negated);

private:
  /* The first expression. */
  const Expression<T>* expr1_;
  /* The second expression. */
  const Expression<T>* expr2_;
  /* Whether the comparison is negated. */
  bool negated_;
};


/* ====================================================================== */
/* LessThan */

/*
 * A less-than comparison state formula.
 */
template<typename T>
struct LessThan : public Comparison<T> {
  /* Constructs a less-than comparison. */
  LessThan(const Expression<T>& expr1, const Expression<T>& expr2,
	   bool negated);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const Values& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const LessThan<T>& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* GreaterThan */

/*
 * A greater-than comparison state formula.
 */
template<typename T>
struct GreaterThan : public Comparison<T> {
  /* Constructs a greater-than comparison. */
  GreaterThan(const Expression<T>& expr1, const Expression<T>& expr2,
	      bool negated);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const Values& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const GreaterThan<T>& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Equality */

/*
 * An equality comparison state formula.
 */
template<typename T>
struct Equality : public Comparison<T> {
  /* Constructs an equality comparison. */
  Equality(const Expression<T>& expr1, const Expression<T>& expr2,
	   bool negated);

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const Values& values) const;

  /* Returns this state formula subject to the given substitutions. */
  virtual const Equality<T>& substitution(const Substitutions& subst) const;

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
	double min_time, double max_time);

  /* Deletes this until formula. */
  virtual ~Until();

  /* Returns the precondition formula. */
  const StateFormula& pre() const { return *pre_; }

  /* Returns the postcondition formula. */
  const StateFormula& post() const { return *post_; }

  /* Returns the lower time bound. */
  double min_time() const { return min_time_; }

  /* Returns the upper time bound. */
  double max_time() const { return max_time_; }

  /* Tests if this path formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Returns this path formula subject to the given substitutions. */
  virtual const Until& substitution(const Substitutions& subst) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The precondition formula. */
  const StateFormula* pre_;
  /* The postcondition formula. */
  const StateFormula* post_;
  /* The lower time bound. */
  double min_time_;
  /* The upper time bound. */
  double max_time_;
};


#endif /* FORMULAS_H */
