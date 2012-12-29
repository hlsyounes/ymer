/*
 * Mixed model checking of CSL formulas.
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
 */

#include "formulas.h"
#include "states.h"
#include "models.h"
#include "cudd.h"
#include <cmath>
#include <iostream>

/* Verbosity level. */
extern int verbosity;
/* Total number of samples (for statistics). */
extern size_t total_samples;
/* Number of samples per trial (for statistics). */
extern std::vector<size_t> samples;
/* Total path lengths (for statistics). */
extern double total_path_lengths;


/* ====================================================================== */
/* Conjunction */

/* Verifies this state formula using the mixed engine. */
bool Conjunction::verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 const State& state, DeltaFun delta,
			 double alpha, double beta,
			 SamplingAlgorithm algorithm,
			 double epsilon) const {
  for (auto fi = conjuncts().rbegin(); fi != conjuncts().rend(); fi++) {
    if (!(*fi)->verify(dd_man, model, state,
		       delta, alpha, beta, algorithm, epsilon)) {
      return false;
    }
  }
  return true;
}


/* ====================================================================== */
/* Disjunction */

/* Verifies this state formula using the mixed engine. */
bool Disjunction::verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 const State& state, DeltaFun delta,
			 double alpha, double beta,
			 SamplingAlgorithm algorithm,
			 double epsilon) const {
  for (auto fi = disjuncts().rbegin(); fi != disjuncts().rend(); fi++) {
    if ((*fi)->verify(dd_man, model, state,
		      delta, alpha, beta, algorithm, epsilon)) {
      return true;
    }
  }
  return false;
}


/* ====================================================================== */
/* Negation */

/* Verifies this state formula using the mixed engine. */
bool Negation::verify(const DecisionDiagramManager& dd_man, const Model& model,
		      const State& state, DeltaFun delta,
		      double alpha, double beta, SamplingAlgorithm algorithm,
		      double epsilon) const {
  return !negand().verify(dd_man, model, state,
			  delta, beta, alpha, algorithm, epsilon);
}


/* ====================================================================== */
/* Implication */

/* Verifies this state formula using the mixed engine. */
bool Implication::verify(const DecisionDiagramManager& dd_man,
                         const Model& model,
			 const State& state, DeltaFun delta,
			 double alpha, double beta,
			 SamplingAlgorithm algorithm,
			 double epsilon) const {
  if (!antecedent().verify(dd_man, model, state,
			   delta, beta, alpha, algorithm, epsilon)) {
    return true;
  } else {
    return consequent().verify(dd_man, model, state,
			       delta, alpha, beta, algorithm, epsilon);
  }
}


/* ====================================================================== */
/* Probabilistic */

/* Verifies this state formula using the mixed engine. */
bool Probabilistic::verify(const DecisionDiagramManager& dd_man,
                           const Model& model,
			   const State& state, DeltaFun delta,
			   double alpha, double beta,
			   SamplingAlgorithm algorithm,
			   double epsilon) const {
  formula_level_++;
  bool res = formula().verify(dd_man, model, state, threshold(), strict(),
			      delta, alpha, beta, algorithm, epsilon);
  formula_level_--;
  return res;
}


/* ====================================================================== */
/* Comparison */

/* Verifies this state formula using the mixed engine. */
bool Comparison::verify(const DecisionDiagramManager& dd_man,
                        const Model& model,
			const State& state, DeltaFun delta,
			double alpha, double beta, SamplingAlgorithm algorithm,
			double epsilon) const {
  return holds(state.values());
}


/* ====================================================================== */
/* Until */

/* Generates a sample for this path formula. */
bool Until::sample(const DecisionDiagramManager& dd_man, const Model& model,
                   const State& state,
		   double epsilon, DdNode* dd1, DdNode* dd2) const {
  double t = 0.0;
  size_t path_size = 1;
  State curr_state = state;
  bool result = false;
  double t_max = max_time().value<double>();
  while (true) {
    if (t <= t_max) {
      DdNode* dds = NULL;
      if (dd2 != NULL) {
	dds = model.state_bdd(dd_man, curr_state.values()).release();
	DdNode* sol = Cudd_bddAnd(dd_man.manager(), dd2, dds);
	Cudd_Ref(sol);
	if (sol != Cudd_ReadLogicZero(dd_man.manager())) {
	  result = true;
	  Cudd_RecursiveDeref(dd_man.manager(), dds);
	  Cudd_RecursiveDeref(dd_man.manager(), sol);
	  break;
	}
	Cudd_RecursiveDeref(dd_man.manager(), sol);
      } else if (post().holds(curr_state.values())) {
	result = true;
	break;
      }
      if (dd1 != NULL) {
	if (dd1 == Cudd_ReadLogicZero(dd_man.manager())) {
	  result = false;
	  break;
	} else if (dd1 != Cudd_ReadOne(dd_man.manager())) {
	  if (dds == NULL) {
	    dds = model.state_bdd(dd_man, curr_state.values()).release();
	  }
	  DdNode* sol = Cudd_bddAnd(dd_man.manager(), dd1, dds);
	  Cudd_Ref(sol);
	  if (sol == Cudd_ReadLogicZero(dd_man.manager())) {
	    result = false;
	    Cudd_RecursiveDeref(dd_man.manager(), dds);
	    Cudd_RecursiveDeref(dd_man.manager(), sol);
	    break;
	  }
	  Cudd_RecursiveDeref(dd_man.manager(), sol);
	}
      } else if (!pre().holds(curr_state.values())) {
	result = false;
	break;
      }
      if (dds != NULL) {
	Cudd_RecursiveDeref(dd_man.manager(), dds);
      }
    } else {
      result = false;
      break;
    }
    if (verbosity > 2) {
      std::cout << "t = " << t << ": ";
      curr_state.print(std::cout);
      std::cout << std::endl;
    }
    State next_state = curr_state.Next();
    t += next_state.time() - curr_state.time();
    if (t <= t_max) {
      path_size++;
    }
    curr_state = std::move(next_state);
  }
  if (verbosity > 2) {
    std::cout << "t = " << t << ": ";
    curr_state.print(std::cout);
    std::cout << std::endl;
    if (result) {
      std::cout << ">>positive sample" << std::endl;
    } else {
      std::cout << ">>negative sample" << std::endl;
    }
  }
  if (StateFormula::formula_level() == 1) {
    total_path_lengths += path_size;
  }
  return result;
}
