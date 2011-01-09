/*
 * Mixed model checking of CSL formulas.
 *
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
 *
 * $Id: mixed.cc,v 2.1 2004-01-25 12:27:03 lorens Exp $
 */
#include "formulas.h"
#include "states.h"
#include "models.h"
#include <cmath>


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
bool Conjunction::verify(DdManager* dd_man, const Model& model,
			 const State& state, double delta, double alpha,
			 double beta, double epsilon) const {
  size_t n = conjuncts().size();
  for (FormulaList::const_reverse_iterator fi = conjuncts().rbegin();
       fi != conjuncts().rend(); fi++, n--) {
    const StateFormula& f = **fi;
    if (f.probabilistic()) {
      /*
       * Fast reject.
       */
      double beta1 = std::min(2.0*beta, 0.49);
      for (FormulaList::const_reverse_iterator fj = fi;
	   fj != conjuncts().rend(); fj++) {
	if (!(*fj)->verify(dd_man, model, state, delta, alpha, beta1,
			   epsilon)) {
	  return false;
	}
      }
      /*
       * Rigorous accept.
       */
      double beta2 = beta/n;
      if (beta1 > beta2) {
	for (FormulaList::const_reverse_iterator fj = fi;
	     fj != conjuncts().rend(); fj++) {
	  if (!(*fi)->verify(dd_man, model, state, delta, alpha, beta2,
			     epsilon)) {
	    return false;
	  }
	}
      }
      return true;
    } else if (!f.holds(state.values())) {
      return false;
    }
  }
  return true;
}


/* ====================================================================== */
/* Disjunction */

/* Verifies this state formula using the mixed engine. */
bool Disjunction::verify(DdManager* dd_man, const Model& model,
			 const State& state, double delta, double alpha,
			 double beta, double epsilon) const {
  size_t n = disjuncts().size();
  for (FormulaList::const_reverse_iterator fi = disjuncts().rbegin();
       fi != disjuncts().rend(); fi++, n--) {
    const StateFormula& f = **fi;
    if (f.probabilistic()) {
      /*
       * Fast accept.
       */
      double alpha1 = std::min(2.0*alpha, 0.49);
      for (FormulaList::const_reverse_iterator fj = fi;
	   fj != disjuncts().rend(); fj++) {
	if ((*fj)->verify(dd_man, model, state, delta, alpha1, beta,
			  epsilon)) {
	  return true;
	}
      }
      /*
       * Rigorous reject.
       */
      double alpha2 = alpha/n;
      if (alpha1 > alpha2) {
	for (FormulaList::const_reverse_iterator fj = fi;
	     fj != disjuncts().rend(); fj++) {
	  if ((*fi)->verify(dd_man, model, state, delta, alpha2, beta,
			    epsilon)) {
	    return true;
	  }
	}
      }
      return false;
    } else if (f.holds(state.values())) {
      return true;
    }
  }
  return false;
}


/* ====================================================================== */
/* Negation */

/* Verifies this state formula using the mixed engine. */
bool Negation::verify(DdManager* dd_man, const Model& model,
		      const State& state, double delta, double alpha,
		      double beta, double epsilon) const {
  return !negand().verify(dd_man, model, state, delta, beta, alpha, epsilon);
}


/* ====================================================================== */
/* Implication */

/* Verifies this state formula using the mixed engine. */
bool Implication::verify(DdManager* dd_man, const Model& model,
			 const State& state, double delta, double alpha,
			 double beta, double epsilon) const {
  bool prob1 = antecedent().probabilistic();
  if (!prob1 && !antecedent().verify(dd_man, model, state, delta, beta, alpha,
				     epsilon)) {
    return true;
  }
  bool prob2 = consequent().probabilistic();
  if (!prob2 && consequent().verify(dd_man, model, state, delta, alpha, beta,
				    epsilon)) {
    return true;
  }
  if (prob1) {
    if (prob2) {
      double alpha1 = std::min(2.0*alpha, 0.49);
      if (!antecedent().verify(dd_man, model, state, delta, beta, alpha1,
			       epsilon)) {
	return true;
      }
      if (consequent().verify(dd_man, model, state, delta, alpha1, beta,
			      epsilon)) {
	return true;
      }
      double alpha2 = alpha/2.0;
      if (alpha1 > alpha2) {
	if (!antecedent().verify(dd_man, model, state, delta, beta, alpha2,
				 epsilon)) {
	  return true;
	}
	if (consequent().verify(dd_man, model, state, delta, alpha2, beta,
				epsilon)) {
	  return true;
	}
      }
      return false;
    } else {
      return !antecedent().verify(dd_man, model, state, delta, beta, alpha,
				  epsilon);
    }
  } else if (prob2) {
    return consequent().verify(dd_man, model, state, delta, alpha, beta,
			       epsilon);
  } else {
    return false;
  }
}


/* ====================================================================== */
/* Probabilistic */

/* Verifies this state formula using the mixed engine. */
bool Probabilistic::verify(DdManager* dd_man, const Model& model,
			   const State& state, double delta, double alpha,
			   double beta, double epsilon) const {
  formula_level_++;
  bool res = formula().verify(dd_man, model, state, threshold(), strict(),
			      delta, alpha, beta, epsilon);
  formula_level_--;
  return res;
}


/* ====================================================================== */
/* Comparison */

/* Verifies this state formula using the mixed engine. */
bool Comparison::verify(DdManager* dd_man, const Model& model,
			const State& state, double delta, double alpha,
			double beta, double epsilon) const {
  return holds(state.values());
}


/* ====================================================================== */
/* Until */


/* Returns a BDD representing the given state. */
static DdNode* state_bdd(DdManager* dd_man, const ValueMap& values) {
  DdNode* dds = Cudd_ReadOne(dd_man);
  Cudd_Ref(dds);
  for (ValueMap::const_iterator vi = values.begin();
       vi != values.end(); vi++) {
    DdNode* ddv = (*vi).first->mtbdd(dd_man);
    double x = (*vi).second.double_value();
    DdNode* ddx = Cudd_addBddInterval(dd_man, ddv, x, x);
    Cudd_Ref(ddx);
    Cudd_RecursiveDeref(dd_man, ddv);
    DdNode* dda = Cudd_bddAnd(dd_man, ddx, dds);
    Cudd_Ref(dda);
    Cudd_RecursiveDeref(dd_man, ddx);
    Cudd_RecursiveDeref(dd_man, dds);
    dds = dda;
  }
  return dds;
}


/* Generates a sample for this path formula. */
bool Until::sample(DdManager* dd_man, const Model& model, const State& state,
		   double epsilon, DdNode* dd1, DdNode* dd2) const {
  double t = 0.0;
  size_t path_size = 1;
  const State* curr_state = &state;
  bool result = false;
  double t_max = max_time().double_value();
  while (true) {
    if (t <= t_max) {
      DdNode* dds = NULL;
      if (dd2 != NULL) {
	dds = state_bdd(dd_man, curr_state->values());
	DdNode* sol = Cudd_bddAnd(dd_man, dd2, dds);
	Cudd_Ref(sol);
	if (sol != Cudd_ReadLogicZero(dd_man)) {
	  result = true;
	  Cudd_RecursiveDeref(dd_man, dds);
	  Cudd_RecursiveDeref(dd_man, sol);
	  break;
	}
	Cudd_RecursiveDeref(dd_man, sol);
      } else if (post().holds(curr_state->values())) {
	result = true;
	break;
      }
      if (dd1 != NULL) {
	if (dd1 == Cudd_ReadLogicZero(dd_man)) {
	  result = false;
	  break;
	} else if (dd1 != Cudd_ReadOne(dd_man)) {
	  if (dds == NULL) {
	    dds = state_bdd(dd_man, curr_state->values());
	  }
	  DdNode* sol = Cudd_bddAnd(dd_man, dd1, dds);
	  Cudd_Ref(sol);
	  if (sol == Cudd_ReadLogicZero(dd_man)) {
	    result = false;
	    Cudd_RecursiveDeref(dd_man, dds);
	    Cudd_RecursiveDeref(dd_man, sol);
	    break;
	  }
	  Cudd_RecursiveDeref(dd_man, sol);
	}
      } else if (!pre().holds(curr_state->values())) {
	result = false;
	break;
      }
      if (dds != NULL) {
	Cudd_RecursiveDeref(dd_man, dds);
      }
    } else {
      result = false;
      break;
    }
    if (verbosity > 2) {
      std::cout << "t = " << t << ": ";
      curr_state->print(std::cout);
      std::cout << std::endl;
    }
    const State& next_state = curr_state->next(model);
    t += next_state.dt();
    if (t <= t_max) {
      path_size++;
    }
    if (curr_state != &state) {
      delete curr_state;
    }
    curr_state = &next_state;
  }
  if (verbosity > 2) {
    std::cout << "t = " << t << ": ";
    curr_state->print(std::cout);
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
  if (curr_state != &state) {
    delete curr_state;
  }
  return result;
}


/* Verifies this path formula using the mixed engine. */
bool Until::verify(DdManager* dd_man, const Model& model,
		   const State& state, const Rational& p, bool strict,
		   double delta, double alpha, double beta,
		   double epsilon) const {
  double theta = p.double_value();
  double p0 = std::min(1.0, theta + delta);
  double p1 = std::max(0.0, theta - delta);
  double logA = -log(alpha);
  /* If p1 is 0, then a beta of 0 can always be guaranteed. */
  if (p1 > 0.0) {
    logA += log(1.0 - beta);
  }
  double logB = log(beta);
  /* If p0 is 1, then an alpha of 0 can always be guaranteed. */
  if (p0 < 1.0) {
    logB -= log(1.0 - alpha);
  }
  if (StateFormula::formula_level() == 1) {
    if (verbosity > 0) {
      std::cout << "Acceptance sampling";
    }
    if (verbosity > 1) {
      std::cout << std::endl;
    }
  }
  size_t n = 0;
  size_t np = 0;
  double logf = 0.0;
  DdNode* dd1 = (pre().probabilistic()
		 ? pre().verify(dd_man, model, epsilon) : NULL);
  if (dd1 != NULL) {
    DdNode* ddr = model.reachability_bdd(dd_man);
    if (dd1 == ddr) {
      Cudd_RecursiveDeref(dd_man, dd1);
      dd1 = Cudd_ReadOne(dd_man);
      Cudd_Ref(dd1);
    }
    Cudd_RecursiveDeref(dd_man, ddr);
  }
  DdNode* dd2 = (post().probabilistic()
		 ? post().verify(dd_man, model, epsilon) : NULL);
  while (logB < logf && logf < logA) {
    if (sample(dd_man, model, state, epsilon, dd1, dd2)) {
      np++;
      if (p1 > 0.0) {
	logf += log(p1) - log(p0);
      } else {
	logf = -HUGE_VAL;
      }
    } else {
      if (p0 < 1.0) {
	logf += log(1.0 - p1) - log(1.0 - p0);
      } else {
	logf = HUGE_VAL;
      }
    }
    n++;
    if (verbosity == 1) {
      if (n % 1000 == 0) {
	std::cout << ':';
      } else if (n % 100 == 0) {
	std::cout << '.';
      }
    } else if (verbosity > 1) {
      std::cout << n << '\t' << np << '\t' << logf << '\t' << logB << '\t' << logA
		<< std::endl;
    }
  }
  if (dd1 != NULL) {
    Cudd_RecursiveDeref(dd_man, dd1);
  }
  if (dd2 != NULL) {
    Cudd_RecursiveDeref(dd_man, dd2);
  }
  if (StateFormula::formula_level() == 1) {
    if (verbosity > 0) {
      std::cout << n << " samples." << std::endl;
    }
    total_samples += n;
    samples.push_back(n);
  }
  return logf <= logB;
}
