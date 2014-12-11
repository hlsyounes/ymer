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
#include <stdexcept>
#include <typeinfo>

#include "formulas.h"
#include "src/distribution.h"
#include "src/strutil.h"

#include "cudd.h"
#include "glog/logging.h"

/*
 * A synchronization map.
 */
struct SynchronizationMap
    : public std::multimap<std::pair<const ParsedModule*, std::string>,
                           const Command*> {};

/* Range for synchronization map. */
typedef std::pair<SynchronizationMap::const_iterator,
                  SynchronizationMap::const_iterator> SynchronizationMapRange;

/* ====================================================================== */
/* PHData */

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

/* ====================================================================== */
/* Model */

namespace {

// Invalid module index.
const int kNoModule = -1;

class ConstantExpressionEvaluator : public ExpressionVisitor {
 public:
  ConstantExpressionEvaluator();

  TypedValue value() const { return value_; }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
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
  LOG(FATAL) << "expecting constant expression";
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

/* Returns the number of phases of the given PH distribution. */
int ph_phases(const PHData& data) {
  if (data.params.n == 0) {
    return data.params2.n;
  } else {
    return data.params.n - ((data.params.pc > 0.0) ? 0 : 1);
  }
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

ADD CompileVariable(const DecisionDiagramManager& manager,
                    const TypedValue& low, int low_bit, int high_bit,
                    bool primed) {
  ADD result = manager.GetConstant(0);
  const int offset = primed ? 1 : 0;
  for (int i = high_bit; i >= low_bit; --i) {
    result = result + (manager.GetAddVariable(2 * i + offset) *
                       manager.GetConstant(1 << (high_bit - i)));
  }
  return result + manager.GetConstant(low.value<double>());
}

class ExpressionCompiler : public ExpressionVisitor {
 public:
  ExpressionCompiler(
      const DecisionDiagramManager* manager,
      const std::map<std::string, IdentifierInfo>* identifiers_by_name,
      bool primed);

  ADD mtbdd() const { return mtbdd_; }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;

  const DecisionDiagramManager* manager_;
  const std::map<std::string, IdentifierInfo>* identifiers_by_name_;
  bool primed_;
  ADD mtbdd_;
};

ExpressionCompiler::ExpressionCompiler(
    const DecisionDiagramManager* manager,
    const std::map<std::string, IdentifierInfo>* identifiers_by_name,
    bool primed)
    : manager_(manager),
      identifiers_by_name_(identifiers_by_name),
      primed_(primed),
      mtbdd_(manager->GetConstant(0)) {}

void ExpressionCompiler::DoVisitLiteral(const Literal& expr) {
  mtbdd_ = manager_->GetConstant(expr.value().value<double>());
}

void ExpressionCompiler::DoVisitIdentifier(const Identifier& expr) {
  auto i = identifiers_by_name_->find(expr.name());
  CHECK(i != identifiers_by_name_->end());
  const IdentifierInfo& p = i->second;
  if (p.is_variable()) {
    mtbdd_ = CompileVariable(*manager_, p.min_value(), p.low_bit(),
                             p.high_bit(), primed_);
  } else {
    mtbdd_ = manager_->GetConstant(p.constant_value().value<double>());
  }
}

void ExpressionCompiler::DoVisitFunctionCall(const FunctionCall& expr) {
  std::vector<ADD> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(mtbdd_);
  }
  switch (expr.function()) {
    case Function::UNKNOWN:
      LOG(FATAL) << "bad function call";
    case Function::MIN:
      CHECK(!arguments.empty());
      mtbdd_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        mtbdd_ = min(mtbdd_, arguments[i]);
      }
      break;
    case Function::MAX:
      CHECK(!arguments.empty());
      mtbdd_ = arguments[0];
      for (size_t i = 1; i < arguments.size(); ++i) {
        mtbdd_ = max(mtbdd_, arguments[i]);
      }
      break;
    case Function::FLOOR:
      CHECK(arguments.size() == 1);
      mtbdd_ = floor(arguments[0]);
      break;
    case Function::CEIL:
      CHECK(arguments.size() == 2);
      mtbdd_ = ceil(arguments[0]);
      break;
    case Function::POW:
      CHECK(arguments.size() == 2);
      mtbdd_ = pow(arguments[0], arguments[1]);
      break;
    case Function::LOG:
      CHECK(arguments.size() == 2);
      mtbdd_ = log(arguments[0]) / log(arguments[1]);
      break;
    case Function::MOD:
      CHECK(arguments.size() == 2);
      mtbdd_ = arguments[0] % arguments[1];
      break;
  }
}

void ExpressionCompiler::DoVisitUnaryOperation(const UnaryOperation& expr) {
  expr.operand().Accept(this);
  switch (expr.op()) {
    case UnaryOperator::NEGATE:
      mtbdd_ = -mtbdd_;
      break;
    case UnaryOperator::NOT:
      mtbdd_ = ADD(!BDD(mtbdd_));
      break;
  }
}

void ExpressionCompiler::DoVisitBinaryOperation(const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  ADD operand1 = mtbdd_;
  expr.operand2().Accept(this);
  switch (expr.op()) {
    case BinaryOperator::PLUS:
      mtbdd_ = operand1 + mtbdd_;
      break;
    case BinaryOperator::MINUS:
      mtbdd_ = operand1 - mtbdd_;
      break;
    case BinaryOperator::MULTIPLY:
      mtbdd_ = operand1 * mtbdd_;
      break;
    case BinaryOperator::DIVIDE:
      mtbdd_ = operand1 / mtbdd_;
      break;
    case BinaryOperator::AND:
      mtbdd_ = ADD(BDD(operand1) && BDD(mtbdd_));
      break;
    case BinaryOperator::OR:
      mtbdd_ = ADD(BDD(operand1) || BDD(mtbdd_));
      break;
    case BinaryOperator::IMPLY:
      mtbdd_ = ADD(!BDD(operand1) || BDD(mtbdd_));
      break;
    case BinaryOperator::IFF:
      mtbdd_ = ADD(BDD(operand1) == BDD(mtbdd_));
      break;
    case BinaryOperator::LESS:
      mtbdd_ = ADD(operand1 < mtbdd_);
      break;
    case BinaryOperator::LESS_EQUAL:
      mtbdd_ = ADD(operand1 <= mtbdd_);
      break;
    case BinaryOperator::GREATER_EQUAL:
      mtbdd_ = ADD(operand1 >= mtbdd_);
      break;
    case BinaryOperator::GREATER:
      mtbdd_ = ADD(operand1 > mtbdd_);
      break;
    case BinaryOperator::EQUAL:
      mtbdd_ = ADD(operand1 == mtbdd_);
      break;
    case BinaryOperator::NOT_EQUAL:
      mtbdd_ = ADD(operand1 != mtbdd_);
      break;
  }
}

void ExpressionCompiler::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  BDD condition = BDD(mtbdd_);
  expr.if_branch().Accept(this);
  ADD if_branch = mtbdd_;
  expr.else_branch().Accept(this);
  mtbdd_ = Ite(condition, if_branch, mtbdd_);
}

void ExpressionCompiler::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  LOG(FATAL) << "not an expression";
}

ADD mtbdd(const DecisionDiagramManager& manager,
          const std::map<std::string, IdentifierInfo>& identifiers_by_name,
          const Expression& e) {
  ExpressionCompiler compiler(&manager, &identifiers_by_name,
                              false /* primed */);
  e.Accept(&compiler);
  return compiler.mtbdd();
}

ADD primed_mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Expression& e) {
  ExpressionCompiler compiler(&manager, &identifiers_by_name,
                              true /* primed */);
  e.Accept(&compiler);
  return compiler.mtbdd();
}

/* Returns a reachability BDD for the given initial state and rate
   matrix. */
BDD reachability_bdd(const DecisionDiagramManager& dd_man, const BDD& init,
                     const ADD& rates) {
  std::cout << "Computing reachable states";
  /*
   * Precompute variable permutations and cubes.
   */
  size_t nvars = dd_man.GetVariableCount() / 2;
  std::vector<int> row_to_col(2 * nvars);
  std::vector<int> col_to_row(2 * nvars);
  for (size_t i = 0; i < nvars; i++) {
    row_to_col[2 * i] = 2 * i + 1;
    row_to_col[2 * i + 1] = 2 * i + 1;
    col_to_row[2 * i] = 2 * i;
    col_to_row[2 * i + 1] = 2 * i;
  }
  BDD row_cube = dd_man.GetCube(dd_man.GetBddVariableArray(0, 2, 2 * nvars));

  /*
   * Fixpoint computation of reachability.
   */
  BDD trans = rates.StrictThreshold(0);
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
    if (next_solc.get() == solc.get()) {
      done = true;
    } else {
      solc = next_solc;
    }
    solr = solc.Permutation(col_to_row);
  }
  std::cout << ' ' << iters << " iterations." << std::endl;
  return solr;
}

ADD variable_mtbdd(const DecisionDiagramManager& manager, const TypedValue& low,
                   int low_bit, int high_bit) {
  return CompileVariable(manager, low, low_bit, high_bit, false /* primed */);
}

ADD variable_primed_mtbdd(const DecisionDiagramManager& manager,
                          const TypedValue& low, int low_bit, int high_bit) {
  return CompileVariable(manager, low, low_bit, high_bit, true /* primed */);
}

// Returns a BDD representing identity between the `current state' and `next
// state' versions of the given variable.
BDD identity_bdd(const DecisionDiagramManager& manager, const TypedValue& low,
                 int low_bit, int high_bit) {
  return variable_mtbdd(manager, low, low_bit, high_bit) ==
         variable_primed_mtbdd(manager, low, low_bit, high_bit);
}

/* Returns a BDD representing the conjunction of dd_start with the
   identity BDDs for the given variables. */
BDD variable_identities(
    const DecisionDiagramManager& dd_man, const BDD& dd_start,
    const Model& model,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const std::set<int>& variables, const std::set<std::string>& excluded) {
  BDD ddu = dd_start;
  for (std::set<int>::const_reverse_iterator i = variables.rbegin();
       i != variables.rend(); i++) {
    const ParsedVariable& v = model.variables()[*i];
    if (excluded.find(v.name()) == excluded.end()) {
      auto j = identifiers_by_name.find(v.name());
      CHECK(j != identifiers_by_name.end());
      const IdentifierInfo& p = j->second;
      CHECK(p.is_variable());
      ddu =
          identity_bdd(dd_man, p.min_value(), p.low_bit(), p.high_bit()) && ddu;
    }
  }
  return ddu;
}

// Returns a BDD representation of the given update.
BDD update_bdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Update& update) {
  auto i = identifiers_by_name.find(update.variable());
  CHECK(i != identifiers_by_name.end());
  const IdentifierInfo& p = i->second;
  CHECK(p.is_variable());
  return variable_primed_mtbdd(manager, p.min_value(), p.low_bit(),
                               p.high_bit()) ==
         mtbdd(manager, identifiers_by_name, update.expr());
}

// Returns a BDD representation of the given command and fills the provided
// set with variables updated by the command.
BDD command_bdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const Command& command, std::set<std::string>* updated_variables) {
  BDD ddu = manager.GetConstant(true);
  for (const Update& update : command.outcomes()[0].updates()) {
    ddu = update_bdd(manager, identifiers_by_name, update) && ddu;
    updated_variables->insert(update.variable());
  }
  return BDD(mtbdd(manager, identifiers_by_name, command.guard())) && ddu;
}

// Returns a BDD representing the conjunction of dd_start with the BDDs for
// updates of all variables for the given model not explicitly mentioned.
BDD variable_updates(
    const DecisionDiagramManager& manager, const BDD& dd_start,
    const Model& model,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name,
    const std::set<std::string>& updated_variables, int command_index,
    const std::map<int, PHData>& ph_commands) {
  BDD ddu = dd_start;
  // Conjunction with identity BDD for untouched module variables.
  for (auto i = model.modules().rbegin(); i != model.modules().rend(); ++i) {
    ddu = variable_identities(manager, ddu, model, identifiers_by_name,
                              i->variables(), updated_variables);
  }
  // Conjunction with identity BDD for untouched global variables.
  ddu = variable_identities(manager, ddu, model, identifiers_by_name,
                            model.global_variables(), updated_variables);
  // Conjunction with update rules for phase variables.
  for (std::map<int, PHData>::const_reverse_iterator ci = ph_commands.rbegin();
       ci != ph_commands.rend(); ci++) {
    if (ci->first != command_index) {
      ddu = ci->second.update_bdd && ddu;
    }
  }
  return ddu;
}

}  // namespace

Model::Model() : type_(ModelType::DEFAULT), current_module_(kNoModule) {}

Model::~Model() {
  for (const Command* command : commands()) {
    if (!command->action().empty()) {
      delete command;
    }
  }
}

bool Model::SetType(ModelType type) {
  if (type_ == ModelType::DEFAULT) {
    type_ = type;
    return true;
  }
  return false;
}

std::string Model::IdentifierIndexTypeToString(IdentifierIndex::Type type) {
  switch (type) {
    case IdentifierIndex::kConstant:
      return "constant";
    case IdentifierIndex::kVariable:
      return "variable";
    case IdentifierIndex::kAction:
      return "action";
  }
  LOG(FATAL) << "bad identifier index type";
}

bool Model::AddConstant(const std::string& name, Type type,
                        std::unique_ptr<const Expression>&& init,
                        std::vector<std::string>* errors) {
  auto i = identifier_indices_.find(name);
  if (i != identifier_indices_.end()) {
    if (i->second.type == IdentifierIndex::kConstant) {
      errors->push_back(StrCat("duplicate constant ", name));
    } else {
      errors->push_back(StrCat("constant ", name, " previously defined as ",
                               IdentifierIndexTypeToString(i->second.type)));
    }
    return false;
  }
  identifier_indices_.insert(
      {name, {IdentifierIndex::kConstant, constants_.size()}});
  constants_.emplace_back(name, type, std::move(init));
  return true;
}

bool Model::AddIntVariable(const std::string& name,
                           std::unique_ptr<const Expression>&& min,
                           std::unique_ptr<const Expression>&& max,
                           std::unique_ptr<const Expression>&& init,
                           std::vector<std::string>* errors) {
  return AddVariable(name, Type::INT, std::move(min), std::move(max),
                     std::move(init), errors);
}

bool Model::AddBoolVariable(const std::string& name,
                            std::unique_ptr<const Expression>&& init,
                            std::vector<std::string>* errors) {
  return AddVariable(name, Type::BOOL, Literal::New(false), Literal::New(true),
                     std::move(init), errors);
}

bool Model::AddVariable(const std::string& name, Type type,
                        std::unique_ptr<const Expression>&& min,
                        std::unique_ptr<const Expression>&& max,
                        std::unique_ptr<const Expression>&& init,
                        std::vector<std::string>* errors) {
  auto i = identifier_indices_.find(name);
  if (i != identifier_indices_.end()) {
    if (i->second.type == IdentifierIndex::kVariable) {
      errors->push_back(StrCat("duplicate variable ", name));
    } else {
      errors->push_back(StrCat("variable ", name, " previously defined as ",
                               IdentifierIndexTypeToString(i->second.type)));
    }
    return false;
  }
  const size_t index = variables_.size();
  identifier_indices_.insert({name, {IdentifierIndex::kVariable, index}});
  variables_.emplace_back(name, type, std::move(min), std::move(max),
                          std::move(init));
  if (current_module_ == kNoModule) {
    global_variables_.insert(index);
  } else {
    modules_[current_module_].add_variable(index);
  }
  return true;
}

bool Model::AddAction(const std::string& name,
                      std::vector<std::string>* errors) {
  if (!name.empty()) {
    auto result = identifier_indices_.insert(
        {name, {IdentifierIndex::kAction, actions_.size()}});
    if (!result.second &&
        result.first->second.type != IdentifierIndex::kAction) {
      errors->push_back(
          StrCat("action ", name, " previously defined as ",
                 IdentifierIndexTypeToString(result.first->second.type)));
      return false;
    }
    if (result.second) {
      actions_.push_back(name);
    }
  }
  return true;
}

bool Model::StartModule(const std::string& name) {
  CHECK_EQ(current_module_, kNoModule);
  current_module_ = modules_.size();
  if (!module_indices_.emplace(name, current_module_).second) {
    return false;
  }
  modules_.emplace_back(name);
  return true;
}

bool Model::AddCommand(Command&& command, std::vector<std::string>* errors) {
  CHECK_NE(current_module_, kNoModule);
  if (!AddAction(command.action(), errors)) {
    return false;
  }
  for (const auto& outcome : command.outcomes()) {
    for (const auto& update : outcome.updates()) {
      auto i = identifier_indices_.find(update.variable());
      if (i == identifier_indices_.end()) {
        errors->push_back(StrCat("command is updating undefined variable ",
                                 update.variable()));
        return false;
      } else if (i->second.type != IdentifierIndex::kVariable) {
        errors->push_back(StrCat("command is updating ",
                                 IdentifierIndexTypeToString(i->second.type),
                                 " ", update.variable()));
        return false;
      } else if (!command.action().empty() &&
                 global_variables_.find(i->second.index) !=
                     global_variables_.end()) {
        errors->push_back(StrCat("command with action ", command.action(),
                                 " is updating global variable ",
                                 update.variable()));
        return false;
      } else if (modules_[current_module_].variables().find(i->second.index) ==
                     modules_[current_module_].variables().end() &&
                 global_variables_.find(i->second.index) ==
                     global_variables_.end()) {
        errors->push_back(StrCat("command is updating variable ",
                                 update.variable(),
                                 " that belongs to a different module"));
        return false;
      }
    }
  }
  modules_[current_module_].add_command(std::move(command));
  return true;
}

namespace {

std::string RewriteIdentifier(
    const std::string& name,
    const std::map<std::string, std::string>& substitutions) {
  auto i = substitutions.find(name);
  return (i == substitutions.end()) ? name : i->second;
}

class ExpressionRewriter : public ExpressionVisitor {
 public:
  explicit ExpressionRewriter(
      const std::map<std::string, std::string>* substitutions);

  std::unique_ptr<const Expression> release_expr() { return std::move(expr_); }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;

  std::unique_ptr<const Expression> expr_;
  const std::map<std::string, std::string>* substitutions_;
};

ExpressionRewriter::ExpressionRewriter(
    const std::map<std::string, std::string>* substitutions)
    : substitutions_(substitutions) {}

void ExpressionRewriter::DoVisitLiteral(const Literal& expr) {
  expr_ = Literal::New(expr.value());
}

void ExpressionRewriter::DoVisitIdentifier(const Identifier& expr) {
  expr_ = Identifier::New(RewriteIdentifier(expr.name(), *substitutions_));
}

void ExpressionRewriter::DoVisitFunctionCall(const FunctionCall& expr) {
  UniquePtrVector<const Expression> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(release_expr());
  }
  expr_ = FunctionCall::New(expr.function(), std::move(arguments));
}

void ExpressionRewriter::DoVisitUnaryOperation(const UnaryOperation& expr) {
  expr.operand().Accept(this);
  expr_ = UnaryOperation::New(expr.op(), release_expr());
}

void ExpressionRewriter::DoVisitBinaryOperation(const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  auto operand1 = release_expr();
  expr.operand2().Accept(this);
  expr_ = BinaryOperation::New(expr.op(), std::move(operand1), release_expr());
}

void ExpressionRewriter::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  auto condition = release_expr();
  expr.if_branch().Accept(this);
  auto if_branch = release_expr();
  expr.else_branch().Accept(this);
  expr_ = Conditional::New(std::move(condition), std::move(if_branch),
                           release_expr());
}

void ExpressionRewriter::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {}

std::unique_ptr<const Expression> RewriteExpression(
    const Expression& expr,
    const std::map<std::string, std::string>& substitutions) {
  ExpressionRewriter rewriter(&substitutions);
  expr.Accept(&rewriter);
  return rewriter.release_expr();
}

class DistributionRewriter : public DistributionVisitor {
 public:
  explicit DistributionRewriter(
      const std::map<std::string, std::string>* substitutions);

  std::unique_ptr<const Distribution> release_dist() {
    return std::move(dist_);
  }

 private:
  void DoVisitMemoryless(const Memoryless& dist) override;
  void DoVisitWeibull(const Weibull& dist) override;
  void DoVisitLognormal(const Lognormal& dist) override;
  void DoVisitUniform(const Uniform& dist) override;

  std::unique_ptr<const Distribution> dist_;
  const std::map<std::string, std::string>* substitutions_;
};

DistributionRewriter::DistributionRewriter(
    const std::map<std::string, std::string>* substitutions)
    : substitutions_(substitutions) {}

void DistributionRewriter::DoVisitMemoryless(const Memoryless& dist) {
  dist_ = Memoryless::New(RewriteExpression(dist.weight(), *substitutions_));
}

void DistributionRewriter::DoVisitWeibull(const Weibull& dist) {
  dist_ = Weibull::New(RewriteExpression(dist.scale(), *substitutions_),
                       RewriteExpression(dist.shape(), *substitutions_));
}

void DistributionRewriter::DoVisitLognormal(const Lognormal& dist) {
  dist_ = Lognormal::New(RewriteExpression(dist.scale(), *substitutions_),
                         RewriteExpression(dist.shape(), *substitutions_));
}

void DistributionRewriter::DoVisitUniform(const Uniform& dist) {
  dist_ = Uniform::New(RewriteExpression(dist.low(), *substitutions_),
                       RewriteExpression(dist.high(), *substitutions_));
}

std::unique_ptr<const Distribution> RewriteDistribution(
    const Distribution& dist,
    const std::map<std::string, std::string>& substitutions) {
  DistributionRewriter rewriter(&substitutions);
  dist.Accept(&rewriter);
  return rewriter.release_dist();
}

std::vector<Update> RewriteUpdates(
    const std::vector<Update>& updates,
    const std::map<std::string, std::string>& substitutions) {
  std::vector<Update> new_updates;
  for (const Update& update : updates) {
    new_updates.emplace_back(
        RewriteIdentifier(update.variable(), substitutions),
        RewriteExpression(update.expr(), substitutions));
  }
  return std::move(new_updates);
}

std::vector<Outcome> RewriteOutcomes(
    const std::vector<Outcome>& outcomes,
    const std::map<std::string, std::string>& substitutions) {
  std::vector<Outcome> new_outcomes;
  for (const Outcome& outcome : outcomes) {
    new_outcomes.emplace_back(
        RewriteDistribution(outcome.delay(), substitutions),
        RewriteUpdates(outcome.updates(), substitutions));
  }
  return std::move(new_outcomes);
}

Command RewriteCommand(
    const Command& command,
    const std::map<std::string, std::string>& substitutions) {
  return Command(RewriteIdentifier(command.action(), substitutions),
                 RewriteExpression(command.guard(), substitutions),
                 RewriteOutcomes(command.outcomes(), substitutions));
}

}  // namespace

bool Model::AddFromModule(
    const std::string& from_name,
    const std::map<std::string, std::string>& substitutions,
    std::vector<std::string>* errors) {
  CHECK_NE(current_module_, kNoModule);
  auto i = module_indices_.find(from_name);
  if (i == module_indices_.end()) {
    errors->push_back(StrCat("undefined module ", from_name));
    return false;
  }
  const ParsedModule& from_module = modules_[i->second];
  // Add renamed module variables, maintaining the same order as in the source
  // module.
  for (int from_variable_index : from_module.variables()) {
    const auto& from_variable = variables_[from_variable_index];
    auto j = substitutions.find(from_variable.name());
    if (j == substitutions.end()) {
      errors->push_back(
          StrCat("missing substitution for variable ", from_variable.name()));
      return false;
    }
    const std::string& to_variable_name = j->second;
    if (!AddVariable(
            to_variable_name, from_variable.type(),
            RewriteExpression(from_variable.min(), substitutions),
            RewriteExpression(from_variable.max(), substitutions),
            from_variable.has_explicit_init()
                ? RewriteExpression(from_variable.init(), substitutions)
                : nullptr,
            errors)) {
      return false;
    }
  }
  // Add module commands.
  for (const auto& from_command : from_module.commands()) {
    if (!AddCommand(RewriteCommand(from_command, substitutions), errors)) {
      return false;
    }
  }

  return true;
}

void Model::EndModule() {
  CHECK_NE(current_module_, kNoModule);
  current_module_ = kNoModule;
}

size_t Model::ActionIndex(const std::string& name) const {
  auto i = identifier_indices_.find(name);
  CHECK(i != identifier_indices_.end());
  CHECK(i->second.type == IdentifierIndex::kAction);
  return i->second.index;
}

namespace {

bool IsUnitDistribution(const Distribution& dist) {
  const Memoryless* exp_dist = dynamic_cast<const Memoryless*>(&dist);
  if (exp_dist == NULL) {
    return false;
  }
  const Literal* rate_literal =
      dynamic_cast<const Literal*>(&exp_dist->weight());
  return rate_literal != NULL && rate_literal->value() == 1;
}

class DistributionCopier : public DistributionVisitor {
 public:
  std::unique_ptr<const Distribution> release_distribution() {
    return std::move(distribution_);
  }

 private:
  void DoVisitMemoryless(const Memoryless& distribution) override;
  void DoVisitWeibull(const Weibull& distribution) override;
  void DoVisitLognormal(const Lognormal& distribution) override;
  void DoVisitUniform(const Uniform& distribution) override;

  std::unique_ptr<const Distribution> distribution_;
};

std::unique_ptr<const Distribution> CopyDistribution(
    const Distribution& distribution) {
  DistributionCopier copier;
  distribution.Accept(&copier);
  return copier.release_distribution();
}

class ExpressionCopier : public ExpressionVisitor {
 public:
  std::unique_ptr<const Expression> release_expr() { return std::move(expr_); }

 private:
  void DoVisitLiteral(const Literal& expr) override;
  void DoVisitIdentifier(const Identifier& expr) override;
  void DoVisitFunctionCall(const FunctionCall& expr) override;
  void DoVisitUnaryOperation(const UnaryOperation& expr) override;
  void DoVisitBinaryOperation(const BinaryOperation& expr) override;
  void DoVisitConditional(const Conditional& expr) override;
  void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr) override;

  std::unique_ptr<const Expression> expr_;
};

std::unique_ptr<const Expression> CopyExpression(const Expression& expr) {
  ExpressionCopier copier;
  expr.Accept(&copier);
  return copier.release_expr();
}

void DistributionCopier::DoVisitMemoryless(const Memoryless& distribution) {
  distribution_ = Memoryless::New(CopyExpression(distribution.weight()));
}

void DistributionCopier::DoVisitWeibull(const Weibull& distribution) {
  distribution_ = Weibull::New(CopyExpression(distribution.scale()),
                               CopyExpression(distribution.shape()));
}

void DistributionCopier::DoVisitLognormal(const Lognormal& distribution) {
  distribution_ = Lognormal::New(CopyExpression(distribution.scale()),
                                 CopyExpression(distribution.shape()));
}

void DistributionCopier::DoVisitUniform(const Uniform& distribution) {
  distribution_ = Uniform::New(CopyExpression(distribution.low()),
                               CopyExpression(distribution.high()));
}

void ExpressionCopier::DoVisitLiteral(const Literal& expr) {
  expr_.reset(new Literal(expr.value()));
}

void ExpressionCopier::DoVisitIdentifier(const Identifier& expr) {
  expr_.reset(new Identifier(expr.name()));
}

void ExpressionCopier::DoVisitFunctionCall(const FunctionCall& expr) {
  UniquePtrVector<const Expression> arguments;
  for (const Expression& argument : expr.arguments()) {
    argument.Accept(this);
    arguments.push_back(release_expr());
  }
  expr_ = FunctionCall::New(expr.function(), std::move(arguments));
}

void ExpressionCopier::DoVisitUnaryOperation(const UnaryOperation& expr) {
  expr.operand().Accept(this);
  expr_ = UnaryOperation::New(expr.op(), release_expr());
}

void ExpressionCopier::DoVisitBinaryOperation(const BinaryOperation& expr) {
  expr.operand1().Accept(this);
  std::unique_ptr<const Expression> operand1 = release_expr();
  expr.operand2().Accept(this);
  expr_ = BinaryOperation::New(expr.op(), std::move(operand1), release_expr());
}

void ExpressionCopier::DoVisitConditional(const Conditional& expr) {
  expr.condition().Accept(this);
  std::unique_ptr<const Expression> condition = release_expr();
  expr.if_branch().Accept(this);
  std::unique_ptr<const Expression> if_branch = release_expr();
  expr.else_branch().Accept(this);
  expr_ = Conditional::New(std::move(condition), std::move(if_branch),
                           release_expr());
}

void ExpressionCopier::DoVisitProbabilityThresholdOperation(
    const ProbabilityThresholdOperation& expr) {
  LOG(FATAL) << "not an expression";
}

}  // namespace

/* Compiles the commands of this model. */
void Model::compile() {
  /*
   * Clear currently compiled commands.
   */
  for (const Command* command : commands_) {
    if (!command->action().empty()) {
      delete command;
    }
  }
  commands_.clear();
  /*
   * Process the commands of each module.
   */
  std::set<std::string> actions;
  SynchronizationMap synch_commands;
  for (auto mi = modules_.rbegin(); mi != modules_.rend(); mi++) {
    for (const Command& command : mi->commands()) {
      const std::string& action = command.action();
      if (!action.empty()) {
        /* Command is synchronized so store it for later processing. */
        synch_commands.insert({{&*mi, action}, &command});
        actions.insert(action);
      } else {
        /* Command is not synchronized. */
        commands_.push_back(&command);
      }
    }
  }
  /*
   * Add synchronized commands.
   */
  for (auto si = actions.begin(); si != actions.end(); si++) {
    for (auto mi = modules_.rbegin(); mi != modules_.rend(); mi++) {
      SynchronizationMapRange sri =
          synch_commands.equal_range(std::make_pair(&*mi, *si));
      for (SynchronizationMap::const_iterator smi = sri.first;
           smi != sri.second; smi++) {
        const Command& ci = *(*smi).second;
        for (auto mj = mi + 1; mj != modules_.rend(); mj++) {
          SynchronizationMapRange srj =
              synch_commands.equal_range(std::make_pair(&*mj, *si));
          for (SynchronizationMap::const_iterator smj = srj.first;
               smj != srj.second; smj++) {
            const Command& cj = *(*smj).second;
            /*
             * Synchronize ci with cj.
             */
            std::unique_ptr<const Expression> guard = BinaryOperation::New(
                BinaryOperator::AND, CopyExpression(ci.guard()),
                CopyExpression(cj.guard()));
            std::vector<Update> updates;
            for (const Update& update : ci.outcomes()[0].updates()) {
              updates.emplace_back(update.variable(),
                                   CopyExpression(update.expr()));
            }
            for (const Update& update : cj.outcomes()[0].updates()) {
              updates.emplace_back(update.variable(),
                                   CopyExpression(update.expr()));
            }
            std::unique_ptr<const Distribution> delay;
            if (IsUnitDistribution(ci.outcomes()[0].delay())) {
              delay = CopyDistribution(cj.outcomes()[0].delay());
            } else if (IsUnitDistribution(cj.outcomes()[0].delay())) {
              delay = CopyDistribution(ci.outcomes()[0].delay());
            } else {
              throw std::logic_error(
                  "at least one command in a"
                  " synchronization pair must have rate 1");
            }
            std::vector<Outcome> outcomes;
            outcomes.emplace_back(std::move(delay), std::move(updates));
            commands_.push_back(new Command(ci.action(), std::move(guard),
                                            std::move(outcomes)));
          }
        }
      }
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Model& m) {
  os << m.type();
  if (!m.constants().empty()) {
    os << std::endl;
    for (const auto& c : m.constants()) {
      os << std::endl << "const " << c.type() << " " << c.name();
      if (c.init() != nullptr) {
        os << " = " << *c.init();
      }
      os << ";";
    }
  }
  if (!m.global_variables().empty()) {
    os << std::endl;
    for (int variable_index : m.global_variables()) {
      const ParsedVariable& v = m.variables()[variable_index];
      os << std::endl << "global " << v.name();
      os << " : [" << v.min() << ".." << v.max() << "]";
      if (v.has_explicit_init()) {
        os << " init " << v.init();
      }
      os << ';';
    }
  }
  for (const auto& module : m.modules()) {
    os << std::endl << std::endl << "module " << module.name();
    for (int variable_index : module.variables()) {
      const ParsedVariable& v = m.variables()[variable_index];
      os << std::endl << "  " << v.name();
      os << " : [" << v.min() << ".." << v.max() << "]";
      if (v.has_explicit_init()) {
        os << " init " << v.init();
      }
      os << ';';
    }
    if (!module.variables().empty() && !module.commands().empty()) {
      os << std::endl;
    }
    for (const auto& command : module.commands()) {
      os << std::endl << "  " << command << ';';
    }
    os << std::endl << "endmodule";
  }
  return os;
}

DecisionDiagramModel::DecisionDiagramModel(
    const DecisionDiagramManager* manager, const ADD& rate_matrix,
    const BDD& reachable_states, const BDD& initial_state,
    int initial_state_index, ODDNode* odd)
    : manager_(manager),
      rate_matrix_(rate_matrix),
      reachable_states_(reachable_states),
      initial_state_(initial_state),
      initial_state_index_(initial_state_index),
      odd_(odd) {}

DecisionDiagramModel::~DecisionDiagramModel() { free_odd(odd_); }

namespace {

int GetInitIndex(const DecisionDiagramManager& dd_man, const BDD& initial_state,
                 ODDNode* odd) {
  int init_index = 0;
  ADD init_add(initial_state);
  DdNode* d = init_add.get();
  ODDNode* o = odd;
  size_t nvars = dd_man.GetVariableCount() / 2;
  for (size_t i = 1; i <= nvars; i++) {
    bool bit;
    if (i == nvars) {
      bit = (Cudd_T(d) == Cudd_ReadOne(dd_man.manager()));
    } else {
      DdNode* t = Cudd_T(d);
      if (Cudd_IsConstant(t)) {
        bit = false;
        d = Cudd_E(d);
      } else {
        bit = true;
        d = t;
      }
    }
    if (bit) {
      init_index += o->eoff;
      o = o->t;
    } else {
      o = o->e;
    }
  }
  return init_index;
}

}  // namespace

DecisionDiagramModel DecisionDiagramModel::Create(
    const DecisionDiagramManager* manager, size_t moments, const Model& model,
    const CompiledModel& compiled_model,
    const std::map<std::string, IdentifierInfo>& identifiers_by_name) {
  CHECK(manager);
  std::cout << "Building model...";
  /* BDD for initial state. */
  BDD init_bdd = manager->GetConstant(true);
  for (size_t i = compiled_model.variables().size(); i > 0; --i) {
    const auto& v = compiled_model.variables()[i - 1];
    const int init_value = compiled_model.init_values()[i - 1];
    auto j = identifiers_by_name.find(v.name());
    CHECK(j != identifiers_by_name.end());
    const IdentifierInfo& p = j->second;
    CHECK(p.is_variable());
    BDD dds = variable_mtbdd(*manager, p.min_value(), p.low_bit(), p.high_bit())
                  .Interval(init_value, init_value);
    init_bdd = dds && init_bdd;
  }
  /*
   * Generate phase-type distributions for commands with
   * non-exponential delay.
   */
  size_t nvars = manager->GetVariableCount() / 2;
  std::map<int, PHData> ph_commands;
  for (int i = model.commands().size() - 1; i >= 0; i--) {
    const Command& command = *model.commands()[i];
    const Distribution& dist = command.outcomes()[0].delay();
    if (typeid(dist) != typeid(Memoryless)) {
      PHData data(*manager);
      switch (moments) {
        case 1:
          data.params = MatchOneMoment(dist);
          break;
        case 2:
          data.params2 = MatchTwoMoments(dist);
          data.params.n = 0;
          break;
        case 3:
        default:
          data.params = MatchThreeMoments(dist);
          break;
      }
      int high = ph_phases(data) - 1;
      if (high > 0) {
        data.low_bit = nvars;
        data.high_bit = data.low_bit + Log2(high);
        nvars = data.high_bit + 1;
        for (int b = data.low_bit; b <= data.high_bit; ++b) {
          Cudd_bddNewVar(manager->manager());
          Cudd_bddNewVar(manager->manager());
        }
        ADD ddv = variable_mtbdd(*manager, 0, data.low_bit, data.high_bit);
        init_bdd = ddv.Interval(0, 0) && init_bdd;
        ADD ddvp =
            variable_primed_mtbdd(*manager, 0, data.low_bit, data.high_bit);
        BDD ddid = identity_bdd(*manager, 0, data.low_bit, data.high_bit);
        /*
         * Constructs BDD representing phase update:
         *
         *   (!phi -> s=0) & (phi' -> s'=s) & (!phi' -> s'=0)
         */
        data.update_bdd =
            (BDD(mtbdd(*manager, identifiers_by_name, command.guard())) ||
             ddv.Interval(0, 0)) &&
            (BDD(primed_mtbdd(*manager, identifiers_by_name,
                              command.guard())) ||
             ddvp.Interval(0, 0)) &&
            (!BDD(primed_mtbdd(*manager, identifiers_by_name,
                               command.guard())) ||
             ddid);
      }
      ph_commands.insert(std::make_pair(i, data));
    }
  }
  /*
   * Compute rate matrix for all commands.
   */
  ADD ddR = manager->GetConstant(0);
  for (int i = model.commands().size() - 1; i >= 0; i--) {
    const Command& command = *model.commands()[i];
    if (VLOG_IS_ON(2)) {
      LOG(INFO) << "processing " << command;
    }
    /* BDD for guard. */
    BDD ddg = BDD(mtbdd(*manager, identifiers_by_name, command.guard()));
    /* BDD for command. */
    std::set<std::string> updated_variables;
    BDD ddc =
        command_bdd(*manager, identifiers_by_name, command, &updated_variables);
    const Memoryless* exp_delay =
        dynamic_cast<const Memoryless*>(&command.outcomes()[0].delay());
    PHData* ph_data = (exp_delay != NULL) ? NULL : &ph_commands.find(i)->second;
    if (ph_data != NULL && ph_data->params.n == 0) {
      if (VLOG_IS_ON(2)) {
        LOG(INFO) << "n=" << ph_data->params2.n << " p=" << ph_data->params2.p
                  << " r1=" << ph_data->params2.r1
                  << " r2=" << ph_data->params2.r2;
      }
      /*
       * Event 1: phi & s=0 => s'=1
       */
      ADD ddv =
          variable_mtbdd(*manager, 0, ph_data->low_bit, ph_data->high_bit);
      BDD dds = ddv.Interval(0, 0);
      ADD ddvp = variable_primed_mtbdd(*manager, 0, ph_data->low_bit,
                                       ph_data->high_bit);
      BDD ddu = dds && ddvp.Interval(1, 1) && ddg;
      ddu = variable_updates(*manager, ddu, model, identifiers_by_name, {}, i,
                             ph_commands);
      ADD ddr = manager->GetConstant(ph_data->params2.p * ph_data->params2.r1);
      ADD ddq = ADD(ddu) * ddr;
      if (VLOG_IS_ON(2)) {
        Cudd_PrintDebug(manager->manager(), ddq.get(),
                        manager->GetVariableCount(), 2);
      }
      ddR = ddq + ddR;
      /*
       * Event 2: phi & s=0 => s'=0 & effects
       */
      BDD ddp = ddvp.Interval(0, 0);
      ddu = ddc && dds && ddp;
      ddu = variable_updates(*manager, ddu, model, identifiers_by_name,
                             updated_variables, i, ph_commands);
      ddr = manager->GetConstant((1.0 - ph_data->params2.p) *
                                 ph_data->params2.r1);
      ddq = ADD(ddu) * ddr;
      if (VLOG_IS_ON(2)) {
        Cudd_PrintDebug(manager->manager(), ddq.get(),
                        manager->GetVariableCount(), 2);
      }
      ddR = ddq + ddR;
      /*
       * Event 3: phi & s=n-1 => s'=0 & effects
       */
      dds = ddv.Interval(ph_data->params2.n - 1, ph_data->params2.n - 1);
      ddu = ddc && dds && ddp;
      ddu = variable_updates(*manager, ddu, model, identifiers_by_name,
                             updated_variables, i, ph_commands);
      ddr = manager->GetConstant(ph_data->params2.r2);
      ddq = ADD(ddu) * ddr;
      if (VLOG_IS_ON(2)) {
        Cudd_PrintDebug(manager->manager(), ddq.get(),
                        manager->GetVariableCount(), 2);
      }
      ddR = ddq + ddR;
      if (ph_data->params2.n > 2) {
        /*
         * Event 4: phi & s>=1 & s<=n-2 => s'=s+1
         */
        dds = ddv.Interval(1, ph_data->params2.n - 2);
        ddu = dds && ddvp == ddv + manager->GetConstant(1) && ddg;
        ddu = variable_updates(*manager, ddu, model, identifiers_by_name, {}, i,
                               ph_commands);
        ddr = manager->GetConstant(ph_data->params2.r1);
        ddq = ADD(ddu) * ddr;
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(), ddq.get(),
                          manager->GetVariableCount(), 2);
        }
        ddR = ddq + ddR;
      }
    } else {
      if (ph_data != NULL && VLOG_IS_ON(2)) {
        LOG(INFO) << "n=" << ph_data->params.n;
        if (ph_data->params.n > 2) {
          LOG(INFO) << "re=" << ph_data->params.re;
        }
        LOG(INFO) << "pc=" << ph_data->params.pc
                  << "rc1=" << ph_data->params.rc1;
        if (ph_data->params.pc > 0.0) {
          LOG(INFO) << "rc2=" << ph_data->params.rc2;
        }
      }
      if (ph_data != NULL && ph_data->params.n > 2) {
        /*
         * Event for phase transition in Erlang part of phase-type
         * distribution:
         *
         *   phi & s<n-2 => s'=s+1
         */
        ADD ddv =
            variable_mtbdd(*manager, 0, ph_data->low_bit, ph_data->high_bit);
        BDD dds = ddv.Interval(0, ph_data->params.n - 3);
        ADD ddvp = variable_primed_mtbdd(*manager, 0, ph_data->low_bit,
                                         ph_data->high_bit);
        ADD ddp = ddv + manager->GetConstant(1);
        BDD ddu = dds && ddvp == ddp && ddg;
        ddu = variable_updates(*manager, ddu, model, identifiers_by_name, {}, i,
                               ph_commands);
        ADD ddq = ADD(ddu) * manager->GetConstant(ph_data->params.re);
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(), ddq.get(),
                          manager->GetVariableCount(), 2);
        }
        ddR = ddq + ddR;
      }
      if (ph_data != NULL && ph_data->params.pc > 0.0) {
        /*
         * Event 1 for phase transition in Coxian part of phase-type
         * distribution:
         *
         *   phi & s=n-2 => s'=n-1
         */
        ADD ddv =
            variable_mtbdd(*manager, 0, ph_data->low_bit, ph_data->high_bit);
        BDD dds = ddv.Interval(ph_data->params.n - 2, ph_data->params.n - 2);
        ADD ddvp = variable_primed_mtbdd(*manager, 0, ph_data->low_bit,
                                         ph_data->high_bit);
        BDD ddu = ddvp.Interval(ph_data->params.n - 1, ph_data->params.n - 1);
        ddu = dds && ddu && ddg;
        ddu = variable_updates(*manager, ddu, model, identifiers_by_name, {}, i,
                               ph_commands);
        ADD ddr =
            manager->GetConstant(ph_data->params.pc * ph_data->params.rc1);
        ADD ddq = ADD(ddu) * ddr;
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(), ddq.get(),
                          manager->GetVariableCount(), 2);
        }
        ddR = ddq + ddR;
        /*
         * Event 2 for phase transition in Coxian part of phase-type
         * distribution:
         *
         *   phi & s=n-2 => s'=0 & effects
         */
        BDD ddp = ddvp.Interval(0, 0);
        ddu = ddc && dds && ddp;
        ddu = variable_updates(*manager, ddu, model, identifiers_by_name,
                               updated_variables, i, ph_commands);
        ddr = manager->GetConstant((1.0 - ph_data->params.pc) *
                                   ph_data->params.rc1);
        ddq = ADD(ddu) * ddr;
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(), ddq.get(),
                          manager->GetVariableCount(), 2);
        }
        ddR = ddq + ddR;
        /*
         * Event 3 for phase transition in Coxian part of phase-type
         * distribution:
         *
         *   phi & s=n-1 => s'=0 & effects
         */
        dds = ddv.Interval(ph_data->params.n - 1, ph_data->params.n - 1);
        ddu = ddc && dds && ddp;
        ddu = variable_updates(*manager, ddu, model, identifiers_by_name,
                               updated_variables, i, ph_commands);
        ddq = ADD(ddu) * manager->GetConstant(ph_data->params.rc2);
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(), ddq.get(),
                          manager->GetVariableCount(), 2);
        }
        ddR = ddq + ddR;
      } else {
        /*
         * Event for exponential (or 1-phase Coxian) distribution.
         */
        BDD dda = manager->GetConstant(false);
        if (ph_data != NULL && ph_data->low_bit >= 0) {
          /* Coxian: s=n-2 => s'=0 */
          BDD dds =
              variable_mtbdd(*manager, 0, ph_data->low_bit, ph_data->high_bit)
                  .Interval(ph_data->params.n - 2, ph_data->params.n - 2);
          BDD ddp = variable_primed_mtbdd(*manager, 0, ph_data->low_bit,
                                          ph_data->high_bit).Interval(0, 0);
          dda = dds && ddp;
          dda = variable_updates(*manager, dda, model, identifiers_by_name,
                                 updated_variables, i, ph_commands);
        } else {
          dda = manager->GetConstant(true);
          dda = variable_updates(*manager, dda, model, identifiers_by_name,
                                 updated_variables, i, ph_commands);
        }
        BDD ddu = ddc && dda;
        ADD ddr =
            (exp_delay != NULL)
                ? mtbdd(*manager, identifiers_by_name, exp_delay->weight())
                : manager->GetConstant(ph_data->params.rc1);
        ADD ddq = ADD(ddu) * ddr;
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(), ddq.get(),
                          manager->GetVariableCount(), 2);
        }
        ddR = ddq + ddR;
      }
    }
  }
  std::cout << manager->GetVariableCount() << " variables." << std::endl;
  /*
   * Reachability analysis.
   */
  BDD reach_bdd = reachability_bdd(*manager, init_bdd, ddR);
  ADD reach_add = ADD(reach_bdd);
  ADD rate_matrix = reach_add * ddR;
  /* Build ODD. */
  ODDNode* odd = build_odd(*manager, reach_add);
  int init_index = GetInitIndex(*manager, init_bdd, odd);
  return DecisionDiagramModel(manager, rate_matrix, reach_bdd, init_bdd,
                              init_index, odd);
}
