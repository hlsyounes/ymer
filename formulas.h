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

#include <deque>
#include <map>

#include "src/ddutil.h"
#include "src/expression.h"
#include "src/statistics.h"

struct Model;
struct State;

/* Sampling algorithm. */
enum SamplingAlgorithm { ESTIMATE, SEQUENTIAL, SPRT, FIXED };

struct ModelCheckingStats {
  Sample<double> time;
  Sample<int> sample_size;
  Sample<size_t> path_length;
};

/* ====================================================================== */
/* StateFormula */

class StateFormulaVisitor;

// Abstract base class for state formulas.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcreteStateFormulaVisitor : public StateFormulaVisitor {
//     ...
//   };
//
//   StateFormula* formula = ...;
//   ConcreteStateFormulaVisitor visitor;
//   formula->Accept(&visitor);
//
class StateFormula {
 public:
  /* Returns the current formula level. */
  static size_t formula_level() { return formula_level_; }

  /* Deletes this state formula. */
  virtual ~StateFormula() {}

  void Accept(StateFormulaVisitor* visitor) const;

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const = 0;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const std::vector<int>& state) const = 0;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const = 0;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const = 0;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const = 0;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 double epsilon, bool estimate) const = 0;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const = 0;

protected:
  /* Nesting level of formula just being verified. */
  static size_t formula_level_;

  /* Constructs a state formula. */
  StateFormula() {}

private:
  // Disallow copy and assign.
  StateFormula(const StateFormula&);
  StateFormula& operator=(const StateFormula&);

  virtual void DoAccept(StateFormulaVisitor* visitor) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const StateFormula& f);
};

// Returns the `current state' BDD representation for a state formula.
BDD bdd(
    const DecisionDiagramManager& dd_man,
    const std::map<std::string, VariableProperties>& variable_properties,
    const StateFormula& f);

// Returns the `next state' BDD representation for a state formula.
BDD primed_bdd(
    const DecisionDiagramManager& dd_man,
    const std::map<std::string, VariableProperties>& variable_properties,
    const StateFormula& f);

// Output operator for state formulas.
std::ostream& operator<<(std::ostream& os, const StateFormula& f);


/* ====================================================================== */
/* PathFormula */

class PathFormulaVisitor;

// Abstract base class for path formulas.
//
// This class supports the visitor pattern.  Example usage:
//
//   class ConcretePathFormulaVisitor : public PathFormulaVisitor {
//     ...
//   };
//
//   PathFormula* formula = ...;
//   ConcretePathFormulaVisitor visitor;
//   formula->Accept(&visitor);
//
class PathFormula {
 public:
  /* Deletes this path formula. */
  virtual ~PathFormula() {}

  void Accept(PathFormulaVisitor* visitor) const;

  /* Tests if this path formula contains probabilistic elements. */
  virtual bool probabilistic() const = 0;

  /* Estimated effort for generating a sample for this path formula. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const = 0;

  /* Generates a sample for this path formula. */
  virtual bool sample(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const = 0;

  /* Generates a sample for this path formula. */
  virtual bool sample(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double epsilon,
		      DdNode* dd1, DdNode* dd2,
                      ModelCheckingStats* stats) const = 0;

  /* Verifies this path formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, const TypedValue& p, bool strict,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const = 0;

  /* Verifies this path formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 const TypedValue& p, bool strict,
			 double epsilon, bool estimate) const = 0;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const = 0;

protected:
  /* Constructs a path formula. */
  PathFormula() {}

private:
  // Disallow copy and assign.
  PathFormula(const PathFormula&);
  PathFormula& operator=(const PathFormula&);

  virtual void DoAccept(PathFormulaVisitor* visitor) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const PathFormula& f);
};

/* Output operator for path formulas. */
std::ostream& operator<<(std::ostream& os, const PathFormula& f);


/* ====================================================================== */
/* Conjunction */

class Conjunction : public StateFormula {
 public:
  /* Deletes this conjunction. */
  virtual ~Conjunction();

  /* Adds a conjunct to this conjunction. */
  void add_conjunct(const StateFormula* conjunct);

  /* Returns the conjuncts for this conjunction. */
  const std::vector<const StateFormula*>& conjuncts() const {
    return conjuncts_;
  }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const std::vector<int>& state) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

private:
  virtual void DoAccept(StateFormulaVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The conjuncts. */
  std::vector<const StateFormula*> conjuncts_;
};


/* ====================================================================== */
/* Disjunction */

class Disjunction : public StateFormula {
 public:
  /* Deletes this disjunction. */
  virtual ~Disjunction();

  /* Adds a disjunct to this disjunction. */
  void add_disjunct(const StateFormula* disjunct);

  /* Returns the disjuncts for this disjunction. */
  const std::vector<const StateFormula*>& disjuncts() const {
    return disjuncts_;
  }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const std::vector<int>& state) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

private:
  virtual void DoAccept(StateFormulaVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The disjuncts. */
  std::vector<const StateFormula*> disjuncts_;
};


/* ====================================================================== */
/* Negation */

/*
 * A negated state formula.
 */
class Negation : public StateFormula {
 public:
  /* Constructs a negation. */
  Negation(const StateFormula* negand);

  /* Deletes this negation. */
  virtual ~Negation();

  /* Returns the negated state formula. */
  const StateFormula& negand() const { return *negand_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const std::vector<int>& state) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

private:
  virtual void DoAccept(StateFormulaVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The negated state formula. */
  const StateFormula* negand_;
};


/* ====================================================================== */
/* Implication */

/*
 * An implication.
 */
class Implication : public StateFormula {
 public:
  /* Constructs an implication. */
  Implication(const StateFormula* antecedent, const StateFormula* consequent);

  /* Deletes this implication. */
  virtual ~Implication();

  /* Returns the antecedent of this implication. */
  const StateFormula& antecedent() const { return *antecedent_; }

  /* Returns the consequent of this implication. */
  const StateFormula& consequent() const { return *consequent_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Tests if this state formula holds in the given state. */
  virtual bool holds(const std::vector<int>& state) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

private:
  virtual void DoAccept(StateFormulaVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

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
class Probabilistic : public StateFormula {
 public:
  /* Constructs a probabilistic path quantification. */
  Probabilistic(const TypedValue& threshold, bool strict,
		const PathFormula* formula);

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
  virtual bool holds(const std::vector<int>& state) const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

private:
  virtual void DoAccept(StateFormulaVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The probability threshold. */
  TypedValue threshold_;
  /* Whether the threshold is strict. */
  bool strict_;
  /* The path formula. */
  const PathFormula* formula_;
  /* Cached acceptance sampling results. */
  mutable std::map<std::vector<int>, std::pair<size_t, double> > cache_;
};


/* ====================================================================== */
/* Comparison */

/*
 * A comparison state formula.
 */
class Comparison : public StateFormula {
 public:
  // Supported computation operators.
  enum Operator {
    LESS, LESS_EQUAL, GREATER_EQUAL, GREATER, EQUAL, NOT_EQUAL
  };

  /* Deletes this comparison. */
  virtual ~Comparison();

  // Returns the operator for this comparison.
  const Operator op() const { return op_; }

  /* Returns the first expression of this comparison. */
  const Expression& expr1() const { return *expr1_; }

  /* Returns the second expression of this comparison. */
  const Expression& expr2() const { return *expr2_; }

  /* Tests if this state formula contains probabilistic elements. */
  virtual bool probabilistic() const;

  /* Estimated effort for verifying this state formula using the
     statistical engine. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Verifies this state formula using the statistical engine. */
  virtual bool verify(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const;

  /* Verifies this state formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

protected:
  /* Constructs a comparison. */
  Comparison(Operator op, const Expression& expr1, const Expression& expr2);

private:
  virtual void DoAccept(StateFormulaVisitor* visitor) const;

  // The operator for this comparison.
  Operator op_;
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
  virtual bool holds(const std::vector<int>& state) const;

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
  virtual bool holds(const std::vector<int>& state) const;

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
  virtual bool holds(const std::vector<int>& state) const;

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
  virtual bool holds(const std::vector<int>& state) const;

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
  virtual bool holds(const std::vector<int>& state) const;

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
  virtual bool holds(const std::vector<int>& state) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;
};


/* ====================================================================== */
/* Until */

/*
 * An until path formula.
 */
class Until : public PathFormula {
 public:
  /* Constructs an until formula. */
  Until(const StateFormula* pre, const StateFormula* post,
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

  /* Estimated effort for generating a sample for this path formula. */
  virtual double effort(double q, double delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const;

  /* Generates a sample for this path formula. */
  virtual bool sample(const Model& model, const State& state,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
                      ModelCheckingStats* stats) const;

  /* Generates a sample for this path formula. */
  virtual bool sample(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, double epsilon,
		      DdNode* dd1, DdNode* dd2,
                      ModelCheckingStats* stats) const;

  /* Verifies this path formula using the mixed engine. */
  virtual bool verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, const TypedValue& p, bool strict,
		      double delta, double alpha, double beta,
		      SamplingAlgorithm algorithm,
		      double epsilon,
                      ModelCheckingStats* stats) const;

  /* Verifies this path formula using the hybrid engine. */
  virtual DdNode* verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 const TypedValue& p, bool strict,
			 double epsilon, bool estimate) const;

  /* Clears the cache of any probabilistic operator. */
  virtual size_t clear_cache() const;

private:
  virtual void DoAccept(PathFormulaVisitor* visitor) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

  /* The precondition formula. */
  const StateFormula* pre_;
  /* The postcondition formula. */
  const StateFormula* post_;
  /* The lower time bound. */
  TypedValue min_time_;
  /* The upper time bound. */
  TypedValue max_time_;
};

// Abstract base class for state formula visitors.
class StateFormulaVisitor {
 public:
  void VisitConjunction(const Conjunction& formula);
  void VisitDisjunction(const Disjunction& formula);
  void VisitNegation(const Negation& formula);
  void VisitImplication(const Implication& formula);
  void VisitProbabilistic(const Probabilistic& formula);
  void VisitComparison(const Comparison& formula);

 protected:
  StateFormulaVisitor();
  StateFormulaVisitor(const StateFormulaVisitor&);
  StateFormulaVisitor& operator=(const StateFormulaVisitor&);
  ~StateFormulaVisitor();

 private:
  virtual void DoVisitConjunction(const Conjunction& formula) = 0;
  virtual void DoVisitDisjunction(const Disjunction& formula) = 0;
  virtual void DoVisitNegation(const Negation& formula) = 0;
  virtual void DoVisitImplication(const Implication& formula) = 0;
  virtual void DoVisitProbabilistic(const Probabilistic& formula) = 0;
  virtual void DoVisitComparison(const Comparison& formula) = 0;
};

// Abstract base class for path formula visitors.
class PathFormulaVisitor {
 public:
  void VisitUntil(const Until& formula);

 protected:
  PathFormulaVisitor();
  PathFormulaVisitor(const PathFormulaVisitor&);
  PathFormulaVisitor& operator=(const PathFormulaVisitor&);
  ~PathFormulaVisitor();

 private:
  virtual void DoVisitUntil(const Until& formula) = 0;
};

#endif /* FORMULAS_H */
