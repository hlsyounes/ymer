/*
 * Main program.
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
#include <config.h>
#include "comm.h"
#include "models.h"
#include "formulas.h"
#include "parser-state.h"
#include "src/compiled-property.h"
#include "src/ddutil.h"
#include "src/distribution.h"
#include "src/rng.h"
#include "src/simulator.h"
#include "src/strutil.h"
#include "src/typed-value.h"
#include "glog/logging.h"
#include <cudd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_GETOPT_LONG
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>
#endif
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

struct yy_buffer_state;

// Init function for lexical analyzer (scanner).
extern int yylex_init(void** scanner_ptr);
// Destroy function for lexical analyzer (scanner).
extern int yylex_destroy(void* scanner);
// Creates a scanner buffer for the given file.
extern yy_buffer_state* yy_create_buffer(FILE* file, int size, void* scanner);
// Switches to the given scanner buffer.
extern void yy_switch_to_buffer(yy_buffer_state* buf, void* scanner);
// Deletes a scanner buffer.
extern void yy_delete_buffer(yy_buffer_state* buf, void* scanner);
// Parse function.
extern int yyparse(void* scanner, ParserState* state);

/* Sockets for communication. */
int server_socket = -1;
/* Current property. */
int current_property;

/* Program options. */
static option long_options[] = {
    {"alpha", required_argument, 0, 'A'},
    {"beta", required_argument, 0, 'B'},
    {"delta", required_argument, 0, 'D'},
    {"epsilon", required_argument, 0, 'E'},
    {"const", required_argument, 0, 'c'},
    {"engine", required_argument, 0, 'e'},
    {"host", required_argument, 0, 'H'},
    {"max-path-length", required_argument, 0, 'L'},
    {"memoization", no_argument, 0, 'M'},
    {"matching-moments", required_argument, 0, 'm'},
    {"fixed-sample-size", required_argument, 0, 'N'},
    {"nested-error", required_argument, 0, 'n'},
    {"estimate-probabilities", no_argument, 0, 'p'},
    {"port", required_argument, 0, 'P'},
    {"sampling-algorithm", required_argument, 0, 's'},
    {"seed", required_argument, 0, 'S'},
    {"trials", required_argument, 0, 'T'},
    {"version", no_argument, 0, 'V'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}};
static const char OPTION_STRING[] = "A:B:c:D:E:e:H:hL:Mm:N:n:pP:s:S:T:V";

namespace {

/* Displays help. */
void display_help() {
  std::cout << "usage: " << PACKAGE << " [options] [file ...]" << std::endl
            << "options:" << std::endl << "  -A a,  --alpha=a\t"
            << "use bound a on false negatives with sampling engine"
            << std::endl << "\t\t\t  (default is 1e-2)" << std::endl
            << "  -B b,  --beta=b\t"
            << "use bound b on false positives with sampling engine"
            << std::endl << "\t\t\t  (default is 1e-2)" << std::endl
            << "  -c c,  --const=c\t"
            << "overrides for model constants" << std::endl
            << "\t\t\t  (for example, --const=N=2,M=3)" << std::endl
            << "  -D d,  --delta=d\t"
            << "use indifference region of width 2*d with sampling" << std::endl
            << "\t\t\t  engine (default is 1e-2)" << std::endl
            << "  -E e,  --epsilon=e\t"
            << "use precision e with hybrid engine (default is 1e-6)"
            << std::endl << "  -e e,  --engine=e\t"
            << "use engine e; can be `sampling' (default), `hybrid',"
            << std::endl << "\t\t\t  or `mixed'" << std::endl
            << "  -H h,  --host=h\t"
            << "connect to server on host h" << std::endl
            << "  -L l,  --max_path-length=l" << std::endl
            << "\t\t\tlimit sample path to l states" << std::endl
            << "  -M,    --memoization\t"
            << "use memoization for sampling engine" << std::endl
            << "  -m m,  --matching-moments=m" << std::endl
            << "\t\t\tmatch the first m moments of general distributions"
            << std::endl << "  -N n,  --fixed-sample-size=n" << std::endl
            << "\t\t\tuse a fixed sample size" << std::endl
            << "  -p,    --estimate-probabilities" << std::endl
            << "\t\t\testimates probabilities of path formulae holding"
            << std::endl << "  -P p,  --port=p\t"
            << "communicate using port p" << std::endl
            << "  -s s,  --sampling-algorithm=s" << std::endl
            << "\t\t\tuse sampling algorithm s" << std::endl
            << "  -S s,  --seed=s\t"
            << "use seed s with random number generator" << std::endl
            << "\t\t\t  (sampling engine only)" << std::endl
            << "  -T t,  --trials=t\t"
            << "number of trials for sampling engine (default is 1)"
            << std::endl
            << "\t\t\t  default level is 1 if optional argument is left out"
            << std::endl << "  -V,    --version\t"
            << "display version information and exit" << std::endl
            << "  -h,    --help\t\t"
            << "display this help and exit" << std::endl << "  file ...\t\t"
            << "files containing models and properties;" << std::endl
            << "\t\t\t  if none, descriptions are read from standard input"
            << std::endl << std::endl
            << "Report bugs to <" PACKAGE_BUGREPORT ">." << std::endl;
}

/* Displays version information. */
void display_version() {
  std::cout << PACKAGE_STRING << std::endl
            << "Copyright (C) 2003--2005 Carnegie Mellon University"
            << std::endl << "Copyright (C) 2011--2012 Google Inc" << std::endl
            << PACKAGE_NAME
            << " comes with NO WARRANTY, to the extent permitted by law."
            << std::endl << "For information about the terms of redistribution,"
            << std::endl << "see the file named COPYING in the " PACKAGE_NAME
            << " distribution." << std::endl << std::endl
            << "Written by Haakan Younes." << std::endl;
}

/* Parses spec for const overrides.  Returns true on success. */
bool parse_const_overrides(const std::string& spec,
                           std::map<std::string, TypedValue>* const_overrides) {
  if (spec.empty()) {
    return true;
  }
  std::string::const_iterator comma = spec.begin() - 1;
  while (comma != spec.end()) {
    std::string::const_iterator next_comma = find(comma + 1, spec.end(), ',');
    std::string::const_iterator assignment = find(comma + 1, next_comma, '=');
    if (assignment == next_comma) {
      return false;
    }
    const std::string name(comma + 1, assignment);
    const std::string value(assignment + 1, next_comma);
    char* endptr;
    TypedValue v = static_cast<int>(strtol(value.c_str(), &endptr, 10));
    if (*endptr != '\0') {
      v = strtod(value.c_str(), &endptr);
    }
    if (endptr == value.c_str()) {
      return false;
    }
    if (!const_overrides->insert(std::make_pair(name, v)).second) {
      return false;
    }
    comma = next_comma;
  }
  return true;
}

struct ModelAndProperties {
  std::unique_ptr<Model> model;
  UniquePtrVector<const Expression> properties;
};

// Prepares a scanner to scan the given file.
yy_buffer_state* PrepareFileScan(FILE* file, void* scanner) {
  yy_buffer_state* buffer_state = yy_create_buffer(file, 32768, scanner);
  yy_switch_to_buffer(buffer_state, scanner);
  return buffer_state;
}

/* Parses the given file, and returns true on success. */
bool read_file(const std::string& filename, ModelAndProperties* result,
               std::vector<std::string>* errors) {
  FILE* file = (filename == "-") ? stdin : fopen(filename.c_str(), "r");
  if (file == nullptr) {
    errors->push_back(StrCat(filename, ":", strerror(errno)));
    return false;
  }

  void* scanner;
  yylex_init(&scanner);
  yy_buffer_state* buffer_state = PrepareFileScan(file, scanner);
  ParserState state(filename, errors);
  bool success = (yyparse(scanner, &state) == 0);
  if (success) {
    if (state.has_model()) {
      result->model = state.release_model();
    } else {
      result->properties = state.release_properties();
    }
  }
  yy_delete_buffer(buffer_state, scanner);
  yylex_destroy(scanner);

  if (file != stdin) {
    fclose(file);
  }
  return success;
}

CompiledExpression CompileAndOptimizeExpression(
    const Expression& expr, Type expected_type,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    std::vector<std::string>* errors) {
  CompileExpressionResult result =
      CompileExpression(expr, expected_type, identifiers_by_name);
  if (!result.errors.empty()) {
    errors->insert(errors->end(), result.errors.begin(), result.errors.end());
    return CompiledExpression({});
  }
  if (expected_type == Type::DOUBLE) {
    return OptimizeDoubleExpression(result.expr);
  } else {
    return OptimizeIntExpression(result.expr);
  }
}

class DistributionCompiler : public DistributionVisitor {
 public:
  DistributionCompiler(
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      std::vector<std::string>* errors);

  bool has_markov_weight() const {
    return !markov_weight_.operations().empty();
  }

  CompiledExpression markov_weight() const { return markov_weight_; }

  CompiledGsmpDistribution gsmp_delay() const { return gsmp_delay_; }

 private:
  void DoVisitMemoryless(const Memoryless& dist) override;
  void DoVisitWeibull(const Weibull& dist) override;
  void DoVisitLognormal(const Lognormal& dist) override;
  void DoVisitUniform(const Uniform& dist) override;

  double GetDoubleValue(const Expression& expr);

  CompiledExpression markov_weight_;
  CompiledGsmpDistribution gsmp_delay_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  std::vector<std::string>* errors_;
};

DistributionCompiler::DistributionCompiler(
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    std::vector<std::string>* errors)
    : markov_weight_({}),
      gsmp_delay_(CompiledGsmpDistribution::MakeWeibull(0, 0)),
      identifiers_by_name_(identifiers_by_name),
      errors_(errors) {
  CHECK(identifiers_by_name);
  CHECK(errors);
}

void DistributionCompiler::DoVisitMemoryless(const Memoryless& dist) {
  markov_weight_ = CompileAndOptimizeExpression(dist.weight(), Type::DOUBLE,
                                                *identifiers_by_name_, errors_);
}

double DistributionCompiler::GetDoubleValue(const Expression& expr) {
  CompiledExpression compiled_expr = CompileAndOptimizeExpression(
      expr, Type::DOUBLE, *identifiers_by_name_, errors_);
  if (errors_->empty()) {
    if (compiled_expr.operations().size() == 1 &&
        compiled_expr.operations()[0].opcode() == Opcode::DCONST &&
        compiled_expr.operations()[0].operand2() == 0) {
      return compiled_expr.operations()[0].doperand1();
    }
    errors_->push_back(StrCat("expecting constant expression; found ", expr));
  }
  return 0.0;
}

void DistributionCompiler::DoVisitWeibull(const Weibull& dist) {
  const double scale = GetDoubleValue(dist.scale());
  const double shape = GetDoubleValue(dist.shape());
  if (shape == 1.0 && errors_->empty()) {
    markov_weight_ =
        CompiledExpression({Operation::MakeDCONST(1.0 / scale, 0)});
  } else {
    markov_weight_ = CompiledExpression({});
    gsmp_delay_ = CompiledGsmpDistribution::MakeWeibull(scale, shape);
  }
}

void DistributionCompiler::DoVisitLognormal(const Lognormal& dist) {
  markov_weight_ = CompiledExpression({});
  gsmp_delay_ = CompiledGsmpDistribution::MakeLognormal(
      GetDoubleValue(dist.scale()), GetDoubleValue(dist.shape()));
}

void DistributionCompiler::DoVisitUniform(const Uniform& dist) {
  markov_weight_ = CompiledExpression({});
  gsmp_delay_ = CompiledGsmpDistribution::MakeUniform(
      GetDoubleValue(dist.low()), GetDoubleValue(dist.high()));
}

CompiledUpdate CompileUpdate(
    const Update& update,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    std::vector<std::string>* errors) {
  auto i = identifiers_by_name.find(update.variable());
  int variable;
  if (i == identifiers_by_name.end()) {
    errors->push_back(
        StrCat("undefined variable '", update.variable(), "' in update"));
    variable = -1;
  } else if (!i->second.is_variable()) {
    errors->push_back(StrCat("constant '", update.variable(), "' in update"));
    variable = -1;
  } else {
    variable = i->second.variable_index();
  }
  return CompiledUpdate(
      variable, CompileAndOptimizeExpression(update.expr(), Type::INT,
                                             identifiers_by_name, errors));
}

std::vector<CompiledUpdate> CompileUpdates(
    const std::vector<Update>& updates,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    std::vector<std::string>* errors) {
  std::vector<CompiledUpdate> compiled_updates;
  compiled_updates.reserve(updates.size());
  for (const auto& update : updates) {
    compiled_updates.push_back(
        CompileUpdate(update, identifiers_by_name, errors));
  }
  return compiled_updates;
}

std::map<std::string, IdentifierInfo> GetConstantIdentifiersByName(
    const std::vector<ParsedConstant>& constants,
    const std::map<std::string, TypedValue>& constant_overrides,
    std::vector<std::string>* errors) {
  std::map<std::string, IdentifierInfo> identifiers_by_name;
  std::map<std::string, TypedValue> constant_values = constant_overrides;
  ResolveConstants(constants, &constant_values, errors);
  for (const auto& entry : constant_values) {
    identifiers_by_name.emplace(entry.first,
                                IdentifierInfo::Constant(entry.second));
  }
  return identifiers_by_name;
}

void AddVariableIdentifiersByName(
    const CompiledModel& model,
    std::map<std::string, IdentifierInfo>* identifiers_by_name) {
  int index = 0;
  int low_bit = 0;
  for (const auto& v : model.variables()) {
    identifiers_by_name->emplace(
        v.name(),
        IdentifierInfo::Variable(model.variable_types()[index], index, low_bit,
                                 low_bit + v.bit_count() - 1, v.min_value()));
    ++index;
    low_bit += v.bit_count();
  }
}

CompiledModelType CompileModelType(ModelType model_type,
                                   std::vector<std::string>* errors) {
  switch (model_type) {
    case ModelType::DEFAULT:
    case ModelType::MDP:
      errors->push_back(
          StrCat(model_type, " not supported for model checking"));
      return CompiledModelType::DTMC;
    case ModelType::DTMC:
      return CompiledModelType::DTMC;
    case ModelType::CTMC:
      return CompiledModelType::CTMC;
    case ModelType::GSMP:
      return CompiledModelType::GSMP;
  }
  errors->push_back("bad model type");
  return CompiledModelType::CTMC;
}

int GetIntValue(
    const Expression& expr, Type expected_type,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    std::vector<std::string>* errors) {
  CompiledExpression compiled_expr = CompileAndOptimizeExpression(
      expr, expected_type, identifiers_by_name, errors);
  if (errors->empty()) {
    if (compiled_expr.operations().size() == 1 &&
        compiled_expr.operations()[0].opcode() == Opcode::ICONST &&
        compiled_expr.operations()[0].operand2() == 0) {
      return compiled_expr.operations()[0].ioperand1();
    }
    errors->push_back(StrCat("expecting constant expression; found ", expr));
  }
  return 0;
}

bool IsUnitWeight(const CompiledExpression& weight) {
  return weight.operations().size() == 1 &&
         weight.operations()[0].opcode() == Opcode::DCONST &&
         weight.operations()[0].operand2() == 0 &&
         weight.operations()[0].doperand1() == 1.0;
}

CompiledExpression ComposeWeightExpressions(BinaryOperator op,
                                            CompiledExpression expr1,
                                            CompiledExpression expr2) {
  std::vector<Operation> operations;
  operations.reserve(expr1.operations().size() + expr2.operations().size() + 1);
  operations.insert(operations.end(), expr1.operations().begin(),
                    expr1.operations().end());
  const int pc_shift = operations.size();
  const int reg_shift = 1;
  for (const Operation& o : expr2.operations()) {
    operations.push_back(o.Shift(pc_shift, reg_shift));
  }
  if (op == BinaryOperator::PLUS) {
    operations.push_back(Operation::MakeDADD(0, 1));
  } else if (op == BinaryOperator::MULTIPLY) {
    operations.push_back(Operation::MakeDMUL(0, 1));
  } else if (op == BinaryOperator::DIVIDE) {
    operations.push_back(Operation::MakeDDIV(0, 1));
  } else {
    LOG(FATAL) << "not implemented";
  }
  return OptimizeDoubleExpression(CompiledExpression(operations));
}

CompiledExpression ComposeGuardExpressions(CompiledExpression expr1,
                                           CompiledExpression expr2) {
  std::vector<Operation> operations;
  operations.reserve(expr1.operations().size() + expr2.operations().size() + 1);
  operations.insert(operations.end(), expr1.operations().begin(),
                    expr1.operations().end());
  const int pc_shift = operations.size() + 1;
  const int reg_shift = 0;
  operations.push_back(
      Operation::MakeIFFALSE(0, pc_shift + expr2.operations().size()));
  for (const Operation& o : expr2.operations()) {
    operations.push_back(o.Shift(pc_shift, reg_shift));
  }
  return CompiledExpression(operations);
}

struct PreCompiledCommands {
  std::vector<CompiledMarkovCommand> single_markov_commands;
  std::map<std::string, std::map<int, std::vector<CompiledMarkovCommand>>>
      factored_markov_commands;
  std::vector<CompiledGsmpCommand> single_gsmp_commands;
  std::map<std::string, std::map<int, std::vector<CompiledGsmpCommand>>>
      factored_gsmp_commands;
};

PreCompiledCommands PreCompileCommands(
    const Model& model,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    std::vector<std::string>* errors) {
  PreCompiledCommands result;
  DistributionCompiler dist_compiler(&identifiers_by_name, errors);
  std::vector<CompiledMarkovOutcome> markov_outcomes;
  for (size_t module_index = 0; module_index < model.modules().size();
       ++module_index) {
    const auto& module = model.modules()[module_index];
    for (const auto& command : module.commands()) {
      const auto compiled_guard = CompileAndOptimizeExpression(
          command.guard(), Type::BOOL, identifiers_by_name, errors);
      markov_outcomes.clear();
      for (const auto& outcome : command.outcomes()) {
        outcome.delay().Accept(&dist_compiler);
        const auto compiled_updates =
            CompileUpdates(outcome.updates(), identifiers_by_name, errors);
        if (dist_compiler.has_markov_weight()) {
          markov_outcomes.emplace_back(dist_compiler.markov_weight(),
                                       compiled_updates);
        } else {
          CompiledGsmpCommand compiled_command(
              compiled_guard, dist_compiler.gsmp_delay(), compiled_updates,
              result.single_gsmp_commands.size());
          if (command.action().empty()) {
            result.single_gsmp_commands.push_back(compiled_command);
          } else {
            result.factored_gsmp_commands[command.action()][module_index]
                .push_back(compiled_command);
          }
        }
      }
      if (!markov_outcomes.empty()) {
        CompiledExpression weight_sum = markov_outcomes[0].probability();
        for (size_t i = 1; i < markov_outcomes.size(); ++i) {
          weight_sum =
              ComposeWeightExpressions(BinaryOperator::PLUS, weight_sum,
                                       markov_outcomes[i].probability());
        }
        for (auto& markov_outcome : markov_outcomes) {
          markov_outcome = CompiledMarkovOutcome(
              ComposeWeightExpressions(BinaryOperator::DIVIDE,
                                       markov_outcome.probability(),
                                       weight_sum),
              markov_outcome.updates());
        }
        CompiledMarkovCommand compiled_command(compiled_guard, weight_sum,
                                               markov_outcomes);
        if (command.action().empty()) {
          result.single_markov_commands.push_back(compiled_command);
        } else {
          result.factored_markov_commands[command.action()][module_index]
              .push_back(compiled_command);
        }
      }
    }
  }
  return result;
}

struct CompiledCommands {
  std::vector<CompiledMarkovCommand> single_markov_commands;
  std::vector<std::vector<std::vector<CompiledMarkovCommand>>>
      factored_markov_commands;
  std::vector<CompiledGsmpCommand> single_gsmp_commands;
  std::vector<FactoredGsmpCommands> factored_gsmp_commands_;
};

bool IsSimpleComposition(
    const std::map<int, std::vector<CompiledMarkovCommand>>&
        factored_markov_commands,
    int excluded_module_index, int multi_command_module_limit) {
  int module_count = 0;
  int multi_command_module_count = 0;
  bool has_multi_outcome_command = false;
  for (const auto& entry : factored_markov_commands) {
    const auto& module_index = entry.first;
    if (module_index != excluded_module_index) {
      ++module_count;
      const auto& commands = entry.second;
      if (commands.size() > 1) {
        ++multi_command_module_count;
      }
      for (const auto& command : commands) {
        if (command.outcomes().size() > 1) {
          has_multi_outcome_command = true;
        }
      }
    }
  }
  if (has_multi_outcome_command &&
      (module_count > 1 || excluded_module_index >= 0)) {
    return false;
  }
  if (multi_command_module_count > multi_command_module_limit) {
    return false;
  }
  return true;
}

CompiledMarkovCommand ComposeMarkovCommands(
    const CompiledMarkovCommand& command1,
    const CompiledMarkovCommand& command2) {
  std::vector<CompiledMarkovOutcome> outcomes;
  outcomes.reserve(command1.outcomes().size() + command2.outcomes().size());
  std::vector<CompiledUpdate> updates;
  for (const auto& outcome1 : command1.outcomes()) {
    for (const auto& outcome2 : command2.outcomes()) {
      updates.clear();
      updates.insert(updates.end(), outcome1.updates().begin(),
                     outcome1.updates().end());
      updates.insert(updates.end(), outcome2.updates().begin(),
                     outcome2.updates().end());
      outcomes.emplace_back(ComposeWeightExpressions(BinaryOperator::MULTIPLY,
                                                     outcome1.probability(),
                                                     outcome2.probability()),
                            updates);
    }
  }
  return CompiledMarkovCommand(
      ComposeGuardExpressions(command1.guard(), command2.guard()),
      ComposeWeightExpressions(BinaryOperator::MULTIPLY, command1.weight(),
                               command2.weight()),
      outcomes);
}

std::vector<CompiledMarkovCommand> ComposeFactoredMarkovCommands(
    const std::map<int, std::vector<CompiledMarkovCommand>>&
        factored_markov_commands,
    int excluded_module_index) {
  std::vector<CompiledMarkovCommand> result;
  for (const auto& entry : factored_markov_commands) {
    const auto& module_index = entry.first;
    if (module_index != excluded_module_index) {
      const auto& commands = entry.second;
      if (result.empty()) {
        result = commands;
      } else {
        size_t n = result.size();
        for (size_t i = 1; i < commands.size(); ++i) {
          for (size_t j = 0; j < n; ++j) {
            result.push_back(ComposeMarkovCommands(result[j], commands[i]));
          }
        }
        for (size_t j = 0; j < n; ++j) {
          result[j] = ComposeMarkovCommands(result[j], commands[0]);
        }
      }
    }
  }
  return result;
}

CompiledCommands CompileCommands(
    const Model& model,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    std::vector<std::string>* errors) {
  CompiledCommands result;
  const PreCompiledCommands pre_compiled_commands =
      PreCompileCommands(model, identifiers_by_name, errors);
  result.single_markov_commands = pre_compiled_commands.single_markov_commands;
  result.single_gsmp_commands = pre_compiled_commands.single_gsmp_commands;

  std::set<std::string> actions;
  std::set<std::string> gsmp_actions;
  std::vector<CompiledUpdate> updates;
  for (const auto& entry : pre_compiled_commands.factored_gsmp_commands) {
    const auto& action = entry.first;
    gsmp_actions.insert(action);
    int gsmp_module_index = -1;
    for (const auto& module_entry : entry.second) {
      const auto& module_index = module_entry.first;
      if (gsmp_module_index < 0) {
        gsmp_module_index = module_index;
      } else if (gsmp_module_index != module_index) {
        errors->push_back(StrCat(
            "action ", action, " has GSMP commands in multiple modules: ",
            model.modules()[gsmp_module_index].name(), " and ",
            model.modules()[module_index].name()));
        return result;
      }
    }
    auto i = pre_compiled_commands.factored_markov_commands.find(action);
    const auto& gsmp_commands = entry.second.find(gsmp_module_index)->second;
    if (i == pre_compiled_commands.factored_markov_commands.end()) {
      for (const auto& gsmp_command : gsmp_commands) {
        result.single_gsmp_commands.emplace_back(
            gsmp_command.guard(), gsmp_command.delay(), gsmp_command.updates(),
            result.single_gsmp_commands.size());
      }
    } else if (IsSimpleComposition(i->second, gsmp_module_index,
                                   (gsmp_commands.size() > 1) ? 0 : 1)) {
      const auto composed_commands =
          ComposeFactoredMarkovCommands(i->second, gsmp_module_index);
      for (const auto& gsmp_command : gsmp_commands) {
        for (const auto& markov_command : composed_commands) {
          if (!IsUnitWeight(markov_command.weight())) {
            errors->push_back(StrCat("action ", action,
                                     " has GSMP command synchronized with "
                                     "command with non-unit weight ",
                                     markov_command.weight()));
            return result;
          }
          CHECK_EQ(markov_command.outcomes().size(), 1);
          const auto& outcome = markov_command.outcomes()[0];
          updates.clear();
          updates.insert(updates.end(), gsmp_command.updates().begin(),
                         gsmp_command.updates().end());
          updates.insert(updates.end(), outcome.updates().begin(),
                         outcome.updates().end());
          result.single_gsmp_commands.emplace_back(
              ComposeGuardExpressions(gsmp_command.guard(),
                                      markov_command.guard()),
              gsmp_command.delay(), updates,
              result.single_gsmp_commands.size());
        }
      }
    } else {
      // Process later.  We will need to add this to factored_gsmp_commands.
      actions.insert(action);
    }
  }
  for (const auto& entry : pre_compiled_commands.factored_markov_commands) {
    const auto& action = entry.first;
    if (gsmp_actions.find(action) == gsmp_actions.end()) {
      if (IsSimpleComposition(entry.second, -1, 1)) {
        const auto composed_commands =
            ComposeFactoredMarkovCommands(entry.second, -1);
        result.single_markov_commands.insert(
            result.single_markov_commands.end(), composed_commands.begin(),
            composed_commands.end());
      } else {
        // Process later.  We will need to add this to factored_markov_commands.
        actions.insert(action);
      }
    }
  }

  // TODO(hlsyounes): Add factored commands for actions.
  if (!actions.empty()) {
    errors->push_back("compilation for factored models not implemented");
  }
  return result;
}

CompiledModel CompileModel(
    const Model& model,
    std::map<std::string, IdentifierInfo>* identifiers_by_name,
    std::vector<std::string>* errors) {
  CompiledModel compiled_model(CompileModelType(model.type(), errors));

  for (const ParsedVariable& v : model.variables()) {
    int min = GetIntValue(v.min(), v.type(), *identifiers_by_name, errors);
    int max = GetIntValue(v.max(), v.type(), *identifiers_by_name, errors);
    int init = GetIntValue(v.init(), v.type(), *identifiers_by_name, errors);
    if (min >= max) {
      errors->push_back(StrCat("empty domain [", min, "..", max,
                               "] for variable ", v.name()));
      max = min + 1;
    } else if (init < min || init > max) {
      errors->push_back(StrCat("init ", init, " not in domain [", min, "..",
                               max, "] for variable ", v.name()));
    }
    compiled_model.AddVariable(v.name(), v.type(), min, max, init);
  }

  AddVariableIdentifiersByName(compiled_model, identifiers_by_name);
  const auto& compiled_commands =
      CompileCommands(model, *identifiers_by_name, errors);
  compiled_model.set_single_markov_commands(
      compiled_commands.single_markov_commands);
  compiled_model.set_factored_markov_commands(
      compiled_commands.factored_markov_commands);
  compiled_model.set_single_gsmp_commands(
      compiled_commands.single_gsmp_commands);

  return compiled_model;
}

std::unique_ptr<const CompiledProperty> CompileAndOptimizeProperty(
    const Expression& property,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const DecisionDiagramManager& dd_manager,
    std::vector<std::string>* errors) {
  CompilePropertyResult result =
      CompileProperty(property, identifiers_by_name, dd_manager);
  if (!result.errors.empty()) {
    errors->insert(errors->end(), result.errors.begin(), result.errors.end());
  }
  return std::move(result.property);
}

class CompiledPathPropertyExtractor : public CompiledPropertyVisitor {
 public:
  const CompiledPathProperty* PathProperty();

 private:
  void DoVisitCompiledNaryProperty(
      const CompiledNaryProperty& property) override;
  void DoVisitCompiledNotProperty(const CompiledNotProperty& property) override;
  void DoVisitCompiledProbabilityThresholdProperty(
      const CompiledProbabilityThresholdProperty& property) override;
  void DoVisitCompiledExpressionProperty(
      const CompiledExpressionProperty& property) override;

  std::vector<const CompiledPathProperty*> path_properties_;
};

const CompiledPathProperty* CompiledPathPropertyExtractor::PathProperty() {
  return path_properties_.size() == 1 ? path_properties_[0] : nullptr;
}

void CompiledPathPropertyExtractor::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  for (const CompiledProperty& operand : property.other_operands()) {
    operand.Accept(this);
  }
}

void CompiledPathPropertyExtractor::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  property.operand().Accept(this);
}

void CompiledPathPropertyExtractor::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  path_properties_.push_back(&property.path_property());
}

void CompiledPathPropertyExtractor::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {}

const CompiledPathProperty* ExtractPathProperty(
    const CompiledProperty& property) {
  CompiledPathPropertyExtractor extractor;
  property.Accept(&extractor);
  return extractor.PathProperty();
}

}  // namespace

/* The main program. */
int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  ModelCheckingParams params;
  /* Verification without estimation by default. */
  bool estimate = false;
  /* Set default engine. */
  enum {
    SAMPLING_ENGINE,
    HYBRID_ENGINE,
    MIXED_ENGINE
  } engine = SAMPLING_ENGINE;
  /* Number of moments to match. */
  size_t moments = 3;
  /* Set default seed. */
  size_t seed = time(0);
  /* Set default number of trials. */
  size_t trials = 1;
  /* Sever hostname */
  std::string hostname;
  /* Server port. */
  int port = -1;
  /* Constant overrides. */
  std::map<std::string, TypedValue> const_overrides;

  ModelAndProperties parse_result;
  std::vector<std::string> errors;
  try {
    /*
     * Get command line options.
     */
    while (1) {
      int option_index = 0;
#if HAVE_GETOPT_LONG
      int c =
          getopt_long(argc, argv, OPTION_STRING, long_options, &option_index);
#else
      int c = getopt(argc, argv, OPTION_STRING);
#endif
      if (c == -1) {
        break;
      }
      switch (c) {
        case 'A':
          params.alpha = atof(optarg);
          if (params.alpha < 1e-10) {
            throw std::invalid_argument("alpha < 1e-10");
          } else if (params.alpha >= 0.5) {
            throw std::invalid_argument("alpha >= 0.5");
          }
          break;
        case 'B':
          params.beta = atof(optarg);
          if (params.beta < 1e-10) {
            throw std::invalid_argument("beta < 1e-10");
          } else if (params.beta >= 0.5) {
            throw std::invalid_argument("beta >= 0.5");
          }
          break;
        case 'c':
          if (!parse_const_overrides(optarg, &const_overrides)) {
            throw std::invalid_argument("bad --const specification");
          }
          break;
        case 'D':
          params.delta = atof(optarg);
          if (params.delta < 1e-10) {
            throw std::invalid_argument("delta < 1e-10");
          } else if (params.delta > 0.5) {
            throw std::invalid_argument("delta > 0.5");
          }
          break;
        case 'E':
          params.epsilon = atof(optarg);
          if (params.epsilon < 1e-10) {
            throw std::invalid_argument("epsilon < 1e-10");
          } else if (params.epsilon > 1.0) {
            throw std::invalid_argument("epsilon > 1.0");
          }
          break;
        case 'e':
          if (strcasecmp(optarg, "sampling") == 0) {
            engine = SAMPLING_ENGINE;
          } else if (strcasecmp(optarg, "hybrid") == 0) {
            engine = HYBRID_ENGINE;
          } else if (strcasecmp(optarg, "mixed") == 0) {
            engine = MIXED_ENGINE;
          } else {
            throw std::invalid_argument("unsupported engine `" +
                                        std::string(optarg) + "'");
          }
          break;
        case 'H':
          hostname = optarg;
          break;
        case 'L':
          params.max_path_length = atoi(optarg);
          break;
        case 'M':
          params.memoization = true;
          break;
        case 'm':
          moments = atoi(optarg);
          if (moments < 1) {
            throw std::invalid_argument("must match at least one moment");
          } else if (moments > 3) {
            throw std::invalid_argument("cannot match more than three moments");
          }
          break;
        case 'N':
          params.algorithm = FIXED;
          params.fixed_sample_size = atoi(optarg);
          break;
        case 'n':
          params.nested_error = atof(optarg);
          break;
        case 'p':
          estimate = true;
          params.algorithm = ESTIMATE;
          break;
        case 'P':
          port = atoi(optarg);
          if (port < 0 || port > 0xffff) {
            throw std::invalid_argument("invalid port number");
          }
          break;
        case 's':
          if (strcasecmp(optarg, "ssp") == 0) {
            params.algorithm = SSP;
          } else if (strcasecmp(optarg, "sprt") == 0) {
            params.algorithm = SPRT;
          } else {
            throw std::invalid_argument("unsupported sampling algorithm `" +
                                        std::string(optarg) + "'");
          }
          break;
        case 'S':
          seed = atoi(optarg);
          break;
        case 'T':
          trials = atoi(optarg);
          break;
        case 'V':
          display_version();
          return 0;
        case 'h':
          display_help();
          return 0;
        case ':':
        default:
          std::cerr << "Try `" PACKAGE " --help' for more information."
                    << std::endl;
          return 1;
      }
    }
    if (params.nested_error > 0) {
      CHECK_LT(params.nested_error, MaxNestedError(params.delta));
    }

    /*
     * Read files.
     */
    if (optind < argc) {
      /*
       * Use remaining command line arguments as file names.
       */
      while (optind < argc) {
        if (!read_file(argv[optind++], &parse_result, &errors)) {
          for (const auto& error : errors) {
            std::cerr << PACKAGE << ":" << error << std::endl;
          }
          return 1;
        }
      }
    } else {
      /*
       * No remaining command line argument, so read from standard input.
       */
      if (!read_file("-", &parse_result, &errors)) {
        return 1;
      }
    }
    if (parse_result.model == nullptr) {
      std::cout << "no model" << std::endl;
      return 0;
    }
    parse_result.model->compile();
    VLOG(2) << *parse_result.model;
    std::map<std::string, IdentifierInfo> identifiers_by_name =
        GetConstantIdentifiersByName(parse_result.model->constants(),
                                     const_overrides, &errors);
    if (!errors.empty()) {
      for (const std::string& error : errors) {
        std::cerr << PACKAGE << ":" << error << std::endl;
      }
      return 1;
    }
    const CompiledModel compiled_model =
        CompileModel(*parse_result.model, &identifiers_by_name, &errors);
    if (compiled_model.type() == CompiledModelType::DTMC &&
        engine != SAMPLING_ENGINE) {
      errors.push_back(StrCat(parse_result.model->type(),
                              " only supported by sampling engine"));
    }
    DecisionDiagramManager dd_man(2 * compiled_model.BitCount());
    std::pair<int, int> reg_counts = compiled_model.GetRegisterCounts();
    UniquePtrVector<const CompiledProperty> compiled_properties;
    for (const Expression& property : parse_result.properties) {
      compiled_properties.push_back(CompileAndOptimizeProperty(
          property, identifiers_by_name, dd_man, &errors));
      auto property_reg_counts =
          GetPropertyRegisterCounts(compiled_properties.back());
      reg_counts.first = std::max(reg_counts.first, property_reg_counts.first);
      reg_counts.second =
          std::max(reg_counts.second, property_reg_counts.second);
    }
    if (!errors.empty()) {
      for (const std::string& error : errors) {
        std::cerr << PACKAGE << ":" << error << std::endl;
      }
      return 1;
    }

    std::cout.setf(std::ios::unitbuf);
    if (port >= 0) {
      sockaddr_in srvaddr;
      srvaddr.sin_family = AF_INET;
      srvaddr.sin_port = htons(port);
      if (!hostname.empty()) {
        /* Client mode. */
        hostent* host = gethostbyname(hostname.c_str());
        if (host == 0) {
          herror(PACKAGE);
          return 1;
        }
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
          perror(PACKAGE);
          return 1;
        }
        srvaddr.sin_addr = *(in_addr*)host->h_addr;
        if (-1 == connect(sockfd, (sockaddr*)&srvaddr, sizeof srvaddr)) {
          perror(PACKAGE);
          return 1;
        }
        ServerMsg smsg;
        int nbytes = recv(sockfd, &smsg, sizeof smsg, 0);
        if (nbytes == -1) {
          perror(PACKAGE);
          return 1;
        } else if (nbytes == 0) {
          VLOG(1) << "Shutting down (server unavailable)";
          return 0;
        }
        if (smsg.id != ServerMsg::REGISTER) {
          throw std::logic_error("expecting register message");
        }
        int client_id = smsg.value;
        std::cout << "Client " << client_id << std::endl;
        std::cout << "Initializing random number generator...";
        DCEngine dc_engine(client_id, seed);
        dc_engine.seed(seed);
        std::cout << "done" << std::endl;
        fd_set master_fds;
        FD_ZERO(&master_fds);
        FD_SET(sockfd, &master_fds);
        CompiledExpressionEvaluator evaluator(reg_counts.first,
                                              reg_counts.second);
        CompiledDistributionSampler<DCEngine> sampler(&dc_engine);
        const State init_state(compiled_model);
        const CompiledPathProperty* path_property = nullptr;
        ModelCheckingParams nested_params = params;
        timeval timeout;
        timeval* to = 0;
        while (true) {
          if (to != 0) {
            to->tv_sec = 0;
            to->tv_usec = 0;
          }
          fd_set read_fds = master_fds;
          int result = select(sockfd + 1, &read_fds, 0, 0, to);
          if (result == -1) {
            perror(PACKAGE);
            exit(-1);
          } else if (result == 0) {
            if (path_property) {
              ClientMsg msg = {ClientMsg::SAMPLE};
              ModelCheckingStats stats;
              msg.value = GetObservation(*path_property, compiled_model,
                                         nullptr, nested_params, &evaluator,
                                         &sampler, init_state, &stats);
              VLOG(2) << "Sending sample " << msg.value;
              nbytes = send(sockfd, &msg, sizeof msg, 0);
              if (nbytes == -1) {
                perror(PACKAGE);
                return 1;
              } else if (nbytes == 0) {
                VLOG(1) << "Shutting down (server unavailable)";
                return 0;
              }
            }
          } else {
            nbytes = recv(sockfd, &smsg, sizeof smsg, 0);
            if (nbytes == -1) {
              perror(PACKAGE);
              return 1;
            } else if (nbytes == 0) {
              VLOG(1) << "Shutting down (server unavailable)";
              return 0;
            }
            if (smsg.id == ServerMsg::START) {
              path_property =
                  ExtractPathProperty(compiled_properties[smsg.value]);
              if (path_property == nullptr) {
                std::cerr << PACKAGE << ":zero or multiple path formulae"
                          << std::endl;
                return 1;
              }
              // Since we currently do not support distributed sampling for
              // properties with nested probabilistic operators, we can just
              // set the nested error bounds to 0.
              nested_params.alpha = 0;
              nested_params.beta = 0;
              to = &timeout;
              VLOG(1) << "Sampling started for property " << smsg.value;
            } else if (smsg.id == ServerMsg::STOP) {
              to = 0;
              VLOG(1) << "Sampling stopped.";
            } else {
              std::cerr << "Message with bad id (" << smsg.id << ") ignored."
                        << std::endl;
            }
          }
        }
      } else {
        /* Server mode. */
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
          perror(PACKAGE);
          return 1;
        }
        int yes = 1;
        if (-1 == setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
                             sizeof yes)) {
          perror(PACKAGE);
          return 1;
        }
        srvaddr.sin_addr.s_addr = INADDR_ANY;
        if (-1 == bind(server_socket, (sockaddr*)&srvaddr, sizeof srvaddr)) {
          perror(PACKAGE);
          return 1;
        }
        if (-1 == listen(server_socket, 100)) {
          perror(PACKAGE);
          return 1;
        }
        VLOG(1) << "Server at port " << port;
      }
    }

    if (engine == SAMPLING_ENGINE) {
      DCEngine dc_engine;
      dc_engine.seed(seed);
      std::cout << "Sampling engine: alpha=" << params.alpha
                << ", beta=" << params.beta << ", delta=" << params.delta
                << ", seed=" << seed << std::endl;
      itimerval timer = {{0L, 0L}, {40000000L, 0L}};
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_PROF, &stimer);
#endif
      CompiledExpressionEvaluator evaluator(reg_counts.first,
                                            reg_counts.second);
      CompiledDistributionSampler<DCEngine> sampler(&dc_engine);
      const State init_state(compiled_model);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
      double t = std::max(0.0, sec + usec * 1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      std::cout << "Variables: " << init_state.values().size() << std::endl;
      std::cout << "Events:    " << compiled_model.EventCount() << std::endl;
      for (auto fi = parse_result.properties.begin();
           fi != parse_result.properties.end(); ++fi) {
        std::cout << std::endl << "Model checking " << *fi << " ..."
                  << std::endl;
        current_property = fi - parse_result.properties.begin();
        const CompiledProperty& property =
            compiled_properties[current_property];
        size_t accepts = 0;
        ModelCheckingStats stats;
        for (size_t i = 0; i < trials; ++i) {
          timeval start_time;
          itimerval timer = {{0, 0}, {40000000L, 0}};
          itimerval stimer;
          if (server_socket != -1) {
            gettimeofday(&start_time, 0);
          } else {
#ifdef PROFILING
            setitimer(ITIMER_VIRTUAL, &timer, 0);
            getitimer(ITIMER_VIRTUAL, &stimer);
#else
            setitimer(ITIMER_PROF, &timer, 0);
            getitimer(ITIMER_PROF, &stimer);
#endif
          }
          bool sol = Verify(property, compiled_model, nullptr, params,
                            &evaluator, &sampler, init_state, &stats);
          double t;
          if (server_socket != -1) {
            timeval end_time;
            gettimeofday(&end_time, 0);
            long sec = end_time.tv_sec - start_time.tv_sec;
            long usec = end_time.tv_usec - start_time.tv_usec;
            if (usec < 0) {
              sec--;
              usec = 1000000 + usec;
            }
            t = std::max(0.0, sec + usec * 1e-6);
          } else {
#ifdef PROFILING
            getitimer(ITIMER_VIRTUAL, &timer);
#else
            getitimer(ITIMER_PROF, &timer);
#endif
            long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
            long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
            t = std::max(0.0, sec + usec * 1e-6);
          }
          if (trials == 1) {
            std::cout << "Model checking completed in " << t << " seconds."
                      << std::endl;
            if (sol) {
              std::cout << "Property is true in the initial state."
                        << std::endl;
            } else {
              std::cout << "Property is false in the initial state."
                        << std::endl;
            }
          } else {
            if (sol) {
              accepts++;
            }
            stats.time.AddObservation(t);
          }
        }
        if (trials > 1) {
          std::cout
              << "Model checking time mean: " << stats.time.mean() << " seconds"
              << std::endl << "Model checking time min: " << stats.time.min()
              << " seconds" << std::endl
              << "Model checking time max: " << stats.time.max() << " seconds"
              << std::endl
              << "Model checking time std.dev.: " << stats.time.sample_stddev()
              << std::endl << "Sample size mean: " << stats.sample_size.mean()
              << std::endl << "Sample size min: " << stats.sample_size.min()
              << std::endl << "Sample size max: " << stats.sample_size.max()
              << std::endl
              << "Sample size std.dev.: " << stats.sample_size.sample_stddev()
              << std::endl << "Path length mean: " << stats.path_length.mean()
              << std::endl << "Path length min: " << stats.path_length.min()
              << std::endl << "Path length max: " << stats.path_length.max()
              << std::endl
              << "Path length std.dev.: " << stats.path_length.sample_stddev()
              << std::endl << accepts << " accepted, " << (trials - accepts)
              << " rejected" << std::endl
              << "Average cached: " << stats.sample_cache_size.mean()
              << std::endl;
        }
      }
    } else if (engine == HYBRID_ENGINE) {
      std::cout << "Hybrid engine: epsilon=" << params.epsilon << std::endl;
      itimerval timer = {{0L, 0L}, {40000000L, 0L}};
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#endif
      DecisionDiagramModel dd_model =
          DecisionDiagramModel::Create(&dd_man, moments, *parse_result.model,
                                       compiled_model, identifiers_by_name);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
      double t = std::max(0.0, sec + usec * 1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      std::cout << "States:      "
                << dd_model.reachable_states().MintermCount(
                       dd_man.GetVariableCount() / 2) << std::endl
                << "Transitions: "
                << dd_model.rate_matrix().MintermCount(
                       dd_man.GetVariableCount()) << std::endl;
      std::cout << "Rate matrix";
      Cudd_PrintDebug(dd_man.manager(), dd_model.rate_matrix().get(),
                      dd_man.GetVariableCount(), 1);
      std::cout << "ODD:         " << get_num_odd_nodes() << " nodes"
                << std::endl;
      for (auto fi = parse_result.properties.begin();
           fi != parse_result.properties.end(); ++fi) {
        std::cout << std::endl << "Model checking " << *fi << " ..."
                  << std::endl;
        current_property = fi - parse_result.properties.begin();
        const CompiledProperty& property =
            compiled_properties[current_property];
        double total_time = 0.0;
        bool accepted = false;
        for (size_t i = 0; i < trials; ++i) {
          itimerval timer = {{0L, 0L}, {40000000L, 0L}};
          itimerval stimer;
#ifdef PROFILING
          setitimer(ITIMER_VIRTUAL, &timer, 0);
          getitimer(ITIMER_VIRTUAL, &stimer);
#else
          setitimer(ITIMER_PROF, &timer, 0);
          getitimer(ITIMER_PROF, &stimer);
#endif
          BDD ddf = Verify(property, dd_model, estimate, true, params.epsilon);
          BDD sol = ddf && dd_model.initial_state();
#ifdef PROFILING
          getitimer(ITIMER_VIRTUAL, &timer);
#else
          getitimer(ITIMER_PROF, &timer);
#endif
          long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
          long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
          double t = std::max(0.0, sec + usec * 1e-6);
          total_time += t;
          accepted = (sol.get() != dd_man.GetConstant(false).get());
          if (t > 1.0) {
            total_time *= trials;
            break;
          }
        }
        std::cout << "Model checking completed in " << total_time / trials
                  << " seconds." << std::endl;
        if (accepted) {
          std::cout << "Property is true in the initial state." << std::endl;
        } else {
          std::cout << "Property is false in the initial state." << std::endl;
        }
      }
    } else if (engine == MIXED_ENGINE) {
      DCEngine dc_engine;
      dc_engine.seed(seed);
      std::cout << "Mixed engine: alpha=" << params.alpha
                << ", beta=" << params.beta << ", delta=" << params.delta
                << ", epsilon=" << params.epsilon << ", seed=" << seed
                << std::endl;
      itimerval timer = {{0L, 0L}, {40000000L, 0L}};
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_PROF, &stimer);
#endif
      DecisionDiagramModel dd_model =
          DecisionDiagramModel::Create(&dd_man, moments, *parse_result.model,
                                       compiled_model, identifiers_by_name);
      CompiledExpressionEvaluator evaluator(reg_counts.first,
                                            reg_counts.second);
      CompiledDistributionSampler<DCEngine> sampler(&dc_engine);
      const State init_state(compiled_model);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
      double t = std::max(0.0, sec + usec * 1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      std::cout << "Variables: " << init_state.values().size() << std::endl;
      std::cout << "Events:    " << compiled_model.EventCount() << std::endl;
      std::cout << "States:      "
                << dd_model.reachable_states().MintermCount(
                       dd_man.GetVariableCount() / 2) << std::endl
                << "Transitions: "
                << dd_model.rate_matrix().MintermCount(
                       dd_man.GetVariableCount()) << std::endl;
      std::cout << "Rate matrix";
      Cudd_PrintDebug(dd_man.manager(), dd_model.rate_matrix().get(),
                      dd_man.GetVariableCount(), 1);
      std::cout << "ODD:         " << get_num_odd_nodes() << " nodes"
                << std::endl;
      for (auto fi = parse_result.properties.begin();
           fi != parse_result.properties.end(); ++fi) {
        std::cout << std::endl << "Model checking " << *fi << " ..."
                  << std::endl;
        current_property = fi - parse_result.properties.begin();
        const CompiledProperty& property =
            compiled_properties[current_property];
        size_t accepts = 0;
        ModelCheckingStats stats;
        for (size_t i = 0; i < trials; ++i) {
          itimerval timer = {{0L, 0L}, {40000000L, 0L}};
          itimerval stimer;
#ifdef PROFILING
          setitimer(ITIMER_VIRTUAL, &timer, 0);
          getitimer(ITIMER_VIRTUAL, &stimer);
#else
          setitimer(ITIMER_PROF, &timer, 0);
          getitimer(ITIMER_PROF, &stimer);
#endif
          bool sol = Verify(property, compiled_model, &dd_model, params,
                            &evaluator, &sampler, init_state, &stats);
#ifdef PROFILING
          getitimer(ITIMER_VIRTUAL, &timer);
#else
          getitimer(ITIMER_PROF, &timer);
#endif
          long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
          long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
          double t = std::max(0.0, sec + usec * 1e-6);
          if (trials == 1) {
            std::cout << "Model checking completed in " << t << " seconds."
                      << std::endl;
            if (sol) {
              std::cout << "Property is true in the initial state."
                        << std::endl;
            } else {
              std::cout << "Property is false in the initial state."
                        << std::endl;
            }
          } else {
            if (sol) {
              accepts++;
            }
            stats.time.AddObservation(t);
          }
        }
        if (trials > 1) {
          std::cout
              << "Model checking time mean: " << stats.time.mean() << " seconds"
              << std::endl << "Model checking time min: " << stats.time.min()
              << " seconds" << std::endl
              << "Model checking time max: " << stats.time.max() << " seconds"
              << std::endl
              << "Model checking time std.dev.: " << stats.time.sample_stddev()
              << std::endl << "Sample size mean: " << stats.sample_size.mean()
              << std::endl << "Sample size min: " << stats.sample_size.min()
              << std::endl << "Sample size max: " << stats.sample_size.max()
              << std::endl
              << "Sample size std.dev.: " << stats.sample_size.sample_stddev()
              << std::endl << "Path length mean: " << stats.path_length.mean()
              << std::endl << "Path length min: " << stats.path_length.min()
              << std::endl << "Path length max: " << stats.path_length.max()
              << std::endl
              << "Path length std.dev.: " << stats.path_length.sample_stddev()
              << std::endl << accepts << " accepted, " << (trials - accepts)
              << " rejected" << std::endl;
        }
      }
    }
  } catch (const std::exception& e) {
    std::cerr << std::endl << PACKAGE ":" << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << std::endl << PACKAGE ":fatal error" << std::endl;
    return 1;
  }

  return 0;
}
