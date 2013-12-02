/*
 * Mixed model checking of CSL formulas.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
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

#include <utility>
#include <vector>

#include "models.h"
#include "states.h"
#include "src/compiled-property.h"

#include "cudd.h"
#include "glog/logging.h"

class CompiledPropertyMixedVerifier : public CompiledPropertyVisitor {
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

void CompiledPropertyMixedVerifier::DoVisitCompiledAndProperty(
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

/* Verifies this state formula using the mixed engine. */
bool Conjunction::verify(const DecisionDiagramManager& dd_man,
                         const Model& model, const State& state,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  ModelCheckingParams nested_params = params;
  nested_params.alpha = params.alpha / conjuncts().size();
  for (auto fi = conjuncts().rbegin(); fi != conjuncts().rend(); fi++) {
    if (!(*fi)->verify(dd_man, model, state, nested_params, stats)) {
      return false;
    }
  }
  return true;
}


/* ====================================================================== */
/* Disjunction */

/* Verifies this state formula using the mixed engine. */
bool Disjunction::verify(const DecisionDiagramManager& dd_man,
                         const Model& model, const State& state,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  ModelCheckingParams nested_params = params;
  nested_params.beta = params.beta / disjuncts().size();
  for (auto fi = disjuncts().rbegin(); fi != disjuncts().rend(); fi++) {
    if ((*fi)->verify(dd_man, model, state, nested_params, stats)) {
      return true;
    }
  }
  return false;
}


/* ====================================================================== */
/* Negation */

void CompiledPropertyMixedVerifier::DoVisitCompiledNotProperty(
    const CompiledNotProperty& property) {
  std::swap(params_.alpha, params_.beta);
  property.operand().Accept(this);
  result_ = !result_;
  std::swap(params_.alpha, params_.beta);
}

/* Verifies this state formula using the mixed engine. */
bool Negation::verify(const DecisionDiagramManager& dd_man,
                      const Model& model, const State& state,
                      const ModelCheckingParams& params,
                      ModelCheckingStats* stats) const {
  ModelCheckingParams nested_params = params;
  std::swap(nested_params.alpha, nested_params.beta);
  return !negand().verify(dd_man, model, state, nested_params, stats);
}


/* ====================================================================== */
/* Implication */

/* Verifies this state formula using the mixed engine. */
bool Implication::verify(const DecisionDiagramManager& dd_man,
                         const Model& model, const State& state,
                         const ModelCheckingParams& params,
                         ModelCheckingStats* stats) const {
  ModelCheckingParams nested_params = params;
  nested_params.beta = params.beta / 2;
  std::swap(nested_params.alpha, nested_params.beta);
  if (!antecedent().verify(dd_man, model, state, nested_params, stats)) {
    return true;
  } else {
    std::swap(nested_params.alpha, nested_params.beta);
    return consequent().verify(dd_man, model, state, nested_params, stats);
  }
}


/* ====================================================================== */
/* Probabilistic */

void CompiledPropertyMixedVerifier::DoVisitCompiledProbabilisticProperty(
    const CompiledProbabilisticProperty& property) {
  // TODO(hlsyounes): implement.
}

/* Verifies this state formula using the mixed engine. */
bool Probabilistic::verify(const DecisionDiagramManager& dd_man,
                           const Model& model, const State& state,
                           const ModelCheckingParams& params,
                           ModelCheckingStats* stats) const {
  formula_level_++;
  bool res = formula().verify(dd_man, model, state, threshold(), strict(),
			      params, stats);
  formula_level_--;
  return res;
}


/* ====================================================================== */
/* Comparison */

void CompiledPropertyMixedVerifier::DoVisitCompiledExpressionProperty(
    const CompiledExpressionProperty& property) {
  result_ = evaluator_->EvaluateIntExpression(property.expr(), *state_);
}

/* Verifies this state formula using the mixed engine. */
bool Comparison::verify(const DecisionDiagramManager& dd_man,
                        const Model& model, const State& state,
                        const ModelCheckingParams& params,
                        ModelCheckingStats* stats) const {
  return holds(state.values());
}


/* ====================================================================== */
/* Until */

/* Generates a sample for this path formula. */
bool Until::sample(const DecisionDiagramManager& dd_man, const Model& model,
                   const State& state, DdNode* dd1, DdNode* dd2,
                   ModelCheckingStats* stats) const {
  double t = 0.0;
  size_t path_length = 1;
  State curr_state = state;
  bool result = false;
  double t_max = max_time().value<double>();
  while (true) {
    if (t <= t_max) {
      DdNode* dds = model.state_bdd(dd_man, curr_state.values()).release();
      DdNode* sol = Cudd_bddAnd(dd_man.manager(), dd2, dds);
      Cudd_Ref(sol);
      if (sol != Cudd_ReadLogicZero(dd_man.manager())) {
        result = true;
        Cudd_RecursiveDeref(dd_man.manager(), dds);
        Cudd_RecursiveDeref(dd_man.manager(), sol);
        break;
      }
      Cudd_RecursiveDeref(dd_man.manager(), sol);
      sol = Cudd_bddAnd(dd_man.manager(), dd1, dds);
      Cudd_Ref(sol);
      if (sol == Cudd_ReadLogicZero(dd_man.manager())) {
        result = false;
        Cudd_RecursiveDeref(dd_man.manager(), dds);
        Cudd_RecursiveDeref(dd_man.manager(), sol);
        break;
      }
      Cudd_RecursiveDeref(dd_man.manager(), dds);
      Cudd_RecursiveDeref(dd_man.manager(), sol);
    } else {
      result = false;
      break;
    }
    if (VLOG_IS_ON(3)) {
      LOG(INFO) << "t = " << t << ": " << curr_state.ToString();
    }
    State next_state = curr_state.Next();
    t += next_state.time() - curr_state.time();
    if (t <= t_max) {
      path_length++;
    }
    curr_state = std::move(next_state);
  }
  if (VLOG_IS_ON(3)) {
    LOG(INFO) << "t = " << t << ": " << curr_state.ToString();
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
