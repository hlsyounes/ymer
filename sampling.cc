/*
 * Sampling-based model checking of CSL formulas.
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

#include "formulas.h"
#include "comm.h"
#include "cudd.h"
#include "distributions.h"
#include "states.h"
#include "models.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>
#ifndef __USE_ISOC99
#define __USE_ISOC99
#endif
#include <math.h>
#include <cstdio>
#include <iostream>

/* Verbosity level. */
extern int verbosity;
/* Whether memoization is enabled. */
extern bool memoization;
/* Fixed nested error. */
extern double nested_error;
/* Fixed sample size. */
extern int fixed_sample_size;
/* Maximum path length. */
extern int max_path_length;
/* Total number of samples (for statistics). */
extern size_t total_samples;
/* Number of samples per trial (for statistics). */
extern std::vector<size_t> samples;
/* Total path lengths (for statistics). */
extern double total_path_lengths;

/* Nesting level of formula just being verified. */
size_t StateFormula::formula_level_ = 0;

/* Registered clients. */
static std::map<int, short> registered_clients;
/* Next client id. */
static short next_client_id = 1;


/* ====================================================================== */
/* Conjunction */

/* Estimated effort for verifying this state formula using the
   statistical engine. */
double Conjunction::effort(double q, DeltaFun delta, double alpha, double beta,
			   double alphap, double betap,
			   SamplingAlgorithm algorithm) const {
  double h = 0.0;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    h += (*fi)->effort(q, delta, alpha, beta, alphap, betap, algorithm);
  }
  return h;
}


/* Verifies this state formula using the statistical engine. */
bool Conjunction::verify(const Model& model, const State& state,
			 DeltaFun delta, double alpha, double beta,
			 SamplingAlgorithm algorithm) const {
  for (FormulaList::const_reverse_iterator fi = conjuncts().rbegin();
       fi != conjuncts().rend(); fi++) {
    if (!(*fi)->verify(model, state, delta, alpha, beta, algorithm)) {
      return false;
    }
  }
  return true;
}


/* Clears the cache of any probabilistic operator. */
size_t Conjunction::clear_cache() const {
  size_t n = 0;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    n += (*fi)->clear_cache();
  }
  return n;
}


/* ====================================================================== */
/* Disjunction */

/* Estimated effort for verifying this state formula using the
   statistical engine. */
double Disjunction::effort(double q, DeltaFun delta, double alpha, double beta,
			   double alphap, double betap,
			   SamplingAlgorithm algorithm) const {
  double h = 0.0;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    h += (*fi)->effort(q, delta, alpha, beta, alphap, betap, algorithm);
  }
  return h;
}


/* Verifies this state formula using the statistical engine. */
bool Disjunction::verify(const Model& model, const State& state,
			 DeltaFun delta, double alpha, double beta,
			 SamplingAlgorithm algorithm) const {
  for (FormulaList::const_reverse_iterator fi = disjuncts().rbegin();
       fi != disjuncts().rend(); fi++) {
    if ((*fi)->verify(model, state, delta, alpha, beta, algorithm)) {
      return true;
    }
  }
  return false;
}


/* Clears the cache of any probabilistic operator. */
size_t Disjunction::clear_cache() const {
  size_t n = 0;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    n += (*fi)->clear_cache();
  }
  return n;
}


/* ====================================================================== */
/* Negation */

/* Estimated effort for verifying this state formula using the
   statistical engine. */
double Negation::effort(double q, DeltaFun delta, double alpha, double beta,
			double alphap, double betap,
			SamplingAlgorithm algorithm) const {
  return negand().effort(q, delta, alpha, beta, alphap, betap, algorithm);
}


/* Verifies this state formula using the statistical engine. */
bool Negation::verify(const Model& model, const State& state,
		      DeltaFun delta, double alpha, double beta,
		      SamplingAlgorithm algorithm) const {
  return !negand().verify(model, state, delta, beta, alpha, algorithm);
}


/* Clears the cache of any probabilistic operator. */
size_t Negation::clear_cache() const {
  return negand().clear_cache();
}


/* ====================================================================== */
/* Implication */

/* Estimated effort for verifying this state formula using the
   statistical engine. */
double Implication::effort(double q, DeltaFun delta, double alpha, double beta,
			   double alphap, double betap,
			   SamplingAlgorithm algorithm) const {
  return (antecedent().effort(q, delta, alpha, beta, alphap, betap, algorithm)
	  + consequent().effort(q,
				delta, alpha, beta, alphap, betap, algorithm));
}


/* Verifies this state formula using the statistical engine. */
bool Implication::verify(const Model& model, const State& state,
			 DeltaFun delta, double alpha, double beta,
			 SamplingAlgorithm algorithm) const {
  if (!antecedent().verify(model, state, delta, beta, alpha, algorithm)) {
    return true;
  } else {
    return consequent().verify(model, state, delta, alpha, beta, algorithm);
  }
}


/* Clears the cache of any probabilistic operator. */
size_t Implication::clear_cache() const {
  return antecedent().clear_cache() + consequent().clear_cache();
}


/* ====================================================================== */
/* Probabilistic */

/*
 * Inverse error function.
 */
static double erfinv(double y) {
  // MATLAB code
  static double a[] = { 0.886226899, -1.645349621, 0.914624893, -0.140543331 };
  static double b[] = { -2.118377725, 1.442710462, -0.329097515, 0.012229801 };
  static double c[] = { -1.970840454, -1.624906493, 3.429567803, 1.641345311 };
  static double d[] = { 3.543889200, 1.637067800 };

  if (y == -1.0) {
    return -HUGE_VAL;
  } else if (y == 1.0) {
    return HUGE_VAL;
  } else if (isnan(y) || fabs(y) > 1.0) {
    return NAN;
  } else {
    double x;
    if (fabs(y) <= 0.7) {
      double z = y*y;
      x = y*(((a[3]*z + a[2])*z + a[1])*z + a[0]);
      x /= (((b[3]*z + b[2])*z + b[1])*z + b[0])*z + 1.0;
    } else if (y > 0.7 && y < 1.0) {
      double z = sqrt(-log(0.5*(1.0 - y)));
      x = (((c[3]*z + c[2])*z + c[1])*z + c[0])/((d[1]*z + d[0])*z + 1.0);
    } else if (y < -0.7 && y > -1.0) {
      double z = sqrt(-log(0.5*(1.0 + y)));
      x = -(((c[3]*z + c[2])*z + c[1])*z + c[0])/((d[1]*z + d[0])*z + 1.0);
    }
    double u = (erf(x) - y)/(M_2_SQRTPI*exp(-x*x));
    return x - u/(1.0 + x*u);
  }
}


/*
 * Inverse of the standard normal cumulative distribution function.
 */
static double norminv(double p) {
  return M_SQRT2*erfinv(2.0*p - 1.0);
}


/*
 * Inverse of Student's T cumulative distribution function.
 */
static double tinv(double p, double v) {
  if (v <= 0.0) {
    return NAN;
  } else if (p == 0.0) {
    return -HUGE_VAL;
  } else if (p == 1.0) {
    return HUGE_VAL;
  } else if (v == 1.0) {
    // MATLAB code
    return tan(M_PI*(p - 0.5));
  } else {
    // Abramowitz & Stegun formula 26.7.5? (from MATLAB)
    double x = norminv(p);
    double z = x*x;
    return x*(1.0 + (z + 1.0)/(4.0*v)
	      + ((5.0*z + 16.0)*z + 3.0)/(96.0*v*v)
	      + (((3.0*z + 19.0)*z + 17)*z - 15.0)/(384.0*v*v*v)
	      + (((((79.0*z + 776.0)*z + 1482.0)*z - 1920.0)*z - 945.0)
		 /(92160.0*v*v*v*v)));
  }
}


/*
 * Inverse of the binomial cumulative distribution function.
 */
int binoinv(double y, int n, double p) {
  if (y < 0.0 || y > 1.0 || n < 1 || p < 0.0 || p > 1.0) {
    return -1;
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


/*
 * Inverse of the binomial cumulative distribution function with
 * linear interpolation.
 */
double realbinoinv(double y, int n, double p) {
  if (y < 0.0 || y > 1.0 || n < 1 || p < 0.0 || p > 1.0) {
    return -1.0;
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


/*
 * Returns a single sampling plan.
 */
static std::pair<int, int>
single_sampling_plan(double p0, double p1, double alpha, double beta) {
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
    int nmin = 1, nmax = -1;
    int n = nmin;
    while (nmax < 0 || nmin < nmax) {
      double x0 = realbinoinv(alpha, n, p0);
      double x1 = realbinoinv(1 - beta, n, p1);
      std::cout << n << '\t' << x0 << '\t' << x1 << '\t' << (x0 >= x1 - 1e-10)
		<< std::endl;
      if (x0 >= x1 - 1e-10 && x0 >= 0.0) {
	nmax = n;
      } else {
	nmin = n + 1;
      }
      if (nmax < 0) {
	n *= 2;
      } else {
	n = (nmin + nmax)/2;
      }
    }
    int c0 = n - binoinv(1 - alpha, n, 1 - p0) - 1;
    int c1 = binoinv(1 - beta, n, p1);
    std::cout << n << '\t' << c0 << '\t' << c1 << '\t' << (c0 >= c1)
	      << std::endl;
    while (c0 < c1 || c0 < 0) {
      n++;
      c0 = n - binoinv(1 - alpha, n, 1 - p0) - 1;
      c1 = binoinv(1 - beta, n, p1);
      std::cout << n << '\t' << c0 << '\t' << c1 << '\t' << (c0 >= c1)
		<< std::endl;
    }
    return std::make_pair(n, (c0 + c1)/2);
  }
}


/* Estimated effort for verifying this state formula using the
   statistical engine. */
double Probabilistic::effort(double q, DeltaFun delta,
			     double alpha, double beta,
			     double alphap, double betap,
			     SamplingAlgorithm algorithm) const {
  double p0, p1;
  double theta = threshold().value<double>();
  double nested_effort;
  if (formula().probabilistic()) {
    double r = 0.5*(sqrt(5) - 1.0);
    double a = 0.0;
    double b = 2.0*(*delta)(theta)/(1.0 + 2.0*(*delta)(theta));
    double x1 = a + (1.0 - r)*(b - a);
    double x2 = a + r*(b - a);
    double f1 = formula().effort(q, delta, alphap, betap, x1, x1, algorithm);
    double f2 = formula().effort(q, delta, alphap, betap, x2, x2, algorithm);
    do {
      if (f2 > f1) {
	b = x2;
	x2 = x1;
	f2 = f1;
	x1 = a + (1.0 - r)*(b - a);
	f1 = formula().effort(q, delta, alphap, betap, x1, x1, algorithm);
      } else {
	a = x1;
	x1 = x2;
	f1 = f2;
	x2 = b - (1.0 - r)*(b - a);
	f2 = formula().effort(q, delta, alphap, betap, x2, x2, algorithm);
      }
    } while ((b - a)/(b + a) > 1e-3);
    nested_effort = 0.5*(f1 + f2);
    p0 = std::min(1.0, (theta + (*delta)(theta))*(1.0 - alphap));
    p1 = std::max(0.0, 1.0 - (1.0 - (theta - (*delta)(theta)))*(1.0 - betap));
  } else {
    p0 = std::min(1.0, theta + (*delta)(theta));
    p1 = std::max(0.0, theta - (*delta)(theta));
    nested_effort = formula().effort(q, delta, alphap, betap, 0, 0, algorithm);
  }
  double n;
  if (algorithm == SEQUENTIAL) {
    double x = (norminv(alpha)*sqrt(p0*(1.0 - p0))
		+ norminv(beta)*sqrt(p1*(1.0 - p1)));
    double y = p0 - p1;
    n = x*x/y/y;
  } else {
    n = -(log(beta/(1.0 - alpha))*log((1.0 - beta)/alpha)
	  /log(p1/p0)/log((1.0 - p0)/(1.0 - p1)));
  }
  return n*nested_effort;
}


/* Verifies this state formula using the statistical engine. */
bool Probabilistic::verify(const Model& model, const State& state,
			   DeltaFun delta, double alpha, double beta,
			   SamplingAlgorithm algorithm) const {
  double p0, p1;
  double theta = threshold().value<double>();
  double alphap;
  double betap;
  if (formula().probabilistic()) {
    double q = 0.0;
    for (CommandList::const_iterator ci = model.commands().begin();
	 ci != model.commands().end(); ci++) {
      std::vector<double> m;
      (*ci)->delay().moments(m, 1);
      q = std::max(q, 1.0/m[0]);
    }
    double r = 0.5*(sqrt(5) - 1.0);
    double a = 0.0;
    double b = 2.0*(*delta)(theta)/(1.0 + 2.0*(*delta)(theta));
    if (nested_error > a && nested_error < b) {
      a = b = nested_error;
    } else {
      double x1 = a + (1.0 - r)*(b - a);
      double x2 = a + r*(b - a);
      double f1 = effort(q, delta, alpha, beta, x1, x1, algorithm);
      double f2 = effort(q, delta, alpha, beta, x2, x2, algorithm);
      do {
	if (f2 > f1) {
	  b = x2;
	  x2 = x1;
	  f2 = f1;
	  x1 = a + (1.0 - r)*(b - a);
	  f1 = effort(q, delta, alpha, beta, x1, x1, algorithm);
	} else {
	  a = x1;
	  x1 = x2;
	  f1 = f2;
	  x2 = b - (1.0 - r)*(b - a);
	  f2 = effort(q, delta, alpha, beta, x2, x2, algorithm);
	}
      } while ((b - a)/(b + a) > 1e-3);
    }
    alphap = betap = 0.5*(a + b);
    if (formula_level() == 0) {
      if (verbosity > 0) {
	std::cout << "Nested error: " << alphap << ", " << betap << std::endl;
	std::cout << "Maximum symmetric nested error: "
		  << 2.0*(*delta)(theta)/(1.0 + 2.0*(*delta)(theta))
		  << std::endl;
      }
    }
  } else {
    alphap = 0.0;
    betap = 0.0;
  }
  p0 = std::min(1.0, (theta + (*delta)(theta))*(1.0 - alphap));
  p1 = std::max(0.0, 1.0 - (1.0 - (theta - (*delta)(theta)))*(1.0 - betap));
  if (algorithm == FIXED) {
    int c = 0;
    if (formula_level() == 0) {
      if (verbosity > 0) {
        std::cout << "Fixed-size sampling";
      }
      if (verbosity > 1) {
        std::cout << std::endl;
      }
    }
    formula_level_++;
    for (int i = 1; i <= fixed_sample_size; ++i) {
      if (formula().sample(model, state, delta, alphap, betap, algorithm)) {
        c++;
      }
      if (verbosity == 1) {
        if (formula_level() == 1) {
          if (i % 1000 == 0) {
            std::cout << ':';
          } else if (i % 100 == 0) {
            std::cout << '.';
          }
        }
      } else if (verbosity > 1) {
        for (size_t j = 0; j < 2*(formula_level() - 1); j++) {
          std::cout << ' ';
        }
        std::cout << i << '\t' << c << std::endl;
      }
    }
    formula_level_--;
    if (formula_level() == 0) {
      if (verbosity > 0) {
        std::cout << fixed_sample_size << " samples." << std::endl;
      }
      total_samples += fixed_sample_size;
      samples.push_back(fixed_sample_size);
    }
    double p = double(c)/fixed_sample_size;
    if (strict()) {
      return p > theta;
    } else {
      return p >= theta;
    }
  }
  if (algorithm == ESTIMATE) {
    int c = 0, n = 0;
    double es = (*delta)(theta)*(*delta)(theta);
    double a = 1.0 - 0.5*alpha;
    double p, t, b;
    if (formula_level() == 0) {
      if (verbosity > 0) {
        std::cout << "Sequential estimation";
      }
      if (verbosity > 1) {
        std::cout << std::endl;
      }
    }
    formula_level_++;
    while (c == 0 || n < 2 || (t + 1.0)/c/c > es/b/b) {
      if (formula().sample(model, state, delta, alphap, betap, algorithm)) {
	c++;
      }
      n++;
      p = double(c)/n;
      if (verbosity == 1) {
        if (formula_level() == 1) {
          if (n % 1000 == 0) {
            std::cout << ':';
          } else if (n % 100 == 0) {
            std::cout << '.';
          }
        }
      } else if (verbosity > 1) {
        for (size_t i = 0; i < 2*(formula_level() - 1); i++) {
          std::cout << ' ';
        }
	std::cout << n << '\t' << c << '\t' << p/(1 + (*delta)(theta)) << '\t'
		  << p/(1 - (*delta)(theta)) << std::endl;
      }
      t = c*(1.0 - p);
      b = tinv(a, n - 1.0);
    }
    formula_level_--;
    if (formula_level() == 0) {
      if (verbosity > 0) {
        std::cout << n << " samples." << std::endl;
      }
      std::cout << "Pr[" << formula() << "] = " << p << " ("
                << p/(1 + (*delta)(theta)) << ',' << p/(1 - (*delta)(theta))
                << ")" << std::endl;
      total_samples += n;
      samples.push_back(n);
    }
    if (strict()) {
      return p > theta;
    } else {
      return p >= theta;
    }
  }

  int n, c;
  double logA, logB;
  if (algorithm == SEQUENTIAL) {
    std::pair<int, int> nc = single_sampling_plan(p0, p1, alpha, beta);
    n = nc.first;
    c = nc.second;
  } else { /* algorithm == SPRT */
    logA = -log(alpha);
    /* If p1 is 0, then a beta of 0 can always be guaranteed. */
    if (p1 > 0.0) {
      logA += log(1.0 - beta);
    }
    logB = log(beta);
    /* If p0 is 1, then an alpha of 0 can always be guaranteed. */
    if (p0 < 1.0) {
      logB -= log(1.0 - alpha);
    }
  }
  if (formula_level() == 0) {
    if (verbosity > 0) {
      std::cout << "Acceptance sampling";
      if (algorithm == SEQUENTIAL) {
	std::cout << " <" << n << ',' << c << ">";
      }
    }
    if (verbosity > 1) {
      std::cout << std::endl;
    }
  }
  int m = 0;
  double d = 0.0;
  if (memoization) {
    std::map<std::vector<int>, std::pair<size_t, double> >::const_iterator ci =
      cache_.find(state.values());
    if (ci != cache_.end()) {
      m = (*ci).second.first;
      d = (*ci).second.second;
    }
  }
  formula_level_++;
  std::queue<short> schedule;
  std::map<short, std::queue<bool> > buffer;
  std::map<short, size_t> sample_count;
  std::map<short, size_t> usage_count;
  std::set<short> dead_clients;
  fd_set master_fds;
  int fdmax;
  if (server_socket != -1) {
    FD_ZERO(&master_fds);
    FD_SET(server_socket, &master_fds);
    fdmax = server_socket;
    std::set<int> closed_sockets;
    for (std::map<int, short>::const_iterator ci = registered_clients.begin();
	 ci != registered_clients.end(); ci++) {
      int sockfd = (*ci).first;
      short client_id = (*ci).second;
      ServerMsg smsg = { ServerMsg::START, current_property };
      int nbytes = send(sockfd, &smsg, sizeof smsg, 0);
      if (nbytes == -1) {
	perror(PACKAGE);
	return -1;
      } else if (nbytes == 0) {
	closed_sockets.insert(sockfd);
	close(sockfd);
      } else {
	schedule.push(client_id);
	FD_SET(sockfd, &master_fds);
	if (sockfd > fdmax) {
	  fdmax = sockfd;
	}
      }
    }
    for (std::set<int>::const_iterator ci = closed_sockets.begin();
	 ci != closed_sockets.end(); ci++) {
      registered_clients.erase(*ci);
    }
  }
  while ((algorithm == SEQUENTIAL && d <= c && d + n - m > c)
	 || (algorithm == SPRT && logB < d && d < logA)) {
    bool s, have_sample = false;
    if (!schedule.empty() && !buffer[schedule.front()].empty()) {
      short client_id = schedule.front();
      s = buffer[client_id].front();
      buffer[client_id].pop();
      schedule.push(client_id);
      schedule.pop();
      if (verbosity > 1) {
	std::cout << "Using sample (" << s << ") from client "
		  << client_id << std::endl;
      }
      usage_count[client_id]++;
      have_sample = true;
    } else if (server_socket != -1) {
      /* Server mode. */
      while (!schedule.empty()
	     && dead_clients.find(schedule.front()) != dead_clients.end()
	     && buffer[schedule.front()].empty()) {
	/* Do not expect messages from dead clients. */
	schedule.pop();
      }
      fd_set read_fds = master_fds;
      if (-1 == select(fdmax + 1, &read_fds, NULL, NULL, NULL)) {
	perror(PACKAGE);
	exit(1);
      }
      if (FD_ISSET(server_socket, &read_fds)) {
	/* register a client */
	sockaddr_in client_addr;
	int addrlen = sizeof client_addr;
	int sockfd = accept(server_socket, (sockaddr*) &client_addr,
			    (socklen_t*) &addrlen);
	if (sockfd == -1) {
	  perror(PACKAGE);
	}
	FD_SET(sockfd, &master_fds);
	if (sockfd > fdmax) {
	  fdmax = sockfd;
	}
	int client_id = next_client_id++;
	ServerMsg smsg = { ServerMsg::REGISTER, client_id };
	if (-1 == send(sockfd, &smsg, sizeof smsg, 0)) {
	  perror(PACKAGE);
	  close(sockfd);
	} else {
	  smsg.id = ServerMsg::START;
	  smsg.value = current_property;
	  if (-1 == send(sockfd, &smsg, sizeof smsg, 0)) {
	    perror(PACKAGE);
	    close(sockfd);
	  } else {
	    registered_clients[sockfd] = client_id;
	    schedule.push(client_id);
	    unsigned long addr = ntohl(client_addr.sin_addr.s_addr);
	    std::cout << "Registering client " << client_id << " @ "
		      << (0xff & (addr >> 24UL)) << '.'
		      << (0xff & (addr >> 16UL)) << '.'
		      << (0xff & (addr >> 8UL)) << '.' << (0xff & addr)
		      << std::endl;
	  }
	}
      }
      std::set<int> closed_sockets;
      for (std::map<int, short>::const_iterator ci =
	     registered_clients.begin();
	   ci != registered_clients.end(); ci++) {
	int sockfd = (*ci).first;
	short client_id = (*ci).second;
	if (FD_ISSET(sockfd, &read_fds)) {
	  /* receive a sample */
	  ClientMsg msg;
	  int nbytes = recv(sockfd, &msg, sizeof msg, 0);
	  if (nbytes <= 0) {
	    if (nbytes == -1) {
	      perror(PACKAGE);
	    } else {
	      std::cout << "Client " << client_id << " disconnected"
			<< std::endl;
	    }
	    closed_sockets.insert(sockfd);
	    dead_clients.insert(client_id);
	    close(sockfd);
	    FD_CLR(sockfd, &master_fds);
	  } else if (msg.id == ClientMsg::SAMPLE) {
	    s = msg.value;
	    if (verbosity > 1) {
	      std::cout << "Receiving sample (" << s << ") from client "
			<< client_id << std::endl;
	    }
	    sample_count[client_id]++;
	    schedule.push(client_id);
	    if (schedule.front() == client_id) {
	      schedule.pop();
	      if (verbosity > 1) {
		std::cout << "Using sample (" << s << ") from client "
			  << client_id << std::endl;
	      }
	      usage_count[client_id]++;
	      have_sample = true;
	    } else {
	      buffer[client_id].push(s);
	    }
	  } else {
	    std::cerr << "Message with bad id (" << msg.id << ") ignored."
		      << std::endl;
	  }
	}
      }
      for (std::set<int>::const_iterator ci = closed_sockets.begin();
	   ci != closed_sockets.end(); ci++) {
	registered_clients.erase(*ci);
      }
    } else {
      /* Local mode. */
      const State& ns = state.resampled();
      s = formula().sample(model, ns, delta, alphap, betap, algorithm);
      if (&ns != &state) {
	delete &ns;
      }
      have_sample = true;
    }
    if (!have_sample) {
      continue;
    }
    if (s) {
      if (algorithm == SEQUENTIAL) {
	d += 1.0;
      } else { /* algorithm == SPRT */
	if (p1 > 0.0) {
	  d += log(p1) - log(p0);
	} else {
	  d = -HUGE_VAL;
	}
      }
    } else {
      if (algorithm == SEQUENTIAL) {
	/* do nothing */
      } else { /* algorithm == SPRT */
	if (p0 < 1.0) {
	  d += log(1.0 - p1) - log(1.0 - p0);
	} else {
	  d = HUGE_VAL;
	}
      }
    }
    m++;
    if (verbosity == 1) {
      if (formula_level() == 1) {
	if (m % 1000 == 0) {
	  std::cout << ':';
	} else if (m % 100 == 0) {
	  std::cout << '.';
	}
      }
    } else if (verbosity > 1) {
      for (size_t i = 0; i < 2*(formula_level() - 1); i++) {
	std::cout << ' ';
      }
      if (algorithm == SEQUENTIAL) {
	std::cout << m << '\t' << d << '\t' << (c + m - n) << '\t' << c
		  << std::endl;
      } else { /* algorithm == SPRT */
	std::cout << m << '\t' << d << '\t' << logB << '\t' << logA
		  << std::endl;
      }
    }
  }
  formula_level_--;
  if (formula_level() == 0) {
    if (verbosity > 0) {
      std::cout << m << " samples." << std::endl;
    }
    total_samples += m;
    samples.push_back(m);
  }
  if (server_socket != -1) {
    if (verbosity > 0) {
      for (std::map<short, size_t>::const_iterator si = sample_count.begin();
	   si != sample_count.end(); si++) {
	std::cout << "Client " << (*si).first << ": "
		  << (*si).second << " generated "
		  << usage_count[(*si).first] << " used" << std::endl;
      }
    }
    for (std::map<int, short>::const_iterator ci = registered_clients.begin();
	 ci != registered_clients.end(); ci++) {
      int sockfd = (*ci).first;
      ServerMsg smsg = { ServerMsg::STOP };
      if (-1 == send(sockfd, &smsg, sizeof smsg, 0)) {
	perror(PACKAGE);
	exit(1);
      }
    }
  }
  if (memoization) {
    cache_[state.values()] = std::make_pair(m, d);
  }
  if (algorithm == SEQUENTIAL) {
    return d > c;
  } else { /* algorithm == SPRT */
    return d <= logB;
  }
}


/* Clears the cache of any probabilistic operator. */
size_t Probabilistic::clear_cache() const {
  size_t n = cache_.size() + formula().clear_cache();
  cache_.clear();
  return n;
}


/* ====================================================================== */
/* Comparison */

/* Estimated effort for verifying this state formula using the
   statistical engine. */
double Comparison::effort(double q, DeltaFun delta, double alpha, double beta,
			  double alphap, double betap,
			  SamplingAlgorithm algorithm) const {
  return 1.0;
}


/* Verifies this state formula using the statistical engine. */
bool Comparison::verify(const Model& model, const State& state,
			DeltaFun delta, double alpha, double beta,
			SamplingAlgorithm algorithm) const {
  return holds(state.values());
}


/* Clears the cache of any probabilistic operator. */
size_t Comparison::clear_cache() const {
  return 0;
}


/* ====================================================================== */
/* Until */

/* Estimated effort for generating a sample for this path formula. */
double Until::effort(double q, DeltaFun delta, double alpha, double beta,
		     double alphap, double betap,
		     SamplingAlgorithm algorithm) const {
  double a = max_time().value<double>();
  double b = (max_time() - min_time()).value<double>();
  return q*(a*pre().effort(q, delta, alpha, beta, alphap, betap, algorithm)
	    + b*post().effort(q, delta, alpha, beta, alphap, betap, algorithm));
}


/* Generates a sample for this path formula. */
bool Until::sample(const Model& model, const State& state,
		   DeltaFun delta, double alpha, double beta,
		   SamplingAlgorithm algorithm) const {
  double t = 0.0;
  const State* curr_state = &state;
  double t_min = min_time().value<double>();
  double t_max = max_time().value<double>();
  size_t path_length = 1;
  bool result = false, done = false, output = false;
  while (!done && path_length < max_path_length) {
    if (verbosity > 2 && StateFormula::formula_level() == 1) {
      std::cout << "t = " << t << ": ";
      curr_state->print(std::cout);
      std::cout << std::endl;
    }
    const State& next_state = curr_state->next();
    double next_t = t + next_state.dt();
    if (t_min <= t) {
      if (post().verify(model, *curr_state, delta, alpha, beta, algorithm)) {
	result = true;
	done = true;
      } else if (!pre().verify(model, *curr_state,
			       delta, alpha, beta, algorithm)) {
	result = false;
	done = true;
      }
    } else {
      if (!pre().verify(model, *curr_state, delta, alpha, beta, algorithm)) {
	result = false;
	done = true;
      } else if (t_min < next_t
		 && post().verify(model, *curr_state,
				  delta, alpha, beta, algorithm)) {
	t = t_min;
	result = true;
	done = true;
	output = true;
      }
    }
    if (done) {
      delete &next_state;
    } else {
      if (curr_state != &state) {
	delete curr_state;
      }
      curr_state = &next_state;
      t = next_t;
      if (t_max < t) {
	result = false;
	done = true;
	output = true;
      }
      path_length++;
    }
  }
  if (verbosity > 2) {
    if (output) {
      std::cout << "t = " << t << ": ";
      curr_state->print(std::cout);
      std::cout << std::endl;
    }
    if (result) {
      std::cout << ">>positive sample" << std::endl;
    } else {
      std::cout << ">>negative sample" << std::endl;
    }
  }
  if (curr_state != &state) {
    delete curr_state;
  }
  if (StateFormula::formula_level() == 1) {
    total_path_lengths += path_length;
  }
  return result;
}


/* Verifies this path formula using the mixed engine. */
bool Until::verify(const DecisionDiagramManager& dd_man, const Model& model,
		   const State& state, const TypedValue& p, bool strict,
		   DeltaFun delta, double alpha, double beta,
		   SamplingAlgorithm algorithm,
		   double epsilon) const {
  double theta = p.value<double>();
  double p0 = std::min(1.0, theta + (*delta)(theta));
  double p1 = std::max(0.0, theta - (*delta)(theta));

  DdNode* dd1 = (pre().probabilistic()
		 ? pre().verify(dd_man, model, epsilon, false) : NULL);
  if (dd1 != NULL) {
    DdNode* ddr = model.reachability_bdd(dd_man);
    if (dd1 == ddr) {
      Cudd_RecursiveDeref(dd_man.manager(), dd1);
      dd1 = Cudd_ReadOne(dd_man.manager());
      Cudd_Ref(dd1);
    }
    Cudd_RecursiveDeref(dd_man.manager(), ddr);
  }
  DdNode* dd2 = (post().probabilistic()
		 ? post().verify(dd_man, model, epsilon, false) : NULL);

  if (algorithm == ESTIMATE) {
    int c = 0, n = 0;
    double es = (*delta)(theta)*(*delta)(theta);
    double a = 1.0 - 0.5*alpha;
    double p, t, b;
    if (verbosity > 0) {
      std::cout << "Sequential estimation";
    }
    if (verbosity > 1) {
      std::cout << std::endl;
    }
    while (c == 0 || n < 2 || (t + 1.0)/c/c > es/b/b) {
      if (sample(dd_man, model, state, epsilon, dd1, dd2)) {
	c++;
      }
      n++;
      p = double(c)/n;
      if (verbosity == 1) {
	if (n % 1000 == 0) {
	  std::cout << ':';
	} else if (n % 100 == 0) {
	  std::cout << '.';
	}
      } else if (verbosity > 1) {
	std::cout << n << '\t' << c << '\t' << p/(1 + (*delta)(theta)) << '\t'
		  << p/(1 - (*delta)(theta)) << std::endl;
      }
      t = c*(1.0 - p);
      b = tinv(a, n - 1.0);
    }
    if (verbosity > 0) {
      std::cout << n << " samples." << std::endl;
    }
    std::cout << "Pr[" << *this << "] = " << p << " ("
	      << p/(1 + (*delta)(theta)) << ',' << p/(1 - (*delta)(theta))
	      << ")" << std::endl;
    if (dd1 != NULL) {
      Cudd_RecursiveDeref(dd_man.manager(), dd1);
    }
    if (dd2 != NULL) {
      Cudd_RecursiveDeref(dd_man.manager(), dd2);
    }
    if (strict) {
      return p > theta;
    } else {
      return p >= theta;
    }
  }

  int n, c;
  double logA, logB;
  if (algorithm == SEQUENTIAL) {
    std::pair<int, int> nc = single_sampling_plan(p0, p1, alpha, beta);
    n = nc.first;
    c = nc.second;
  } else { /* algorithm == SPRT */
    logA = -log(alpha);
    /* If p1 is 0, then a beta of 0 can always be guaranteed. */
    if (p1 > 0.0) {
      logA += log(1.0 - beta);
    }
    logB = log(beta);
    /* If p0 is 1, then an alpha of 0 can always be guaranteed. */
    if (p0 < 1.0) {
      logB -= log(1.0 - alpha);
    }
  }
  if (verbosity > 0) {
    std::cout << "Acceptance sampling";
    if (algorithm == SEQUENTIAL) {
      std::cout << " <" << n << ',' << c << ">";
    }
  }
  if (verbosity > 1) {
    std::cout << std::endl;
  }
  int m = 0;
  double d = 0.0;
  while ((algorithm == SEQUENTIAL && d <= c && d + n - m > c)
	 || (algorithm == SPRT && logB < d && d < logA)) {
    const State& ns = state.resampled();
    bool s = sample(dd_man, model, ns, epsilon, dd1, dd2);
    if (&ns != &state) {
      delete &ns;
    }
    if (s) {
      if (algorithm == SEQUENTIAL) {
	d += 1.0;
      } else { /* algorithm == SPRT */
	if (p1 > 0.0) {
	  d += log(p1) - log(p0);
	} else {
	  d = -HUGE_VAL;
	}
      }
    } else {
      if (algorithm == SEQUENTIAL) {
	/* do nothing */
      } else { /* algorithm == SPRT */
	if (p0 < 1.0) {
	  d += log(1.0 - p1) - log(1.0 - p0);
	} else {
	  d = HUGE_VAL;
	}
      }
    }
    m++;
    if (verbosity == 1) {
      if (m % 1000 == 0) {
	std::cout << ':';
      } else if (m % 100 == 0) {
	std::cout << '.';
      }
    } else if (verbosity > 1) {
      if (algorithm == SEQUENTIAL) {
	std::cout << m << '\t' << d << '\t' << (c + m - n) << '\t' << c
		  << std::endl;
      } else { /* algorithm == SPRT */
	std::cout << m << '\t' << d << '\t' << logB << '\t' << logA
		  << std::endl;
      }
    }
  }
  if (verbosity > 0) {
    std::cout << m << " samples." << std::endl;
  }
  total_samples += m;
  samples.push_back(m);
  if (dd1 != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), dd1);
  }
  if (dd2 != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), dd2);
  }
  if (algorithm == SEQUENTIAL) {
    return d > c;
  } else { /* algorithm == SPRT */
    return d <= logB;
  }
}


/* Clears the cache of any probabilistic operator. */
size_t Until::clear_cache() const {
  return pre().clear_cache() + post().clear_cache();
}
