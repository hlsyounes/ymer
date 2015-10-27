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

#include <condition_variable>
#include <iostream>
#include <limits>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <unordered_map>
#include <vector>

#include "src/compiled-property.h"
#include "src/ddmodel.h"
#include "src/simulator.h"
#include "src/statistics.h"
#include "src/strutil.h"

#include "glog/logging.h"
#include "gsl/gsl_cdf.h"

namespace {

template <typename Algorithm>
struct ResultType {
};

template <>
struct ResultType<ThresholdAlgorithm> {
  using type = bool;
};

template <>
struct ResultType<EstimationAlgorithm> {
  using type = double;
};

void PrintProgress(int n) {
  if (n % 1000 == 0) {
    std::cout << ':';
  } else if (n % 100 == 0) {
    std::cout << '.';
  }
}

struct DdCache {
  struct Entry {
    BDD dd1;
    BDD dd2;
    Optional<BDD> feasible;
  };

  std::unordered_map<int, Entry> entries;
  std::mutex mutex;
};

class SamplingVerifier : public CompiledPropertyVisitor,
                         public CompiledPathPropertyVisitor {
 private:
  struct Result {
    int path_length;
    bool early_termination;
    bool value;
  };

  class ResultQueue {
   public:
    ResultQueue();

    int pop_count() const { return pop_count_; }

    int push_count() const { return push_count_; }

    bool Enabled() const;

    void Push(const Result& result);

    Result Pop();

    void Disable();

   private:
    std::queue<Result> results_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    int push_count_;
    int pop_count_;
    bool enabled_;
  };

 public:
  SamplingVerifier(
      const CompiledModel* model, const DecisionDiagramModel* dd_model,
      DdCache* dd_cache, ModelCheckingStats* stats,
      const ModelCheckingParams& params, const State* state,
      std::vector<CompiledExpressionEvaluator>* evaluators,
      std::vector<CompiledDistributionSampler<std::mt19937_64>>* samplers,
      std::vector<NextStateSampler<std::mt19937_64>>* simulators);

  SamplingVerifier(
      const CompiledModel* model, const DecisionDiagramModel* dd_model,
      DdCache* dd_cache, ModelCheckingStats* stats,
      const ModelCheckingParams& params, const State* state,
      std::vector<CompiledExpressionEvaluator>* evaluators,
      std::vector<CompiledDistributionSampler<std::mt19937_64>>* samplers,
      std::vector<NextStateSampler<std::mt19937_64>>* simulators,
      int thread_index, ResultQueue* result_queue);

  bool result() const { return result_.value; }

  int GetSampleCacheSize() const;

 private:
  void DoVisitCompiledNaryProperty(
      const CompiledNaryProperty& property) override;
  void DoVisitCompiledNotProperty(const CompiledNotProperty& property) override;
  void DoVisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property) override;
  void DoVisitCompiledProbabilityEstimationProperty(
      const CompiledProbabilityEstimationProperty& property) override;
  void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property) override;
  void DoVisitCompiledUntilProperty(
      const CompiledUntilProperty& path_property) override;

  template <typename Algorithm>
  std::unique_ptr<SequentialTester<typename ResultType<Algorithm>::type>>
  VerifyProbabilisticProperty(Algorithm algorithm, double theta,
                              const CompiledPathProperty& path_property);
  template <typename Algorithm>
  std::unique_ptr<SequentialTester<typename ResultType<Algorithm>::type>>
  NewSequentialTester(Algorithm algorithm, double theta0, double theta1) const;
  template <typename OutputIterator>
  bool VerifyHelper(const CompiledProperty& property, const Optional<BDD>& ddf,
                    bool default_result, OutputIterator* state_inserter);
  std::string StateToString(const State& state) const;

  const CompiledModel* const model_;
  const DecisionDiagramModel* const dd_model_;
  DdCache* const dd_cache_;
  ModelCheckingStats* const stats_;
  Result result_;
  ModelCheckingParams params_;
  const State* state_;
  int probabilistic_level_;
  std::vector<CompiledExpressionEvaluator>* evaluators_;
  CompiledExpressionEvaluator* evaluator_;
  std::vector<CompiledDistributionSampler<std::mt19937_64>>* samplers_;
  CompiledDistributionSampler<std::mt19937_64>* sampler_;
  std::vector<NextStateSampler<std::mt19937_64>>* simulators_;
  NextStateSampler<std::mt19937_64>* simulator_;
  ResultQueue* const result_queue_;
  std::unordered_map<int, std::map<std::vector<int>, Sample<double>>>
      sample_cache_;
};

SamplingVerifier::SamplingVerifier(
    const CompiledModel* model, const DecisionDiagramModel* dd_model,
    DdCache* dd_cache, ModelCheckingStats* stats,
    const ModelCheckingParams& params, const State* state,
    std::vector<CompiledExpressionEvaluator>* evaluators,
    std::vector<CompiledDistributionSampler<std::mt19937_64>>* samplers,
    std::vector<NextStateSampler<std::mt19937_64>>* simulators)
    : model_(model),
      dd_model_(dd_model),
      dd_cache_(dd_cache),
      stats_(stats),
      params_(params),
      state_(state),
      probabilistic_level_(0),
      evaluators_(evaluators),
      evaluator_(&(*evaluators)[0]),
      samplers_(samplers),
      sampler_(&(*samplers)[0]),
      simulators_(simulators),
      simulator_(&(*simulators)[0]),
      result_queue_(nullptr) {}

SamplingVerifier::SamplingVerifier(
    const CompiledModel* model, const DecisionDiagramModel* dd_model,
    DdCache* dd_cache, ModelCheckingStats* stats,
    const ModelCheckingParams& params, const State* state,
    std::vector<CompiledExpressionEvaluator>* evaluators,
    std::vector<CompiledDistributionSampler<std::mt19937_64>>* samplers,
    std::vector<NextStateSampler<std::mt19937_64>>* simulators,
    int thread_index, ResultQueue* result_queue)
    : model_(model),
      dd_model_(dd_model),
      dd_cache_(dd_cache),
      stats_(stats),
      params_(params),
      state_(state),
      probabilistic_level_(1),
      evaluators_(evaluators),
      evaluator_(&(*evaluators)[thread_index]),
      samplers_(samplers),
      sampler_(&(*samplers)[thread_index]),
      simulators_(simulators),
      simulator_(&(*simulators)[thread_index]),
      result_queue_(result_queue) {}

void SamplingVerifier::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  switch (property.op()) {
    case CompiledNaryOperator::AND:
      result_.value = true;
      if (property.has_expr_operand()) {
        result_.value = evaluator_->EvaluateIntExpression(
            property.expr_operand().expr(), state_->values());
      }
      if (result_.value == true && !property.other_operands().empty()) {
        double alpha = params_.alpha / property.other_operands().size();
        std::swap(params_.alpha, alpha);
        for (const CompiledProperty& operand : property.other_operands()) {
          operand.Accept(this);
          if (result_.value == false) {
            break;
          }
        }
        std::swap(params_.alpha, alpha);
      }
      break;
    case CompiledNaryOperator::OR:
      result_.value = false;
      if (property.has_expr_operand()) {
        result_.value = evaluator_->EvaluateIntExpression(
            property.expr_operand().expr(), state_->values());
      }
      if (result_.value == false && !property.other_operands().empty()) {
        double beta = params_.beta / property.other_operands().size();
        std::swap(params_.beta, beta);
        for (const CompiledProperty& operand : property.other_operands()) {
          operand.Accept(this);
          if (result_.value == true) {
            break;
          }
        }
        std::swap(params_.beta, beta);
      }
      break;
    case CompiledNaryOperator::IFF: {
      bool has_value = false;
      if (property.has_expr_operand()) {
        result_.value = evaluator_->EvaluateIntExpression(
            property.expr_operand().expr(), state_->values());
        has_value = true;
      }
      double alpha = std::min(params_.alpha, params_.beta) /
                     property.other_operands().size();
      double beta = std::min(params_.alpha, params_.beta) /
                    property.other_operands().size();
      std::swap(params_.alpha, alpha);
      std::swap(params_.beta, beta);
      for (const CompiledProperty& operand : property.other_operands()) {
        bool prev_value = result_.value;
        operand.Accept(this);
        if (has_value) {
          result_.value = prev_value == result_.value;
        }
        has_value = true;
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
  result_.value = !result_.value;
  std::swap(params_.alpha, params_.beta);
}

void SamplingVerifier::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  if (dd_model_ == nullptr && property.path_property().is_unbounded()) {
    VerifyProbabilisticProperty(params_.estimation_algorithm,
                                property.threshold(), property.path_property());
  } else {
    VerifyProbabilisticProperty(params_.threshold_algorithm,
                                property.threshold(), property.path_property());
  }
}

void SamplingVerifier::DoVisitCompiledProbabilityEstimationProperty(
    const CompiledProbabilityEstimationProperty& property) {
  auto tester = VerifyProbabilisticProperty(params_.estimation_algorithm, 0.5,
                                            property.path_property());
  if (probabilistic_level_ == 0) {
    std::cout << "Pr[" << property.path_property().string()
              << "] = " << tester->sample().mean() << " ("
              << std::max(0.0, tester->sample().mean() - params_.delta) << ','
              << std::min(1.0, tester->sample().mean() + params_.delta) << ")"
              << std::endl;
  }
}

template <typename Algorithm>
std::unique_ptr<SequentialTester<typename ResultType<Algorithm>::type>>
SamplingVerifier::VerifyProbabilisticProperty(
    Algorithm algorithm, const double theta,
    const CompiledPathProperty& path_property) {
  ++probabilistic_level_;
  double nested_error = 0.0;
  if (dd_model_ == nullptr && path_property.is_probabilistic()) {
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
  const double theta0 =
      std::min(1.0, (theta + params_.delta) * (1.0 - nested_error));
  const double theta1 = std::max(
      0.0, 1.0 - (1.0 - (theta - params_.delta)) * (1.0 - nested_error));
  ModelCheckingParams nested_params = params_;
  nested_params.alpha = nested_error;
  nested_params.beta = nested_error;

  auto tester = NewSequentialTester(algorithm, theta0, theta1);
  if (probabilistic_level_ == 1) {
    std::cout << "Acceptance sampling";
  }
  if (params_.memoization) {
    auto& sample_cache = sample_cache_[path_property.index()];
    auto ci = sample_cache.find(state_->values());
    if (ci != sample_cache.end()) {
      tester->SetSample(ci->second);
    }
  }
  std::vector<ResultQueue> result_queues;
  std::vector<SamplingVerifier> verifiers;
  std::vector<std::thread> workers;
  std::queue<int> schedule;
  if (result_queue_ == nullptr && evaluators_->size() > 1) {
    result_queues = std::vector<ResultQueue>(evaluators_->size());
    verifiers.reserve(evaluators_->size());
    for (size_t i = 0; i < evaluators_->size(); ++i) {
      verifiers.emplace_back(model_, dd_model_, dd_cache_, nullptr,
                             nested_params, state_, evaluators_, samplers_,
                             simulators_, i, &result_queues[i]);
      workers.emplace_back([&path_property, &result_queues, &verifiers, i]() {
        while (result_queues[i].Enabled()) {
          path_property.Accept(&verifiers[i]);
          result_queues[i].Push(verifiers[i].result_);
        }
      });
      schedule.push(i);
    }
  }
  std::swap(params_, nested_params);
  while (!tester->done()) {
    if (result_queues.empty()) {
      path_property.Accept(this);
    } else {
      int i = schedule.front();
      schedule.pop();
      result_ = result_queues[i].Pop();
      schedule.push(i);
    }
    double observation_weight = 1;
    if (dd_model_ == nullptr && path_property.is_unbounded()) {
      observation_weight =
          pow(1 - params_.termination_probability, -(result_.path_length - 1));
    }
    tester->AddObservation(result_.value ? observation_weight : 0);
    if (probabilistic_level_ == 1) {
      PrintProgress(tester->sample().count());
      stats_->path_length.AddObservation(result_.path_length);
      if (result_.value) {
        stats_->path_length_accept.AddObservation(result_.path_length);
      } else if (result_.early_termination) {
        stats_->path_length_terminate.AddObservation(result_.path_length);
      } else {
        stats_->path_length_reject.AddObservation(result_.path_length);
      }
    }
    if (VLOG_IS_ON(2)) {
      LOG(INFO) << std::string(2 * (probabilistic_level_ - 1), ' ')
                << tester->StateToString();
    }
  }
  std::swap(params_, nested_params);
  if (probabilistic_level_ == 1) {
    std::cout << tester->sample().count() << " observations." << std::endl;
    stats_->sample_size.AddObservation(tester->sample().count());
  }
  if (!result_queues.empty()) {
    for (size_t i = 0; i < result_queues.size(); ++i) {
      result_queues[i].Disable();
      workers[i].join();
      std::cout << "Used " << result_queues[i].pop_count() << " of "
                << result_queues[i].push_count() << " observations from thread "
                << i + 1 << "." << std::endl;
    }
  }
  if (params_.memoization) {
    sample_cache_[path_property.index()][state_->values()] = tester->sample();
  }
  result_.value = tester->accept();
  --probabilistic_level_;
  return std::move(tester);
}

template <>
std::unique_ptr<SequentialTester<bool>> SamplingVerifier::NewSequentialTester(
    ThresholdAlgorithm algorithm, double theta0, double theta1) const {
  switch (algorithm) {
    case ThresholdAlgorithm::FIXED:
      return std::unique_ptr<SequentialTester<bool>>(
          new FixedSampleSizeTester<bool>(theta0, theta1,
                                          params_.fixed_sample_size));
    case ThresholdAlgorithm::SSP:
      return std::unique_ptr<SequentialTester<bool>>(
          new SingleSamplingBernoulliTester(theta0, theta1, params_.alpha,
                                            params_.beta));
    case ThresholdAlgorithm::SPRT:
      return std::unique_ptr<SequentialTester<bool>>(
          new SprtBernoulliTester(theta0, theta1, params_.alpha, params_.beta));
    case ThresholdAlgorithm::CHOW_ROBBINS:
      return std::unique_ptr<SequentialTester<bool>>(
          new ChowRobbinsTester<bool>(theta0, theta1, params_.alpha));
  }
  LOG(FATAL) << "bad threshold algorithm";
}

template <>
std::unique_ptr<SequentialTester<double>> SamplingVerifier::NewSequentialTester(
    EstimationAlgorithm algorithm, double theta0, double theta1) const {
  switch (algorithm) {
    case EstimationAlgorithm::FIXED:
      return std::unique_ptr<SequentialTester<double>>(
          new FixedSampleSizeTester<double>(theta0, theta1,
                                            params_.fixed_sample_size));
    case EstimationAlgorithm::CHOW_ROBBINS:
      return std::unique_ptr<SequentialTester<double>>(
          new ChowRobbinsTester<double>(theta0, theta1, params_.alpha));
  }
  LOG(FATAL) << "bad estimation algorithm";
}

void SamplingVerifier::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  result_.value =
      evaluator_->EvaluateIntExpression(property.expr(), state_->values());
}

class StateLess {
 public:
  bool operator()(const State& lhs, const State& rhs) const {
    return lhs.values() < rhs.values();
  }
};

void SamplingVerifier::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  Optional<BDD> dd1;
  Optional<BDD> dd2;
  Optional<BDD> feasible;
  bool use_termination_probability = false;
  if (dd_model_ == nullptr) {
    // Sampling engine.
    use_termination_probability = path_property.is_unbounded();
  } else  {
    // Mixed engine.
    std::lock_guard<std::mutex> lock(dd_cache_->mutex);
    auto i = dd_cache_->entries.find(path_property.index());
    if (i != dd_cache_->entries.end()) {
      dd1 = i->second.dd1;
      dd2 = i->second.dd2;
      feasible = i->second.feasible;
    } else {
      dd1 = Verify(path_property.pre_property(), *dd_model_, false,
                   params_.epsilon);
      dd2 = Verify(path_property.post_property(), *dd_model_, false,
                   params_.epsilon);
      if (path_property.is_unbounded()) {
        feasible = VerifyExistsUntil(*dd_model_, dd1.value(), dd2.value());
        if (feasible.value().is_same(dd_model_->reachable_states())) {
          // All reachable states are feasible.
          feasible = dd_model_->manager().GetConstant(true);
        }
      }
      dd_cache_->entries.insert(
          {path_property.index(), {dd1.value(), dd2.value(), feasible}});
    }
  }
  double t = 0.0;
  State curr_state = *state_;
  State next_state = *state_;
  const double t_min = path_property.min_time();
  const double t_max = path_property.max_time();
  int path_length = 1;
  bool done = false, early_termination = false;
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
    if ((use_termination_probability &&
         sampler_->StandardUniform() < params_.termination_probability) ||
        (feasible.has_value() &&
         !feasible.value().ValueInState(curr_state.values(),
                                        model_->variables()))) {
      t = std::numeric_limits<double>::infinity();
      result_.value = false;
      done = true;
      early_termination = true;
    } else {
      simulator_->NextState(curr_state, &next_state);
      double next_t = t + (next_state.time() - curr_state.time());
      const State* curr_state_ptr = &curr_state;
      std::swap(state_, curr_state_ptr);
      if (t_min <= t) {
        if (VerifyHelper(path_property.post_property(), dd2, false,
                         post_states_inserter_ptr)) {
          result_.value = true;
          done = true;
        } else if (!VerifyHelper(path_property.pre_property(), dd1, true,
                                 false ? pre_states_inserter_ptr : nullptr)) {
          result_.value = false;
          done = true;
        }
      } else {
        if (!VerifyHelper(path_property.pre_property(), dd1, true,
                          t_min < next_t ? nullptr : pre_states_inserter_ptr)) {
          result_.value = false;
          done = true;
        } else if (t_min < next_t &&
                   VerifyHelper(path_property.post_property(), dd2, false,
                                post_states_inserter_ptr)) {
          t = t_min;
          result_.value = true;
          done = true;
        }
      }
      std::swap(state_, curr_state_ptr);
      if (!done) {
        curr_state.swap(next_state);
        t = next_t;
        if (t_max < t || t == std::numeric_limits<double>::infinity()) {
          result_.value = false;
          done = true;
        }
        path_length++;
      }
    }
  }
  if (!early_termination &&
      (!unique_pre_states.empty() || !post_states.empty())) {
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
            result_.value = true;
            double alpha = params_.alpha / (unique_pre_states.size() + i + 1);
            std::swap(params_.alpha, alpha);
            for (const State& state : unique_pre_states) {
              const State* curr_state_ptr = &state;
              std::swap(state_, curr_state_ptr);
              path_property.pre_property().Accept(this);
              std::swap(state_, curr_state_ptr);
              if (result_.value == false) {
                break;
              }
            }
            for (size_t j = 0; result_.value == true && j < i; ++j) {
              const State* curr_state_ptr = &post_states[j];
              std::swap(state_, curr_state_ptr);
              path_property.pre_property().Accept(this);
              std::swap(state_, curr_state_ptr);
            }
            if (result_.value == true) {
              const State* curr_state_ptr = &post_states[i];
              std::swap(state_, curr_state_ptr);
              path_property.post_property().Accept(this);
              std::swap(state_, curr_state_ptr);
            }
            std::swap(params_.alpha, alpha);
            if (result_.value == true) {
              break;
            }
          }
          std::swap(params_.beta, beta);
        }
      } else {
        // Just verify pre_property in unique_pre_states, treating each
        // verification as a conjunct.
        result_.value = true;
        double alpha = params_.alpha / unique_pre_states.size();
        std::swap(params_.alpha, alpha);
        for (const State& state : unique_pre_states) {
          const State* curr_state_ptr = &state;
          std::swap(state_, curr_state_ptr);
          path_property.pre_property().Accept(this);
          std::swap(state_, curr_state_ptr);
          if (result_.value == false) {
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
      result_.value = false;
      double beta = params_.beta / unique_post_states.size();
      std::swap(params_.beta, beta);
      for (const State& state : unique_post_states) {
        const State* curr_state_ptr = &state;
        std::swap(state_, curr_state_ptr);
        path_property.pre_property().Accept(this);
        std::swap(state_, curr_state_ptr);
        if (result_.value == true) {
          break;
        }
      }
      std::swap(params_.beta, beta);
    }
  }
  if (VLOG_IS_ON(3) && probabilistic_level_ == 1) {
    LOG(INFO) << "t = " << t << ": " << StateToString(curr_state);
    if (result_.value) {
      LOG(INFO) << ">>positive sample";
    } else {
      LOG(INFO) << ">>negative sample";
    }
  }
  result_.path_length = path_length;
  result_.early_termination = early_termination;
}

template <typename OutputIterator>
bool SamplingVerifier::VerifyHelper(const CompiledProperty& property,
                                    const Optional<BDD>& ddf,
                                    bool default_result,
                                    OutputIterator* state_inserter) {
  if (dd_model_ != nullptr) {
    return ddf.value().ValueInState(state_->values(), model_->variables());
  } else if (!property.is_probabilistic()) {
    property.Accept(this);
    return result_.value;
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

SamplingVerifier::ResultQueue::ResultQueue()
    : push_count_(0), pop_count_(0), enabled_(true) {}

bool SamplingVerifier::ResultQueue::Enabled() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return enabled_;
}

void SamplingVerifier::ResultQueue::Push(const Result& result) {
  ++push_count_;
  std::unique_lock<std::mutex> lock(mutex_);
  results_.push(result);
  lock.unlock();
  cv_.notify_one();
}

SamplingVerifier::Result SamplingVerifier::ResultQueue::Pop() {
  ++pop_count_;
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [this] { return !results_.empty(); });
  Result result = results_.front();
  results_.pop();
  return result;
}

void SamplingVerifier::ResultQueue::Disable() {
  std::lock_guard<std::mutex> lock(mutex_);
  enabled_ = false;
}

}  // namespace

bool Verify(const CompiledProperty& property, const CompiledModel& model,
            const DecisionDiagramModel* dd_model,
            const ModelCheckingParams& params, const State& state,
            std::vector<CompiledExpressionEvaluator>* evaluators,
            std::vector<CompiledDistributionSampler<std::mt19937_64>>* samplers,
            ModelCheckingStats* stats) {
  DdCache dd_cache;
  std::vector<NextStateSampler<std::mt19937_64>> simulators;
  simulators.reserve(evaluators->size());
  for (size_t i = 0; i < evaluators->size(); ++i) {
    simulators.emplace_back(&model, &(*evaluators)[i], &(*samplers)[i]);
  }
  SamplingVerifier verifier(&model, dd_model, &dd_cache, stats, params, &state,
                            evaluators, samplers, &simulators);
  property.Accept(&verifier);
  stats->sample_cache_size.AddObservation(verifier.GetSampleCacheSize());
  return verifier.result();
}
