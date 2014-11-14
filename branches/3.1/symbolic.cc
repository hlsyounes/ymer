/*
 * Symbolic model checking of CSL formulas.
 *
 * Copyright (C) 2002 Dave Parker
 * Copyright (C) 2003--2004 Carnegie Mellon University
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

#include <float.h>

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "models.h"
#include "hybrid.h"
#include "src/compiled-property.h"

#include "cudd.h"

namespace {

class SymbolicVerifier
    : public CompiledPropertyVisitor, public CompiledPathPropertyVisitor {
 public:
  explicit SymbolicVerifier(const DecisionDiagramModel* dd_model,
                            bool estimate, bool top_level_property,
                            double epsilon);

  BDD result() const { return result_; }

 private:
  virtual void DoVisitCompiledNaryProperty(
      const CompiledNaryProperty& property);
  virtual void DoVisitCompiledNotProperty(const CompiledNotProperty& property);
  virtual void DoVisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property);
  virtual void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property);
  virtual void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& path_property);

  const DecisionDiagramModel* dd_model_;
  bool estimate_;
  bool top_level_property_;
  double epsilon_;
  double threshold_;
  bool strict_;
  BDD result_;
};

SymbolicVerifier::SymbolicVerifier(const DecisionDiagramModel* dd_model,
                                   bool estimate, bool top_level_property,
                                   double epsilon)
    : dd_model_(dd_model), estimate_(estimate),
      top_level_property_(top_level_property), epsilon_(epsilon),
      result_(dd_model->manager().GetConstant(false)) {
}

void SymbolicVerifier::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  switch (property.op()) {
    case CompiledNaryOperator::AND: {
      BDD result = dd_model_->manager().GetConstant(true);
      if (property.has_expr_operand()) {
        result = property.expr_operand().bdd() && result;
      }
      for (const CompiledProperty& operand : property.other_operands()) {
        operand.Accept(this);
        result = result_ && result;
      }
      result_ = result;
      break;
    }
    case CompiledNaryOperator::OR: {
      BDD result = dd_model_->manager().GetConstant(false);
      if (property.has_expr_operand()) {
        result = property.expr_operand().bdd() || result;
      }
      for (const CompiledProperty& operand : property.other_operands()) {
        operand.Accept(this);
        result = result_ || result;
      }
      result_ = result;
      break;
    }
    case CompiledNaryOperator::IFF: {
      BDD result = dd_model_->manager().GetConstant(false);
      bool has_result = false;
      if (property.has_expr_operand()) {
        result = property.expr_operand().bdd();
        has_result = true;
      }
      for (const CompiledProperty& operand : property.other_operands()) {
        operand.Accept(this);
        result = has_result ? result == result_ : result_;
        has_result = true;
      }
      result_ = result;
      break;
    }
  }
  result_ = dd_model_->reachable_states() && result_;
}

void SymbolicVerifier::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  property.operand().Accept(this);
  result_ = dd_model_->reachable_states() && !result_;
}

void SymbolicVerifier::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  threshold_ = property.threshold();
  strict_ = property.op() == CompiledProbabilityThresholdOperator::GREATER;
  property.path_property().Accept(this);
}

void SymbolicVerifier::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  result_ = dd_model_->reachable_states() && property.bdd();
}

// Recursive component of mtbdd_to_double_vector.
static void mtbdd_to_double_vector_rec(const DecisionDiagramManager& ddman,
                                       DdNode* dd, int level,
				       ODDNode* odd, long o, double* res) {
  if (dd != Cudd_ReadZero(ddman.manager())) {
    DdNode* e;
    DdNode* t;
    if (level == ddman.GetVariableCount() / 2) {
      res[o] = Cudd_V(dd);
      return;
    } else if (dd->index > ddman.GetBddVariable(2 * level).get()->index) {
      e = t = dd;
    } else {
      e = Cudd_E(dd);
      t = Cudd_T(dd);
    }
    mtbdd_to_double_vector_rec(ddman, e, level + 1, odd->e, o, res);
    mtbdd_to_double_vector_rec(ddman, t, level + 1, odd->t, o+odd->eoff,
                               res);
  }
}

// Converts an MTBDD to a double vector.
static double* mtbdd_to_double_vector(const DecisionDiagramManager& ddman,
                                      const ADD& dd, ODDNode* odd) {
  // Determine size.
  size_t n = odd->eoff + odd->toff;
  // Create array.
  double* res = new double[n];
  for (size_t i = 0; i < n; i++) {
    res[i] = 0.0;
  }
  mtbdd_to_double_vector_rec(ddman, dd.get(), 0, odd, 0, res);

  return res;
}

// Recursive component of double_vector_to_bdd.
static BDD double_vector_to_bdd_rec(const DecisionDiagramManager& ddman,
                                    const std::vector<double>& vec,
                                    bool strict, double bound,
                                    int level, ODDNode* odd, long o) {
  if (level == ddman.GetVariableCount() / 2) {
    return ddman.GetConstant((strict && vec[o] > bound)
                             || (!strict && vec[o] >= bound));
  } else {
    BDD e = (odd->eoff > 0)
        ? double_vector_to_bdd_rec(
            ddman, vec, strict, bound, level + 1, odd->e, o)
        : ddman.GetConstant(false);
    BDD t = (odd->toff > 0)
        ? double_vector_to_bdd_rec(
            ddman, vec, strict, bound, level + 1, odd->t, o+odd->eoff)
        : ddman.GetConstant(false);
    if (e.get() == t.get()) {
      return e;
    } else {
      return Ite(ddman.GetBddVariable(2 * level), t, e);
    }
  }
}

// Converts a double vector to a BDD.
static BDD double_vector_to_bdd(const DecisionDiagramManager& ddman,
                                const std::vector<double>& vec,
                                bool strict, double bound,
                                ODDNode* odd) {
  return double_vector_to_bdd_rec(ddman, vec, strict, bound, 0, odd, 0);
}

// Matrix vector multiplication stuff.
static void mult_rec(double* soln2, double* soln, double unif, HDDMatrix* hddm,
		     HDDNode* hdd, int level, long row, long col)
{
  if (hdd == hddm->zero) {
    // It is the zero node.
    return;
  } else if (hdd->sb) {
    // There is a sparse bit.
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
    // We have reached the bottom.
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

// Poisson tail bounds.
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

// Finds truncation points L and R and computes weights for a sum of
// Poisson probabilities with rate lambda given an error bound of
// epsilon (Fox & Glynn, CACM 31(4):440-445, 1988).
static void fox_glynn_weighter(int& left, int& right, double*& weights,
			       double& weight_sum, double lambda,
			       double epsilon) {
  // Set the mode.
  int m = int(lambda);

  // Make sure lambda is positive.
  if (lambda <= 0.0) {
    throw std::invalid_argument("lambda <= 0.0");
  }

  // Find truncation points L and R.
  double sqrt_lambda = sqrt(lambda);
  double sqrt_2pi = sqrt(2.0*M_PI);
  if (lambda < 25.0) {
    left = 0;
    if (exp(-lambda) < DBL_MIN) {
      throw std::underflow_error("potential underflow for Fox-Glynn");
    }
  } else {  // lambda >= 25.0
    // Find left using Corollary 2 with actual lambda.
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
    // Find right using Corollary 1 with lambda=400.
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
  } else {  // lambda >= 400.0
    // Find right using Corollary 1 with actual lambda.
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
  }

  // Compute weights.
  weights = new double[right - left + 1];
  weights[m - left] = (DBL_MAX*1e-10)/(right - left);
  // Down.
  for (int j = m; j > left; j--) {
    weights[j - 1 - left] = (j/lambda)*weights[j - left];
  }
  // Up.
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

  // Compute sum of weights.
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

void SymbolicVerifier::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  /*
   * Detect trivial cases.
   */
  if (threshold_ == 0 && !strict_) {
    /* Satisfied by all reachable states. */
    result_ = dd_model_->reachable_states();
    return;
  } else if (threshold_ == 1 && strict_) {
    /* Not satisfied by any states. */
    result_ = dd_model_->manager().GetConstant(false);
    return;
  }

  /*
   * Verify postcondition property.
   */
  bool estimate = false;
  bool top_level_property = false;
  std::swap(estimate_, estimate);
  std::swap(top_level_property_, top_level_property);
  path_property.post_property().Accept(this);
  BDD dd2 = result_;
  std::swap(estimate_, estimate);
  std::swap(top_level_property_, top_level_property);
  if (path_property.max_time() == 0) {
    /* No time is allowed to pass so solution is simply dd2. */
    result_ = dd2;
    return;
  }

  /*
   * Verify precondition property.
   */
  std::swap(estimate_, estimate);
  std::swap(top_level_property_, top_level_property);
  path_property.pre_property().Accept(this);
  BDD dd1 = result_;
  std::swap(estimate_, estimate);
  std::swap(top_level_property_, top_level_property);

  if (path_property.min_time() > 0) {
    // TODO(hlsyounes): implement support for interval time bounds.
    throw std::invalid_argument("interval time bounds not supported");
  }

  /*
   * Compute BDD representing non-absorbing states in transformed model.
   */
  BDD maybe = dd1 && !dd2;
  if (maybe.get() == dd_model_->manager().GetConstant(false).get()) {
    /* All states are absorbing so solution is simply dd2. */
    result_ = dd2;
    return;
  }

  /* ---------------------------------------------------------------------- */
  /* Transient analysis. */

  /* Time limit. */
  double time = path_property.max_time();
  /* ODD for model. */
  ODDNode* odd = dd_model_->odd();
  /* Number of states. */
  size_t nstates = odd->eoff + odd->toff;

  /*
   * Build HDD for matrix.
   */
  if (top_level_property_) {
    std::cout << "Building hybrid MTBDD matrix...";
  }
  ADD ddR = dd_model_->rate_matrix() * ADD(maybe);
  HDDMatrix* hddm = build_hdd_matrix(dd_model_->manager(), ddR, odd);
  if (top_level_property_) {
    std::cout << hddm->num_nodes << " nodes." << std::endl;
  }

  /*
   * Add sparse bits.
   */
  if (top_level_property_) {
    std::cout << "Adding sparse bits...";
  }
  add_sparse_bits(hddm);
  if (top_level_property_) {
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
  double* soln = mtbdd_to_double_vector(dd_model_->manager(), ADD(dd2), odd);
  double* soln2 = new double[nstates];
  int init = top_level_property_ ? dd_model_->initial_state_index() : -1;
  std::vector<double> sum((init >= 0) ? 1 : nstates, 0);

  /*
   * Compute poisson probabilities.
   */
  int left, right;
  double* weights;
  double weight_sum;
  if (top_level_property_) {
    std::cout << "Uniformization: " << max_diag << "*" << time << " = "
              << (max_diag*time) << std::endl;
  }
  fox_glynn_weighter(left, right, weights, weight_sum,
		     1.01*max_diag*time, epsilon_);
  if (top_level_property_) {
    std::cout << "Fox-Glynn: left = " << left << ", right = " << right
              << std::endl;
  }

  /*
   * Iterations before left bound to update vector.
   */
  if (top_level_property_) {
    std::cout << "Computing probabilities";
  }
  int iters;
  bool done = false;
  bool steady = false;
  for (iters = 1; iters < left && !done; iters++) {
    if (top_level_property_) {
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
    if (!estimate_) {
      done = true;
      double sqnorm = 0.0;
      double sqbound = epsilon_ * epsilon_ / 64.0;
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
  for (; iters <= right && !done; iters++) {
    if (top_level_property_) {
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
      if (!estimate_) {
	done = true;
	double sqnorm = 0.0;
	double sqbound = epsilon_ * epsilon_ / 64.0;
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
				left, right, epsilon_);
      if (VLOG_IS_ON(2) && init >= 0) {
	LOG(INFO) << iters
		  << " p_init in [" << sum[0] << ',' << (sum[0] + slack) << "]"
		  << "; threshold = " << threshold_;
      }
      if (!estimate_) {
	bool pass = false;
	if (strict_) {
	  pass = (sum[i] + slack <= threshold_ || sum[i] > threshold_);
	} else {
	  pass = (sum[i] + slack < threshold_ || sum[i] >= threshold_);
	}
	if (pass) {
	  num_pass++;
	  if (init >= 0 || num_pass == nstates) {
	    done = true;
	    break;
	  }
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
  if (top_level_property_) {
    std::cout << ' ' << iters << " iterations." << std::endl;
  }
  if (estimate_) {
    std::cout.precision(10);
    std::cout << "Pr[" << path_property.string() << "] = " << sum[0]
              << std::endl;
    std::cout.precision(6);
  }
  if (steady && top_level_property_) {
    std::cout << "Steady state detected." << std::endl;
  }
  /*
   * Free memory.
   */
  free_hdd_matrix(hddm);
  delete diags;
  delete soln;
  delete soln2;
  delete weights;

  if (init >= 0) {
    if ((strict_ && sum[0] > threshold_)
      || (!strict_ && sum[0] >= threshold_)) {
      result_ = dd_model_->initial_state();
      return;
    } else {
      result_ = dd_model_->manager().GetConstant(false);
      return;
    }
  } else {
    result_ = double_vector_to_bdd(
        dd_model_->manager(), sum, strict_, threshold_, odd);
    return;
  }
}

}  // namespace

BDD Verify(const CompiledProperty& property,
           const DecisionDiagramModel& dd_model,
           bool estimate, bool top_level_property, double epsilon) {
  SymbolicVerifier verifier(&dd_model, estimate, top_level_property, epsilon);
  property.Accept(&verifier);
  return verifier.result();
}
