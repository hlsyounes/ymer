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

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <limits>
#include <queue>

#include "comm.h"
#include "distributions.h"
#include "models.h"
#include "states.h"
#include "src/compiled-property.h"
#include "src/statistics.h"

#include "cudd.h"
#include "glog/logging.h"
#include "gsl/gsl_cdf.h"

/* Nesting level of formula just being verified. */
size_t StateFormula::formula_level_ = 0;

/* Registered clients. */
static std::map<int, short> registered_clients;
/* Next client id. */
static short next_client_id = 1;

namespace {

void PrintProgress(int n) {
  if (n % 1000 == 0) {
    std::cout << ':';
  } else if (n % 100 == 0) {
    std::cout << '.';
  }
}

}  // namespace

class CompiledPropertySamplingVerifier : public CompiledPropertyVisitor {
 private:
  virtual void DoVisitCompiledLogicalOperationProperty(
      const CompiledLogicalOperationProperty& property);
  virtual void DoVisitCompiledProbabilisticProperty(
      const CompiledProbabilisticProperty& property);
  virtual void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property);

  bool result_;
  CompiledExpressionEvaluator* evaluator_;
  const std::vector<int>* state_;
};

/* ====================================================================== */
/* Conjunction */

void CompiledPropertySamplingVerifier::DoVisitCompiledLogicalOperationProperty(
    const CompiledLogicalOperationProperty& property) {
  bool short_circuit_result =
      (property.op() == CompiledLogicalOperationProperty::Operator::OR);
  // TODO(hlsyounes): for AND use alpha/n; for OR use beta/n.
  for (const CompiledProperty& operand : property.operands()) {
    operand.Accept(this);
    if (result_ == short_circuit_result) {
      return;
    }
  }
}

/* Verifies this state formula using the statistical engine. */
bool Conjunction::verify(const Model& model, const State& state,
			 double alpha, double beta,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  const double alpha_n = alpha / conjuncts().size();
  for (auto fi = conjuncts().rbegin(); fi != conjuncts().rend(); fi++) {
    if (!(*fi)->verify(model, state, alpha_n, beta, params, stats)) {
      return false;
    }
  }
  return true;
}


/* Clears the cache of any probabilistic operator. */
size_t Conjunction::clear_cache() const {
  size_t n = 0;
  for (const StateFormula* conjunct : conjuncts()) {
    n += conjunct->clear_cache();
  }
  return n;
}


/* ====================================================================== */
/* Disjunction */

/* Verifies this state formula using the statistical engine. */
bool Disjunction::verify(const Model& model, const State& state,
			 double alpha, double beta,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  const double beta_n = beta / disjuncts().size();
  for (auto fi = disjuncts().rbegin(); fi != disjuncts().rend(); fi++) {
    if ((*fi)->verify(model, state, alpha, beta_n, params, stats)) {
      return true;
    }
  }
  return false;
}


/* Clears the cache of any probabilistic operator. */
size_t Disjunction::clear_cache() const {
  size_t n = 0;
  for (const StateFormula* disjunct : disjuncts()) {
    n += disjunct->clear_cache();
  }
  return n;
}


/* ====================================================================== */
/* Negation */

/* Verifies this state formula using the statistical engine. */
bool Negation::verify(const Model& model, const State& state,
		      double alpha, double beta,
                      const ModelCheckingParams& params,
                      ModelCheckingStats* stats) const {
  return !negand().verify(model, state, beta, alpha, params, stats);
}


/* Clears the cache of any probabilistic operator. */
size_t Negation::clear_cache() const {
  return negand().clear_cache();
}


/* ====================================================================== */
/* Implication */

/* Verifies this state formula using the statistical engine. */
bool Implication::verify(const Model& model, const State& state,
			 double alpha, double beta,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  const double beta_n = beta / 2;
  if (!antecedent().verify(model, state, beta_n, alpha, params, stats)) {
    return true;
  } else {
    return consequent().verify(model, state, alpha, beta_n, params, stats);
  }
}


/* Clears the cache of any probabilistic operator. */
size_t Implication::clear_cache() const {
  return antecedent().clear_cache() + consequent().clear_cache();
}


/* ====================================================================== */
/* Probabilistic */

void CompiledPropertySamplingVerifier::DoVisitCompiledProbabilisticProperty(
    const CompiledProbabilisticProperty& property) {
  // TODO(hlsyounes): implement.
}

/* Verifies this state formula using the statistical engine. */
bool Probabilistic::verify(const Model& model, const State& state,
			   double alpha, double beta,
                           const ModelCheckingParams& params,
                           ModelCheckingStats* stats) const {
  double p0, p1;
  double theta = threshold().value<double>();
  double nested_error = 0.0;
  if (formula().probabilistic()) {
    if (params.nested_error > 0) {
      // User-specified nested error.
      nested_error = params.nested_error;
    } else {
      // Simple heuristic for nested error.
      nested_error = 0.8 * MaxNestedError(params.delta);
    }
    if (formula_level() == 0) {
      VLOG(1) << "Nested error: " << nested_error;
      VLOG(1) << "Maximum symmetric nested error: "
              << MaxNestedError(params.delta);
    }
  }
  p0 = std::min(1.0, (theta + params.delta)*(1.0 - nested_error));
  p1 = std::max(0.0, 1.0 - (1.0 - (theta - params.delta))*(1.0 - nested_error));
  if (params.algorithm == FIXED) {
    int c = 0;
    if (formula_level() == 0) {
      std::cout << "Fixed-size sampling";
    }
    formula_level_++;
    for (int i = 1; i <= params.fixed_sample_size; ++i) {
      if (formula().sample(model, state, nested_error, nested_error, params,
                           stats)) {
        c++;
      }
      if (formula_level() == 1) {
        PrintProgress(i);
      }
      if (VLOG_IS_ON(2)) {
        LOG(INFO) << std::string(' ', 2*(formula_level() - 1))
                  << i << '\t' << c;
      }
    }
    formula_level_--;
    if (formula_level() == 0) {
      std::cout << params.fixed_sample_size << " observations." << std::endl;
      stats->sample_size.AddObservation(params.fixed_sample_size);
    }
    double p = double(c)/params.fixed_sample_size;
    if (strict()) {
      return p > theta;
    } else {
      return p >= theta;
    }
  }
  if (params.algorithm == ESTIMATE) {
    if (formula_level() == 0) {
      std::cout << "Sequential estimation";
    }
    formula_level_++;
    SequentialEstimator<int> estimator(params.delta, alpha);
    while (true) {
      const bool x = formula().sample(model, state, nested_error, nested_error,
                                      params, stats);
      estimator.AddObservation(x ? 1 : 0);
      if (formula_level() == 1) {
        PrintProgress(estimator.count());
      }
      if (VLOG_IS_ON(2)) {
        LOG(INFO) << std::string(' ', 2*(formula_level() - 1))
                  << estimator.count() << '\t' << estimator.value() << '\t'
                  << estimator.state() << '\t' << estimator.bound();
      }
      if (estimator.state() <= estimator.bound()) {
        break;
      }
    }
    formula_level_--;
    if (formula_level() == 0) {
      std::cout << estimator.count() << " observations." << std::endl;
      std::cout << "Pr[" << formula() << "] = " << estimator.value() << " ("
                << std::max(0.0, estimator.value() - params.delta) << ','
                << std::min(1.0, estimator.value() + params.delta) << ")"
                << std::endl;
      stats->sample_size.AddObservation(estimator.count());
    }
    if (strict()) {
      return estimator.value() > theta;
    } else {
      return estimator.value() >= theta;
    }
  }

  int n = 0, c = 0;
  double logA, logB = 0;
  if (params.algorithm == SSP) {
    const auto ssp = SingleSamplingPlan::Create(p0, p1, alpha, beta);
    n = ssp.n();
    c = ssp.c();
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
    std::cout << "Acceptance sampling";
    if (params.algorithm == SSP) {
      std::cout << " <" << n << ',' << c << ">";
    }
  }
  int m = 0;
  double d = 0.0;
  if (params.memoization) {
    auto ci = cache_.find(state.values());
    if (ci != cache_.end()) {
      m = ci->second.first;
      d = ci->second.second;
    }
  }
  formula_level_++;
  std::queue<short> schedule;
  std::map<short, std::queue<bool> > buffer;
  std::map<short, size_t> sample_count;
  std::map<short, size_t> usage_count;
  std::set<short> dead_clients;
  fd_set master_fds;
  int fdmax = -1;
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
  while ((params.algorithm == SSP && d <= c && d + n - m > c)
	 || (params.algorithm == SPRT && logB < d && d < logA)) {
    bool s = false, have_sample = false;
    if (!schedule.empty() && !buffer[schedule.front()].empty()) {
      short client_id = schedule.front();
      s = buffer[client_id].front();
      buffer[client_id].pop();
      schedule.push(client_id);
      schedule.pop();
      if (VLOG_IS_ON(2)) {
	LOG(INFO) << "Using sample (" << s << ") from client " << client_id;
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
	    if (VLOG_IS_ON(2)) {
	      LOG(INFO) << "Receiving sample (" << s << ") from client "
			<< client_id;
	    }
	    sample_count[client_id]++;
	    schedule.push(client_id);
	    if (schedule.front() == client_id) {
	      schedule.pop();
	      if (VLOG_IS_ON(2)) {
		LOG(INFO) << "Using sample (" << s << ") from client "
			  << client_id;
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
      s = formula().sample(model, state, nested_error, nested_error, params,
                           stats);
      have_sample = true;
    }
    if (!have_sample) {
      continue;
    }
    if (s) {
      if (params.algorithm == SSP) {
	d += 1.0;
      } else { /* algorithm == SPRT */
	if (p1 > 0.0) {
	  d += log(p1) - log(p0);
	} else {
	  d = -std::numeric_limits<double>::infinity();
	}
      }
    } else {
      if (params.algorithm == SSP) {
	/* do nothing */
      } else { /* algorithm == SPRT */
	if (p0 < 1.0) {
	  d += log(1.0 - p1) - log(1.0 - p0);
	} else {
	  d = std::numeric_limits<double>::infinity();
	}
      }
    }
    m++;
    if (formula_level() == 1) {
      PrintProgress(m);
    }
    if (VLOG_IS_ON(2)) {
      if (params.algorithm == SSP) {
	LOG(INFO) << std::string(' ', 2*(formula_level() - 1))
                  << m << '\t' << d << '\t' << (c + m - n) << '\t' << c;
      } else { /* algorithm == SPRT */
	LOG(INFO) << std::string(' ', 2*(formula_level() - 1))
                  << m << '\t' << d << '\t' << logB << '\t' << logA;
      }
    }
  }
  formula_level_--;
  if (formula_level() == 0) {
    std::cout << m << " observations." << std::endl;
    stats->sample_size.AddObservation(m);
  }
  if (server_socket != -1) {
    if (VLOG_IS_ON(1)) {
      for (std::map<short, size_t>::const_iterator si = sample_count.begin();
	   si != sample_count.end(); si++) {
	LOG(INFO) << "Client " << (*si).first << ": "
		  << (*si).second << " generated "
		  << usage_count[(*si).first] << " used";
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
  if (params.memoization) {
    cache_[state.values()] = { m, d };
  }
  if (params.algorithm == SSP) {
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

void CompiledPropertySamplingVerifier::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  result_ = evaluator_->EvaluateIntExpression(property.expr(), *state_);
}

/* Verifies this state formula using the statistical engine. */
bool Comparison::verify(const Model& model, const State& state,
			double alpha, double beta,
                        const ModelCheckingParams& params,
                        ModelCheckingStats* stats) const {
  return holds(state.values());
}


/* Clears the cache of any probabilistic operator. */
size_t Comparison::clear_cache() const {
  return 0;
}


/* ====================================================================== */
/* Until */

/* Generates a sample for this path formula. */
bool Until::sample(const Model& model, const State& state,
		   double alpha, double beta,
                   const ModelCheckingParams& params,
                   ModelCheckingStats* stats) const {
  double t = 0.0;
  State curr_state = state;
  const double t_min = min_time().value<double>();
  const double t_max = max_time().value<double>();
  size_t path_length = 1;
  bool result = false, done = false, output = false;
  while (!done && path_length < params.max_path_length) {
    if (VLOG_IS_ON(3) && StateFormula::formula_level() == 1) {
      LOG(INFO) << "t = " << t << ": " << curr_state.ToString();
    }
    State next_state = curr_state.Next();
    double next_t = t + (next_state.time() - curr_state.time());
    if (t_min <= t) {
      if (post().verify(model, curr_state, alpha, beta, params, stats)) {
	result = true;
	done = true;
      } else if (!pre().verify(model, curr_state, alpha, beta, params, stats)) {
	result = false;
	done = true;
      }
    } else {
      if (!pre().verify(model, curr_state, alpha, beta, params, stats)) {
	result = false;
	done = true;
      } else if (t_min < next_t
		 && post().verify(model, curr_state, alpha, beta, params,
                                  stats)) {
	t = t_min;
	result = true;
	done = true;
	output = true;
      }
    }
    if (!done) {
      curr_state = std::move(next_state);
      t = next_t;
      if (t_max < t) {
	result = false;
	done = true;
	output = true;
      }
      path_length++;
    }
  }
  if (VLOG_IS_ON(3)) {
    if (output) {
      LOG(INFO) << "t = " << t << ": " << curr_state.ToString();
    }
    if (result) {
      LOG(INFO) << ">>positive sample";
    } else {
      LOG(INFO) << ">>negative sample";
    }
  }
  if (StateFormula::formula_level() == 1) {
    stats->path_length.AddObservation(path_length);
  }
  return result;
}


/* Verifies this path formula using the mixed engine. */
bool Until::verify(const DecisionDiagramManager& dd_man, const Model& model,
		   const State& state, const TypedValue& p, bool strict,
		   double alpha, double beta,
                   const ModelCheckingParams& params,
                   ModelCheckingStats* stats) const {
  double theta = p.value<double>();
  double p0 = std::min(1.0, theta + params.delta);
  double p1 = std::max(0.0, theta - params.delta);

  DdNode* dd1 = (pre().probabilistic()
		 ? pre().verify(dd_man, model, false, params) : NULL);
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
		 ? post().verify(dd_man, model, false, params) : NULL);

  if (params.algorithm == ESTIMATE) {
    std::cout << "Sequential estimation";
    SequentialEstimator<int> estimator(params.delta, alpha);
    while (true) {
      const bool x = sample(dd_man, model, state, dd1, dd2, stats);
      estimator.AddObservation(x ? 1 : 0);
      PrintProgress(estimator.count());
      if (VLOG_IS_ON(2)) {
	LOG(INFO) << estimator.count() << '\t' << estimator.value() << '\t'
                  << estimator.state() << '\t' << estimator.bound();
      }
      if (estimator.state() <= estimator.bound()) {
        break;
      }
    }
    std::cout << estimator.count() << " observations." << std::endl;
    std::cout << "Pr[" << *this << "] = " << estimator.value() << " ("
	      << std::max(0.0, estimator.value() - params.delta) << ','
              << std::min(1.0, estimator.value() + params.delta) << ")"
              << std::endl;
    stats->sample_size.AddObservation(estimator.count());
    if (dd1 != NULL) {
      Cudd_RecursiveDeref(dd_man.manager(), dd1);
    }
    if (dd2 != NULL) {
      Cudd_RecursiveDeref(dd_man.manager(), dd2);
    }
    if (strict) {
      return estimator.value() > theta;
    } else {
      return estimator.value() >= theta;
    }
  }

  int n = 0, c = 0;
  double logA, logB = 0;
  if (params.algorithm == SSP) {
    const auto ssp = SingleSamplingPlan::Create(p0, p1, alpha, beta);
    n = ssp.n();
    c = ssp.c();
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
  std::cout << "Acceptance sampling";
  if (params.algorithm == SSP) {
    std::cout << " <" << n << ',' << c << ">";
  }
  int m = 0;
  double d = 0.0;
  while ((params.algorithm == SSP && d <= c && d + n - m > c)
	 || (params.algorithm == SPRT && logB < d && d < logA)) {
    bool s = sample(dd_man, model, state, dd1, dd2, stats);
    if (s) {
      if (params.algorithm == SSP) {
	d += 1.0;
      } else { /* algorithm == SPRT */
	if (p1 > 0.0) {
	  d += log(p1) - log(p0);
	} else {
	  d = -std::numeric_limits<double>::infinity();
	}
      }
    } else {
      if (params.algorithm == SSP) {
	/* do nothing */
      } else { /* algorithm == SPRT */
	if (p0 < 1.0) {
	  d += log(1.0 - p1) - log(1.0 - p0);
	} else {
	  d = std::numeric_limits<double>::infinity();
	}
      }
    }
    m++;
    PrintProgress(m);
    if (VLOG_IS_ON(2)) {
      if (params.algorithm == SSP) {
	LOG(INFO) << m << '\t' << d << '\t' << (c + m - n) << '\t' << c;
      } else { /* algorithm == SPRT */
	LOG(INFO) << m << '\t' << d << '\t' << logB << '\t' << logA;
      }
    }
  }
  std::cout << m << " observations." << std::endl;
  stats->sample_size.AddObservation(m);
  if (dd1 != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), dd1);
  }
  if (dd2 != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), dd2);
  }
  if (params.algorithm == SSP) {
    return d > c;
  } else { /* algorithm == SPRT */
    return d <= logB;
  }
}


/* Clears the cache of any probabilistic operator. */
size_t Until::clear_cache() const {
  return pre().clear_cache() + post().clear_cache();
}
