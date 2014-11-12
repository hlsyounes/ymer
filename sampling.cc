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
#include "models.h"
#include "states.h"
#include "src/compiled-property.h"
#include "src/rng.h"
#include "src/statistics.h"
#include "src/strutil.h"

#include "cudd.h"
#include "glog/logging.h"
#include "gsl/gsl_cdf.h"

namespace {

void PrintProgress(int n) {
  if (n % 1000 == 0) {
    std::cout << ':';
  } else if (n % 100 == 0) {
    std::cout << '.';
  }
}

class SamplingVerifier
    : public CompiledPropertyVisitor, public CompiledPathPropertyVisitor {
 public:
  SamplingVerifier(const CompiledModel* model,
                   const DecisionDiagramModel* dd_model,
                   const ModelCheckingParams& params,
                   CompiledExpressionEvaluator* evaluator,
                   CompiledDistributionSampler<DCEngine>* sampler,
                   const State* state, int probabilistic_level,
                   ModelCheckingStats* stats);

  bool result() const { return result_; }

  int GetSampleCacheSize() const;

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

  template <typename OutputIterator>
  bool VerifyHelper(const CompiledProperty& property, const BDD* ddf,
                    bool default_result, OutputIterator* state_inserter);
  std::string StateToString(const State& state) const;

  bool result_;
  const CompiledModel* model_;
  const DecisionDiagramModel* dd_model_;
  ModelCheckingParams params_;
  CompiledExpressionEvaluator* evaluator_;
  CompiledDistributionSampler<DCEngine>* sampler_;
  const State* state_;
  int probabilistic_level_;
  ModelCheckingStats* stats_;
  std::map<int, std::map<std::vector<int>, Sample<int>>> sample_cache_;
  std::map<int, std::pair<BDD, BDD>> dd_cache_;

  // Registered clients.
  std::map<int, short> registered_clients_;
  // Next client id.
  short next_client_id_;
};

SamplingVerifier::SamplingVerifier(
    const CompiledModel* model, const DecisionDiagramModel* dd_model,
    const ModelCheckingParams& params, CompiledExpressionEvaluator* evaluator,
    CompiledDistributionSampler<DCEngine>* sampler, const State* state,
    int probabilistic_level, ModelCheckingStats* stats)
    : model_(model),
      dd_model_(dd_model),
      params_(params),
      evaluator_(evaluator),
      sampler_(sampler),
      state_(state),
      probabilistic_level_(probabilistic_level),
      stats_(stats),
      next_client_id_(1) {}

void SamplingVerifier::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  switch (property.op()) {
    case CompiledNaryOperator::AND:
      result_ = true;
      if (property.has_expr_operand()) {
        result_ = evaluator_->EvaluateIntExpression(
            property.expr_operand().expr(), state_->values());
      }
      if (result_ == true && !property.other_operands().empty()) {
        double alpha = params_.alpha / property.other_operands().size();
        std::swap(params_.alpha, alpha);
        for (const CompiledProperty& operand : property.other_operands()) {
          operand.Accept(this);
          if (result_ == false) {
            break;
          }
        }
        std::swap(params_.alpha, alpha);
      }
      break;
    case CompiledNaryOperator::OR:
      result_ = false;
      if (property.has_expr_operand()) {
        result_ = evaluator_->EvaluateIntExpression(
            property.expr_operand().expr(), state_->values());
      }
      if (result_ == false && !property.other_operands().empty()) {
        double beta = params_.beta / property.other_operands().size();
        std::swap(params_.beta, beta);
        for (const CompiledProperty& operand : property.other_operands()) {
          operand.Accept(this);
          if (result_ == true) {
            break;
          }
        }
        std::swap(params_.beta, beta);
      }
      break;
    case CompiledNaryOperator::IFF: {
      bool has_result = false;
      if (property.has_expr_operand()) {
        result_ = evaluator_->EvaluateIntExpression(
            property.expr_operand().expr(), state_->values());
        has_result = true;
      }
      double alpha = std::min(params_.alpha, params_.beta)
          / property.other_operands().size();
      double beta = std::min(params_.alpha, params_.beta)
          / property.other_operands().size();
      std::swap(params_.alpha, alpha);
      std::swap(params_.beta, beta);
      for (const CompiledProperty& operand : property.other_operands()) {
        bool prev_result = result_;
        operand.Accept(this);
        if (has_result) {
          result_ = prev_result == result_;
        }
        has_result = true;
      }
      std::swap(params_.beta, beta);
      std::swap(params_.alpha, alpha);
      break;
    }
  }
}

void SamplingVerifier::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  std::swap(params_.alpha, params_.beta);
  property.operand().Accept(this);
  result_ = !result_;
  std::swap(params_.alpha, params_.beta);
}

void SamplingVerifier::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  ++probabilistic_level_;
  double nested_error = 0.0;
  if (dd_model_ == nullptr && property.path_property().is_probabilistic()) {
    if (params_.nested_error > 0) {
      // User-specified nested error.
      nested_error = params_.nested_error;
    } else {
      // Simple heuristic for nested error.
      nested_error = 0.8 * MaxNestedError(params_.delta);
    }
    if (probabilistic_level_ == 1) {
      VLOG(1) << "Nested error: " << nested_error;
      VLOG(1) << "Maximum symmetric nested error: "
              << MaxNestedError(params_.delta);
    }
  }
  const double theta = property.threshold();
  const double theta0 =
      std::min(1.0, (theta + params_.delta)*(1.0 - nested_error));
  const double theta1 =
      std::max(0.0, 1.0 - (1.0 - (theta - params_.delta))*(1.0 - nested_error));
  ModelCheckingParams nested_params = params_;
  nested_params.alpha = nested_error;
  nested_params.beta = nested_error;
  if (params_.algorithm == ESTIMATE) {
    if (probabilistic_level_ == 1) {
      std::cout << "Sequential estimation";
    }
    SequentialEstimator<int> estimator(params_.delta, params_.alpha);
    std::swap(params_, nested_params);
    while (true) {
      property.path_property().Accept(this);
      const bool x = result_;
      estimator.AddObservation(x ? 1 : 0);
      if (probabilistic_level_ == 1) {
        PrintProgress(estimator.count());
      }
      if (VLOG_IS_ON(2)) {
        LOG(INFO) << std::string(2*(probabilistic_level_ - 1), ' ')
                  << estimator.count() << '\t' << estimator.value() << '\t'
                  << estimator.state() << '\t' << estimator.bound();
      }
      if (estimator.state() <= estimator.bound()) {
        break;
      }
    }
    std::swap(params_, nested_params);
    if (probabilistic_level_ == 1) {
      std::cout << estimator.count() << " observations." << std::endl;
      std::cout << "Pr[" << property.path_property().string() << "] = "
                << estimator.value() << " ("
                << std::max(0.0, estimator.value() - params_.delta) << ','
                << std::min(1.0, estimator.value() + params_.delta) << ")"
                << std::endl;
      stats_->sample_size.AddObservation(estimator.count());
    }
    switch (property.op()) {
      case CompiledProbabilityThresholdOperator::GREATER_EQUAL:
        result_ = estimator.value() >= theta;
        break;
      case CompiledProbabilityThresholdOperator::GREATER:
        result_ = estimator.value() > theta;
        break;
    }
    --probabilistic_level_;
    return;
  }

  std::unique_ptr<BernoulliTester> tester;
  if (params_.algorithm == FIXED) {
    tester.reset(new FixedBernoulliTester(
        theta, theta, params_.fixed_sample_size));
  } else if (params_.algorithm == SSP) {
    tester.reset(new SingleSamplingBernoulliTester(
        theta0, theta1, params_.alpha, params_.beta));
  } else { /* algorithm == SPRT */
    tester.reset(new SprtBernoulliTester(
        theta0, theta1, params_.alpha, params_.beta));
  }
  if (probabilistic_level_ == 1) {
    std::cout << "Acceptance sampling";
  }
  if (params_.memoization) {
    auto& sample_cache = sample_cache_[property.path_property().index()];
    auto ci = sample_cache.find(state_->values());
    if (ci != sample_cache.end()) {
      tester->SetSample(ci->second);
    }
  }
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
    for (std::map<int, short>::const_iterator ci = registered_clients_.begin();
	 ci != registered_clients_.end(); ci++) {
      int sockfd = (*ci).first;
      short client_id = (*ci).second;
      ServerMsg smsg = { ServerMsg::START, current_property };
      int nbytes = send(sockfd, &smsg, sizeof smsg, 0);
      if (nbytes == -1) {
	perror(PACKAGE);
        LOG(FATAL) << "server error";
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
      registered_clients_.erase(*ci);
    }
  }
  std::swap(params_, nested_params);
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
	int client_id = next_client_id_++;
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
	    registered_clients_[sockfd] = client_id;
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
	     registered_clients_.begin();
	   ci != registered_clients_.end(); ci++) {
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
	registered_clients_.erase(*ci);
      }
    } else {
      /* Local mode. */
      property.path_property().Accept(this);
      s = result_;
      have_sample = true;
    }
    if (!have_sample) {
      continue;
    }
    tester->AddObservation(s);
    if (probabilistic_level_ == 1) {
      PrintProgress(tester->sample().count());
    }
    if (VLOG_IS_ON(2)) {
      LOG(INFO) << std::string(2*(probabilistic_level_ - 1), ' ')
                << tester->StateToString();
    }
  }
  std::swap(params_, nested_params);
  if (probabilistic_level_ == 1) {
    std::cout << tester->sample().count() << " observations." << std::endl;
    stats_->sample_size.AddObservation(tester->sample().count());
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
    for (std::map<int, short>::const_iterator ci = registered_clients_.begin();
	 ci != registered_clients_.end(); ci++) {
      int sockfd = (*ci).first;
      ServerMsg smsg = { ServerMsg::STOP };
      if (-1 == send(sockfd, &smsg, sizeof smsg, 0)) {
	perror(PACKAGE);
	exit(1);
      }
    }
  }
  if (params_.memoization) {
    sample_cache_[property.path_property().index()][state_->values()] =
        tester->sample();
  }
  result_ = tester->accept();
  --probabilistic_level_;
}

void SamplingVerifier::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  result_ = evaluator_->EvaluateIntExpression(property.expr(),
                                              state_->values());
}

class StateLess {
 public:
  bool operator()(const State& lhs, const State& rhs) const {
    return lhs.values() < rhs.values();
  }
};

void SamplingVerifier::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  const BDD* dd1 = nullptr;
  const BDD* dd2 = nullptr;
  auto cached_dds = dd_cache_.end();
  if (dd_model_ != nullptr) {
    // Mixed engine.
    cached_dds = dd_cache_.find(path_property.index());
    if (cached_dds == dd_cache_.end()) {
      BDD dd1 = Verify(path_property.pre_property(), *dd_model_, false, false,
          params_.epsilon);
      BDD dd2 = Verify(path_property.post_property(), *dd_model_, false, false,
                       params_.epsilon);
      cached_dds = dd_cache_.insert(
          {path_property.index(), std::make_pair(dd1, dd2)}).first;
    }
    dd1 = &cached_dds->second.first;
    dd2 = &cached_dds->second.second;
  }
  double t = 0.0;
  State curr_state = *state_;
  const double t_min = path_property.min_time();
  const double t_max = path_property.max_time();
  size_t path_length = 1;
  bool done = false, output = false;
  std::set<State, StateLess> unique_pre_states;
  auto pre_states_inserter =
      inserter(unique_pre_states, unique_pre_states.begin());
  auto* pre_states_inserter_ptr =
      path_property.pre_property().is_probabilistic() ? &pre_states_inserter
                                                      : nullptr;
  std::vector<State> post_states;
  auto post_states_inserter = back_inserter(post_states);
  auto* post_states_inserter_ptr =
      path_property.post_property().is_probabilistic() ? &post_states_inserter
                                                       : nullptr;
  while (!done && path_length < params_.max_path_length) {
    if (VLOG_IS_ON(3) && probabilistic_level_ == 1) {
      LOG(INFO) << "t = " << t << ": " << StateToString(curr_state);
    }
    State next_state = curr_state.Next(*model_, evaluator_, sampler_);
    double next_t = t + (next_state.time() - curr_state.time());
    const State* curr_state_ptr = &curr_state;
    std::swap(state_, curr_state_ptr);
    if (t_min <= t) {
      if (VerifyHelper(path_property.post_property(), dd2, false,
                       post_states_inserter_ptr)) {
        result_ = true;
        done = true;
      } else if (!VerifyHelper(path_property.pre_property(), dd1, true,
                               false ? pre_states_inserter_ptr : nullptr)) {
        result_ = false;
        done = true;
      }
    } else {
      if (!VerifyHelper(path_property.pre_property(), dd1, true,
                        t_min < next_t ? nullptr : pre_states_inserter_ptr)) {
        result_ = false;
        done = true;
      } else if (t_min < next_t &&
                 VerifyHelper(path_property.post_property(), dd2, false,
                              post_states_inserter_ptr)) {
        t = t_min;
        result_ = true;
        done = true;
        output = true;
      }
    }
    std::swap(state_, curr_state_ptr);
    if (!done) {
      curr_state = std::move(next_state);
      t = next_t;
      if (t_max < t) {
	result_ = false;
	done = true;
	output = true;
      }
      path_length++;
    }
  }
  if (!unique_pre_states.empty() || !post_states.empty()) {
    if (path_property.pre_property().is_probabilistic()) {
      if (path_property.post_property().is_probabilistic()) {
        // For each post state, verify post_property in that state, and verify
        // pre_property in unique_pre_states and all prior post states, treating
        // each verification as a conjunct.  Each such verification is treated
        // as a disjunct.
        if (!post_states.empty()) {
          double beta = params_.beta / post_states.size();
          std::swap(params_.beta, beta);
          for (size_t i = 0; i < post_states.size(); ++i) {
            result_ = true;
            double alpha = params_.alpha / (unique_pre_states.size() + i + 1);
            std::swap(params_.alpha, alpha);
            for (const State& state : unique_pre_states) {
              const State* curr_state_ptr = &state;
              std::swap(state_, curr_state_ptr);
              path_property.pre_property().Accept(this);
              std::swap(state_, curr_state_ptr);
              if (result_ == false) {
                break;
              }
            }
            for (size_t j = 0; result_ == true && j < i; ++j) {
              const State* curr_state_ptr = &post_states[j];
              std::swap(state_, curr_state_ptr);
              path_property.pre_property().Accept(this);
              std::swap(state_, curr_state_ptr);
            }
            if (result_ == true) {
              const State* curr_state_ptr = &post_states[i];
              std::swap(state_, curr_state_ptr);
              path_property.post_property().Accept(this);
              std::swap(state_, curr_state_ptr);
            }
            std::swap(params_.alpha, alpha);
            if (result_ == true) {
              break;
            }
          }
          std::swap(params_.beta, beta);
        }
      } else {
        // Just verify pre_property in unique_pre_states, treating each
        // verification as a conjunct.
        result_ = true;
        double alpha = params_.alpha / unique_pre_states.size();
        std::swap(params_.alpha, alpha);
        for (const State& state : unique_pre_states) {
          const State* curr_state_ptr = &state;
          std::swap(state_, curr_state_ptr);
          path_property.pre_property().Accept(this);
          std::swap(state_, curr_state_ptr);
          if (result_ == false) {
            break;
          }
        }
        std::swap(params_.alpha, alpha);
      }
    } else if (path_property.post_property().is_probabilistic()) {
      // Just verify post_property in unique_post_states, treating each
      // verification as a disjunct.
      std::set<State, StateLess> unique_post_states(post_states.begin(),
                                                    post_states.end());
      result_ = false;
      double beta = params_.beta / unique_post_states.size();
      std::swap(params_.beta, beta);
      for (const State& state : unique_post_states) {
        const State* curr_state_ptr = &state;
        std::swap(state_, curr_state_ptr);
        path_property.pre_property().Accept(this);
        std::swap(state_, curr_state_ptr);
        if (result_ == true) {
          break;
        }
      }
      std::swap(params_.beta, beta);
    }
  }
  if (VLOG_IS_ON(3)) {
    if (output) {
      LOG(INFO) << "t = " << t << ": " << StateToString(curr_state);
    }
    if (result_) {
      LOG(INFO) << ">>positive sample";
    } else {
      LOG(INFO) << ">>negative sample";
    }
  }
  if (probabilistic_level_ == 1) {
    stats_->path_length.AddObservation(path_length);
  }
}

template <typename OutputIterator>
bool SamplingVerifier::VerifyHelper(const CompiledProperty& property,
                                    const BDD* ddf, bool default_result,
                                    OutputIterator* state_inserter) {
  if (dd_model_ != nullptr) {
    return ddf->ValueInState(state_->values(), model_->variables());
  } else if (!property.is_probabilistic()) {
    property.Accept(this);
    return result_;
  } else if (state_inserter != nullptr) {
    **state_inserter = *state_;
  }
  return default_result;
}

std::string SamplingVerifier::StateToString(const State& state) const {
  std::string result;
  for (size_t i = 0; i < model_->variables().size(); ++i) {
    if (i > 0) {
      result += " & ";
    }
    result += StrCat(model_->variables()[i].name(), '=', state.values()[i]);
  }
  return result;
}

int SamplingVerifier::GetSampleCacheSize() const {
  int sample_cache_size = 0;
  for (const auto& cache : sample_cache_) {
    sample_cache_size += cache.second.size();
  }
  return sample_cache_size;
}

}  // namespace

bool Verify(const CompiledProperty& property, const CompiledModel& model,
            const DecisionDiagramModel* dd_model,
            const ModelCheckingParams& params,
            CompiledExpressionEvaluator* evaluator,
            CompiledDistributionSampler<DCEngine>* sampler, const State& state,
            ModelCheckingStats* stats) {
  SamplingVerifier verifier(&model, dd_model, params, evaluator, sampler,
                            &state, 0, stats);
  property.Accept(&verifier);
  stats->sample_cache_size.AddObservation(verifier.GetSampleCacheSize());
  return verifier.result();
}

bool GetObservation(const CompiledPathProperty& property,
                    const CompiledModel& model,
                    const DecisionDiagramModel* dd_model,
                    const ModelCheckingParams& params,
                    CompiledExpressionEvaluator* evaluator,
                    CompiledDistributionSampler<DCEngine>* sampler,
                    const State& state, ModelCheckingStats* stats) {
  SamplingVerifier verifier(&model, dd_model, params, evaluator, sampler,
                            &state, 1, stats);
  property.Accept(&verifier);
  stats->sample_cache_size.AddObservation(verifier.GetSampleCacheSize());
  return verifier.result();
}
