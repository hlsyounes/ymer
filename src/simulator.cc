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

// TODO(hlsyounes): Hack to avoid bug in GCC 4.7.0 (trunk).
#ifdef NDEBUG
#undef DVLOG
#define DVLOG(verbositylevel) (true) ? (void) 0 : google::LogMessageVoidify() & LOG(INFO)
#endif

template <ModelType type, typename RandomNumberEngine>
class NextStateSampler {
 public:
  NextStateSampler(const CompiledModel* model, RandomNumberEngine* engine);

  bool NextState(const CompiledState& state, CompiledState* next_state);

 private:
  void GetCommand(const CompiledState& state);

  void SampleSynchronizedCommands(
      const CompiledState& state, int action, int module, double factor);

  void ConsiderCandidateCommands(double weight);

  void GetOutcome(const CompiledState& state);

  const CompiledModel* const model_;
  RandomNumberEngine* const engine_;
  double weight_sum_;
  std::vector<const CompiledCommand*> candidate_commands_;
  std::vector<const CompiledCommand*> selected_commands_;
  double selected_commands_key_;
  std::vector<const CompiledOutcome*> selected_outcomes_;
};

template <ModelType type, typename RandomNumberEngine>
NextStateSampler<type, RandomNumberEngine>::NextStateSampler(
    const CompiledModel* model, RandomNumberEngine* engine)
    : model_(model), engine_(engine) {
}

template <ModelType type, typename RandomNumberEngine>
bool NextStateSampler<type, RandomNumberEngine>::NextState(
    const CompiledState& state, CompiledState* next_state) {
  GetCommand(state);
  if (selected_commands_.empty()) {
    return false;
  }
  GetOutcome(state);
  *next_state = state;
  for (const CompiledOutcome* outcome: selected_outcomes_) {
    for (const CompiledUpdate& update: outcome->updates()) {
      (*next_state)[update.variable()] = update.expr().ValueInState(state);
    }
  }
  return true;
}

template <ModelType type>
class CommandWeight {
 public:
  static double Get(const CompiledState& state,
                    const CompiledCommand& command,
                    double factor);
};

template <>
double CommandWeight<ModelType::DTMC>::Get(
    const CompiledState& state, const CompiledCommand& command, double factor) {
  return 1.0;
}

template <>
double CommandWeight<ModelType::CTMC>::Get(
    const CompiledState& state, const CompiledCommand& command, double factor) {
  double weight = 0.0;
  for (const CompiledOutcome& outcome: command.outcomes()) {
    weight += factor * outcome.probability().ValueInState(state);
  }
  return weight;
}

template <ModelType type, typename RandomNumberEngine>
void NextStateSampler<type, RandomNumberEngine>::GetCommand(
    const CompiledState& state) {
  weight_sum_ = 0.0;
  selected_commands_.clear();
  selected_commands_key_ = -std::numeric_limits<double>::infinity();
  for (const CompiledCommand& command: model_->commands_without_action()) {
    if (command.guard().ValueInState(state)) {
      candidate_commands_.push_back(&command);
      ConsiderCandidateCommands(CommandWeight<type>::Get(state, command, 1.0));
      candidate_commands_.pop_back();
    }
  }
  const int num_actions = model_->num_actions();
  for (int i = 0; i < num_actions; ++i) {
    if (!model_->commands_with_action(i).empty()) {
      SampleSynchronizedCommands(state, i, 0, 1.0);
    }
  }
}

template <ModelType type, typename RandomNumberEngine>
void NextStateSampler<type, RandomNumberEngine>::SampleSynchronizedCommands(
    const CompiledState& state, int action, int module, double factor) {
  const std::vector<std::vector<CompiledCommand> >& commands_per_module =
      model_->commands_with_action(action);
  if (module == commands_per_module.size()) {
    ConsiderCandidateCommands(factor);
    return;
  }
  DVLOG(2) << "SampleSynchronizedCommands(" << action << ", " << module << ", "
           << factor << ")";
  for (const CompiledCommand& command: commands_per_module[module]) {
    if (command.guard().ValueInState(state)) {
      candidate_commands_.push_back(&command);
      SampleSynchronizedCommands(
          state, action, module + 1,
          CommandWeight<type>::Get(state, command, factor));
      candidate_commands_.pop_back();
    }
  }
}

template <ModelType type, typename RandomNumberEngine>
void NextStateSampler<type, RandomNumberEngine>::ConsiderCandidateCommands(
    double weight) {
  weight_sum_ += weight;
  double key =
      log(1.0 - std::uniform_real_distribution<>(0, 1)(*engine_)) / weight;
  if (key > selected_commands_key_) {
    DVLOG(2) << "select enabled command weight=" << weight
             << " (" << key << ")";
    selected_commands_ = candidate_commands_;
    selected_commands_key_ = key;
  } else {
    DVLOG(2) << "relect enabled command weight=" << weight
             << " (" << key << ")";
  }
}

template <ModelType type, typename RandomNumberEngine>
void NextStateSampler<type, RandomNumberEngine>::GetOutcome(
    const CompiledState& state) {
  selected_outcomes_.resize(selected_commands_.size());
  DVLOG(2) << "SampleOutcomes()";
  for (int i = 0; i < selected_commands_.size(); ++i) {
    const CompiledCommand& command = *selected_commands_[i];
    double selected_key = -std::numeric_limits<double>::infinity();
    for (const CompiledOutcome& outcome: command.outcomes()) {
      const double p = outcome.probability().ValueInState(state);
      const double key =
          log(1.0 - std::uniform_real_distribution<>(0, 1)(*engine_)) / p;
      if (key > selected_key) {
        DVLOG(2) << i << ": select outcome with p=" << p << " (" << key << ")";
        selected_outcomes_[i] = &outcome;
        selected_key = key;
      } else {
        DVLOG(2) << i << ": reject outcome with p=" << p << " (" << key << ")";
      }
    }
  }
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

template <typename RandomNumberEngine>
class Simulator {
 public:
  Simulator(const CompiledModel* model, RandomNumberEngine* engine);

  void GeneratePath(const CompiledState& init_state, int max_length) const;

 private:
  template <ModelType type>
  void GeneratePathImpl(const CompiledState& init_state, int max_length) const;

  const CompiledModel* const model_;
  RandomNumberEngine* const engine_;
};

template <typename RandomNumberEngine>
Simulator<RandomNumberEngine>::Simulator(const CompiledModel* model,
                                         RandomNumberEngine* engine)
    : model_(model), engine_(engine) {
  CHECK(model_);
  CHECK(engine_);
}

template <typename RandomNumberEngine>
void Simulator<RandomNumberEngine>::GeneratePath(
    const CompiledState& init_state, int max_length) const {
  switch (model_->model_type()) {
    case ModelType::DTMC:
      GeneratePathImpl<ModelType::DTMC>(init_state, max_length);
      break;
    case ModelType::CTMC:
      GeneratePathImpl<ModelType::CTMC>(init_state, max_length);
      break;
    case ModelType::MDP:
      LOG(FATAL) << "unsupported model type for Simulator";
  }
}

template <typename RandomNumberEngine>
template <ModelType type>
void Simulator<RandomNumberEngine>::GeneratePathImpl(
    const CompiledState& init_state, int max_length) const {
  NextStateSampler<type, RandomNumberEngine> next_state_sampler(model_,
                                                                engine_);
  CompiledState state = init_state;
  DVLOG(1) << "state:" << StateString(state);
  CompiledState next_state;
  for (int i = 1; i <= max_length; ++i) {
    if (next_state_sampler.NextState(state, &next_state)) {
      swap(state, next_state);
      DVLOG(1) << "state:" << StateString(state);
    } else {
      DVLOG(1) << "dead end at step " << i;
      break;
    }
  }
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
  CompiledState init_state(compiled_model.num_variables());
  for (int i = 0; i < compiled_model.num_variables(); ++i) {
    init_state[i] = compiled_model.variable_info(i).init_value.value<int>();
  }
  for (int k = 1; k <= sample_size; ++k) {
    Simulator<std::mt19937> simulator(&compiled_model, &engine);
    simulator.GeneratePath(init_state, path_length);
  }
  double simulation_end_time = GetCurrentTimeSeconds();
  printf("Simulation time: %g\n", simulation_end_time - simulation_start_time);

  return 0;
}
