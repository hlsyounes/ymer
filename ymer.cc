/*
 * Main program.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011--2015 Google Inc
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
#include "formulas.h"
#include "src/compiled-property.h"
#include "src/ddmodel.h"
#include "src/ddutil.h"
#include "src/distribution.h"
#include "src/model.h"
#include "src/model-checking-params.h"
#include "src/parser.h"
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
    {"port", required_argument, 0, 'P'},
    {"termination-probability", required_argument, 0, 'p'},
    {"estimation-algorithm", required_argument, 0, 'q'},
    {"seed", required_argument, 0, 'S'},
    {"trials", required_argument, 0, 'T'},
    {"threshold-algorithm", required_argument, 0, 't'},
    {"version", no_argument, 0, 'V'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}};
static const char OPTION_STRING[] = "A:B:c:D:E:e:H:hL:Mm:N:n:p:P:q:S:T:t:V";

namespace {

/* Displays help. */
void display_help() {
  std::cout
      << "usage: " << PACKAGE << " [options] [file ...]" << std::endl
      << "options:" << std::endl
      << "  -A a,  --alpha=a\t"
      << "use bound a on false negatives with sampling engine" << std::endl
      << "\t\t\t  (default is 1e-2)" << std::endl
      << "  -B b,  --beta=b\t"
      << "use bound b on false positives with sampling engine" << std::endl
      << "\t\t\t  (default is 1e-2)" << std::endl
      << "  -c c,  --const=c\t"
      << "overrides for model constants" << std::endl
      << "\t\t\t  (for example, --const=N=2,M=3)" << std::endl
      << "  -D d,  --delta=d\t"
      << "use indifference region of width 2*d with sampling" << std::endl
      << "\t\t\t  engine (default is 1e-2)" << std::endl
      << "  -E e,  --epsilon=e\t"
      << "use precision e with hybrid engine (default is 1e-6)" << std::endl
      << "  -e e,  --engine=e\t"
      << "use engine e; can be `sampling' (default), `hybrid'," << std::endl
      << "\t\t\t  or `mixed'" << std::endl
      << "  -H h,  --host=h\t"
      << "connect to server on host h" << std::endl
      << "  -L l,  --max_path-length=l" << std::endl
      << "\t\t\tlimit sample path to l states" << std::endl
      << "  -M,    --memoization\t"
      << "use memoization for sampling engine" << std::endl
      << "  -m m,  --matching-moments=m" << std::endl
      << "\t\t\tmatch the first m moments of general distributions" << std::endl
      << "  -N n,  --fixed-sample-size=n" << std::endl
      << "\t\t\tuse a fixed sample size" << std::endl
      << "  -P p,  --port=p\t"
      << "communicate using port p" << std::endl
      << "  -p p,  --termination-probability=p" << std::endl
      << "\t\t\tuse termination probability p for unbounded path properties"
      << std::endl
      << "  -q q,  --estimation-algorithm=q" << std::endl
      << "\t\t\tuse sampling algorithm q for estimation" << std::endl
      << "  -S s,  --seed=s\t"
      << "use seed s with random number generator" << std::endl
      << "\t\t\t  (sampling engine only)" << std::endl
      << "  -T t,  --trials=t\t"
      << "number of trials for sampling engine (default is 1)" << std::endl
      << "  -t t,  --threshold-algorithm=t" << std::endl
      << "\t\t\tuse sampling algorithm t for hypothesis testing" << std::endl
      << "  -V,    --version\t"
      << "display version information and exit" << std::endl
      << "  -h,    --help\t\t"
      << "display this help and exit" << std::endl
      << "  file ...\t\t"
      << "files containing models and properties;" << std::endl
      << "\t\t\t  if none, descriptions are read from standard input"
      << std::endl
      << std::endl
      << "Report bugs to <" PACKAGE_BUGREPORT ">." << std::endl;
}

/* Displays version information. */
void display_version() {
  std::cout << PACKAGE_STRING << std::endl
            << "Copyright (C) 2003--2005 Carnegie Mellon University"
            << std::endl << "Copyright (C) 2011--2015 Google Inc" << std::endl
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

CompiledExpression CompileAndOptimizeExpression(
    const Expression& expr, Type expected_type,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager,
    std::vector<std::string>* errors) {
  CompileExpressionResult result = CompileExpression(
      expr, expected_type, formulas_by_name, identifiers_by_name, dd_manager);
  if (!result.errors.empty()) {
    errors->insert(errors->end(), result.errors.begin(), result.errors.end());
    return CompiledExpression();
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
      const std::map<std::string, const Expression*>* formulas_by_name,
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      const Optional<DecisionDiagramManager>* dd_manager,
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
  const std::map<std::string, const Expression*>* formulas_by_name_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  const Optional<DecisionDiagramManager>* dd_manager_;
  std::vector<std::string>* errors_;
};

DistributionCompiler::DistributionCompiler(
    const std::map<std::string, const Expression*>* formulas_by_name,
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    const Optional<DecisionDiagramManager>* dd_manager,
    std::vector<std::string>* errors)
    : markov_weight_({}),
      gsmp_delay_(CompiledGsmpDistribution::MakeWeibull(0, 0)),
      formulas_by_name_(formulas_by_name),
      identifiers_by_name_(identifiers_by_name),
      dd_manager_(dd_manager),
      errors_(errors) {
  CHECK(formulas_by_name);
  CHECK(identifiers_by_name);
  CHECK(errors);
}

void DistributionCompiler::DoVisitMemoryless(const Memoryless& dist) {
  markov_weight_ = CompileAndOptimizeExpression(
      dist.weight(), Type::DOUBLE, *formulas_by_name_, *identifiers_by_name_,
      *dd_manager_, errors_);
}

double DistributionCompiler::GetDoubleValue(const Expression& expr) {
  CompiledExpression compiled_expr = CompileAndOptimizeExpression(
      expr, Type::DOUBLE, *formulas_by_name_, *identifiers_by_name_,
      *dd_manager_, errors_);
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
    Optional<ADD> dd;
    if (dd_manager_->has_value()) {
      dd = dd_manager_->value().GetConstant(1.0 / scale);
    }
    markov_weight_ =
        CompiledExpression({Operation::MakeDCONST(1.0 / scale, 0)}, dd);
  } else {
    markov_weight_ = CompiledExpression();
    gsmp_delay_ = CompiledGsmpDistribution::MakeWeibull(scale, shape);
  }
}

void DistributionCompiler::DoVisitLognormal(const Lognormal& dist) {
  markov_weight_ = CompiledExpression();
  gsmp_delay_ = CompiledGsmpDistribution::MakeLognormal(
      GetDoubleValue(dist.scale()), GetDoubleValue(dist.shape()));
}

void DistributionCompiler::DoVisitUniform(const Uniform& dist) {
  markov_weight_ = CompiledExpression();
  gsmp_delay_ = CompiledGsmpDistribution::MakeUniform(
      GetDoubleValue(dist.low()), GetDoubleValue(dist.high()));
}

CompiledUpdate CompileUpdate(
    const Update& update,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager,
    std::vector<std::string>* errors) {
  auto i = identifiers_by_name.find(update.variable());
  int variable;
  Type variable_type;
  if (i == identifiers_by_name.end()) {
    errors->push_back(
        StrCat("undefined variable '", update.variable(), "' in update"));
    variable = -1;
    variable_type = Type::INT;
  } else if (!i->second.is_variable()) {
    errors->push_back(StrCat("constant '", update.variable(), "' in update"));
    variable = -1;
    variable_type = Type::INT;
  } else {
    variable = i->second.variable_index();
    variable_type = i->second.type();
  }
  return CompiledUpdate(variable,
                        CompileAndOptimizeExpression(
                            update.expr(), variable_type, formulas_by_name,
                            identifiers_by_name, dd_manager, errors));
}

std::vector<CompiledUpdate> CompileUpdates(
    const std::vector<Update>& updates,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager,
    std::vector<std::string>* errors) {
  std::vector<CompiledUpdate> compiled_updates;
  compiled_updates.reserve(updates.size());
  for (const auto& update : updates) {
    compiled_updates.push_back(CompileUpdate(
        update, formulas_by_name, identifiers_by_name, dd_manager, errors));
  }
  return compiled_updates;
}

std::map<std::string, const Expression*> GetExpressionsByName(
    const std::vector<NamedExpression>& named_expressions) {
  std::map<std::string, const Expression*> expressions_by_name;
  for (const auto& named_expression : named_expressions) {
    expressions_by_name.emplace(named_expression.name(),
                                &named_expression.expr());
  }
  return expressions_by_name;
}

std::map<std::string, IdentifierInfo> GetConstantIdentifiersByName(
    const std::vector<ParsedConstant>& constants,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, TypedValue>& constant_overrides,
    std::vector<std::string>* errors) {
  std::map<std::string, IdentifierInfo> identifiers_by_name;
  std::map<std::string, TypedValue> constant_values = constant_overrides;
  ResolveConstants(constants, formulas_by_name, &constant_values, errors);
  for (const auto& entry : constant_values) {
    identifiers_by_name.emplace(entry.first,
                                IdentifierInfo::Constant(entry.second));
  }
  return identifiers_by_name;
}

int GetIntValue(
    const Expression& expr, Type expected_type,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    std::vector<std::string>* errors) {
  CompiledExpression compiled_expr = CompileAndOptimizeExpression(
      expr, expected_type, formulas_by_name, identifiers_by_name, {}, errors);
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

struct CompileVariablesResult {
  std::vector<StateVariableInfo> variables;
  std::vector<int> init_values;
  bool has_explicit_init;
  std::vector<int> max_values;
  int total_bit_count;
};

CompileVariablesResult CompileVariables(
    const std::vector<ParsedVariable>& parsed_variables,
    const std::map<std::string, const Expression*>& formulas_by_name,
    std::map<std::string, IdentifierInfo>* identifiers_by_name,
    std::vector<std::string>* errors) {
  CompileVariablesResult result;
  int index = 0;
  int low_bit = 0;
  result.has_explicit_init = false;
  for (const auto& v : parsed_variables) {
    int min = GetIntValue(v.min(), v.type(), formulas_by_name,
                          *identifiers_by_name, errors);
    int max = GetIntValue(v.max(), v.type(), formulas_by_name,
                          *identifiers_by_name, errors);
    int init = GetIntValue(v.init(), v.type(), formulas_by_name,
                           *identifiers_by_name, errors);
    if (v.has_explicit_init()) {
      result.has_explicit_init = true;
    }
    if (min >= max) {
      errors->push_back(StrCat("empty domain [", min, "..", max,
                               "] for variable ", v.name()));
      max = min + 1;
    } else if (init < min || init > max) {
      errors->push_back(StrCat("init ", init, " not in domain [", min, "..",
                               max, "] for variable ", v.name()));
    }
    const int bit_count = Log2(max - min) + 1;
    result.variables.emplace_back(v.name(), min, bit_count);
    result.init_values.push_back(init);
    result.max_values.push_back(max);
    identifiers_by_name->emplace(
        v.name(), IdentifierInfo::Variable(v.type(), index, low_bit,
                                           low_bit + bit_count - 1, min));
    ++index;
    low_bit += bit_count;
  }
  result.total_bit_count = low_bit;
  return result;
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

std::vector<std::set<int>> CompileModuleVariables(const Model& model) {
  LOG(INFO) << "Compiling module variables: " << model.modules().size();
  std::vector<std::set<int>> module_variables;
  module_variables.reserve(model.modules().size());
  for (const auto& module : model.modules()) {
    module_variables.push_back(module.variables());
  }
  LOG(INFO) << "Done compiling module variables.";
  return module_variables;
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
  Optional<ADD> dd;
  if (op == BinaryOperator::PLUS) {
    operations.push_back(Operation::MakeDADD(0, 1));
    if (expr1.dd().has_value() && expr2.dd().has_value()) {
      dd = expr1.dd().value() + expr2.dd().value();
    }
  } else if (op == BinaryOperator::MULTIPLY) {
    operations.push_back(Operation::MakeDMUL(0, 1));
    if (expr1.dd().has_value() && expr2.dd().has_value()) {
      dd = expr1.dd().value() * expr2.dd().value();
    }
  } else if (op == BinaryOperator::DIVIDE) {
    operations.push_back(Operation::MakeDDIV(0, 1));
    if (expr1.dd().has_value() && expr2.dd().has_value()) {
      dd = expr1.dd().value() / expr2.dd().value();
    }
  } else {
    LOG(FATAL) << "not implemented";
  }
  return OptimizeDoubleExpression(CompiledExpression(operations, dd));
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
  Optional<ADD> dd;
  if (expr1.dd().has_value() && expr2.dd().has_value()) {
    dd = ADD(BDD(expr1.dd().value()) && BDD(expr2.dd().value()));
  }
  return OptimizeIntExpression(CompiledExpression(operations, dd));
}

struct PreCompiledCommands {
  std::vector<CompiledMarkovCommand> single_markov_commands;
  std::map<int, std::map<int, std::vector<CompiledMarkovCommand>>>
      factored_markov_commands;
  std::vector<CompiledGsmpCommand> single_gsmp_commands;
  std::map<int, std::map<int, std::vector<CompiledGsmpCommand>>>
      factored_gsmp_commands;
};

PreCompiledCommands PreCompileCommands(
    const Model& model,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager,
    std::vector<std::string>* errors) {
  PreCompiledCommands result;
  DistributionCompiler dist_compiler(&formulas_by_name, &identifiers_by_name,
                                     &dd_manager, errors);
  std::vector<CompiledMarkovOutcome> markov_outcomes;
  for (size_t module_index = 0; module_index < model.modules().size();
       ++module_index) {
    const auto& module = model.modules()[module_index];
    for (const auto& command : module.commands()) {
      const auto compiled_guard = CompileAndOptimizeExpression(
          command.guard(), Type::BOOL, formulas_by_name, identifiers_by_name,
          dd_manager, errors);
      markov_outcomes.clear();
      for (const auto& outcome : command.outcomes()) {
        outcome.delay().Accept(&dist_compiler);
        const auto compiled_updates =
            CompileUpdates(outcome.updates(), formulas_by_name,
                           identifiers_by_name, dd_manager, errors);
        if (dist_compiler.has_markov_weight()) {
          markov_outcomes.emplace_back(dist_compiler.markov_weight(),
                                       compiled_updates);
        } else {
          CompiledGsmpCommand compiled_command(
              module_index, compiled_guard, dist_compiler.gsmp_delay(),
              compiled_updates, result.single_gsmp_commands.size());
          if (command.action().empty()) {
            result.single_gsmp_commands.push_back(compiled_command);
          } else {
            const int action_index = model.ActionIndex(command.action());
            result.factored_gsmp_commands[action_index][module_index].push_back(
                compiled_command);
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
        CompiledMarkovCommand compiled_command(module_index, compiled_guard,
                                               weight_sum, markov_outcomes);
        if (command.action().empty()) {
          result.single_markov_commands.push_back(compiled_command);
        } else {
          const int action_index = model.ActionIndex(command.action());
          result.factored_markov_commands[action_index][module_index].push_back(
              compiled_command);
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
  std::vector<CompiledGsmpCommandFactors> factored_gsmp_commands;
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
      {}, ComposeGuardExpressions(command1.guard(), command2.guard()),
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

std::vector<int> FactoredMarkovCommandOffsets(
    const std::map<int, std::vector<CompiledMarkovCommand>>&
        factored_markov_commands,
    int excluded_module_index) {
  std::vector<int> offsets;
  for (const auto& entry : factored_markov_commands) {
    const auto& module_index = entry.first;
    if (module_index != excluded_module_index) {
      const auto& commands = entry.second;
      offsets.push_back(commands.size());
    }
  }
  for (size_t i = offsets.size() - 1; i > 0; --i) {
    offsets[i - 1] *= offsets[i];
  }
  return offsets;
}

CompiledCommands CompileCommands(
    const Model& model,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager,
    std::vector<std::string>* errors) {
  CompiledCommands result;
  const PreCompiledCommands pre_compiled_commands = PreCompileCommands(
      model, formulas_by_name, identifiers_by_name, dd_manager, errors);
  result.single_markov_commands = pre_compiled_commands.single_markov_commands;
  result.single_gsmp_commands = pre_compiled_commands.single_gsmp_commands;

  std::set<int> action_indices;
  std::set<int> gsmp_action_indices;
  std::vector<CompiledUpdate> updates;
  for (const auto& entry : pre_compiled_commands.factored_gsmp_commands) {
    const int action_index = entry.first;
    gsmp_action_indices.insert(action_index);
    int gsmp_module_index = -1;
    for (const auto& module_entry : entry.second) {
      const auto& module_index = module_entry.first;
      if (gsmp_module_index < 0) {
        gsmp_module_index = module_index;
      } else if (gsmp_module_index != module_index) {
        errors->push_back(StrCat("action ", model.actions()[action_index],
                                 " has GSMP commands in multiple modules: ",
                                 model.modules()[gsmp_module_index].name(),
                                 " and ",
                                 model.modules()[module_index].name()));
        return result;
      }
    }
    auto i = pre_compiled_commands.factored_markov_commands.find(action_index);
    const auto& gsmp_commands = entry.second.find(gsmp_module_index)->second;
    if (i == pre_compiled_commands.factored_markov_commands.end()) {
      for (const auto& gsmp_command : gsmp_commands) {
        result.single_gsmp_commands.emplace_back(
            gsmp_module_index, gsmp_command.guard(), gsmp_command.delay(),
            gsmp_command.updates(), result.single_gsmp_commands.size());
      }
    } else if (IsSimpleComposition(i->second, gsmp_module_index,
                                   (gsmp_commands.size() > 1) ? 0 : 1)) {
      const auto composed_commands =
          ComposeFactoredMarkovCommands(i->second, gsmp_module_index);
      for (const auto& gsmp_command : gsmp_commands) {
        for (const auto& markov_command : composed_commands) {
          if (!IsUnitWeight(markov_command.weight())) {
            errors->push_back(StrCat("action ", model.actions()[action_index],
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
              Optional<int>(), ComposeGuardExpressions(gsmp_command.guard(),
                                                       markov_command.guard()),
              gsmp_command.delay(), updates,
              result.single_gsmp_commands.size());
        }
      }
    } else {
      // Process later.  We will need to add this to factored_gsmp_commands.
      action_indices.insert(action_index);
    }
  }
  for (const auto& entry : pre_compiled_commands.factored_markov_commands) {
    const int action_index = entry.first;
    if (gsmp_action_indices.find(action_index) == gsmp_action_indices.end()) {
      if (IsSimpleComposition(entry.second, -1, 1)) {
        const auto composed_commands =
            ComposeFactoredMarkovCommands(entry.second, -1);
        result.single_markov_commands.insert(
            result.single_markov_commands.end(), composed_commands.begin(),
            composed_commands.end());
      } else {
        // Process later.  We will need to add this to factored_markov_commands.
        action_indices.insert(action_index);
      }
    }
  }

  for (int action_index : action_indices) {
    auto i = pre_compiled_commands.factored_markov_commands.find(action_index);
    CHECK(i != pre_compiled_commands.factored_markov_commands.end());
    result.factored_markov_commands.emplace_back();
    auto j = pre_compiled_commands.factored_gsmp_commands.find(action_index);
    int gsmp_module_index = -1;
    if (j == pre_compiled_commands.factored_gsmp_commands.end()) {
      if (!gsmp_action_indices.empty()) {
        result.factored_gsmp_commands.emplace_back();
      }
    } else {
      const auto& entry = *j->second.begin();
      gsmp_module_index = entry.first;
      auto k = i->second.find(gsmp_module_index);
      if (k == i->second.end()) {
        result.factored_markov_commands.back().emplace_back();
      } else {
        result.factored_markov_commands.back().push_back(k->second);
      }
      result.factored_gsmp_commands.push_back(
          {entry.second,
           FactoredMarkovCommandOffsets(i->second, gsmp_module_index)});
    }
    for (const auto& entry : i->second) {
      if (entry.first != gsmp_module_index) {
        result.factored_markov_commands.back().push_back(entry.second);
      }
    }
  }
  return result;
}

CompiledExpression OptimizeWithAssignment(
    const CompiledExpression& expr, const IdentifierInfo& variable, int value,
    const Optional<DecisionDiagramManager>& dd_manager) {
  return OptimizeIntExpression(
      expr.WithAssignment(variable, value, dd_manager));
}

CompiledModel CompileModel(
    const Model& model, const std::vector<StateVariableInfo>& variables,
    const std::vector<int>& init_values,
    const Optional<CompiledExpression>& init_expr,
    const std::vector<int>& max_values,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager,
    std::vector<std::string>* errors) {
  CompiledModel compiled_model(CompileModelType(model.type(), errors),
                               variables, CompileModuleVariables(model),
                               init_values, init_expr);

  const auto& compiled_commands = CompileCommands(
      model, formulas_by_name, identifiers_by_name, dd_manager, errors);

  int pivot_variable = -1;
  std::vector<std::vector<CompiledMarkovCommand>>
      pivoted_single_markov_commands;
  std::vector<CompiledMarkovCommand> single_markov_commands(
      compiled_commands.single_markov_commands);
  size_t min_command_count = single_markov_commands.size();
  for (const auto& v : model.variables()) {
    auto i = identifiers_by_name.find(v.name());
    CHECK(i != identifiers_by_name.end());
    const auto& variable = i->second;
    const int min_value = variable.min_value().value<int>();
    const int max_value = max_values[variable.variable_index()];
    std::vector<std::vector<CompiledMarkovCommand>> pivoted_commands(
        max_value - min_value + 1);
    std::vector<CompiledMarkovCommand> other_commands;
    for (const auto& command : compiled_commands.single_markov_commands) {
      Optional<std::pair<int, CompiledExpression>> pivot_element;
      bool unique = true;
      for (int value = min_value; value <= max_value; ++value) {
        CompiledExpression guard = OptimizeWithAssignment(
            command.guard(), variable, value, dd_manager);
        if (guard.operations().size() != 1 ||
            guard.operations()[0].opcode() != Opcode::ICONST ||
            guard.operations()[0].operand2() != 0 ||
            guard.operations()[0].ioperand1() != 0) {
          if (pivot_element.has_value()) {
            unique = false;
          } else {
            pivot_element = std::make_pair(value, guard);
          }
        }
      }
      if (pivot_element.has_value() && unique) {
        pivoted_commands[pivot_element.value().first - min_value].emplace_back(
            command.module(), pivot_element.value().second, command.weight(),
            command.outcomes());
      } else {
        other_commands.push_back(command);
      }
    }
    size_t command_count = 0;
    for (const auto& commands : pivoted_commands) {
      command_count =
          std::max(commands.size() + other_commands.size(), command_count);
    }
    if (command_count < min_command_count) {
      VLOG(2) << "Command fraction for pivot on " << v.name() << ": "
              << static_cast<double>(command_count) /
                     compiled_commands.single_markov_commands.size();
      min_command_count = command_count;
      pivot_variable = variable.variable_index();
      pivoted_single_markov_commands = std::move(pivoted_commands);
      single_markov_commands = std::move(other_commands);
    }
  }

  if (pivot_variable != -1) {
    compiled_model.set_pivoted_single_markov_commands(
        pivot_variable, pivoted_single_markov_commands);
  }
  compiled_model.set_single_markov_commands(single_markov_commands);
  compiled_model.set_factored_markov_commands(
      compiled_commands.factored_markov_commands);
  compiled_model.set_single_gsmp_commands(
      compiled_commands.single_gsmp_commands);

  return compiled_model;
}

std::unique_ptr<const CompiledProperty> CompileAndOptimizeProperty(
    const Expression& property,
    const std::map<std::string, const Expression*>& formulas_by_name,
    const std::map<std::string, const Expression*>& labels_by_name,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Optional<DecisionDiagramManager>& dd_manager,
    std::vector<std::string>* errors) {
  CompilePropertyResult result =
      CompileProperty(property, formulas_by_name, labels_by_name,
                      identifiers_by_name, dd_manager);
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
  void DoVisitCompiledProbabilityEstimationProperty(
      const CompiledProbabilityEstimationProperty& property) override;
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

void CompiledPathPropertyExtractor::
    DoVisitCompiledProbabilityEstimationProperty(
        const CompiledProbabilityEstimationProperty& property) {
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

class CompiledPropertyInspector : public CompiledPropertyVisitor,
                                  public CompiledPathPropertyVisitor {
 public:
  CompiledPropertyInspector();

  bool has_nested() const { return has_nested_; }
  bool has_unbounded() const { return has_unbounded_; }
  bool has_nested_nested() const { return has_nested_nested_; }
  bool has_nested_unbounded() const { return has_nested_unbounded_; }
  bool is_estimation() const { return is_estimation_; }

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

  bool has_nested_;
  bool has_unbounded_;
  bool has_nested_nested_;
  bool has_nested_unbounded_;
  bool is_estimation_;
};

CompiledPropertyInspector::CompiledPropertyInspector()
    : has_nested_(false),
      has_unbounded_(false),
      has_nested_nested_(false),
      has_nested_unbounded_(false),
      is_estimation_(false) {}

void CompiledPropertyInspector::DoVisitCompiledNaryProperty(
    const CompiledNaryProperty& property) {
  for (const auto& operand : property.other_operands()) {
    operand.Accept(this);
  }
}

void CompiledPropertyInspector::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  property.operand().Accept(this);
}

void CompiledPropertyInspector::DoVisitCompiledProbabilityThresholdProperty(
    const CompiledProbabilityThresholdProperty& property) {
  property.path_property().Accept(this);
}

void CompiledPropertyInspector::DoVisitCompiledProbabilityEstimationProperty(
    const CompiledProbabilityEstimationProperty& property) {
  is_estimation_ = true;
  property.path_property().Accept(this);
}

void CompiledPropertyInspector::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {}

void CompiledPropertyInspector::DoVisitCompiledUntilProperty(
    const CompiledUntilProperty& path_property) {
  if (path_property.is_unbounded()) {
    has_unbounded_ = true;
    if (has_nested_) {
      has_nested_unbounded_ = true;
    }
  }
  if (path_property.is_probabilistic()) {
    if (has_nested_) {
      has_nested_nested_ = true;
    } else {
      has_nested_ = true;
    }
  }
  path_property.pre_property().Accept(this);
  path_property.post_property().Accept(this);
}

std::vector<bool> CheckUnsupported(
    const ModelCheckingParams& params, const CompiledModelType& model_type,
    const UniquePtrVector<const CompiledProperty>& properties,
    std::vector<std::string>* errors) {
  std::vector<bool> is_estimation;
  is_estimation.reserve(properties.size());
  for (const auto& property : properties) {
    CompiledPropertyInspector inspector;
    property.Accept(&inspector);
    is_estimation.push_back(inspector.is_estimation());
    switch (params.engine) {
      case ModelCheckingEngine::SAMPLING:
        if (inspector.has_nested() && model_type == CompiledModelType::GSMP) {
          // Not a feasible combination.
          errors->push_back(
              "sampling engine does not support nested probabilistic "
              "properties for GSMPs");
        }
        break;
      case ModelCheckingEngine::HYBRID:
        if (model_type == CompiledModelType::DTMC) {
          // TODO(hlsyounes): Implement support.
          errors->push_back("hybrid engine does not support DTMCs");
        } else if (model_type == CompiledModelType::GSMP) {
          // TODO(hlsyounes): Implement support with phase type distributions.
          errors->push_back("hybrid engine does not support GSMPs");
        }
        if (inspector.has_unbounded()) {
          // TODO(hlsyounes): Implement support.
          errors->push_back(
              "hybrid engine does not support unbounded properties");
        }
        break;
      case ModelCheckingEngine::MIXED:
        if (inspector.has_nested() && model_type == CompiledModelType::GSMP) {
          // Not a feasible combination.
          errors->push_back(
              "mixed engine does not support nested probabilistic properties "
              "for GSMPs");
        }
        if (inspector.has_nested_nested() &&
            model_type == CompiledModelType::DTMC) {
          // TODO(hlsyounes): Implement support.
          errors->push_back(
              "mixed engine does not support doubly-nested probabilistic "
              "properties for DTMCs");
        }
        if (inspector.has_nested_unbounded()) {
          // TODO(hlsyounes): Implement support.
          errors->push_back(
              "mixed engine does not support nested unbounded properties");
        }
        break;
    }
  }
  return is_estimation;
}

ThresholdAlgorithm ParseThresholdAlgorithm(const std::string& name) {
  if (strcasecmp(name.c_str(), "ssp") == 0) {
    return ThresholdAlgorithm::SSP;
  } else if (strcasecmp(name.c_str(), "sprt") == 0) {
    return ThresholdAlgorithm::SPRT;
  } else if (strcasecmp(name.c_str(), "chow-robbins") == 0) {
    return ThresholdAlgorithm::CHOW_ROBBINS;
  }
  throw std::invalid_argument(
      StrCat("unsupported threshold algorithm `", name, "'"));
}

EstimationAlgorithm ParseEstimationAlgorithm(const std::string& name) {
  if (strcasecmp(name.c_str(), "chow-robbins") == 0) {
    return EstimationAlgorithm::CHOW_ROBBINS;
  }
  throw std::invalid_argument(
      StrCat("unsupported estimation algorithm `", name, "'"));
}

}  // namespace

/* The main program. */
int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  ModelCheckingParams params;
  params.alpha = 1e-2;
  params.beta = 1e-2;
  params.delta = 1e-2;
  params.epsilon = 1e-6;
  params.termination_probability = 1e-6;
  params.engine = ModelCheckingEngine::SAMPLING;
  params.threshold_algorithm = ThresholdAlgorithm::SPRT;
  params.estimation_algorithm = EstimationAlgorithm::CHOW_ROBBINS;
  params.fixed_sample_size = 0;
  params.max_path_length = std::numeric_limits<int>::max();
  params.nested_error = -1;
  params.memoization = false;
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
            params.engine = ModelCheckingEngine::SAMPLING;
          } else if (strcasecmp(optarg, "hybrid") == 0) {
            params.engine = ModelCheckingEngine::HYBRID;
          } else if (strcasecmp(optarg, "mixed") == 0) {
            params.engine = ModelCheckingEngine::MIXED;
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
          params.threshold_algorithm = ThresholdAlgorithm::FIXED;
          params.estimation_algorithm = EstimationAlgorithm::FIXED;
          params.fixed_sample_size = atoi(optarg);
          break;
        case 'n':
          params.nested_error = atof(optarg);
          break;
        case 'P':
          port = atoi(optarg);
          if (port < 0 || port > 0xffff) {
            throw std::invalid_argument("invalid port number");
          }
          break;
        case 'p':
          params.termination_probability = atof(optarg);
          break;
        case 'q':
          params.estimation_algorithm = ParseEstimationAlgorithm(optarg);
          break;
        case 'S':
          seed = atoi(optarg);
          break;
        case 'T':
          trials = atoi(optarg);
          break;
        case 't':
          params.threshold_algorithm = ParseThresholdAlgorithm(optarg);
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
        if (!ParseFile(argv[optind++], &parse_result, &errors)) {
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
      if (!ParseFile("-", &parse_result, &errors)) {
        return 1;
      }
    }
    if (!parse_result.model.has_value()) {
      std::cout << "no model" << std::endl;
      return 0;
    }
    const Model& model = parse_result.model.value();
    VLOG(2) << model;
    const std::map<std::string, const Expression*> formulas_by_name =
        GetExpressionsByName(model.formulas());
    const std::map<std::string, const Expression*> labels_by_name =
        GetExpressionsByName(model.labels());
    std::map<std::string, IdentifierInfo> identifiers_by_name =
        GetConstantIdentifiersByName(model.constants(), formulas_by_name,
                                     const_overrides, &errors);
    if (!errors.empty()) {
      for (const std::string& error : errors) {
        std::cerr << PACKAGE << ":" << error << std::endl;
      }
      return 1;
    }
    auto compile_variables_result = CompileVariables(
        model.variables(), formulas_by_name, &identifiers_by_name, &errors);
    Optional<DecisionDiagramManager> dd_manager;
    if (params.engine == ModelCheckingEngine::HYBRID ||
        params.engine == ModelCheckingEngine::MIXED) {
      dd_manager =
          DecisionDiagramManager(2 * compile_variables_result.total_bit_count);
    }
    Optional<CompiledExpression> init_expr;
    if (model.init() != nullptr) {
      if (compile_variables_result.has_explicit_init) {
        errors.push_back(
            "global init not allowed in model with explicit variable inits");
      }
      init_expr = CompileAndOptimizeExpression(
          *model.init(), Type::BOOL, formulas_by_name, identifiers_by_name,
          dd_manager, &errors);
      if (params.engine != ModelCheckingEngine::HYBRID) {
        // TODO(hyounes): Set value from init expr.  Report error if init expr
        // does not identify a single state.
      }
    }
    const CompiledModel compiled_model =
        CompileModel(model, compile_variables_result.variables,
                     compile_variables_result.init_values, init_expr,
                     compile_variables_result.max_values, formulas_by_name,
                     identifiers_by_name, dd_manager, &errors);
    std::pair<int, int> reg_counts = compiled_model.GetRegisterCounts();
    UniquePtrVector<const CompiledProperty> compiled_properties;
    for (const Expression& property : parse_result.properties) {
      compiled_properties.push_back(
          CompileAndOptimizeProperty(property, formulas_by_name, labels_by_name,
                                     identifiers_by_name, dd_manager, &errors));
      auto property_reg_counts =
          GetPropertyRegisterCounts(compiled_properties.back());
      reg_counts.first = std::max(reg_counts.first, property_reg_counts.first);
      reg_counts.second =
          std::max(reg_counts.second, property_reg_counts.second);
    }
    const auto is_estimation = CheckUnsupported(params, compiled_model.type(),
                                                compiled_properties, &errors);
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

    if (params.engine == ModelCheckingEngine::SAMPLING) {
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
            if (!is_estimation[current_property]) {
              if (sol) {
                std::cout << "Property is true in the initial state."
                          << std::endl;
              } else {
                std::cout << "Property is false in the initial state."
                          << std::endl;
              }
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
    } else if (params.engine == ModelCheckingEngine::HYBRID) {
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
      DecisionDiagramModel dd_model = DecisionDiagramModel::Make(
          &dd_manager.value(), compiled_model, moments, identifiers_by_name);
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
                       dd_manager.value().GetVariableCount() / 2) << std::endl
                << "Transitions: "
                << dd_model.rate_matrix().MintermCount(
                       dd_manager.value().GetVariableCount()) << std::endl;
      std::cout << "Rate matrix";
      Cudd_PrintDebug(dd_manager.value().manager(),
                      dd_model.rate_matrix().get(),
                      dd_manager.value().GetVariableCount(), 1);
      std::cout << "ODD:         " << dd_model.odd().node_count() << " nodes"
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
          BDD ddf = Verify(property, dd_model, true, params.epsilon);
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
          accepted = !sol.is_same(dd_manager.value().GetConstant(false));
          if (t > 1.0) {
            total_time *= trials;
            break;
          }
        }
        std::cout << "Model checking completed in " << total_time / trials
                  << " seconds." << std::endl;
        if (!is_estimation[current_property]) {
          if (accepted) {
            std::cout << "Property is true in the initial state." << std::endl;
          } else {
            std::cout << "Property is false in the initial state." << std::endl;
          }
        }
      }
    } else if (params.engine == ModelCheckingEngine::MIXED) {
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
      DecisionDiagramModel dd_model = DecisionDiagramModel::Make(
          &dd_manager.value(), compiled_model, moments, identifiers_by_name);
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
                       dd_manager.value().GetVariableCount() / 2) << std::endl
                << "Transitions: "
                << dd_model.rate_matrix().MintermCount(
                       dd_manager.value().GetVariableCount()) << std::endl;
      std::cout << "Rate matrix";
      Cudd_PrintDebug(dd_manager.value().manager(),
                      dd_model.rate_matrix().get(),
                      dd_manager.value().GetVariableCount(), 1);
      std::cout << "ODD:         " << dd_model.odd().node_count() << " nodes"
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
            if (!is_estimation[current_property]) {
              if (sol) {
                std::cout << "Property is true in the initial state."
                          << std::endl;
              } else {
                std::cout << "Property is false in the initial state."
                          << std::endl;
              }
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
