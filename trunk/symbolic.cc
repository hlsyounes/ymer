/*
 * Symbolic model checking of CSL formulas.
 *
 * Copyright (C) 2002 Dave Parker
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
 * $Id: symbolic.cc,v 1.4 2003-11-07 04:26:37 lorens Exp $
 */
#include "formulas.h"
#include "models.h"
#include "hybrid.h"
#include <float.h>
#include <cmath>
#include <stdexcept>


/* Verbosity level. */
extern int verbosity;


/* ====================================================================== */
/* Conjunction */

/* Verifies this state formula using the hybrid engine. */
DdNode* Conjunction::verify(DdManager* dd_man, const Model& model,
			    double epsilon) const {
  DdNode* sol = model.reachability_bdd(dd_man);
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    DdNode* ddf = (*fi)->verify(dd_man, model, epsilon);
    DdNode* dda = Cudd_bddAnd(dd_man, ddf, sol);
    Cudd_Ref(dda);
    Cudd_RecursiveDeref(dd_man, ddf);
    Cudd_RecursiveDeref(dd_man, sol);
    sol = dda;
  }
  return sol;
}


/* ====================================================================== */
/* Disjunction */

/* Verifies this state formula using the hybrid engine. */
DdNode* Disjunction::verify(DdManager* dd_man, const Model& model,
			    double epsilon) const {
  DdNode* sol = Cudd_ReadLogicZero(dd_man);
  Cudd_Ref(sol);
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    DdNode* ddf = (*fi)->verify(dd_man, model, epsilon);
    DdNode* ddo = Cudd_bddOr(dd_man, ddf, sol);
    Cudd_Ref(ddo);
    Cudd_RecursiveDeref(dd_man, ddf);
    Cudd_RecursiveDeref(dd_man, sol);
    sol = ddo;
  }
  DdNode* ddr = model.reachability_bdd(dd_man);
  DdNode* dda = Cudd_bddAnd(dd_man, ddr, sol);
  Cudd_Ref(dda);
  Cudd_RecursiveDeref(dd_man, ddr);
  Cudd_RecursiveDeref(dd_man, sol);
  return dda;
}


/* ====================================================================== */
/* Negation */

/* Verifies this state formula using the hybrid engine. */
DdNode* Negation::verify(DdManager* dd_man, const Model& model,
			 double epsilon) const {
  DdNode* ddf = negand().verify(dd_man, model, epsilon);
  DdNode* sol = Cudd_Not(ddf);
  Cudd_Ref(sol);
  Cudd_RecursiveDeref(dd_man, ddf);
  DdNode* ddr = model.reachability_bdd(dd_man);
  DdNode* dda = Cudd_bddAnd(dd_man, ddr, sol);
  Cudd_Ref(dda);
  Cudd_RecursiveDeref(dd_man, ddr);
  Cudd_RecursiveDeref(dd_man, sol);
  return dda;
}


/* ====================================================================== */
/* Implication */

/* Verifies this state formula using the hybrid engine. */
DdNode* Implication::verify(DdManager* dd_man, const Model& model,
			    double epsilon) const {
  DdNode* dda = antecedent().verify(dd_man, model, epsilon);
  DdNode* ddn = Cudd_Not(dda);
  Cudd_Ref(ddn);
  Cudd_RecursiveDeref(dd_man, dda);
  DdNode* ddc = consequent().verify(dd_man, model, epsilon);
  DdNode* ddi = Cudd_bddOr(dd_man, ddn, ddc);
  Cudd_Ref(ddi);
  Cudd_RecursiveDeref(dd_man, ddn);
  Cudd_RecursiveDeref(dd_man, ddc);
  DdNode* ddr = model.reachability_bdd(dd_man);
  DdNode* sol = Cudd_bddAnd(dd_man, ddr, ddi);
  Cudd_Ref(sol);
  Cudd_RecursiveDeref(dd_man, ddr);
  Cudd_RecursiveDeref(dd_man, ddi);
  return sol;
}


/* ====================================================================== */
/* Probabilistic */

/* Verifies this state formula using the hybrid engine. */
DdNode* Probabilistic::verify(DdManager* dd_man, const Model& model,
			      double epsilon) const {
  formula_level_++;
  DdNode* res = formula().verify(dd_man, model, threshold(), strict(),
				 epsilon);
  formula_level_--;
  return res;
}


/* ====================================================================== */
/* Comparison */

/* Verifies this state formula using the hybrid engine. */
DdNode* Comparison::verify(DdManager* dd_man, const Model& model,
			   double epsilon) const {
  DdNode* ddc = bdd(dd_man);
  DdNode* ddr = model.reachability_bdd(dd_man);
  DdNode* sol = Cudd_bddAnd(dd_man, ddc, ddr);
  Cudd_Ref(sol);
  Cudd_RecursiveDeref(dd_man, ddc);
  Cudd_RecursiveDeref(dd_man, ddr);
  return sol;
}


/* ====================================================================== */
/* Until */

/* Recursive component of mtbdd_to_double_vector. */
static void mtbdd_to_double_vector_rec(DdManager* ddman, DdNode* dd,
				       DdNode** vars, int num_vars, int level,
				       ODDNode* odd, long o, double* res) {
  if (dd != Cudd_ReadZero(ddman)) {
    DdNode* e;
    DdNode* t;
    if (level == num_vars) {
      res[o] = Cudd_V(dd);
      return;
    } else if (dd->index > vars[level]->index) {
      e = t = dd;
    } else {
      e = Cudd_E(dd);
      t = Cudd_T(dd);
    }
    mtbdd_to_double_vector_rec(ddman, e, vars, num_vars, level + 1,
			       odd->e, o, res);
    mtbdd_to_double_vector_rec(ddman, t, vars, num_vars, level + 1,
			       odd->t, o+odd->eoff, res);
  }
}


/* Converts an MTBDD to a double vector. */
static double* mtbdd_to_double_vector(DdManager* ddman, DdNode* dd,
				      DdNode** vars, int num_vars,
				      ODDNode* odd) {
  /* Determine size. */
  size_t n = odd->eoff + odd->toff;
  /* Create array. */
  double* res = new double[n];
  for (size_t i = 0; i < n; i++) {
    res[i] = 0.0;
  }
  mtbdd_to_double_vector_rec(ddman, dd, vars, num_vars, 0, odd, 0, res);

  return res;
}


/* Recursive component of double_vector_to_bdd. */
static DdNode* double_vector_to_bdd_rec(DdManager* ddman, double* vec,
					bool strict, double bound,
					DdNode** vars, int num_vars,
					int level, ODDNode* odd, long o) {
  if (level == num_vars) {
    DdNode* dd = (((strict && vec[o] > bound) || (!strict && vec[o] >= bound))
		  ? Cudd_ReadOne(ddman) : Cudd_ReadLogicZero(ddman));
    Cudd_Ref(dd);
    return dd;
  } else {
    DdNode* e;
    DdNode* t;
    if (odd->eoff > 0) {
      e = double_vector_to_bdd_rec(ddman, vec, strict, bound, vars, num_vars,
				   level + 1, odd->e, o);
    } else {
      e = Cudd_ReadLogicZero(ddman);
      Cudd_Ref(e);
    }
    if (odd->toff > 0) {
      t = double_vector_to_bdd_rec(ddman, vec, strict, bound, vars, num_vars,
				   level + 1, odd->t, o+odd->eoff);
    } else {
      t = Cudd_ReadLogicZero(ddman);
      Cudd_Ref(t);
    }
    if (e == t) {
      Cudd_RecursiveDeref(ddman, t);
      return e;
    } else {
      Cudd_Ref(vars[level]);
      DdNode* dd = Cudd_bddIte(ddman, vars[level], t, e);
      Cudd_Ref(dd);
      Cudd_RecursiveDeref(ddman, vars[level]);
      Cudd_RecursiveDeref(ddman, t);
      Cudd_RecursiveDeref(ddman, e);
      return dd;
    }
  }
}


/* Converts a double vector to a BDD. */
static DdNode* double_vector_to_bdd(DdManager* ddman, double* vec,
				    bool strict, double bound,
				    DdNode** vars, int num_vars,
				    ODDNode* odd) {
  return double_vector_to_bdd_rec(ddman, vec, strict, bound, vars, num_vars,
				  0, odd, 0);
}


/* Matrix vector multiplication stuff. */
static void mult_rec(double* soln2, double* soln, double unif, HDDMatrix* hddm,
		     HDDNode* hdd, int level, long row, long col)
{
  if (hdd == hddm->zero) {
    /* It is the zero node. */
    return;
  } else if (hdd->sb) {
    /* There is a sparse bit. */
    SparseBit* sb = hdd->sb;
    int n = sb->n;
    double* non_zeros = sb->non_zeros;
    int* cols = sb->cols;
    int* row_starts = sb->row_starts;
    for (int i = 0; i < n; i++) {
      int l = row_starts[i];
      int h = row_starts[i+1];
      for (int j = l; j < h; j++) {		
	soln2[row + i] += soln[col + cols[j]]*non_zeros[j]*unif;
      }
    }
    return;
  } else if (level == hddm->num_levels) {
    /* We have reached the bottom. */
    soln2[row] += soln[col]*hdd->type.val*unif;
    return;
  }
  HDDNode* e = hdd->type.kids.e;
  if (e != hddm->zero) {
    mult_rec(soln2, soln, unif, hddm, e->type.kids.e, level + 1, row, col);
    mult_rec(soln2, soln, unif, hddm,
	     e->type.kids.t, level + 1, row, col + e->off);
  }
  HDDNode* t = hdd->type.kids.t;
  if (t != hddm->zero) {
    mult_rec(soln2, soln, unif, hddm,
	     t->type.kids.e, level + 1, row + hdd->off, col);
    mult_rec(soln2, soln, unif, hddm,
	     t->type.kids.t, level + 1, row + hdd->off, col + t->off);
  }
}


/* Poisson tail bounds. */
static double tail_bound(int n, double lambda, int left, int right,
			 double epsilon) {
  int m = (int) lambda;
  if (m >= 2 && n >= 2 + m) {
    double sqrt_lambda = sqrt(lambda);
    double sqrt_2pi = sqrt(2.0*M_PI);
    double k = (n - m - 3.0/2.0)/M_SQRT2/sqrt_lambda;
    double a = (1.0 - 1.0/lambda)*exp(1.0/16.0)*M_SQRT2;
    double d = 1.0/(1.0 - exp(-2.0/9.0*(k*M_SQRT2*sqrt_lambda + 3.0/2.0)));
    double tail = a*d*exp(-k*k/2.0)/k/sqrt_2pi;
    if (left > 0) {
      k = (m - n - 3.0/2.0)/sqrt_lambda;
      double b = (1.0 + 1.0/lambda)*exp(1.0/8.0/lambda);
      tail += b*exp(-k*k/2.0)/k/sqrt_2pi;
    }
    return (tail < 1.0) ? tail : 1.0;
  } else {
    return 1.0;
  }
}


/* Finds truncation points L and R and computes weights for a sum of
   Poisson probabilities with rate lambda given an error bound of
   epsilon (Fox & Glynn, CACM 31(4):440-445, 1988) */
static void fox_glynn_weighter(int& left, int& right, double*& weights,
			       double& weight_sum, double lambda,
			       double epsilon) {
  /* Set the mode. */
  int m = int(lambda);

  /* Make sure lambda is positive. */
  if (lambda <= 0.0) {
    throw std::invalid_argument("lambda <= 0.0");
  }

  /*
   * Find truncation points L and R.
   */
  double sqrt_lambda = sqrt(lambda);
  double sqrt_2pi = sqrt(2.0*M_PI);
  if (lambda < 25.0) {
    left = 0;
    if (exp(-lambda) < DBL_MIN) {
      throw std::underflow_error("potential underflow for Fox-Glynn");
    }
  } else { /* lambda >= 25.0 */
    /* Find left using Corollary 2 with actual lambda. */
    double b = (1.0 + 1.0/lambda)*exp(1.0/8.0/lambda);
    int i = 2;
    for (; i < m; i++) {
      double k = (i - 3.0/2.0)/sqrt_lambda;
      double T = b*exp(-k*k/2.0)/k/sqrt_2pi;
      if (T < epsilon/2) {
	break;
      }
    }
    left = m - i;
  }
  if (lambda < 400.0) {
    /* Find right using Corollary 1 with lambda=400. */
    double a = (1.0 + 1.0/400.0)*exp(1.0/16.0)*M_SQRT2;
    int i = 2;
    int i_max = (400 + 3)/2;
    for (; i < i_max; i++) {
      double k = (i - 3.0/2.0)/M_SQRT2/20.0;
      double d = 1.0/(1.0 - exp(-2.0/9.0*(k*M_SQRT2*20.0 + 3.0/2.0)));
      double Q = a*d*exp(-k*k/2.0)/k/sqrt_2pi;
      if ((left == 0 && Q < epsilon) || (left > 0 && Q < epsilon/2)) {
	break;
      }
    }
    right = m + i;
  } else { /* lambda >= 400.0 */
    /* Find right using Corollary 1 with actual lambda. */
    double a = (1.0 + 1.0/lambda)*exp(1.0/16.0)*M_SQRT2;
    int i = 2;
    int i_max = int((lambda + 3.0)/2.0);
    for (; i < i_max; i++) {
      double k = (i - 3.0/2.0)/M_SQRT2/sqrt_lambda;
      double d = 1.0/(1.0 - exp(-2.0/9.0*(k*M_SQRT2*sqrt_lambda + 3.0/2.0)));
      double Q = a*d*exp(-k*k/2.0)/k/sqrt_2pi;
      if ((left == 0 && Q < epsilon) || (left > 0 && Q < epsilon/2)) {
	break;
      }
    }
    right = m + i;
#if 0
    /* Check for underflow using corollary 3. */
    double c_m = 1.0/sqrt_2pi/sqrt(m)*exp(m - lambda - 1.0/12.0/m);
    double k_hat1 = right/sqrt_lambda + 1.0;
    double e = exp(1.0)*pow(DBL_MAX*1e-10/(right - left), -2.0/k_hat1/k_hat1);
    if (c_m*pow(e, -k_hat1*k_hat1/2.0) < DBL_MIN) {
      throw std::underflow_error("potential underflow for Fox-Glynn");
    }
#endif
  }
#if 0
  if (lambda >= 25.0) {
    /* Check for underflow using corollary 4. */
    double c_m = 1.0/sqrt_2pi/sqrt(m)*exp(m - lambda - 1.0/12.0/m);
    double k_tilde = (m - left)/sqrt_lambda;
    double p;
    if (k_tilde > 0 && k_tilde <= sqrt_lambda/2.0) {
      p = c_m*exp(-k_tilde*k_tilde/2.0
		  - k_tilde*k_tilde*k_tilde/3.0/sqrt_lambda);
    } else {
      double sqrt_m1 = sqrt(m + 1.0);
      p = std::max(c_m*pow(1.0 - k_tilde/sqrt_m1, k_tilde*sqrt_m1),
		   exp(-lambda));
    }
    if (p*((DBL_MAX*1e-10)/(right - left)) < DBL_MIN) {
      throw std::underflow_error("potential underflow for Fox-Glynn");
    }
  }
#endif

  /*
   * Compute weights.
   */
  weights = new double[right - left + 1];
  weights[m - left] = (DBL_MAX*1e-10)/(right - left);
  /* Down. */
  for (int j = m; j > left; j--) {
    weights[j - 1 - left] = (j/lambda)*weights[j - left];
  }
  /* Up. */
  if (lambda < 400.0) {
    if (right > 600) {
      throw std::underflow_error("potential undeflow for Fox-Glynn");
    } else {
      for (int j = m; j < right; ) {
	double q = lambda/(j + 1.0);
	if (weights[j - left] > DBL_MIN/q) {
	  weights[j + 1 - left] = q*weights[j - left];
	  j++;
	} else {
	  right = j;
	}
      }
    }
  } else {
    for (int j = m; j < right; j++) {
      weights[j + 1 - left] = (lambda/(j + 1.0))*weights[j - left];
    }
  }

  /*
   * Compute sum of weights.
   */
  weight_sum = 0.0;
  int s = left;
  int t = right;
  while (s < t) {
    if (weights[s - left] <= weights[t - left]) {
      weight_sum += weights[s - left];
      s++;
    } else {
      weight_sum += weights[t - left];
      t--;
    }
  }
  weight_sum += weights[s - left];
}


#if 0
/* this function was written by joachim meyer-kayser (converted from java) */
void compute_weights(int& left, int& right, double*& weights,
		     double& total_weight, double lambda,
		     double underflow, double overflow, double epsilon) {
  int m = (int) lambda;

  if (lambda < 25.0) {
    left = 0;
  }

  if (lambda < 400.0) {
    /* Find right using Corollary 1 with lambda=400. */
    double a = 1.0025*exp(1.0/16.0)*M_SQRT2;
    double startk = 1.0/(2.0*M_SQRT2*400.0);
    double stopk = 20.0/(2.0*M_SQRT2);
    double k;
    for (k = startk; k <= stopk; k += 3.0) {
      double d = 1.0/(1 - exp((-2.0/9.0)*(k*M_SQRT2*20.0 + 1.5)));
      double f = a*d*exp(-0.5*k*k)/(k*sqrt(2.0*M_PI));
      if (f <= epsilon/2.0) {
	break;
      }
    }
    if (k > stopk) {
      k = stopk;
    }
    right = (int) ceil(m + k*M_SQRT2*20.0 + 1.5);
  }

  if (lambda >= 400.0) {
    /* Find right using Corollary 1 using actual lambda. */
    double sqrtl  = sqrt(lambda);
    double a = (1.0 + 1.0/lambda)*exp(1.0/16.0)*M_SQRT2;
    double startk = 1.0/(2.0*M_SQRT2*lambda);
    double stopk = sqrtl/(2*M_SQRT2);
    double k;
    for (k = startk; k <= stopk; k += 3.0) {
      double d = 1.0/(1.0 - exp((-2.0/9.0)*(k*M_SQRT2*sqrtl + 1.5)));
      double f = a*d*exp(-0.5*k*k)/(k*sqrt(2.0*M_PI));
      if (f <= epsilon/2.0) {
	break;
      }
    }
    if (k > stopk) {
      k = stopk;
    }
    right = (int) ceil(m + k*M_SQRT2*sqrtl + 1.5);
  }

  if (lambda >= 25.0) {
    /* Find left using Corollary 2 using actual lambda. */
    double sqrtl  = sqrt(lambda);
    double b = (1.0 + 1.0/lambda)*exp(0.125/lambda);
    double startk = 1.0/(M_SQRT2*sqrtl);
    double stopk =  (m - 1.5)/(M_SQRT2*sqrtl);
    double k;
    for (k = startk; k <= stopk; k += 3.0) {
      if (b*exp(-0.5*k*k)/(k*sqrt(2.0*M_PI)) <= epsilon/2.0) {
	break;
      }
    }
    if (k > stopk) {
      k = stopk;
    }
    left = (int) floor(m - k*sqrtl - 1.5);
  }

  if (left < 0) {
    left = 0;
  }

  double q = overflow/(1e10*(right - left));
  weights = new double[right - left + 1];
  weights[m - left] = q;

  /* down */
  for (int j = m; j > left; j--) {
    weights[j - 1 - left] = (j/lambda)*weights[j - left];
  }

  /* up */
  if (lambda < 400) {
    if (right > 600) {
      throw std::overflow_error("overflow: right truncation point > 600");
    }
    for (int j = m; j < right; ) {
      q = lambda/(j+1);
      if (weights[j - left] > underflow/q) {
	weights[j + 1 - left] = q*weights[j - left];
	j++;
      } else {
	right = j;
      }
    }
  } else {
    for (int j = m; j < right; j++) {
      weights[j + 1 - left] = (lambda/(j + 1))*weights[j - left];
    }
  }

  int l = left;
  int r = right;
  total_weight = 0.0;
  while (l < r) {
    if (weights[l - left] <= weights[r - left]) {
      total_weight += weights[l - left];
      l++;
    } else {
      total_weight += weights[r - left];
      r--;
    }
  }
  total_weight += weights[l - left];
}
#endif


/* Verifies this path formula using the hybrid engine. */
DdNode* Until::verify(DdManager* dd_man, const Model& model,
		      const Rational& p, bool strict, double epsilon) const {
  /*
   * Detect trivial cases.
   */
  if (p == 0 && !strict) {
    /* Satisfied by all reachable states. */
    return model.reachability_bdd(dd_man);
  } else if (p == 1 && strict) {
    /* Not satisfied by any states. */
    DdNode* sol = Cudd_ReadLogicZero(dd_man);
    Cudd_Ref(sol);
    return sol;
  }

  /*
   * Verify postcondition formula.
   */
  DdNode* dd2 = post().verify(dd_man, model, epsilon);
  if (max_time() == 0) {
    /* No time is allowed to pass so solution is simply dd2. */
    return dd2;
  }

  /*
   * Verify precondition formula.
   */
  DdNode* dd1 = pre().verify(dd_man, model, epsilon);

  if (min_time() > 0) {
    // TODO
    throw std::invalid_argument("interval time bounds not supported");
  }

  /*
   * Compute BDD representing non-absorbing states in transformed model.
   */
  DdNode* ddn = Cudd_Not(dd2);
  Cudd_Ref(ddn);
  DdNode* maybe = Cudd_bddAnd(dd_man, dd1, ddn);
  Cudd_Ref(maybe);
  Cudd_RecursiveDeref(dd_man, dd1);
  Cudd_RecursiveDeref(dd_man, ddn);
  if (maybe == Cudd_ReadLogicZero(dd_man)) {
    /* All states are absorbing so solution is simply dd2. */
    Cudd_RecursiveDeref(dd_man, maybe);
    return dd2;
  }

  /* ---------------------------------------------------------------------- */
  /* Transient analysis. */

  /* Probability threshold. */
  double threshold = p.double_value();
  /* Time limit. */
  double time = max_time().double_value();
  /* ODD for model. */
  ODDNode* odd = model.odd(dd_man);
  /* Number of states. */
  size_t nstates = odd->eoff + odd->toff;

  /*
   * Build HDD for matrix.
   */
  if (verbosity > 0) {
    std::cout << "Building hybrid MTBDD matrix...";
  }
  DdNode* ddm = Cudd_BddToAdd(dd_man, maybe);
  Cudd_Ref(ddm);
  Cudd_RecursiveDeref(dd_man, maybe);
  DdNode* ddT = model.rate_mtbdd(dd_man);
  DdNode* ddR = Cudd_addApply(dd_man, Cudd_addTimes, ddT, ddm);
  Cudd_Ref(ddR);
  Cudd_RecursiveDeref(dd_man, ddT);
  Cudd_RecursiveDeref(dd_man, ddm);
  DdNode** rvars = model.row_variables(dd_man);
  DdNode** cvars = model.column_variables(dd_man);
  int nvars = Cudd_ReadSize(dd_man) / 2;
  HDDMatrix* hddm = build_hdd_matrix(dd_man, ddR, rvars, cvars, nvars, odd);
  if (verbosity > 0) {
    std::cout << hddm->num_nodes << " nodes." << std::endl;
  }

  /*
   * Add sparse bits.
   */
  if (verbosity > 0) {
    std::cout << "Adding sparse bits...";
  }
  add_sparse_bits(hddm);
  if (verbosity > 0) {
    std::cout << hddm->sbl << " levels, " << hddm->num_sb << " bits."
	      << std::endl;
  }

  /* Get vector of diagonals. */
  double* diags = hdd_negative_row_sums(hddm, nstates);

  /*
   * Find max diagonal element.
   */
  double max_diag = diags[0];
  for (size_t i = 1; i < nstates; i++) {
    if (diags[i] < max_diag) {
      max_diag = diags[i];
    }
  }
  max_diag = -max_diag;

  /* Uniformization constant. */
  double unif = 1.0/(1.01*max_diag);

  /*
   * Modify diagonals.
   */
  for (size_t i = 0; i < nstates; i++) {
    diags[i] = unif*diags[i] + 1;
  }

  /*
   * Create solution/iteration vectors.
   */
  DdNode* yes = Cudd_BddToAdd(dd_man, dd2);
  Cudd_Ref(yes);
  Cudd_RecursiveDeref(dd_man, dd2);
  double* soln = mtbdd_to_double_vector(dd_man, yes, rvars, nvars, odd);
  Cudd_RecursiveDeref(dd_man, yes);
  double* soln2 = new double[nstates];
  double* sum;
  int init = ((StateFormula::formula_level() == 1)
	      ? model.init_index(dd_man) : -1);
  if (init >= 0) {
    sum = new double[1];
    sum[0] = 0.0;
  } else {
    sum = new double[nstates];
    for (size_t i = 0; i < nstates; i++) {
      sum[i] = 0.0;
    }
  }

  /*
   * Compute poisson probabilities.
   */
  int left, right;
  double* weights;
  double weight_sum;
  if (verbosity > 0) {
    std::cout << "Uniformization: " << max_diag << "*" << time << " = "
	      << (max_diag*time) << std::endl;
  }
  fox_glynn_weighter(left, right, weights, weight_sum,
		     1.01*max_diag*time, epsilon);
  if (verbosity > 0) {
    std::cout << "Fox-Glynn: left = " << left << ", right = " << right
	      << std::endl;
  }

  /*
   * Iterations before left bound to update vector.
   */
  if (verbosity > 0) {
    std::cout << "Computing probabilities";
  }
  int iters;
  bool done = false;
  bool steady = false;
  for (iters = 1; iters < left && !done; iters++) {
    if (verbosity > 0) {
      if (iters % 1000 == 0) {
	std::cout << ':';
      } else if (iters % 100 == 0) {
	std::cout << '.';
      }
    }
    /*
     * Matrix vector multiplication.
     */
    for (size_t i = 0; i < nstates; i++) {
      soln2[i] = diags[i]*soln[i];
    }
    mult_rec(soln2, soln, unif, hddm, hddm->top, 0, 0, 0);
    /*
     * Check for steady state convergence.
     */
    done = true;
    double sqnorm = 0.0;
    double sqbound = epsilon*epsilon/64.0;
    for (size_t i = 0; i < nstates; i++) {
      double diff = soln2[i] - soln[i];
      sqnorm += diff*diff;
      if (sqnorm > sqbound) {
	done = false;
	break;
      }
    }
    if (done) {
      steady = true;
      if (init >= 0) {
	sum[0] = soln2[init];
      } else {
	for (size_t i = 0; i < nstates; i++) {
	  sum[i] = soln2[i];
	}
      }
      break;
    }
    /*
     * Prepare for next iteration.
     */
    double* tmp = soln;
    soln = soln2;
    soln2 = tmp;
  }

  /*
   * Accumulate weights.
   */
  if (verbosity > 1) {
    std::cout << std::endl;
  }
  for (; iters <= right && !done; iters++) {
    if (verbosity == 1) {
      if (iters % 1000 == 0) {
	std::cout << ':';
      } else if (iters % 100 == 0) {
	std::cout << '.';
      }
    }
    if (iters > 0) {
      /*
       * Matrix vector multiplication.
       */
      for (size_t i = 0; i < nstates; i++) {
	soln2[i] = diags[i]*soln[i];
      }
      mult_rec(soln2, soln, unif, hddm, hddm->top, 0, 0, 0);
      /*
       * Check for steady state convergence.
       */
      done = true;
      double sqnorm = 0.0;
      double sqbound = epsilon*epsilon/64.0;
      for (size_t i = 0; i < nstates; i++) {
	double diff = soln2[i] - soln[i];
	sqnorm += diff*diff;
	if (sqnorm > sqbound) {
	  done = false;
	  break;
	}
      }
      if (done) {
	steady = true;
	double weight = 0.0;
	for (int i = iters; i <= right; i++) {
	  weight += weights[i - left]/weight_sum;
	}
	if (init >= 0) {
	  sum[0] += weight*soln2[init];
	} else {
	  for (size_t i = 0; i < nstates; i++) {
	    sum[i] += weight*soln2[i];
	  }
	}
	break;
      }
      /*
       * Prepare for next iteration.
       */
      double* tmp = soln;
      soln = soln2;
      soln2 = tmp;
    }
    /*
     * Add to sum.
     */
    size_t num_pass = 0;
    for (size_t i = 0; i < nstates; i++) {
      if (init >= 0) {
	sum[0] += weights[iters - left]/weight_sum*soln[init];
      } else {
	sum[i] += weights[iters - left]/weight_sum*soln[i];
      }
      double slack = tail_bound(iters, 1.01*max_diag*time,
				left, right, epsilon);
      if (verbosity > 1 && init >= 0) {
	std::cout << iters
		  << " p_init in [" << sum[0] << ',' << (sum[0] + slack) << "]"
		  << "; threshold = " << threshold << std::endl;
      }
      bool pass = false;
      if (strict) {
	pass = (sum[i] + slack <= threshold || sum[i] > threshold);
      } else {
	pass = (sum[i] + slack < threshold || sum[i] >= threshold);
      }
      if (pass) {
	num_pass++;
	if (init >= 0 || num_pass == nstates) {
	  done = true;
	  break;
	}
      }
      if (init >= 0) {
	break;
      }
    }
    if (done) {
      break;
    }
  }
  if (iters > right) {
    iters = right;
  }
  if (verbosity > 0) {
    std::cout << iters << " iterations." << std::endl;
  }
  if (verbosity > 0 && steady) {
    std::cout << "Steady state detected." << std::endl;
  }
  /*
   * Free memory.
   */
  Cudd_RecursiveDeref(dd_man, ddR);
  free_hdd_matrix(hddm);
  delete diags;
  delete soln;
  delete soln2;
  delete weights;

  DdNode* sol;
  if (init >= 0) {
    if ((strict && sum[0] > threshold) || (!strict && sum[0] >= threshold)) {
      sol = model.init_bdd(dd_man);
    } else {
      sol = Cudd_ReadLogicZero(dd_man);
      Cudd_Ref(sol);
    }
  } else {
    sol = double_vector_to_bdd(dd_man, sum, strict, threshold,
			       rvars, nvars, odd);
  }
  delete sum;
  return sol;
}
