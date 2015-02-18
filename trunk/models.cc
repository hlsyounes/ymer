/*
 * Copyright (C) 2003, 2004 Carnegie Mellon University
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

#include "models.h"

#include <iostream>
#include <set>

#include "src/distribution.h"

#include "cudd.h"
#include "glog/logging.h"

namespace {

/*
 * Parameters defining an acyclic continuous phase-type (ACPH)
 * distribution that is either a Cox-2 distribution or a generalized
 * Erlang distribution.
 */
struct ACPH2Parameters {
  /* Number of phases of this distribution. */
  size_t n;
  /* Rate associated with either the first phase (Cox-2) or all phases
     (Erlang). */
  double r1;
  /* Rate associated with the second phase. */
  double r2;
  /* Probability of transitioning from the first phase to the second
     phase. */
  double p;
};

/*
 * Parameters defining an acyclic continuous phase-type (ACPH)
 * distribution in the class of EC distributions with p=1.
 */
struct ECParameters {
  /* Number of phases of this EC distribution. */
  size_t n;
  /* Rate associated with phases of the Erlang part of this EC
     distribution. */
  double re;
  /* Rate associated with the first phase of the two-phase Coxian part
     of this EC distribution. */
  double rc1;
  /* Rate associated with the second phase of the two-phase Coxian
     part of this EC distribution. */
  double rc2;
  /* Probability of transitioning to from the first phase to the
     second phase of the two-phase Coxian part of this EC distribution
     (1-pc is the probability of bypassing the second phase). */
  double pc;
};

/*
 * Data for phase-type distribution.
 */
struct PHData {
  explicit PHData(const DecisionDiagramManager& manager)
      : low_bit(-1), update_bdd(manager.GetConstant(true)) {}

  ECParameters params;
  ACPH2Parameters params2;
  int low_bit;
  int high_bit;
  BDD update_bdd;
};

class ConstantExpressionEvaluator : public ExpressionVisitor {
 public:
  ConstantExpressionEvaluator();

  TypedValue value() const { return value_; }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitLabel(const Label& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;

  TypedValue value_;
};

ConstantExpressionEvaluator::ConstantExpressionEvaluator() : value_(0) {}

void ConstantExpressionEvaluator::DoVisitLiteral(const Literal& expr) {
  value_ = expr.value();
}

void ConstantExpressionEvaluator::DoVisitIdentifier(const Identifier& expr) {
  LOG(FATAL) << "expecting constant expression";
}

void ConstantExpressionEvaluator::DoVisitLabel(const Label& expr) {
  LOG(FATAL) << "not an expression";
}

void ConstantExpressionEvaluator::DoVisitFunctionCall(
    const FunctionCall& expr) {
  std::vector<TypedValue> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(value_);
  }
  switch (expr.function()) {
    case Function::UNKNOWN:
      LOG(FATAL) << "bad function call";
    case Function::MIN:
      CHECK(!arguments.empty());
      value_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        value_ = std::min(value_, arguments[i]);
      }
      break;
    case Function::MAX:
      CHECK(!arguments.empty());
      value_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        value_ = std::max(value_, arguments[i]);
      }
      break;
    case Function::FLOOR:
      CHECK(arguments.size() == 1);
      value_ = floor(arguments[0]);
      break;
    case Function::CEIL:
      CHECK(arguments.size() == 2);
      value_ = ceil(arguments[0]);
      break;
    case Function::POW:
      CHECK(arguments.size() == 2);
      value_ = pow(arguments[0], arguments[1]);
      break;
    case Function::LOG:
      CHECK(arguments.size() == 2);
      value_ = log(arguments[0]) / log(arguments[1]);
      break;
    case Function::MOD:
      CHECK(arguments.size() == 2);
      value_ = arguments[0] % arguments[1];
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitUnaryOperation(
    const UnaryOperation& expr) {
  expr.operand().Accept(this);
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      value_ = -value_;
      break;
    case UnaryOperator::NOT:
      value_ = !value_;
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitBinaryOperation(
    const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  TypedValue operand1 = value_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case BinaryOperator::PLUS:
      value_ = operand1 + value_;
      break;
    case BinaryOperator::MINUS:
      value_ = operand1 - value_;
      break;
    case BinaryOperator::MULTIPLY:
      value_ = operand1 * value_;
      break;
    case BinaryOperator::DIVIDE:
      value_ = operand1 / value_;
      break;
    case BinaryOperator::AND:
      value_ = operand1.value<bool>() && value_.value<bool>();
      break;
    case BinaryOperator::OR:
      value_ = operand1.value<bool>() || value_.value<bool>();
      break;
    case BinaryOperator::IMPLY:
      value_ = !operand1.value<bool>() || value_.value<bool>();
      break;
    case BinaryOperator::IFF:
      value_ = operand1.value<bool>() == value_.value<bool>();
      break;
    case BinaryOperator::LESS:
      value_ = operand1 < value_;
      break;
    case BinaryOperator::LESS_EQUAL:
      value_ = operand1 <= value_;
      break;
    case BinaryOperator::GREATER_EQUAL:
      value_ = operand1 >= value_;
      break;
    case BinaryOperator::GREATER:
      value_ = operand1 > value_;
      break;
    case BinaryOperator::EQUAL:
      value_ = operand1 == value_;
      break;
    case BinaryOperator::NOT_EQUAL:
      value_ = operand1 != value_;
      break;
  }
}

void ConstantExpressionEvaluator::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  if (value_.value<bool>()) {
    expr.if_branch().Accept(this);
  } else {
    expr.else_branch().Accept(this);
  }
}

void ConstantExpressionEvaluator::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  LOG(FATAL) << "not an expression";
}

TypedValue EvaluateConstantExpression(const Expression& expr) {
  ConstantExpressionEvaluator evaluator;
  expr.Accept(&evaluator);
  return evaluator.value();
}

class MomentsCalculator : public DistributionVisitor {
 public:
  explicit MomentsCalculator(int n);

  const std::vector<double>& moments() const { return moments_; }

 private:
  void DoVisitMemoryless(const Memoryless& dist) override;
  void DoVisitWeibull(const Weibull& dist) override;
  void DoVisitLognormal(const Lognormal& dist) override;
  void DoVisitUniform(const Uniform& dist) override;

  int n_;
  std::vector<double> moments_;
};

MomentsCalculator::MomentsCalculator(int n) : n_(n) { moments_.reserve(n); }

void MomentsCalculator::DoVisitMemoryless(const Memoryless& dist) {
  moments_.clear();
  // N.B. this function should never be called for a distribution with
  // non-constant parameters.
  double lambda_inv =
      1.0 / EvaluateConstantExpression(dist.weight()).value<double>();
  double mi = 1.0;
  for (int i = 1; i <= n_; i++) {
    mi *= i * lambda_inv;
    moments_.push_back(mi);
  }
}

void MomentsCalculator::DoVisitWeibull(const Weibull& dist) {
  moments_.clear();
  // N.B. this function should never be called for a distribution with
  // non-constant parameters.
  double eta = EvaluateConstantExpression(dist.scale()).value<double>();
  double beta_inv =
      1.0 / EvaluateConstantExpression(dist.shape()).value<double>();
  double ei = 1.0;
  double bi = 1.0;
  for (int i = 1; i <= n_; i++) {
    ei *= eta;
    bi += beta_inv;
    moments_.push_back(ei * tgamma(bi));
  }
}

void MomentsCalculator::DoVisitLognormal(const Lognormal& dist) {
  moments_.clear();
  // N.B. this function should never be called for a distribution with
  // non-constant parameters.
  double mu = EvaluateConstantExpression(dist.scale()).value<double>();
  double sigma = EvaluateConstantExpression(dist.shape()).value<double>();
  double mean = log(mu) - sigma * sigma / 2.0;
  for (int i = 1; i <= n_; i++) {
    moments_.push_back(exp(i * mean + i * i * sigma * sigma / 2.0));
  }
}

void MomentsCalculator::DoVisitUniform(const Uniform& dist) {
  moments_.clear();
  // N.B. this function should never be called for a distribution with
  // non-constant parameters.
  double a = EvaluateConstantExpression(dist.low()).value<double>();
  double b = EvaluateConstantExpression(dist.high()).value<double>();
  double ai = a;
  double bi = b;
  for (int i = 1; i <= n_; i++) {
    ai *= a;
    bi *= b;
    moments_.push_back((bi - ai) / ((i + 1) * (b - a)));
  }
}

// Returns the n first moments of the given distribution.
std::vector<double> GetMoments(const Distribution& dist, int n) {
  MomentsCalculator calculator(n);
  dist.Accept(&calculator);
  return calculator.moments();
}

// Returns the EC distribution matching the first moment of the given
// distribution.
ECParameters MatchOneMoment(const Distribution& dist) {
  ECParameters params;
  params.n = 2;
  std::vector<double> mu = GetMoments(dist, 1);
  params.rc1 = 1.0 / mu[0];
  params.pc = 0.0;
  return params;
}

// Provides the parameters for an acyclic continuous phase-type (ACPH)
// distribution matching the first two moments of this distribution.
ACPH2Parameters MatchTwoMoments(const Distribution& dist) {
  ACPH2Parameters params;
  std::vector<double> m = GetMoments(dist, 2);
  double mu = m[0];
  double cv2 = m[1] / (mu * mu) - 1.0;
  if (cv2 >= 0.5) {
    params.n = 2;
    params.p = 1 / (2.0 * cv2);
    params.r1 = 2.0 / mu;
    params.r2 = 1.0 / (mu * cv2);
  } else {
    double n = 1.0 / cv2;
    if (fabs(rint(n) - n) < 1e-10) {
      n = rint(n);
    } else {
      n = ceil(n);
    }
    params.n = int(n + 0.5);
    params.p =
        1.0 - ((2.0 * n * cv2 + n - 2.0 - sqrt(n * n + 4.0 - 4.0 * n * cv2)) /
               2.0 / (n - 1.0) / (cv2 + 1.0));
    params.r1 = (1.0 - params.p + n * params.p) / mu;
    params.r2 = params.r1;
  }
  return params;
}

// Provides the paramters for an acyclic continuous phase-type (ACPH)
// distribution in the class of EC distributions matching the first three
// moments of this distribution using the moments matching algorithm described
// in:
//
//   Osogami, Takayuki, and Mor Harchol-Balter.  2003.  "A closed-form solution
//       for mapping general distributions to minimal PH distributions".  In
//       Proceedings of the 13th International Conference on Tools and
//       Algorithms for the Construnction and Analysis of Systems.
//
ECParameters MatchThreeMoments(const Distribution& dist) {
  // This is a slightly modified version avoiding solutions with p<1, at the
  // price that some positive distributions cannot be properly represented.  For
  // a distribution that the original algorithm would map to an EC distribution
  // with p<1, this algorithm modifies the second and third moments so that the
  // modified distribution can be matched to an EC distribution with p=1.  This
  // is to avoid EC distributions with mass probability at zero, which do not
  // fit well with the model of discrete event systems that we are using.
  ECParameters params;
  std::vector<double> m = GetMoments(dist, 3);
  double mu1G = m[0];
  double mu2G = m[1];
  double mu3G = m[2];
  double m2G = mu2G / (mu1G * mu1G);
  double m3G = mu3G / (mu1G * mu2G);
  double x = 1.0 / (m2G - 1.0);
  if (fabs(rint(x) - x) < 1e-10) {
    // Adjust second moment so that 1/(m2G - 1) is not an integer.
    m2G *= (1.0 - 1e-5);
    std::cerr << std::endl << PACKAGE ": second moment for " << dist
              << " is changed from " << mu2G << " to " << m2G* mu1G* mu1G
              << std::endl;
  }
  if (m3G < 2.0 * m2G - 1.0) {
    // Adjust third moment so that the resulting EC distribution has no mass
    // probability at zero (p=1).
    m3G = 2.0 * m2G - 1.0;
    std::cerr << std::endl << PACKAGE ": third moment for " << dist
              << " is changed from " << mu3G << " to " << m3G* mu1G* mu2G
              << std::endl;
  }
  double n;
  if (m2G < 2.0 && fabs(m3G - (2.0 * m2G - 1.0)) < 1e-10) {
    n = floor(m2G / (m2G - 1.0));
  } else {
    n = floor(m2G / (m2G - 1.0) + 1.0);
  }
  double m2X = ((n - 3.0) * m2G - (n - 2.0)) / ((n - 2.0) * m2G - (n - 1.0));
  double mu1X = mu1G / ((n - 2.0) * m2X - (n - 3.0));
  double alpha = (n - 2.0) * (m2X - 1.0) *
                 (n * (n - 1.0) * m2X * m2X - n * (2.0 * n - 5.0) * m2X +
                  (n - 1.0) * (n - 3.0));
  double tmp = (n - 2.0) * m2X - (n - 3.0);
  double beta = ((n - 1.0) * m2X - (n - 2.0)) * tmp * tmp;
  double m3X = (beta * m3G - alpha) / m2X;
  double u, v;
  if (fabs(3.0 * m2X - 2.0 * m3X) < 1e-10) {
    u = 1.0;
    v = 0.0;
  } else {
    u = (6.0 - 2.0 * m3X) / (3.0 * m2X - 2.0 * m3X);
    v = (12.0 - 6.0 * m2X) / (m2X * (3.0 * m2X - 2.0 * m3X));
  }
  params.n = int(n + 0.5);
  params.re = 1.0 / ((m2X - 1.0) * mu1X);
  tmp = sqrt(u * u - 4.0 * v);
  params.rc1 = (u + tmp) / (2.0 * mu1X);
  if (fabs(params.rc1 * mu1X - 1.0) < 1e-10) {
    params.pc = 0.0;
  } else {
    params.rc2 = (u - tmp) / (2.0 * mu1X);
    params.pc = params.rc2 * (params.rc1 * mu1X - 1.0) / params.rc1;
  }
  return params;
}

}  // namespace

DecisionDiagramModel::DecisionDiagramModel(
    const DecisionDiagramManager* manager, const BDD& initial_state,
    const ADD& rate_matrix, const BDD& reachable_states)
    : manager_(manager),
      initial_state_(initial_state),
      rate_matrix_(rate_matrix),
      reachable_states_(reachable_states),
      odd_(ODD::Make(reachable_states)),
      initial_state_index_(odd_.StateIndex(initial_state)) {}

namespace {

std::vector<IdentifierInfo> GetVariables(
    const CompiledModel& model,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name) {
  std::vector<IdentifierInfo> variables;
  variables.reserve(model.variables().size());
  for (const auto& v : model.variables()) {
    auto i = identifiers_by_name.find(v.name());
    CHECK(i != identifiers_by_name.end());
    variables.push_back(i->second);
  }
  return variables;
}

BDD VariableIdentityBdd(const DecisionDiagramManager& dd_manager,
                        const IdentifierInfo& variable) {
  return IdentifierToAdd(dd_manager, variable) ==
         PrimedIdentifierToAdd(dd_manager, variable);
}

BDD CompiledUpdateToBdd(const DecisionDiagramManager& dd_manager,
                        const std::vector<IdentifierInfo>& variables,
                        const CompiledUpdate& update) {
  return PrimedIdentifierToAdd(dd_manager, variables[update.variable()]) ==
         update.expr().dd().value();
}

ADD CompiledMarkovOutcomeToAdd(const DecisionDiagramManager& dd_manager,
                               const std::vector<IdentifierInfo>& variables,
                               const CompiledMarkovOutcome& outcome) {
  BDD updates = dd_manager.GetConstant(true);
  std::set<int> updated_variables;
  for (const auto& update : outcome.updates()) {
    updates = CompiledUpdateToBdd(dd_manager, variables, update) && updates;
    updated_variables.insert(update.variable());
  }
  for (size_t i = variables.size(); i > 0; --i) {
    if (updated_variables.find(i - 1) == updated_variables.end()) {
      updates = VariableIdentityBdd(dd_manager, variables[i - 1]) && updates;
    }
  }
  return outcome.probability().dd().value() * ADD(updates);
}

ADD CompiledMarkovCommandToAdd(const DecisionDiagramManager& dd_manager,
                               const std::vector<IdentifierInfo>& variables,
                               const CompiledMarkovCommand& command) {
  ADD outcomes = dd_manager.GetConstant(1);
  for (const auto& outcome : command.outcomes()) {
    outcomes =
        CompiledMarkovOutcomeToAdd(dd_manager, variables, outcome) * outcomes;
  }
  return command.guard().dd().value() * command.weight().dd().value() *
         outcomes;
}

BDD ReachabilityBdd(const DecisionDiagramManager& dd_manager, const BDD& init,
                    const ADD& rates) {
  std::cout << "Computing reachable states";
  // Precompute variable permutations and cubes.
  size_t nvars = dd_manager.GetVariableCount() / 2;
  std::vector<int> row_to_col(2 * nvars);
  std::vector<int> col_to_row(2 * nvars);
  for (size_t i = 0; i < nvars; i++) {
    row_to_col[2 * i] = 2 * i + 1;
    row_to_col[2 * i + 1] = 2 * i + 1;
    col_to_row[2 * i] = 2 * i;
    col_to_row[2 * i + 1] = 2 * i;
  }
  BDD row_cube =
      dd_manager.GetCube(dd_manager.GetBddVariableArray(0, 2, 2 * nvars));

  // Fixpoint computation of reachability.
  BDD trans = BDD(rates);
  BDD solr = init;
  BDD solc = solr.Permutation(row_to_col);
  bool done = false;
  size_t iters = 0;
  while (!done) {
    iters++;
    if (iters % 1000 == 0) {
      std::cout << ':';
    } else if (iters % 100 == 0) {
      std::cout << '.';
    }
    BDD next_solc = solc || (trans && solr).ExistAbstract(row_cube);
    if (next_solc.is_same(solc)) {
      done = true;
    } else {
      solc = next_solc;
    }
    solr = solc.Permutation(col_to_row);
  }
  std::cout << ' ' << iters << " iterations." << std::endl;
  return solr;
}

}  // namespace

DecisionDiagramModel DecisionDiagramModel::Make(
    const DecisionDiagramManager* manager, const CompiledModel& model,
    size_t moments,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name) {
  CHECK(manager);
  std::cout <<"Building model...";

  const std::vector<IdentifierInfo> variables =
      GetVariables(model, identifiers_by_name);

  // BDD for initial state.
  BDD init_bdd = manager->GetConstant(true);
  for (size_t i = model.variables().size(); i > 0; --i) {
    const auto& v = model.variables()[i - 1];
    const int init_value = model.init_values()[i - 1];
    auto j = identifiers_by_name.find(v.name());
    CHECK(j != identifiers_by_name.end());
    init_bdd = IdentifierToAdd(*manager, variables[i - 1])
                   .Interval(init_value, init_value) &&
               init_bdd;
  }

  // TODO(hlsyounes): Add phase variables for GSMP commands.

  // Compute rate matrix for all commands.
  ADD rate_matrix = manager->GetConstant(0);
  if (model.pivot_variable().has_value()) {
    const int variable = model.pivot_variable().value();
    const int min_value = model.variables()[variable].min_value();
    const auto& pivoted_commands = model.pivoted_single_markov_commands();
    for (size_t i = pivoted_commands.size(); i > 0; --i) {
      const int value = min_value + i - 1;
      ADD variable_assignment =
          ADD(IdentifierToAdd(*manager, variables[variable])
                  .Interval(value, value));
      const auto& commands = pivoted_commands[i - 1];
      for (size_t j = commands.size(); j > 0; --j) {
        const auto& command = commands[j - 1];
        rate_matrix = variable_assignment * CompiledMarkovCommandToAdd(
                                                *manager, variables, command) +
                      rate_matrix;
      }
    }
  }
  for (size_t i = model.single_markov_commands().size(); i > 0; --i) {
    const auto& command = model.single_markov_commands()[i - 1];
    rate_matrix =
        CompiledMarkovCommandToAdd(*manager, variables, command) + rate_matrix;
  }
  for (size_t i = model.factored_markov_commands().size(); i > 0; --i) {
    // TODO(hlsyounes): implement.
    LOG(FATAL) << "not implemented";
  }
  if (model.type() == CompiledModelType::DTMC) {
    // Normalize rate matrix.
    ADD col_cube = manager->GetCube(
        manager->GetAddVariableArray(1, 2, manager->GetVariableCount()));
    rate_matrix = rate_matrix / rate_matrix.ExistAbstract(col_cube);
  }

  std::cout << manager->GetVariableCount() << " variables." << std::endl;

  // Reachability analysis.
  BDD reach_bdd = ReachabilityBdd(*manager, init_bdd, rate_matrix);
  rate_matrix = ADD(reach_bdd) * rate_matrix;

  return DecisionDiagramModel(manager, init_bdd, rate_matrix, reach_bdd);
}
