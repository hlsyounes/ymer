/*
 * Sampling-based model checking of CSL formulas.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: sampling.cc,v 1.1 2003-08-13 18:48:30 lorens Exp $
 */
#include "formulas.h"
#include "states.h"
#include <cmath>


/* Verbosity level. */
extern int verbosity;
/* Total number of samples (for statistics). */
extern size_t total_samples;
/* Number of samples per trial (for statistics). */
extern std::vector<size_t> samples;
/* Total path lengths (for statistics). */
extern double total_path_lengths;

/* Nesting level of formula just being verified. */
static size_t formula_level = 0;


/* ====================================================================== */
/* Conjunction */

/* Verifies this state formula using acceptance sampling. */
bool Conjunction::verify(const Model& model, const State& state,
			 double delta, double alpha, double beta) const {
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
	if (!(*fj)->verify(model, state, delta, alpha, beta1)) {
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
	  if (!(*fi)->verify(model, state, delta, alpha, beta2)) {
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

/* Verifies this state formula using acceptance sampling. */
bool Disjunction::verify(const Model& model, const State& state,
			 double delta, double alpha, double beta) const {
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
	if ((*fj)->verify(model, state, delta, alpha1, beta)) {
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
	  if ((*fi)->verify(model, state, delta, alpha2, beta)) {
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

/* Verifies this state formula using acceptance sampling. */
bool Negation::verify(const Model& model, const State& state,
		      double delta, double alpha, double beta) const {
  return !negand().verify(model, state, delta, beta, alpha);
}


/* ====================================================================== */
/* Implication */

/* Verifies this state formula using acceptance sampling. */
bool Implication::verify(const Model& model, const State& state,
			 double delta, double alpha, double beta) const {
  bool prob1 = antecedent().probabilistic();
  if (!prob1 && !antecedent().verify(model, state, delta, beta, alpha)) {
    return true;
  }
  bool prob2 = consequent().probabilistic();
  if (!prob2 && consequent().verify(model, state, delta, alpha, beta)) {
    return true;
  }
  if (prob1) {
    if (prob2) {
      double alpha1 = std::min(2.0*alpha, 0.49);
      if (!antecedent().verify(model, state, delta, beta, alpha1)) {
	return true;
      }
      if (consequent().verify(model, state, delta, alpha1, beta)) {
	return true;
      }
      double alpha2 = alpha/2.0;
      if (alpha1 > alpha2) {
	if (!antecedent().verify(model, state, delta, beta, alpha2)) {
	  return true;
	}
	if (consequent().verify(model, state, delta, alpha2, beta)) {
	  return true;
	}
      }
      return false;
    } else {
      return !antecedent().verify(model, state, delta, beta, alpha);
    }
  } else if (prob2) {
    return consequent().verify(model, state, delta, alpha, beta);
  } else {
    return false;
  }
}


/* ====================================================================== */
/* Probabilistic */

/* Verifies this state formula using acceptance sampling. */
bool Probabilistic::verify(const Model& model, const State& state,
			   double delta, double alpha, double beta) const {
  double p0, p1;
  double theta = threshold().double_value();
  double alphap = alpha;
  double betap = beta;
  if (formula().probabilistic()) {
    while (true) {
      p0 = std::min(1.0, (theta + delta)*(1.0 - betap));
      p1 = std::max(0.0, 1.0 - (1.0 - (theta - delta))*(1.0 - alphap));
      if (p1 < p0) {
	break;
      } else {
	alphap *= 0.5;
	betap *= 0.5;
      }
    }
  } else {
    p0 = std::min(1.0, theta + delta);
    p1 = std::max(0.0, theta - delta);
  }
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
  if (formula_level == 0) {
    if (verbosity > 0) {
      std::cout << "Acceptance sampling";
    }
    if (verbosity > 1) {
      std::cout << std::endl;
    }
  }
  size_t n = 0;
  double logf = 0.0;
  formula_level++;
  while (logB < logf && logf < logA) {
    if (formula().sample(model, state, delta, alphap, betap)) {
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
      std::cout << n << '\t' << logf << '\t' << logB << '\t' << logA
		<< std::endl;
    }
  }
  formula_level--;
  if (formula_level == 0) {
    if (verbosity > 0) {
      std::cout << n << " samples." << std::endl;
    }
    total_samples += n;
    samples.push_back(n);
  }
  return logf <= logB;
}


/* ====================================================================== */
/* Comparison */

/* Verifies this state formula using acceptance sampling. */
bool Comparison::verify(const Model& model, const State& state,
			double delta, double alpha, double beta) const {
  return holds(state.values());
}


/* ====================================================================== */
/* Until */

/* Verifies an until formula over a path. */
static bool verify(bool& result, const Model& model,
		   const std::vector<const State*>& path, bool time_left,
		   const StateFormula& pre, bool prob1,
		   const StateFormula& post, bool prob2,
		   double delta, double alpha, double beta) {
  double alpha1 = std::min(2.0*alpha, 0.49);
  double beta1 = std::min(2.0*beta, 0.49);
  if (!prob1) {
    if (!prob2) {
      /*
       * Case 1: Neither pre nor post are probabilistic.
       *
       * If we get here, pre is true and post is false in all prior
       * states.  Therefore we only need to consider the last state.
       *
       * Formula is true if there is time left and post is true in the
       * last state.
       *
       * Formula is false if there is no time left or pre is false in
       * the last state.
       *
       * Otherwise, truth value cannot be determined.
       */
      const State& state = *path.back();
      if (time_left && post.holds(state.values())) {
	result = true;
	return true;
      } else if (!time_left || !pre.holds(state.values())) {
	result = false;
	return true;
      }
    } else {
      /*
       * Case 2: pre is not probabilistic, but post is.
       *
       * If we get here, pre is true in all prior states, but we do
       * not know for sure whether post is true in any prior state.
       *
       * Formula is true if there is time left and post is true in the
       * last state.
       *
       * If there is no time left or pre is false in the last state,
       * we need not look further.  The formula is then false iff post
       * is false in all prior state.
       *
       * Otherwise, truth value cannot be determined.
       */
      const State& state = *path.back();
      if (time_left && post.verify(model, state, delta, alpha1, beta)) {
	result = true;
	return true;
      } else if (!time_left || !pre.holds(state.values())) {
	size_t n = path.size();
	for (size_t i = 0; i < n; i++) {
	  if (post.verify(model, *path[i], delta, alpha/n, beta)) {
	    result = true;
	    return true;
	  }
	}
	result = false;
	return true;
      }
    }
  } else if (!prob2) {
    /*
     * Case 3: pre is probabilistic, but post is not.
     *
     * If we get here, post is false in all prior state, but we do not
     * know for sure whether pre is false in any prior state.
     *
     * If there is time left and post is true in the last state, we
     * need not look further.  The formula is then true iff pre is
     * true in all prior states.
     *
     * Formula is false if there is no time left or pre is false in
     * the last state.
     *
     * Otherwise, truth value cannot be determined.
     */
    const State& state = *path.back();
    if (time_left && post.holds(state.values())) {
      size_t n = path.size() - 1;
      for (size_t i = 0; i < n; i++) {
	if (!pre.verify(model, *path[i], delta, alpha, beta/n)) {
	  result = false;
	  return true;
	}
      }
      result = true;
      return true;
    } else if (!time_left || !pre.verify(model, state, delta, alpha, beta1)) {
      result = false;
      return true;
    }
  } else {
    /*
     * Case 4: Both pre and post are probabilistic.
     *
     * Formula holds if there is time left and post holds in the last
     * state and pre holds in all prior states.
     *
     * If there is no time left or the last state is absorbing, we
     * need not look further.  */
    const State& state = *path.back();
    if (time_left) {
      if (!post.verify(model, state, delta, alpha1, beta1)) {
	return false;
      }
      size_t i = path.size();
      for (size_t j = 0; j < i - 1; j++) {
	if (!pre.verify(model, *path[j], delta, alpha1, beta1)) {
	  return false;
	}
      }
      if (!post.verify(model, state, delta, alpha1, beta/i)) {
	return false;
      }
      for (size_t j = 0; j < i - 1; j++) {
	if (!pre.verify(model, *path[j], delta, alpha1, beta/i)) {
	  return false;
	}
      }
      result = true;
      return true;
    } else { /* time_left */
      size_t n = path.size();
      for (size_t i = 0; i < n; i++) {
	bool possibly_true = true;
	if (!post.verify(model, *path[i], delta, alpha/n, beta1)) {
	  possibly_true = false;
	}
	for (size_t j = 0; j < i - 1 && possibly_true; j++) {
	  if (!pre.verify(model, *path[j], delta, alpha/n, beta1)) {
	    possibly_true = false;
	  }
	}
	if (possibly_true) {
	  result = true;
	  return true;
	}
      }
      for (size_t i = 0; i < n; i++) {
	bool possibly_true = true;
	if (!post.verify(model, *path[i], delta, alpha/n, beta/i)) {
	  possibly_true = false;
	}
	for (size_t j = 0; j < i - 1 && possibly_true; j++) {
	  if (!pre.verify(model, *path[j], delta, alpha/n, beta/i)) {
	    possibly_true = false;
	  }
	}
	if (possibly_true) {
	  result = true;
	  return true;
	}
      }
      result = false;
      return true;
    }
  }
  return false;
}


/* Generates a sample for this path formula. */
bool Until::sample(const Model& model, const State& state,
		   double delta, double alpha, double beta) const {
  double t = 0.0;
  std::vector<const State*> path;
  path.push_back(&state);
  bool prob1 = pre().probabilistic();
  bool prob2 = post().probabilistic();
  bool result = false;
  double t_max = max_time().double_value();
  while (!::verify(result, model, path, t <= t_max,
		   pre(), prob1, post(), prob2, delta, alpha, beta)) {
    const State& curr_state = *path.back();
    if (verbosity > 2) {
      std::cout << "t = " << t << ": ";
      curr_state.print(std::cout);
      std::cout << std::endl;
    }
    const State& next_state = curr_state.next(model);
    t += next_state.dt();
    if (t <= t_max) {
      path.push_back(&next_state);
    } else {
      delete &next_state;
    }
  }
  if (verbosity > 2) {
    std::cout << "t = " << t << ": ";
    path.back()->print(std::cout);
    std::cout << std::endl;
    if (result) {
      std::cout << ">>positive sample" << std::endl;
    } else {
      std::cout << ">>negative sample" << std::endl;
    }
  }
  if (formula_level == 1) {
    total_path_lengths += path.size();
  }
  for (size_t i = 1; i < path.size(); i++) {
    delete path[i];
  }
  return result;
}
