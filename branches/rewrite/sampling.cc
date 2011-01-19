/*
 * Sampling-based model-checking engine.
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
#include "sampling.h"
#include "learning.h"
#include "mtbdd.h"
#include "verbosity.h"
#include "gsl/gsl_cdf.h"
#include <cmath>
#include <set>
#include <stdexcept>


namespace Sampling {

  /* Memo data. */
  struct MemoData {
    int m, d0, d1;

    MemoData() : m(0), d0(0), d1(0) {}

    MemoData(int m, int d0, int d1) : m(m), d0(d0), d1(d1) {}
  };

  /* Verification memo. */
  struct VerificationMemo : public std::map<Values, MemoData> {
  };

  /* Indifference-region function. */
  static DeltaFun delta_;
  /* Sampling algorithm. */
  static Algorithm algorithm_ = SPRT;
  /* Whether a probability estimate is desired. */
  static bool estimate_ = false;
  /* Fixed nested error (negative value results in heuristic estimate). */
  static double nested_error_ = -1.0;
  /* Whether memoization is enabled for probabilistic statements. */
  static bool memoization_ = false;
  /* Memoized acceptance sampling results. */
  static std::map<const Probabilistic*, VerificationMemo> memo_;
  /* Whether sample trajectories should be recorded. */
  static bool record_trajectories_ = false;
  /* Nesting level of formula just being verified. */
  static int formula_level_ = 0;
  /* Whether the current top-level probabilistic operator is negated. */
  static bool negated_test_;
  /* Unreachability BDD for unbounded until. */
  static std::map<const PathFormula*, DdNode*> unreach_bdd_;
  /* Termination probability for nonparametric sampling. */
  static double termination_prob_ = 1e-3;
  /* Number of trials (for statistics). */
  static int num_trials_;
  /* Sample size mean (for statistics). */
  static double sample_mean_;
  /* Sample size variance [times trials - 1] (for statistics). */
  static double sample_var_;
  /* Total number of sample paths (for statistics). */
  static int num_paths_;
  /* Path length mean (for statistics). */
  static double path_mean_;
  /* Path length variance [times paths - 1] (for statisitcs). */
  static double path_var_;


  /* ====================================================================== */
  /* Statistics */

  /* Binomial cumulative distribution function. */
  double binocdf(int x, int n, double p) {
    if (x < 0 || x > n || n < 1 || p < 0.0 || p > 1.0) {
      throw std::invalid_argument("binocdf");
    } else if (x == n) {
      return 1.0;
    } else {
      double q = 1.0 - p;
      double lp = log(p);
      double lq = log(q);
      double y = pow(q, n);
      for (int i = 1; i <= x; i++) {
	double lc = lgamma(n + 1) - lgamma(n - i + 1) - lgamma(i + 1);
	y += exp(lc + i*lp + (n - i)*lq);
      }
      return std::min(1.0, y);
    }
  }


  /* Inverse of the binomial cumulative distribution function. */
  int binoinv(double y, int n, double p) {
    if (y < 0.0 || y > 1.0 || n < 1 || p < 0.0 || p > 1.0) {
      throw std::invalid_argument("binoinv");
    } else if (y == 0.0 || p == 0.0) {
      return 0;
    } else if (y == 1.0 || p == 1.0) {
      return n;
    } else {
      double q = 1.0 - p;
      double lp = log(p);
      double lq = log(q);
      double yy = pow(q, n);
      int x = 0;
      while (yy < y && x < n) {
	x++;
	double lc = lgamma(n + 1) - lgamma(n - x + 1) - lgamma(x + 1);
	yy += exp(lc + x*lp + (n - x)*lq);
      }
      return x;
    }
  }


  /* Inverse of the binomial cumulative distribution function with
     linear interpolation. */
  double realbinoinv(double y, int n, double p) {
    if (y < 0.0 || y > 1.0 || n < 1 || p < 0.0 || p > 1.0) {
      throw std::invalid_argument("realbinoinv");
    } else if (y == 0.0 || p == 0.0) {
      return 0.0;
    } else if (y == 1.0 || p == 1.0) {
      return n;
    } else {
      double q = 1.0 - p;
      double lp = log(p);
      double lq = log(q);
      double yy = pow(q, n);
      if (yy > y) {
	return -1.0;
      } else {
	int x = 0;
	while (yy < y && x < n) {
	  x++;
	  double lc = lgamma(n + 1) - lgamma(n - x + 1) - lgamma(x + 1);
	  double f = exp(lc + x*lp + (n - x)*lq);
	  if (yy + f > y) {
	    return x - 1 + (y - yy)/f;
	  } else {
	    yy += f;
	  }
	}
	return n;
      }
    }
  }


  /* Returns a sample size derived from the Hoeffding bound. */
  static int hoeffding_size(double alpha, double delta) {
    return int(ceil(0.5*log(2.0/alpha)/delta/delta) + 0.5);
  }


  /* Returns a single sampling plan based on the Hoeffding bound. */
  static std::pair<int, int>
  hoeffding_ssp(double p0, double p1, double alpha, double beta) {
    double theta = 0.5*(p1 + p0);
    int n = hoeffding_size(std::min(alpha, beta), 0.5*(p0 - p1));
    int c = int(floor(n*theta + 1.0) + 0.5);
    return std::make_pair(n, c);
  }


  /* Returns a single sampling plan. */
  static std::pair<int, int>
  ssp(double p0, double p1, double alpha, double beta) {
    if (p1 <= 0.0) {
      if (p0 >= 1.0) {
	return std::make_pair(1, 0);
      } else {
	return std::make_pair(int(ceil(log(alpha)/log(1.0 - p0))), 0);
      }
    } else if (p0 >= 1.0) {
      int n = int(ceil(log(beta)/log(p1)) + 0.5);
      return std::make_pair(n, n - 1);
    } else {
      int nmin = 1;
      int nmax = hoeffding_size(std::min(alpha, beta), 0.5*(p0 - p1));
      while (nmin < nmax) {
	int n = (nmin + nmax)/2;
	double x0 = realbinoinv(alpha, n, p0);
	double x1 = realbinoinv(1 - beta, n, p1);
	if (verbosity.sampling_plan) {
	  std::cout << n << '\t' << x0 << '\t' << x1 << '\t'
		    << (x0 >= x1 - 1e-10) << std::endl;
	}
	// Why x1 - 10e-10?
	if (x0 >= x1 - 1e-10 && x0 >= 0.0) {
	  nmax = n;
	} else {
	  nmin = n + 1;
	}
      }
      int c0 = nmax - binoinv(1 - alpha, nmax, 1 - p0) - 1;
      int c1 = binoinv(1 - beta, nmax, p1);
      if (verbosity.sampling_plan) {
	std::cout << nmax << '\t' << c0 << '\t' << c1 << '\t' << (c0 >= c1)
		  << std::endl;
      }
      while (c0 < c1 || c0 < 0) {
	nmax++;
	c0 = nmax - binoinv(1 - alpha, nmax, 1 - p0) - 1;
	c1 = binoinv(1 - beta, nmax, p1);
	if (verbosity.sampling_plan) {
	  std::cout << nmax << '\t' << c0 << '\t' << c1 << '\t' << (c0 >= c1)
		    << std::endl;
	}
      }
      return std::make_pair(nmax, (c0 + c1)/2);
    }
  }


  /* ====================================================================== */
  /* sample */

  /* Verifies the given state formula using the statistical engine. */
  static VerificationResult verify(const StateFormula& formula,
				   const Model& model, const State& state,
				   double alpha, double beta, double gamma);

  /* Checks for reachability. */
  static bool reachable(const State& state, DdNode* ddu) {
    bool result = true;
    if (ddu != 0) {
      DdNode* dds = state_bdd(state.values());
      DdNode* dda = bdd_and(ddu, dds);
      dd_deref(dds);
      if (dda != bdd_zero()) {
	result = false;
      }
      dd_deref(dda);
    }
    return result;
  }


  /* Extends the given trajectory. */
  static void extend_trajectory(Trajectory*& node, const Command* trigger,
				double dt, const State& state) {
    if (node != 0) {
      node->set_trigger(trigger, dt);
      Trajectory* next_node = new Trajectory(state.values());
      node->set_next(next_node);
      node = next_node;
    }
  }


  /* Verifies the given until formula over the given trajectory. */
  static VerificationResult
  until_verify(const StateFormula& pre, const StateFormula& post,
	       const Model& model,
	       const std::vector<Values>::const_iterator si,
	       const std::vector<Values>::const_iterator& end,
	       double alpha, double beta,
	       double alpha_n, double beta_n, double gamma_n) {
    if (si == end) {
      return ACCEPT;
    }
    State s(*si);
    VerificationResult ri = verify(post, model, s, alpha, beta_n, gamma_n);
    if (ri == ACCEPT) {
      return ACCEPT;
    } else {
      VerificationResult rj = verify(pre, model, s, alpha_n, beta_n, gamma_n);
      if (rj == REJECT) {
	return ri;
      } else {
	rj = std::min(rj, until_verify(pre, post, model, si + 1, end,
				       alpha, beta, alpha_n, beta_n, gamma_n));
	return std::max(ri, rj);
      }
    }
  }


  /* Generates a sample for the given until formula, assuming both pre
     and post are deterministic. */
  static VerificationResult
  until_sample_dd(const StateFormula& pre, const StateFormula& post,
		  double t_min, double t_max,
		  const Model& model, const State& state, DdNode* ddu,
		  Trajectory* trajectory) {
    double t = 0.0;
    const State* curr_state = &state;
    Trajectory* curr_node = trajectory;
    int path_length = 1;
    bool bresult = false, done = false, output = false;
    while (!done) {
      if (verbosity.schedule) {
	std::cout << "t = " << t << ": " << *curr_state << std::endl;
      }
      if (termination_prob_ > 0.0) {
        if (Distribution::rand01ex() < termination_prob_) {
          bresult = false;
          break;
        }
      } else if (!reachable(*curr_state, ddu)) {
        if (verbosity.schedule) {
          std::cout << "  satisfying state is not reachable" << std::endl;
        }
	bresult = false;
	break;
      }
      VerificationResult post_res = verify(post, model, *curr_state,
					   0.0, 0.0, 0.0);
      if (t_min <= t && post_res == ACCEPT) {
	bresult = true;
	break;
      }
      if (verify(pre, model, *curr_state, 0.0, 0.0, 0.0) == REJECT) {
	bresult = false;
	break;
      }
      const Command* trigger;
      double t_next;
      const State& next_state = curr_state->next(trigger, t_next, model);
      extend_trajectory(curr_node, trigger, t_next, next_state);
      t_next += t;
      if (t_min < t_next && post_res == ACCEPT) {
	t = t_min;
	bresult = true;
	done = true;
	output = true;
      }
      if (done) {
	delete &next_state;
      } else {
	if (curr_state != &state) {
	  delete curr_state;
	}
	curr_state = &next_state;
	t = t_next;
	if (t_max < t || !finite(t)) {
	  bresult = false;
	  done = true;
	  output = true;
	}
	path_length++;
      }
    }
    if (verbosity.schedule) {
      if (output) {
	std::cout << "t = " << t << ": " << *curr_state << std::endl;
      }
      if (bresult) {
	std::cout << ">>positive sample" << std::endl;
      } else {
	std::cout << ">>negative sample" << std::endl;
      }
    }
    if (curr_state != &state) {
      delete curr_state;
    }
    if (formula_level_ == 1) {
      num_paths_++;
      double q = path_length - path_mean_;
      double r = q/num_paths_;
      path_mean_ += r;
      path_var_ += (num_paths_ - 1)*q*r;
    }
    return bresult ? ACCEPT : REJECT;
  }


  /* Generates a sample for the given until formula, assuming pre is
     probabilistic and post is deterministic. */
  static VerificationResult
  until_sample_pd(const StateFormula& pre, const StateFormula& post,
		  double t_min, double t_max,
		  const Model& model, const State& state,
		  double alpha, double beta, double gamma,
		  DdNode* ddu, Trajectory* trajectory) {
    double t = 0.0;
    const State* curr_state = &state;
    Trajectory* curr_node = trajectory;
    int path_length = 1;
    bool bresult = false, done = false, output = false;
    std::set<Values> unique_states;
    while (!done) {
      if (verbosity.schedule) {
	std::cout << "t = " << t << ": " << *curr_state << std::endl;
      }
      if (!reachable(*curr_state, ddu)) {
	bresult = false;
	break;
      }
      VerificationResult post_res = verify(post, model, *curr_state,
					   0.0, 0.0, 0.0);
      if (t_min <= t && post_res == ACCEPT) {
	bresult = true;
	break;
      }
      unique_states.insert(curr_state->values());
      const Command* trigger;
      double t_next;
      const State& next_state = curr_state->next(trigger, t_next, model);
      extend_trajectory(curr_node, trigger, t_next, next_state);
      t_next += t;
      if (t_min < t_next && post_res == ACCEPT) {
	t = t_min;
	bresult = true;
	done = true;
	output = true;
      }
      if (done) {
	delete &next_state;
      } else {
	if (curr_state != &state) {
	  delete curr_state;
	}
	curr_state = &next_state;
	t = t_next;
	if (t_max < t || !finite(t)) {
	  bresult = false;
	  done = true;
	  output = true;
	}
	path_length++;
      }
    }
    if (verbosity.schedule && output) {
      std::cout << "t = " << t << ": " << *curr_state << std::endl;
    }
    VerificationResult result = bresult ? ACCEPT : REJECT;
    if (result == ACCEPT && !unique_states.empty()) {
      double alpha_n = alpha/unique_states.size();
      double gamma_n = gamma/unique_states.size();
      for (std::set<Values>::const_iterator si = unique_states.begin();
	   si != unique_states.end(); si++) {
	VerificationResult ri = verify(pre, model, State(*si),
				       alpha_n, beta, gamma_n);
	if (ri == REJECT) {
	  result = REJECT;
	  break;
	} else if (ri == UNDECIDED) {
	  result = UNDECIDED;
	}
      }
    }
    if (verbosity.schedule) {
      if (result == ACCEPT) {
	std::cout << ">>positive sample" << std::endl;
      } else if (result == REJECT) {
	std::cout << ">>negative sample" << std::endl;
      } else {
	std::cout << ">>undecided sample" << std::endl;
      }
    }
    if (curr_state != &state) {
      delete curr_state;
    }
    if (formula_level_ == 1) {
      num_paths_++;
      double q = path_length - path_mean_;
      double r = q/num_paths_;
      path_mean_ += r;
      path_var_ += (num_paths_ - 1)*q*r;
    }
    return result;
  }


  /* Generates a sample for the given until formula, assuming pre is
     deterministic and post is probabilistic. */
  static VerificationResult
  until_sample_dp(const StateFormula& pre, const StateFormula& post,
		  double t_min, double t_max,
		  const Model& model, const State& state,
		  double alpha, double beta, double gamma,
		  DdNode* ddu, Trajectory* trajectory) {
    double t = 0.0;
    const State* curr_state = &state;
    Trajectory* curr_node = trajectory;
    int path_length = 1;
    bool done = false, output = false;
    std::set<Values> unique_states;
    while (!done) {
      if (verbosity.schedule) {
	std::cout << "t = " << t << ": " << *curr_state << std::endl;
      }
      if (!reachable(*curr_state, ddu)) {
	break;
      }
      if (verify(pre, model, *curr_state, 0.0, 0.0, 0.0) == REJECT) {
	if (t_min <= t) {
	  unique_states.insert(curr_state->values());
	}
	break;
      }
      const Command* trigger;
      double t_next;
      const State& next_state = curr_state->next(trigger, t_next, model);
      extend_trajectory(curr_node, trigger, t_next, next_state);
      t_next += t;
      if (t_min < t_next) {
	unique_states.insert(curr_state->values());
      }
      if (curr_state != &state) {
	delete curr_state;
      }
      curr_state = &next_state;
      t = t_next;
      if (t_max < t || !finite(t)) {
	done = true;
	output = true;
      }
      path_length++;
    }
    if (verbosity.schedule && output) {
      std::cout << "t = " << t << ": " << *curr_state << std::endl;
    }
    VerificationResult result = REJECT;
    if (!unique_states.empty()) {
      double beta_n = beta/unique_states.size();
      double gamma_n = gamma/unique_states.size();
      for (std::set<Values>::const_iterator si = unique_states.begin();
	   si != unique_states.end(); si++) {
	VerificationResult ri = verify(pre, model, State(*si),
				       alpha, beta_n, gamma_n);
	if (ri == ACCEPT) {
	  result = ACCEPT;
	  break;
	} else if (ri == UNDECIDED) {
	  result = UNDECIDED;
	}
      }
    }
    if (verbosity.schedule) {
      if (result == ACCEPT) {
	std::cout << ">>positive sample" << std::endl;
      } else if (result == REJECT) {
	std::cout << ">>negative sample" << std::endl;
      } else {
	std::cout << ">>undecided sample" << std::endl;
      }
    }
    if (curr_state != &state) {
      delete curr_state;
    }
    if (formula_level_ == 1) {
      num_paths_++;
      double q = path_length - path_mean_;
      double r = q/num_paths_;
      path_mean_ += r;
      path_var_ += (num_paths_ - 1)*q*r;
    }
    return result;
  }


  /* Generates a sample for the given until formula, assuming both pre
     and post are probabilistic. */
  static VerificationResult
  until_sample_pp(const StateFormula& pre, const StateFormula& post,
		  double t_min, double t_max,
		  const Model& model, const State& state,
		  double alpha, double beta, double gamma,
		  DdNode* ddu, Trajectory* trajectory) {
    double t = 0.0;
    const State* curr_state = &state;
    Trajectory* curr_node = trajectory;
    int path_length = 1;
    bool done = false, output = false;
    std::set<Values> unique_pre_states;
    std::vector<Values> post_states;
    while (!done) {
      if (verbosity.schedule) {
	std::cout << "t = " << t << ": " << *curr_state << std::endl;
      }
      if (!reachable(*curr_state, ddu)) {
	break;
      }
      if (t < t_min) {
	unique_pre_states.insert(curr_state->values());
      }
      const Command* trigger;
      double t_next;
      const State& next_state = curr_state->next(trigger, t_next, model);
      extend_trajectory(curr_node, trigger, t_next, next_state);
      t_next += t;
      if (t_min < t_next) {
	post_states.push_back(curr_state->values());
      }
      if (curr_state != &state) {
	delete curr_state;
      }
      curr_state = &next_state;
      t = t_next;
      if (t_max < t || !finite(t)) {
	done = true;
	output = true;
      }
      path_length++;
    }
    if (verbosity.schedule && output) {
      std::cout << "t = " << t << ": " << *curr_state << std::endl;
    }
    int m = post_states.size();
    VerificationResult result;
    if (m == 0) {
      /* There are no states to satisfy post formula. */
      result = REJECT;
    } else {
      int n = unique_pre_states.size();
      int k = n + 2*m - 1;
      result = ACCEPT;
      double alpha_n = alpha/k;
      double beta_n = beta/k;
      double gamma_n = gamma/k;
      if (n > 0) {
	for (std::set<Values>::const_iterator si = unique_pre_states.begin();
	     si != unique_pre_states.end(); si++) {
	  VerificationResult ri = verify(pre, model, State(*si),
					 alpha_n, beta, gamma_n);
	  if (ri == REJECT) {
	    result = REJECT;
	    break;
	  } else if (ri == UNDECIDED) {
	    result = UNDECIDED;
	  }
	}
      }
      if (result != REJECT) {
	/* Pre formula may hold before time interval. */
	result = std::min(result,
			  until_verify(pre, post, model,
				       post_states.begin(), post_states.end(),
				       alpha, beta, alpha_n, beta_n, gamma_n));
      }
    }
    if (verbosity.schedule) {
      if (result == ACCEPT) {
	std::cout << ">>positive sample" << std::endl;
      } else if (result == REJECT) {
	std::cout << ">>negative sample" << std::endl;
      } else {
	std::cout << ">>undecided sample" << std::endl;
      }
    }
    if (curr_state != &state) {
      delete curr_state;
    }
    if (formula_level_ == 1) {
      num_paths_++;
      double q = path_length - path_mean_;
      double r = q/num_paths_;
      path_mean_ += r;
      path_var_ += (num_paths_ - 1)*q*r;
    }
    return result;
  }


  /* Generates a sample for the given until formula. */
  static VerificationResult
  until_sample(const Until& until,
	       const Model& model, const State& state,
	       double alpha, double beta, double gamma) {
    double t_min = until.min_time();
    double t_max = until.max_time();
    DdNode* ddu;
    if (!finite(t_max) && termination_prob_ == 0.0) {
      std::map<const PathFormula*, DdNode*>::const_iterator di =
	unreach_bdd_.find(&until);
      if (di != unreach_bdd_.end()) {
	ddu = (*di).second;
      } else {
	mtbdd_init(model);
	DdNode* ddf = formula_bdd(until.post());
	DdNode* ddr = preimage_bdd(model, ddf);
	dd_deref(ddf);
	if (ddr == dd_one()) {
	  ddu = 0;
	} else {
	  ddu = dd_not(ddr);
	}
	dd_deref(ddr);
	unreach_bdd_.insert(std::make_pair(&until, ddu));
      }
    }
    Trajectory* trajectory;
    if (formula_level_ == 1 && record_trajectories_) {
      trajectory = new Trajectory(state.values());
    } else {
      trajectory = 0;
    }
    VerificationResult result;
    if (until.pre().probabilistic()) {
      if (until.post().probabilistic()) {
	result = until_sample_pp(until.pre(), until.post(), t_min, t_max,
				 model, state, alpha, beta, gamma,
				 ddu, trajectory);
      } else {
	result = until_sample_pd(until.pre(), until.post(), t_min, t_max,
				 model, state, alpha, beta, gamma,
				 ddu, trajectory);
      }
    } else if (until.post().probabilistic()) {
      result = until_sample_dp(until.pre(), until.post(), t_min, t_max,
			       model, state, alpha, beta, gamma,
			       ddu, trajectory);
    } else {
      result = until_sample_dd(until.pre(), until.post(), t_min, t_max,
			       model, state, ddu, trajectory);
    }
    if (trajectory != 0) {
      if ((result == ACCEPT && !negated_test_)
	  || (result == REJECT && negated_test_)) {
	success_trajectories.push_back(trajectory);
      } else if (result != UNDECIDED) {
	failure_trajectories.push_back(trajectory);
      } else {
	delete trajectory;
      }
    }
    return result;
  }


  /* Generates a sample for the given path formula. */
  static VerificationResult sample(const PathFormula& formula,
				   const Model& model, const State& state,
				   double alpha, double beta, double gamma) {
    const Until* up = dynamic_cast<const Until*>(&formula);
    if (up != 0) {
      return until_sample(*up, model, state, alpha, beta, gamma);
    }
    throw std::logic_error("unexpected formula in `sample'");
  }


  /* ====================================================================== */
  /* wsample */

  /* Generates a weighted sample for the given until formula. */
  static double until_wsample(const Until& until,
			      const Model& model, const State& state) {
    if (until.pre().probabilistic() || until.post().probabilistic()) {
      throw std::logic_error("nonparametric engine does not support nesting");
    }
    Trajectory* trajectory;
    if (formula_level_ == 1 && record_trajectories_) {
      trajectory = new Trajectory(state.values());
    } else {
      trajectory = 0;
    }
    double t_min = until.min_time();
    double t_max = until.max_time();
    const StateFormula& pre = until.pre();
    const StateFormula& post = until.post();
    double t = 0.0;
    const State* curr_state = &state;
    Trajectory* curr_node = trajectory;
    int path_length = 1;
    bool bresult = false, terminated = false, done = false, output = false;
    while (!done) {
      if (verbosity.schedule) {
	std::cout << "t = " << t << ": " << *curr_state << std::endl;
      }
      if (Distribution::rand01ex() < termination_prob_) {
	terminated = true;
	bresult = false;
	break;
      }
      VerificationResult post_res = verify(post, model, *curr_state,
					   0.0, 0.0, 0.0);
      if (t_min <= t && post_res == ACCEPT) {
	bresult = true;
	break;
      }
      if (verify(pre, model, *curr_state, 0.0, 0.0, 0.0) == REJECT) {
	bresult = false;
	break;
      }
      const Command* trigger;
      double t_next;
      const State& next_state = curr_state->next(trigger, t_next, model);
      extend_trajectory(curr_node, trigger, t_next, next_state);
      t_next += t;
      if (t_min < t_next && post_res == ACCEPT) {
	t = t_min;
	bresult = true;
	done = true;
	output = true;
      }
      if (done) {
	delete &next_state;
      } else {
	if (curr_state != &state) {
	  delete curr_state;
	}
	curr_state = &next_state;
	t = t_next;
	if (t_max < t || !finite(t)) {
	  bresult = false;
	  done = true;
	  output = true;
	}
	path_length++;
      }
    }
    double result =
      bresult ? pow(1.0 - termination_prob_, -(path_length - 1)) : 0.0;
    if (verbosity.schedule) {
      if (output) {
	std::cout << "t = " << t << ": " << *curr_state << std::endl;
      }
      if (bresult) {
	std::cout << ">>positive sample (" << result << ")" << std::endl;
      } else {
	std::cout << ">>negative sample" << std::endl;
      }
    }
    if (curr_state != &state) {
      delete curr_state;
    }
    if (formula_level_ == 1) {
      num_paths_++;
      double q = path_length - path_mean_;
      double r = q/num_paths_;
      path_mean_ += r;
      path_var_ += (num_paths_ - 1)*q*r;
    }
    if (trajectory != 0) {
      if ((result > 0.0 && !negated_test_)
	  || (terminated && negated_test_)) {
	success_trajectories.push_back(trajectory);
      } else if ((terminated && !negated_test_)
		 || (result > 0.0 && negated_test_)) {
	failure_trajectories.push_back(trajectory);
      } else {
	delete trajectory;
      }
    }
    return result;
  }


  /* Generates a weighted sample for the given path formula. */
  static double wsample(const PathFormula& formula,
			const Model& model, const State& state) {
    const Until* up = dynamic_cast<const Until*>(&formula);
    if (up != 0) {
      return until_wsample(*up, model, state);
    }
    throw std::logic_error("unexpected formula in `sample'");
  }


  /* ====================================================================== */
  /* probabilistic_wverify */

  static VerificationResult
  probabilistic_wverify(const Probabilistic& probabilistic,
			const Model& model, const State& state,
			double alpha) {
    if (probabilistic.formula().probabilistic()) {
      throw std::logic_error("nonparametric engine does not support nesting");
    }

    double theta = probabilistic.threshold();
    double delta = (*delta_)(theta);

    /*
     * Detect trivial cases.
     */
    if (theta == 0.0 && !probabilistic.strict()) {
      /* P>=0[phi] or P<0[phi] */
      return probabilistic.negated() ? REJECT : ACCEPT;
    } else if (theta == 1.0 && probabilistic.strict()) {
      /* P>1[phi] or P<=1[phi] */
      return probabilistic.negated() ? ACCEPT : REJECT;
    }

    /*
     * Sequential estimation.
     */
    double m = 0.0;
    double t = 0.0;
    double a;
    int i = 0;
    if (formula_level_ == 0) {
      negated_test_ = probabilistic.negated();
    }
    formula_level_++;
    do {
      i++;
      double q = wsample(probabilistic.formula(), model, state) - m;
      double r = q/i;
      m += r;
      t += (i - 1)*q*r;
      if (i > 1) {
	a = gsl_cdf_tdist_Pinv(1.0 - 0.5*alpha, i - 1);
      }
      if (verbosity.sample) {
	if (i < 1000 || i % 1000 == 0) {
	  for (int j = 0; j < 2*(formula_level_ - 1); j++) {
	    std::cout << ' ';
	  }
	  std::cout << i << '\t' << m;
	  if (i > 1) {
	    std::cout << '\t' << (t + 1.0)/i << '\t' << delta*delta*i/a/a;
	  }
	  std::cout << std::endl;
	}
      }
    } while (i < 2 || (t + 1.0)/i > delta*delta*i/a/a);
    formula_level_--;
    if (formula_level_ == 0) {
      num_trials_++;
      double q = i - sample_mean_;
      double r = q/num_trials_;
      sample_mean_ += r;
      sample_var_ += (num_trials_ - 1)*q*r;
    }
    bool holds;
    if (probabilistic.strict()) {
      holds = m > theta;
    } else {
      holds = m >= theta;
    }
    if (holds) {
      return probabilistic.negated() ? REJECT : ACCEPT;
    } else {
      return probabilistic.negated() ? ACCEPT : REJECT;
    }
  }


  /* ====================================================================== */
  /* set_parameters */

  /* Sets engine parameters. */
  void set_parameters(DeltaFun delta, Algorithm algorithm, bool estimate,
		      double nested_error, bool memoization,
		      bool record_trajectories, double termination_prob) {
    delta_ = delta;
    algorithm_ = algorithm;
    estimate_ = estimate;
    nested_error_ = nested_error;
    memoization_ = memoization;
    record_trajectories_ = record_trajectories;
    termination_prob_ = termination_prob;
  }


  /* ====================================================================== */
  /* verify */

  /* Verifies the given conjunction using the statistical engine. */
  static VerificationResult
  conjunction_verify(const Conjunction& conjunction,
		     const Model& model, const State& state,
		     double alpha, double beta, double gamma) {
    int n = 0;
    for (FormulaList::const_iterator fi = conjunction.conjuncts().begin();
	 fi != conjunction.conjuncts().end(); fi++) {
      if ((*fi)->probabilistic()) {
	n++;
      }
    }
    double alpha_n = (n > 0) ? alpha/n : alpha;
    double gamma_n = (n > 0) ? gamma/n : gamma;
    VerificationResult result = ACCEPT;
    for (FormulaList::const_iterator fi = conjunction.conjuncts().begin();
	 fi != conjunction.conjuncts().end(); fi++) {
      VerificationResult ri =
	verify(**fi, model, state, alpha_n, beta, gamma_n);
      if (ri == REJECT) {
	return REJECT;
      } else if (ri == UNDECIDED) {
	result = UNDECIDED;
      }
    }
    return result;
  }


  /* Verifies the given conjunction using the statistical engine. */
  static VerificationResult
  disjunction_verify(const Disjunction& disjunction,
		     const Model& model, const State& state,
		     double alpha, double beta, double gamma) {
    int n = 0;
    for (FormulaList::const_iterator fi = disjunction.disjuncts().begin();
	 fi != disjunction.disjuncts().end(); fi++) {
      if ((*fi)->probabilistic()) {
	n++;
      }
    }
    double beta_n = (n > 0) ? beta/n : beta;
    double gamma_n = (n > 0) ? gamma/n : gamma;
    VerificationResult result = REJECT;
    for (FormulaList::const_iterator fi = disjunction.disjuncts().begin();
	 fi != disjunction.disjuncts().end(); fi++) {
      VerificationResult ri =
	verify(**fi, model, state, alpha, beta_n, gamma_n);
      if (ri == ACCEPT) {
	return ACCEPT;
      } else if (ri == UNDECIDED) {
	result = UNDECIDED;
      }
    }
    return result;
  }


  /* Verifies the given probabilistic formula using the statistical
     engine. */
  static VerificationResult
  probabilistic_verify(const Probabilistic& probabilistic,
		       const Model& model, const State& state,
		       double alpha, double beta, double gamma) {
    double theta = probabilistic.threshold();
    double delta = (*delta_)(theta);

    /*
     * Detect trivial cases.
     */
    if (theta == 0.0 && !probabilistic.strict()) {
      /* P>=0[phi] or P<0[phi] */
      return probabilistic.negated() ? REJECT : ACCEPT;
    } else if (theta == 1.0 && probabilistic.strict()) {
      /* P>1[phi] or P<=1[phi] */
      return probabilistic.negated() ? ACCEPT : REJECT;
    }

    /*
     * Adjust thresholds to account for observation error (nested
     * probabilistic statements).
     */
    double alphap, betap, gammap;
    if (probabilistic.formula().probabilistic()) {
      double q = 0.0;
      for (CommandList::const_iterator ci = model.commands().begin();
	   ci != model.commands().end(); ci++) {
	std::vector<double> m;
	(*ci)->delay().moments(m, 1);
	q = std::max(q, 1.0/m[0]);
      }
      double b;
      if (gamma > 0.0) {
	if (theta <= 0.5) {
	  b = delta/(2.0 + 2.0*delta - theta);
	} else {
	  b = delta/(1.0 + 2.0*delta + theta);
	}
      } else {
	b = 2.0*delta/(1.0 + 2.0*delta);
      }
      if (nested_error_ > 0.0 && nested_error_ < b) {
	alphap = betap = nested_error_;
      } else {
	alphap = betap = 0.8*b;
      }
      gammap = (gamma > 0.0) ? alphap : 0.0;
      if (formula_level_ == 0 && verbosity.engine) {
	std::cout << "Nested error: " << alphap << std::endl
		  << "Maximum nested error: " << b << std::endl;
      }
    } else {
      alphap = betap = gammap = 0.0;
    }
    double p00 = std::min(1.0, (theta + delta)*(1.0 - gammap - alphap));
    double p01 = std::max(theta, theta + (1.0 - theta)*betap);
    double p10 = std::min(theta, theta*(1.0 - alphap));
    double p11 =
      std::max(0.0, 1.0 - (1.0 - (theta - delta))*(1.0 - gammap - betap));
    if (probabilistic.negated()) {
      std::swap(alpha, beta);
    }

    /*
     * Compute acceptance and rejection lines.
     *
     * NB: The SPRT reduces to a curtailed single sampling plan when
     * p0 is 1 or p1 is 0.
     */
    double s00, h00, s01, h01, s10, h10, s11, h11;
    if (formula_level_ == 0 && verbosity.engine) {
      std::cout << "Acceptance sampling";
    }
    if (algorithm_ == HOEFFDING || algorithm_ == SSSP
	|| (algorithm_ == SPRT && (p00 >= 1.0 || p11 <= 0.0))) {
      std::pair<int, int> nc0, nc1;
      if (algorithm_ == HOEFFDING) {
	if (gamma > 0.0) {
	  nc0 = hoeffding_ssp(p00, p01, gamma, beta);
	  nc1 = hoeffding_ssp(p10, p11, alpha, gamma);
	} else {
	  nc0 = nc1 = hoeffding_ssp(p00, p11, alpha, beta);
	}
      } else {
	if (gamma > 0.0) {
	  nc0 = ssp(p00, p01, gamma, beta);
	  nc1 = ssp(p10, p11, alpha, gamma);
	} else {
	  nc0 = nc1 = ssp(p00, p11, alpha, beta);
	}
      }
      s00 = 0.0;
      h00 = nc0.second + 0.5;
      s01 = 1.0;
      h01 = nc0.second - nc0.first;
      s10 = 0.0;
      h10 = nc1.second + 0.5;
      s11 = 1.0;
      h11 = nc1.second - nc1.first;
      if (formula_level_ == 0 && verbosity.engine) {
	std::cout << " <" << nc0.first << ',' << nc0.second;
	if (gamma > 0.0) {
	  std::cout << ';' << nc1.first << ',' << nc1.second;
	}
	std::cout << ">";
      }
    } else { /* algorithm_ == SPRT && p0 < 1.0 && p1 > 0.0 */
      if (gamma > 0.0) {
	double denom0 = log(p01*(1.0 - p00)/p00/(1.0 - p01));
	s00 = s01 = log((1.0 - p00)/(1.0 - p01))/denom0;
	h00 = log(beta/(1.0 - gamma))/denom0;
	h01 = log((1.0 - beta)/gamma)/denom0;
	double denom1 = log(p11*(1.0 - p10)/p10/(1.0 - p11));
	s10 = s11 = log((1.0 - p10)/(1.0 - p11))/denom1;
	h10 = log(gamma/(1.0 - alpha))/denom1;
	h11 = log((1.0 - gamma)/alpha)/denom1;
      } else {
	double denom = log(p11*(1.0 - p00)/p00/(1.0 - p11));
	s00 = s01 = s10 = s11 = log((1.0 - p00)/(1.0 - p11))/denom;
	h00 = h10 = log(beta/(1.0 - alpha))/denom;
	h01 = h11 = log((1.0 - beta)/alpha)/denom;
      }
    }
    if (formula_level_ == 0 && (verbosity.sample || verbosity.schedule)) {
      std::cout << std::endl;
    }

    /*
     * Perform acceptance sampling.
     */
    int m = 0;
    int d0 = 0, d1 = 0;
    VerificationMemo* formula_memo;
    if (memoization_) {
      formula_memo = &memo_[&probabilistic];
      VerificationMemo::const_iterator mi = formula_memo->find(state.values());
      if (mi != formula_memo->end()) {
	m = (*mi).second.m;
	d0 = (*mi).second.d0;
	d1 = (*mi).second.d1;
      }
    }
    if (formula_level_ == 0) {
      negated_test_ = probabilistic.negated();
    }
    formula_level_++;
    while (d0 < h00 + s00*m && d1 > h11 + s11*m
	   && (gamma == 0 || d0 > h01 + s01*m || d1 < h10 + s10*m)) {
      const State& ns = state.resampled(model);
      VerificationResult s =
	sample(probabilistic.formula(), model, ns, alphap, betap, gammap);
      if (s == ACCEPT) {
	d0++;
      }
      if (s != REJECT) {
	d1++;
      }
      m++;
      if (&ns != &state) {
	delete &ns;
      }
      if (verbosity.engine && !verbosity.sample && !verbosity.schedule) {
	if (formula_level_ == 1) {
	  if (m % 1000 == 0) {
	    std::cout << ':';
	  } else if (m % 100 == 0) {
	    std::cout << '.';
	  }
	}
      } else if (verbosity.sample) {
	for (int i = 0; i < 2*(formula_level_ - 1); i++) {
	  std::cout << ' ';
	}
	std::cout << m << '\t' << d0;
	if (gamma > 0.0) {
	  std::cout << '\t' << d1;
	}
	if (d0 > h01 + s01*m || (s01 - s10)*d0 < (h10 - h01)) {
	  std::cout << "\t+ " << h00 + s00*m;
	} else {
	  std::cout << "\t? " << h10 + s10*m;
	}
	if (d1 < h10 + s10*m || (s01 - s10)*d1 < (h10 - h01)) {
	  std::cout << "\t- " << h11 + s11*m;
	} else {
	  std::cout << "\t? " << h01 + s01*m;
	}
	std::cout << std::endl;
      }
    }
    formula_level_--;
    if (formula_level_ == 0) {
      if (verbosity.engine) {
	std::cout << m << " samples." << std::endl;
	if (d0 >= h00 + s00*m) {
	  /* Pr[sum X_i >= d0 | p < theta] = 1 - F(d0-1,m,theta) */
	  std::cout << "p-value: " << 1.0 - binocdf(d0 - 1, m, theta)
		    << " (" << d0 << ' ' << m << ")" << std::endl;
	} else if (d1 <= h11 + s11*m) {
	  /* Pr[sum X_i <= d1 | p > theta] = F(d1,m,theta) */
	  std::cout << "p-value: " << binocdf(d1, m, theta)
		    << " (" << d1 << ' ' << m << ")" << std::endl;
	}
      }
      num_trials_++;
      double q = m - sample_mean_;
      double r = q/num_trials_;
      sample_mean_ += r;
      sample_var_ += (num_trials_ - 1)*q*r;
    }
    if (memoization_) {
      (*formula_memo)[state.values()] = MemoData(m, d0, d1);
    }
    VerificationResult result;
    if (d0 >= h00 + s00*m) {
      result = probabilistic.negated() ? REJECT : ACCEPT;
    } else if (d1 <= h11 + s11*m) {
      result = probabilistic.negated() ? ACCEPT : REJECT;
    } else {
      result = UNDECIDED;
    }

    /*
     * Generate additional samples for probability estimates.
     */
    if (formula_level_ == 0 && estimate_) {
      int n = hoeffding_size(std::min(alpha, beta), 0.5*(p00 - p11));
      if (verbosity.engine) {
	std::cout << "Statistical estimation <" << n << '>';
      }
      if (verbosity.sample || verbosity.schedule) {
	std::cout << std::endl;
      }
      formula_level_++;
      while (m < n) {
	const State& ns = state.resampled(model);
	VerificationResult s =
	  sample(probabilistic.formula(), model, ns, alphap, betap, gammap);
	if (s == ACCEPT) {
	  d0++;
	}
	if (s != REJECT) {
	  d1++;
	}
	m++;
	if (&ns != &state) {
	  delete &ns;
	}
	if (verbosity.engine && !verbosity.sample && !verbosity.schedule) {
	  if (formula_level_ == 1) {
	    if (m % 1000 == 0) {
	      std::cout << ':';
	    } else if (m % 100 == 0) {
	      std::cout << '.';
	    }
	  }
	} else if (verbosity.sample) {
	  std::cout << m << '\t' << d0 << '\t' << double(d0)/m;
	  if (gamma > 0.0) {
	    std::cout << d1 << '\t' << double(d1)/m;
	  }
	  std::cout << std::endl;
	}
      }
      formula_level_--;
      std::cout << std::endl
		<< "Probability estimate: " << double(d0)/m;
      if (gamma > 0.0) {
	std::cout << ' ' << double(d1)/m;
      }
      std::cout << std::endl;
    }

    /*
     * Clean up any reachability data.
     */
    std::map<const PathFormula*, DdNode*>::iterator di =
      unreach_bdd_.find(&probabilistic.formula());
    if (di != unreach_bdd_.end()) {
      if ((*di).second != 0) {
        dd_deref((*di).second);
      }
      unreach_bdd_.erase(di);
      if (unreach_bdd_.empty()) {
	mtbdd_finalize();
      }
    }

    /*
     * Return verification result.
     */
    return result;
  }


  /* Verifies the given state formula using the statistical engine. */
  static VerificationResult verify(const StateFormula& formula,
				   const Model& model, const State& state,
				   double alpha, double beta, double gamma) {
    const Conjunction* cp = dynamic_cast<const Conjunction*>(&formula);
    if (cp != 0) {
      return conjunction_verify(*cp, model, state, alpha, beta, gamma);
    }
    const Disjunction* dp = dynamic_cast<const Disjunction*>(&formula);
    if (dp != 0) {
      return disjunction_verify(*dp, model, state, alpha, beta, gamma);
    }
    const Probabilistic* pp = dynamic_cast<const Probabilistic*>(&formula);
    if (pp != 0) {
      if (algorithm_ == NONPARAMETRIC) {
	return probabilistic_wverify(*pp, model, state, std::min(alpha, beta));
      } else {
	return probabilistic_verify(*pp, model, state, alpha, beta, gamma);
      }
    }
    const Comparison<int>* icp =
      dynamic_cast<const Comparison<int>*>(&formula);
    if (icp != 0) {
      return icp->holds(state.values()) ? ACCEPT : REJECT;
    }
    const Comparison<double>* dcp =
      dynamic_cast<const Comparison<double>*>(&formula);
    if (dcp != 0) {
      return dcp->holds(state.values()) ? ACCEPT : REJECT;
    }
    throw std::logic_error("unexpected formula in `verify'");
  }


  /* Verifies the given state formula using the statistical engine. */
  VerificationResult verify(const StateFormula& formula, const Model& model,
			    double alpha, double beta, double gamma) {
    return verify(formula, model, State(model), alpha, beta, gamma);
  }


  /* Generates a sample for the given path formula. */
  VerificationResult sample(const PathFormula& formula, const Model& model,
			    double alpha, double beta, double gamma) {
    return sample(formula, model, State(model), alpha, beta, gamma);
  }


  /* ====================================================================== */
  /* clear_memo */

  /* Clears memoized verification results. */
  int clear_memo() {
    int n = 0;
    for (std::map<const Probabilistic*, VerificationMemo>::iterator pi =
	   memo_.begin();
	 pi != memo_.end(); pi++) {
      n += (*pi).second.size();
      (*pi).second.clear();
    }
    return n;
  }


  /* ====================================================================== */
  /* statistics */

  /* Gets the current statistics. */
  void get_statistics(double& sample_avg, double& sample_var,
		      double& path_avg, double& path_var) {
    sample_avg = sample_mean_;
    sample_var = (num_trials_ > 1) ? sample_var_/(num_trials_ - 1) : 0.0;
    path_avg = path_mean_;
    path_var = (num_paths_ > 1) ? path_var_/(num_paths_ - 1) : 0.0;
  }


  /* Clears the current statistics. */
  void clear_statistics() {
    num_trials_ = 0;
    sample_mean_ = sample_var_ = 0.0;
    num_paths_ = 0;
    path_mean_ = path_var_ = 0.0;
  }

} /* namespace Sampling */
