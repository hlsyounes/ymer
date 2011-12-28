#include <sys/time.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <utility>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "compiled-model.h"
#include "expression.h"
#include "model.h"
#include "parse.h"

template <typename RandomNumberEngine>
class DTMCSimulator {
 public:
  DTMCSimulator(const CompiledModel* model, RandomNumberEngine* engine);

  bool NextState(const CompiledState& state, CompiledState* next_state) const;

 private:
  const CompiledModel* model_;
  RandomNumberEngine* engine_;
};

template <typename RandomNumberEngine>
DTMCSimulator<RandomNumberEngine>::DTMCSimulator(const CompiledModel* model,
                                                 RandomNumberEngine* engine)
    : model_(model), engine_(engine) {
  CHECK(model_);
  CHECK(engine_);
}

template <typename RandomNumberEngine>
class DTMCNextStateSampler {
 public:
  DTMCNextStateSampler(const CompiledModel* model, RandomNumberEngine* engine);

  bool NextState(const CompiledState& state, CompiledState* next_state);

 private:
  void GetCommand();

  void SampleSynchronizedCommands(int action, int module);

  void ConsiderCandidateCommands();

  void GetOutcome();

  const CompiledModel* model_;
  RandomNumberEngine* engine_;
  const CompiledState* state_;
  int num_enabled_;
  std::vector<const CompiledCommand*> candidate_commands_;
  std::vector<const CompiledCommand*> selected_commands_;
  std::vector<const CompiledOutcome*> selected_outcomes_;
};

template <typename RandomNumberEngine>
DTMCNextStateSampler<RandomNumberEngine>::DTMCNextStateSampler(
    const CompiledModel* model, RandomNumberEngine* engine)
    : model_(model), engine_(engine) {
}

template <typename RandomNumberEngine>
bool DTMCNextStateSampler<RandomNumberEngine>::NextState(
    const CompiledState& state, CompiledState* next_state) {
  state_ = &state;
  GetCommand();
  if (selected_commands_.empty()) {
    return false;
  }
  GetOutcome();
  *next_state = state;
  for (const CompiledOutcome* outcome: selected_outcomes_) {
    for (const CompiledUpdate& update: outcome->updates()) {
      (*next_state)[update.variable()] = update.expr().ValueInState(state);
    }
  }
  return true;
}

template <typename RandomNumberEngine>
void DTMCNextStateSampler<RandomNumberEngine>::GetCommand() {
  num_enabled_ = 0;
  for (const CompiledCommand& command: model_->commands_without_action()) {
    if (command.guard().ValueInState(*state_)) {
      candidate_commands_.push_back(&command);
      ConsiderCandidateCommands();
      candidate_commands_.pop_back();
    }
  }
  const int num_actions = model_->num_actions();
  for (int i = 0; i < num_actions; ++i) {
    if (!model_->commands_with_action(i).empty()) {
      SampleSynchronizedCommands(i, 0);
    }
  }
}

template <typename RandomNumberEngine>
void DTMCNextStateSampler<RandomNumberEngine>::SampleSynchronizedCommands(
    int action, int module) {
  const std::vector<std::vector<CompiledCommand> >& commands_per_module =
      model_->commands_with_action(action);
  if (module == commands_per_module.size()) {
    ConsiderCandidateCommands();
    return;
  }
  VLOG(2) << "SampleSynchronizedCommands(" << action << ", " << module << ")";
  for (const CompiledCommand& command: commands_per_module[module]) {
    if (command.guard().ValueInState(*state_)) {
      candidate_commands_.push_back(&command);
      SampleSynchronizedCommands(action, module + 1);
      candidate_commands_.pop_back();
    }
  }
}

template <typename RandomNumberEngine>
void DTMCNextStateSampler<RandomNumberEngine>::ConsiderCandidateCommands() {
  ++num_enabled_;
  if (num_enabled_ == 1 ||
      std::uniform_int_distribution<>(1, num_enabled_)(*engine_) <= 1) {
    VLOG(2) << "select enabled command " << num_enabled_;
    selected_commands_ = candidate_commands_;
  } else {
    VLOG(2) << "reject enabled command " << num_enabled_;
  }
}

template <typename RandomNumberEngine>
void DTMCNextStateSampler<RandomNumberEngine>::GetOutcome() {
  selected_outcomes_.resize(selected_commands_.size());
  for (int i = 0; i < selected_commands_.size(); ++i) {
    VLOG(2) << "SampleOutcomes(" << i << ")";
    const CompiledCommand& command = *selected_commands_[i];
    double selected_key = -std::numeric_limits<double>::infinity();
    for (const CompiledOutcome& outcome: command.outcomes()) {
      const double p = outcome.probability().ValueInState(*state_);
      const double key =
          log(1.0 - std::uniform_real_distribution<>(0, 1)(*engine_)) / p;
      if (key > selected_key) {
        VLOG(2) << "select outcome with probability " << p
                << " (key = " << key << ")";
        selected_outcomes_[i] = &outcome;
        selected_key = key;
      } else {
        VLOG(2) << "reject outcome with probability " << p
                << " (key = " << key << ")";
      }
    }
  }
}

template <typename RandomNumberEngine>
bool DTMCSimulator<RandomNumberEngine>::NextState(
    const CompiledState& state, CompiledState* next_state) const {
  return DTMCNextStateSampler<RandomNumberEngine>(model_, engine_).NextState(
      state, next_state);
}

template <typename RandomNumberEngine>
class CTMCSimulator {
 public:
  CTMCSimulator(const CompiledModel* model, RandomNumberEngine* engine);

  bool NextState(const CompiledState& state, CompiledState* next_state) const;

 private:
  const CompiledModel* model_;
  RandomNumberEngine* engine_;
};

template <typename RandomNumberEngine>
CTMCSimulator<RandomNumberEngine>::CTMCSimulator(const CompiledModel* model,
                                                 RandomNumberEngine* engine)
    : model_(model), engine_(engine) {
  CHECK(model_);
  CHECK(engine_);
}

template <typename RandomNumberEngine>
class CTMCNextStateSampler {
 public:
  CTMCNextStateSampler(const CompiledModel* model, RandomNumberEngine* engine);

  bool NextState(const CompiledState& state, CompiledState* next_state);

 private:
  void GetCommand();

  void SampleSynchronizedCommands(int action, int module, double factor);

  void ConsiderCandidateCommands(double weight);

  void GetOutcome();

  const CompiledModel* model_;
  RandomNumberEngine* engine_;
  const CompiledState* state_;
  double weight_sum_;
  std::vector<const CompiledCommand*> candidate_commands_;
  std::vector<const CompiledCommand*> selected_commands_;
  double selected_key_;
  std::vector<const CompiledOutcome*> selected_outcomes_;
};

template <typename RandomNumberEngine>
CTMCNextStateSampler<RandomNumberEngine>::CTMCNextStateSampler(
    const CompiledModel* model, RandomNumberEngine* engine)
    : model_(model), engine_(engine) {
}

template <typename RandomNumberEngine>
bool CTMCNextStateSampler<RandomNumberEngine>::NextState(
    const CompiledState& state, CompiledState* next_state) {
  state_ = &state;
  GetCommand();
  if (selected_commands_.empty()) {
    return false;
  }
  GetOutcome();
  *next_state = state;
  for (const CompiledOutcome* outcome: selected_outcomes_) {
    for (const CompiledUpdate& update: outcome->updates()) {
      (*next_state)[update.variable()] = update.expr().ValueInState(state);
    }
  }
  return true;
}

double GetWeight(
    const CompiledCommand& command, double factor, const CompiledState& state) {
  double weight = 0.0;
  for (const CompiledOutcome& outcome: command.outcomes()) {
    weight += factor * outcome.probability().ValueInState(state);
  }
  return weight;
}

template <typename RandomNumberEngine>
void CTMCNextStateSampler<RandomNumberEngine>::GetCommand() {
  weight_sum_ = 0.0;
  selected_key_ = -std::numeric_limits<double>::infinity();
  for (const CompiledCommand& command: model_->commands_without_action()) {
    if (command.guard().ValueInState(*state_)) {
      candidate_commands_.push_back(&command);
      ConsiderCandidateCommands(GetWeight(command, 1.0, *state_));
      candidate_commands_.pop_back();
    }
  }
  const int num_actions = model_->num_actions();
  for (int i = 0; i < num_actions; ++i) {
    if (!model_->commands_with_action(i).empty()) {
      SampleSynchronizedCommands(i, 0, 1.0);
    }
  }
}

template <typename RandomNumberEngine>
void CTMCNextStateSampler<RandomNumberEngine>::SampleSynchronizedCommands(
    int action, int module, double factor) {
  const std::vector<std::vector<CompiledCommand> >& commands_per_module =
      model_->commands_with_action(action);
  if (module == commands_per_module.size()) {
    ConsiderCandidateCommands(factor);
    return;
  }
  VLOG(2) << "SampleSynchronizedCommands(" << action << ", " << module << ", "
          << factor << ")";
  for (const CompiledCommand& command: commands_per_module[module]) {
    if (command.guard().ValueInState(*state_)) {
      candidate_commands_.push_back(&command);
      const double weight = GetWeight(command, factor, *state_);
      SampleSynchronizedCommands(action, module + 1, weight);
      candidate_commands_.pop_back();
    }
  }
}

template <typename RandomNumberEngine>
void CTMCNextStateSampler<RandomNumberEngine>::ConsiderCandidateCommands(
    double weight) {
  weight_sum_ += weight;
  double key =
      log(1.0 - std::uniform_real_distribution<>(0, 1)(*engine_)) / weight;
  if (key > selected_key_) {
    VLOG(2) << "select enabled command " << weight << " (" << key << ")";
    selected_commands_ = candidate_commands_;
    selected_key_ = key;
  } else {
    VLOG(2) << "relect enabled command " << weight << " (" << key << ")";
  }
}

template <typename RandomNumberEngine>
void CTMCNextStateSampler<RandomNumberEngine>::GetOutcome() {
  selected_outcomes_.resize(selected_commands_.size());
  for (int i = 0; i < selected_commands_.size(); ++i) {
    VLOG(2) << "SampleOutcomes(" << i << ")";
    const CompiledCommand& command = *selected_commands_[i];
    double selected_key = -std::numeric_limits<double>::infinity();
    for (const CompiledOutcome& outcome: command.outcomes()) {
      const double p = outcome.probability().ValueInState(*state_);
      const double key =
          log(1.0 - std::uniform_real_distribution<>(0, 1)(*engine_)) / p;
      if (key > selected_key) {
        VLOG(2) << "select outcome with rate " << p << " (key = " << key << ")";
        selected_outcomes_[i] = &outcome;
        selected_key = key;
      } else {
        VLOG(2) << "reject outcome with rate " << p << " (key = " << key << ")";
      }
    }
  }
}

template <typename RandomNumberEngine>
bool CTMCSimulator<RandomNumberEngine>::NextState(
    const CompiledState& state, CompiledState* next_state) const {
  return CTMCNextStateSampler<RandomNumberEngine>(model_, engine_).NextState(
      state, next_state);
}

DEFINE_string(const, "", "Value assignments for constants.");
DEFINE_int32(simsamples, 10000, "Sample size.");
DEFINE_int32(simpathlen, 1000, "Sample path length.");

namespace {

// Parses spec for const overrides.  Returns true on success.
bool ParseConstOverrides(
    const std::string& spec,
    std::map<std::string,
             std::unique_ptr<const ParsedExpression> >* const_overrides) {
  if (spec.empty()) {
    return true;
  }
  auto comma = spec.begin() - 1;
  while (comma != spec.end()) {
    auto next_comma = find(comma + 1, spec.end(), ',');
    auto assignment = find(comma + 1, next_comma, '=');
    if (assignment == next_comma) {
      return false;
    }
    const std::string name(comma + 1, assignment);
    const std::string value(assignment + 1, next_comma);
    std::unique_ptr<const ParsedExpression> expr;
    if (value == "true" || value == "false") {
      expr = ParsedLiteral::Create(value == "true");
    } else {
      char* end;
      int int_value = strtol(value.c_str(), &end, 10);
      if (*end == '\0' && end != value.c_str()) {
        expr = ParsedLiteral::Create(int_value);
      } else {
        double double_value = strtod(value.c_str(), &end);
        if (*end == '\0' && end != value.c_str()) {
          expr = ParsedLiteral::Create(double_value);
        } else {
          return false;
        }
      }
    }
    CHECK(expr);
    if (!const_overrides->insert(
            std::make_pair(name, std::move(expr))).second) {
      return false;
    }
    comma = next_comma;
  }
  return true;
}

std::string StateString(const CompiledState& state) {
  std::ostringstream out;
  for (const TypedValue& v: state) {
    out << " " << v;
  }
  return out.str();
}

double GetCurrentTimeSeconds() {
  timeval tv;
  gettimeofday(&tv, nullptr);
  return tv.tv_sec + 1e-6*tv.tv_usec;
}

}  // namespace

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);

  double model_start_time = GetCurrentTimeSeconds();
  std::string filename = (argc > 1) ? argv[1] : "-";
  Model parsed_model;
  std::string message;
  bool success = ParseFile(filename, &parsed_model, &message);
  if (!message.empty()) {
    fprintf(stderr, "%s\n", message.c_str());
  }
  if (!success) {
    return 1;
  }
  std::map<std::string,
      std::unique_ptr<const ParsedExpression> > owned_const_overrides;
  if (!ParseConstOverrides(FLAGS_const, &owned_const_overrides)) {
    fprintf(stderr, "bad --const specification '%s'\n", FLAGS_const.c_str());
    return 1;
  }
  std::map<std::string, const ParsedExpression*> const_overrides;
  for (const auto& p: owned_const_overrides) {
    const_overrides.insert({ p.first, p.second.get() });
  }
  std::string error;
  CompiledModel compiled_model =
      CompiledModel::Make(parsed_model, const_overrides, &error);
  if (!compiled_model.is_valid()) {
    fprintf(stderr, "%s\n", error.c_str());
    return 1;
  }
  double model_end_time = GetCurrentTimeSeconds();
  printf("Compilation time: %g\n", model_end_time - model_start_time);

  double simulation_start_time = GetCurrentTimeSeconds();
  std::mt19937 engine;
  const int sample_size = FLAGS_simsamples;
  const int path_length = FLAGS_simpathlen;
  if (compiled_model.init()) {
    fprintf(stderr, "Simulator ignores init expression.\n");
  }
  if (compiled_model.model_type() == ModelType::DTMC) {
    DTMCSimulator<std::mt19937> simulator(&compiled_model, &engine);
    CompiledState next_state;
    for (int k = 1; k <= sample_size; ++k) {
      CompiledState state(compiled_model.num_variables());
      for (int i = 0; i < compiled_model.num_variables(); ++i) {
        state[i] = compiled_model.variable_info(i).init_value.value<int>();
      }
      if (VLOG_IS_ON(1)) {
        LOG(INFO) << "state:" << StateString(state);
      }
      for (int i = 1; i <= path_length; ++i) {
        if (simulator.NextState(state, &next_state)) {
          swap(state, next_state);
          if (VLOG_IS_ON(1)) {
            LOG(INFO) << "state:" << StateString(state);
          }
        } else {
          VLOG(1) << "dead end at step " << i;
          break;
        }
      }
    }
  } else if (compiled_model.model_type() == ModelType::CTMC) {
    CTMCSimulator<std::mt19937> simulator(&compiled_model, &engine);
    for (int k = 1; k <= sample_size; ++k) {
      CompiledState state(compiled_model.num_variables());
      for (int i = 0; i < compiled_model.num_variables(); ++i) {
        state[i] = compiled_model.variable_info(i).init_value.value<int>();
      }
      if (VLOG_IS_ON(1)) {
        LOG(INFO) << "state:" << StateString(state);
      }
      CompiledState next_state;
      for (int i = 1; i <= path_length; ++i) {
        if (simulator.NextState(state, &next_state)) {
          swap(state, next_state);
          if (VLOG_IS_ON(1)) {
            LOG(INFO) << "state:" << StateString(state);
          }
        } else {
          VLOG(1) << "dead end at step " << i;
          break;
        }
      }
    }
  } else {
    fprintf(stderr, "Simulator does not support the given model type.\n");
    return 1;
  }
  double simulation_end_time = GetCurrentTimeSeconds();
  printf("Simulation time: %g\n", simulation_end_time - simulation_start_time);

  return 0;
}
