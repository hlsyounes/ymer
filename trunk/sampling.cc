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
#include <utility>

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
  virtual void DoVisitCompiledAndProperty(const CompiledAndProperty& property);
  virtual void DoVisitCompiledNotProperty(const CompiledNotProperty& property);
  virtual void DoVisitCompiledProbabilisticProperty(
      const CompiledProbabilisticProperty& property);
  virtual void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property);

  bool result_;
  ModelCheckingParams params_;
  CompiledExpressionEvaluator* evaluator_;
  const std::vector<int>* state_;
};

/* ====================================================================== */
/* Conjunction */

void CompiledPropertySamplingVerifier::DoVisitCompiledAndProperty(
    const CompiledAndProperty& property) {
  double alpha = params_.alpha / property.operands().size();
  std::swap(params_.alpha, alpha);
  for (const CompiledProperty& operand : property.operands()) {
    operand.Accept(this);
    if (result_ == false) {
      break;
    }
  }
  std::swap(params_.alpha, alpha);
}

/* Verifies this state formula using the statistical engine. */
bool Conjunction::verify(const DecisionDiagramManager* manager,
                         const DecisionDiagramModel* dd_model,
                         const Model& model, const State& state,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  ModelCheckingParams nested_params = params;
  nested_params.alpha = params.alpha / conjuncts().size();
  for (auto fi = conjuncts().rbegin(); fi != conjuncts().rend(); fi++) {
    if (!(*fi)->verify(manager, dd_model, model, state, nested_params, stats)) {
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
bool Disjunction::verify(const DecisionDiagramManager* manager,
                         const DecisionDiagramModel* dd_model,
                         const Model& model, const State& state,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  ModelCheckingParams nested_params = params;
  nested_params.beta = params.beta / disjuncts().size();
  for (auto fi = disjuncts().rbegin(); fi != disjuncts().rend(); fi++) {
    if ((*fi)->verify(manager, dd_model, model, state, nested_params, stats)) {
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

void CompiledPropertySamplingVerifier::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  std::swap(params_.alpha, params_.beta);
  property.operand().Accept(this);
  result_ = !result_;
  std::swap(params_.alpha, params_.beta);
}

/* Verifies this state formula using the statistical engine. */
bool Negation::verify(const DecisionDiagramManager* manager,
                      const DecisionDiagramModel* dd_model,
                      const Model& model, const State& state,
                      const ModelCheckingParams& params,
                      ModelCheckingStats* stats) const {
  ModelCheckingParams nested_params = params;
  std::swap(nested_params.alpha, nested_params.beta);
  return !negand().verify(manager, dd_model, model, state, nested_params,
                          stats);
}


/* Clears the cache of any probabilistic operator. */
size_t Negation::clear_cache() const {
  return negand().clear_cache();
}


/* ====================================================================== */
/* Implication */

/* Verifies this state formula using the statistical engine. */
bool Implication::verify(const DecisionDiagramManager* manager,
                         const DecisionDiagramModel* dd_model,
                         const Model& model, const State& state,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  ModelCheckingParams nested_params = params;
  nested_params.beta = params.beta / 2;
  std::swap(nested_params.alpha, nested_params.beta);
  if (!antecedent().verify(manager, dd_model, model, state, nested_params,
                           stats)) {
    return true;
  } else {
    std::swap(nested_params.alpha, nested_params.beta);
    return consequent().verify(manager, dd_model, model, state, nested_params,
                               stats);
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
bool Probabilistic::verify(const DecisionDiagramManager* manager,
                           const DecisionDiagramModel* dd_model,
                           const Model& model, const State& state,
                           const ModelCheckingParams& params,
                           ModelCheckingStats* stats) const {
  if (manager != nullptr) {
    // Mixed engine.
    formula_level_++;
    bool res = formula().verify(*manager, model, *dd_model, state, threshold(),
                                strict(), params, stats);
    formula_level_--;
    return res;
  }
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
  ModelCheckingParams nested_params = params;
  nested_params.alpha = nested_error;
  nested_params.beta = nested_error;
  if (params.algorithm == ESTIMATE) {
    if (formula_level() == 0) {
      std::cout << "Sequential estimation";
    }
    formula_level_++;
    SequentialEstimator<int> estimator(params.delta, params.alpha);
    while (true) {
      const bool x =
          formula().sample(manager, dd_model, model, state, nullptr, nullptr,
                           nested_params, stats);
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

  std::unique_ptr<BernoulliTester> tester;
  if (params.algorithm == FIXED) {
    tester.reset(
        new FixedBernoulliTester(theta, theta, params.fixed_sample_size));
  } else if (params.algorithm == SSP) {
    tester.reset(
        new SingleSamplingBernoulliTester(p0, p1, params.alpha, params.beta));
  } else { /* algorithm == SPRT */
    tester.reset(new SprtBernoulliTester(p0, p1, params.alpha, params.beta));
  }
  if (formula_level() == 0) {
    std::cout << "Acceptance sampling";
  }
  if (params.memoization) {
    auto ci = cache_.find(state.values());
    if (ci != cache_.end()) {
      tester->SetSample(ci->second);
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
  while (!tester->done()) {
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
      s = formula().sample(manager, dd_model, model, state, nullptr, nullptr,
                           nested_params, stats);
      have_sample = true;
    }
    if (!have_sample) {
      continue;
    }
    tester->AddObservation(s);
    if (formula_level() == 1) {
      PrintProgress(tester->sample().count());
    }
    if (VLOG_IS_ON(2)) {
      LOG(INFO) << std::string(' ', 2*(formula_level() - 1))
                << tester->StateToString();
    }
  }
  formula_level_--;
  if (formula_level() == 0) {
    std::cout << tester->sample().count() << " observations." << std::endl;
    stats->sample_size.AddObservation(tester->sample().count());
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
    cache_[state.values()] = tester->sample();
  }
  return tester->accept();
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
bool Comparison::verify(const DecisionDiagramManager* manager,
                        const DecisionDiagramModel* dd_model,
                        const Model& model, const State& state,
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

namespace {

BDD StateBdd(
    const DecisionDiagramManager& dd_man,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Model& model, const std::vector<int>& state) {
  BDD dds = dd_man.GetConstant(true);
  for (int i = model.variables().size() - 1; i >= 0; --i) {
    const ParsedVariable& v = model.variables()[i];
    auto j = variable_properties.find(v.name());
    CHECK(j != variable_properties.end());
    const VariableProperties& p = j->second;
    const int value = state.at(i);
    dds = variable_mtbdd(dd_man, v.min_value(), p.low_bit(), p.high_bit())
        .Interval(value, value) && dds;
  }
  return dds;
}

bool Verify(const StateFormula& formula,
            const DecisionDiagramManager* manager,
            const DecisionDiagramModel* dd_model, DdNode* ddf,
            const Model& model, const State& state,
            const ModelCheckingParams& params, ModelCheckingStats* stats) {
  if (manager != nullptr) {
    BDD state_bdd = StateBdd(
        *manager, dd_model->variable_properties(), model, state.values());
    DdNode* sol = Cudd_bddAnd(manager->manager(), ddf, state_bdd.get());
    Cudd_Ref(sol);
    const bool result = (sol != manager->GetConstant(false).get());
    Cudd_RecursiveDeref(manager->manager(), sol);
    return result;
  } else {
    return formula.verify(nullptr, nullptr, model, state, params, stats);
  }
}

}  // namespace

/* Generates a sample for this path formula. */
bool Until::sample(const DecisionDiagramManager* manager,
                   const DecisionDiagramModel* dd_model,
                   const Model& model, const State& state,
                   DdNode* dd1, DdNode* dd2,
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
      if (Verify(post(), manager, dd_model, dd2, model, curr_state, params,
                 stats)) {
        result = true;
        done = true;
      } else if (!Verify(pre(), manager, dd_model, dd1, model, curr_state,
                         params, stats)) {
        result = false;
        done = true;
      }
    } else {
      if (!Verify(pre(), manager, dd_model, dd1, model, curr_state, params,
                  stats)) {
        result = false;
        done = true;
      } else if (t_min < next_t
                 && Verify(post(), manager, dd_model, dd2, model, curr_state,
                           params, stats)) {
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
                   const DecisionDiagramModel& dd_model,
		   const State& state, const TypedValue& p, bool strict,
                   const ModelCheckingParams& params,
                   ModelCheckingStats* stats) const {
  double theta = p.value<double>();
  double p0 = std::min(1.0, theta + params.delta);
  double p1 = std::max(0.0, theta - params.delta);

  DdNode* dd1 = pre().verify(dd_man, dd_model, false, params).release();
  DdNode* dd2 = post().verify(dd_man, dd_model, false, params).release();

  if (params.algorithm == ESTIMATE) {
    std::cout << "Sequential estimation";
    SequentialEstimator<int> estimator(params.delta, params.alpha);
    while (true) {
      const bool x = sample(&dd_man, &dd_model, model, state, dd1, dd2, params,
                            stats);
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
    Cudd_RecursiveDeref(dd_man.manager(), dd1);
    Cudd_RecursiveDeref(dd_man.manager(), dd2);
    if (strict) {
      return estimator.value() > theta;
    } else {
      return estimator.value() >= theta;
    }
  }

  std::unique_ptr<BernoulliTester> tester;
  if (params.algorithm == FIXED) {
    tester.reset(
        new FixedBernoulliTester(theta, theta, params.fixed_sample_size));
  } else if (params.algorithm == SSP) {
    tester.reset(
        new SingleSamplingBernoulliTester(p0, p1, params.alpha, params.beta));
  } else { /* algorithm == SPRT */
    tester.reset(new SprtBernoulliTester(p0, p1, params.alpha, params.beta));
  }
  std::cout << "Acceptance sampling";
  while (!tester->done()) {
    bool s = sample(&dd_man, &dd_model, model, state, dd1, dd2, params, stats);
    tester->AddObservation(s);
    PrintProgress(tester->sample().count());
    if (VLOG_IS_ON(2)) {
      LOG(INFO) << tester->StateToString();
    }
  }
  std::cout << tester->sample().count() << " observations." << std::endl;
  stats->sample_size.AddObservation(tester->sample().count());
  Cudd_RecursiveDeref(dd_man.manager(), dd1);
  Cudd_RecursiveDeref(dd_man.manager(), dd2);
  return tester->accept();
}

/* Clears the cache of any probabilistic operator. */
size_t Until::clear_cache() const {
  return pre().clear_cache() + post().clear_cache();
}
