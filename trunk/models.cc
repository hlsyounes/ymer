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

#include "distributions.h"
#include "formulas.h"

#include "cudd.h"
#include "glog/logging.h"

ParsedVariable::ParsedVariable(
    const std::string& name, int min_value, int max_value, int init_value)
    : name_(name),
      min_value_(min_value), max_value_(max_value), init_value_(init_value) {
}

/* ====================================================================== */
/* SynchronizationMap */

/*
 * A synchronization map.
 */
struct SynchronizationMap
  : public std::multimap<std::pair<const Module*, size_t>, const Command*> {
};

/* Range for synchronization map. */
typedef std::pair<SynchronizationMap::const_iterator,
		  SynchronizationMap::const_iterator> SynchronizationMapRange;


/* ====================================================================== */
/* PHData */

/*
 * Data for phase-type distribution.
 */
struct PHData {
  explicit PHData(const DecisionDiagramManager& manager)
      : low_bit(-1), update_bdd(manager.GetConstant(true)) {
  }

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

/* Returns the number of phases of the given PH distribution. */
int ph_phases(const PHData& data) {
  if (data.params.n == 0) {
    return data.params2.n;
  } else {
    return data.params.n - ((data.params.pc > 0.0) ? 0 : 1);
  }
}

/*
 * Returns the EC distribution matching the first moment of the given
 * distribution.
 */
void match_first_moment(ECParameters& params, const Distribution& dist) {
  params.n = 2;
  std::vector<double> mu;
  dist.moments(mu, 1);
  params.rc1 = 1.0/mu[0];
  params.pc = 0.0;
}

/* Returns a reachability BDD for the given initial state and rate
   matrix. */
BDD reachability_bdd(const DecisionDiagramManager& dd_man,
                     const BDD& init, const ADD& rates) {
  std::cout << "Computing reachable states";
  /*
   * Precompute variable permutations and cubes.
   */
  size_t nvars = dd_man.GetVariableCount()/2;
  std::vector<int> row_to_col(2*nvars);
  std::vector<int> col_to_row(2*nvars);
  for (size_t i = 0; i < nvars; i++) {
    row_to_col[2*i] = 2*i + 1;
    row_to_col[2*i + 1] = 2*i + 1;
    col_to_row[2*i] = 2*i;
    col_to_row[2*i + 1] = 2*i;
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

// Returns a BDD representing identity between the `current state' and `next
// state' versions of the given variable.
BDD identity_bdd(const DecisionDiagramManager& manager,
                 int low, int low_bit, int high_bit) {
  return variable_mtbdd(manager, low, low_bit, high_bit)
      == variable_primed_mtbdd(manager, low, low_bit, high_bit);
}

/* Returns a BDD representing the conjunction of dd_start with the
   identity BDDs for the given variables. */
BDD variable_identities(
    const DecisionDiagramManager& dd_man,
    const BDD& dd_start,
    const Model& model,
    const std::map<std::string, VariableProperties>& variable_properties,
    const std::set<int>& variables,
    const std::set<std::string>& excluded) {
  BDD ddu = dd_start;
  for (std::set<int>::const_reverse_iterator i = variables.rbegin();
       i != variables.rend(); i++) {
    const ParsedVariable& v = model.variables()[*i];
    if (excluded.find(v.name()) == excluded.end()) {
      auto j = variable_properties.find(v.name());
      CHECK(j != variable_properties.end());
      const VariableProperties& p = j->second;
      ddu = identity_bdd(dd_man, v.min_value(), p.low_bit(), p.high_bit())
          && ddu;
    }
  }
  return ddu;
}

// Returns a BDD representation of the given update.
BDD update_bdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Update& update) {
  auto i = variable_properties.find(update.variable());
  const VariableProperties& p = i->second;
  return variable_primed_mtbdd(manager,
                               p.min_value(), p.low_bit(), p.high_bit())
      == mtbdd(manager, variable_properties, update.expr());
}

// Returns a BDD representation of the given command and fills the provided
// set with variables updated by the command.
BDD command_bdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Command& command,
    std::set<std::string>* updated_variables) {
  BDD ddu = manager.GetConstant(true);
  for (const Update* update : command.updates()) {
    ddu = update_bdd(manager, variable_properties, *update) && ddu;
    updated_variables->insert(update->variable());
  }
  return BDD(mtbdd(manager, variable_properties, command.guard())) && ddu;
}


// Returns a BDD representing the conjunction of dd_start with the BDDs for
// updates of all variables for the given model not explicitly mentioned.
BDD variable_updates(
    const DecisionDiagramManager& manager,
    const BDD& dd_start,
    const Model& model,
    const std::map<std::string, VariableProperties>& variable_properties,
    const std::set<const Module*>& touched_modules,
    const std::set<std::string>& updated_variables,
    int command_index,
    const std::map<int, PHData>& ph_commands) {
  BDD ddu = dd_start;
  // Conjunction with identity BDD for untouched module variables.
  for (int i = model.modules().size() - 1; i >= 0; --i) {
    const Module* module = model.modules()[i];
    if (touched_modules.find(module) != touched_modules.end()) {
      ddu = variable_identities(manager, ddu, model, variable_properties,
                                model.module_variables(i), updated_variables);
    } else {
      for (std::set<int>::const_reverse_iterator vi =
               model.module_variables(i).rbegin();
           vi != model.module_variables(i).rend(); ++vi) {
        const ParsedVariable& v = model.variables()[*vi];
        auto i = variable_properties.find(v.name());
        CHECK(i != variable_properties.end());
        const VariableProperties& p = i->second;
        ddu = identity_bdd(manager, v.max_value(), p.low_bit(), p.high_bit())
            && ddu;
      }
    }
  }
  // Conjunction with identity BDD for untouched global variables.
  ddu = variable_identities(manager, ddu, model, variable_properties,
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

/* Constructs a model. */
Model::Model()
    : current_module_(kNoModule) {
}


/* Deletes this model. */
Model::~Model() {
  for (const Command* command : commands()) {
    if (command->synch() != 0) {
      delete command;
    }
  }
  for (const Module* module : modules()) {
    delete module;
  }
}

void Model::AddIntVariable(
    const std::string& name, int min_value, int max_value, int init_value) {
  const int index = variables_.size();
  variables_.push_back(ParsedVariable(name, min_value, max_value, init_value));
  if (current_module_ == kNoModule) {
    global_variables_.insert(index);
  } else {
    module_variables_[current_module_].insert(index);
  }
}

void Model::OpenModuleScope() {
  CHECK_EQ(current_module_, kNoModule);
  current_module_ = module_variables_.size();
  module_variables_.emplace_back();
}

void Model::CloseModuleScope() {
  CHECK_NE(current_module_, kNoModule);
  current_module_ = kNoModule;
}


/* Adds a module to this model. */
void Model::add_module(const Module& module) {
  modules_.push_back(&module);
}

namespace {

bool IsUnitDistribution(const Distribution& dist) {
  const Exponential* exp_dist = dynamic_cast<const Exponential*>(&dist);
  if (exp_dist == NULL) {
    return false;
  }
  const Literal* rate_literal = dynamic_cast<const Literal*>(&exp_dist->rate());
  return rate_literal != NULL && rate_literal->value() == 1;
}

class DistributionCopier : public DistributionVisitor {
 public:
  DistributionCopier();

  ~DistributionCopier();

  const Distribution* release_distribution();

 private:
  virtual void DoVisitExponential(const Exponential& distribution);
  virtual void DoVisitWeibull(const Weibull& distribution);
  virtual void DoVisitLognormal(const Lognormal& distribution);
  virtual void DoVisitUniform(const Uniform& distribution);

  const Distribution* distribution_;
};

const Distribution* CopyDistribution(const Distribution& distribution) {
  DistributionCopier copier;
  distribution.Accept(&copier);
  return copier.release_distribution();
}

class ExpressionCopier : public ExpressionVisitor {
 public:
  std::unique_ptr<const Expression> release_expr() { return std::move(expr_); }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);
  virtual void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr);

  std::unique_ptr<const Expression> expr_;
};

std::unique_ptr<const Expression> CopyExpression(const Expression& expr) {
  ExpressionCopier copier;
  expr.Accept(&copier);
  return copier.release_expr();
}

DistributionCopier::DistributionCopier()
    : distribution_(NULL) {
}

DistributionCopier::~DistributionCopier() {
  delete distribution_;
}

const Distribution* DistributionCopier::release_distribution() {
  const Distribution* distribution = distribution_;
  distribution_ = NULL;
  return distribution;
}

void DistributionCopier::DoVisitExponential(const Exponential& distribution) {
  distribution_ = Exponential::make(CopyExpression(distribution.rate()));
}

void DistributionCopier::DoVisitWeibull(const Weibull& distribution) {
  distribution_ = Weibull::make(CopyExpression(distribution.scale()),
                                CopyExpression(distribution.shape()));
}

void DistributionCopier::DoVisitLognormal(const Lognormal& distribution) {
  distribution_ = Lognormal::make(CopyExpression(distribution.scale()),
                                  CopyExpression(distribution.shape()));
}

void DistributionCopier::DoVisitUniform(const Uniform& distribution) {
  distribution_ = Uniform::make(CopyExpression(distribution.low()),
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
  expr_ = Conditional::New(std::move(condition),
                           std::move(if_branch), release_expr());
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
  for (const Command* command : commands()) {
    if (command->synch() != 0) {
      delete command;
    }
  }
  commands_.clear();
  command_modules_.clear();
  /*
   * Process the commands of each module.
   */
  std::set<size_t> synchs;
  SynchronizationMap synch_commands;
  for (auto mi = modules().rbegin(); mi != modules().rend(); mi++) {
    for (const Command* command : (*mi)->commands()) {
      size_t synch = command->synch();
      if (synch != 0) {
	/* Command is synchronized so store it for later processing. */
	synch_commands.insert({ { *mi, synch }, command });
	synchs.insert(synch);
      } else { /* synch == 0 */
	/* Command is not synchronized. */
	commands_.push_back(command);
	command_modules_.push_back({});
	command_modules_.back().insert(*mi);
      }
    }
  }
  /*
   * Add synchronized commands.
   */
  for (std::set<size_t>::const_iterator si = synchs.begin();
       si != synchs.end(); si++) {
    for (auto mi = modules().rbegin(); mi != modules().rend(); mi++) {
      SynchronizationMapRange sri =
	synch_commands.equal_range(std::make_pair(*mi, *si));
      for (SynchronizationMap::const_iterator smi = sri.first;
	   smi != sri.second; smi++) {
	const Command& ci = *(*smi).second;
	for (auto mj = mi + 1; mj != modules().rend(); mj++) {
	  SynchronizationMapRange srj =
	    synch_commands.equal_range(std::make_pair(*mj, *si));
	  for (SynchronizationMap::const_iterator smj = srj.first;
	       smj != srj.second; smj++) {
	    const Command& cj = *(*smj).second;
	    /*
	     * Synchronize ci with cj.
	     */
            std::unique_ptr<const Expression> guard =
                BinaryOperation::New(BinaryOperator::AND,
                                     CopyExpression(ci.guard()),
                                     CopyExpression(cj.guard()));
	    Command* c;
	    if (IsUnitDistribution(ci.delay())) {
	      c = new Command(*si, std::move(guard),
                              CopyDistribution(cj.delay()));
	    } else if (IsUnitDistribution(cj.delay())) {
              c = new Command(*si, std::move(guard),
                              CopyDistribution(ci.delay()));
	    } else {
	      throw std::logic_error("at least one command in a"
				     " synchronization pair must have rate 1");
	    }
	    for (const Update* update : ci.updates()) {
	      c->add_update(new Update(update->variable(),
                                       CopyExpression(update->expr())));
	    }
	    for (const Update* update : cj.updates()) {
	      c->add_update(new Update(update->variable(),
                                       CopyExpression(update->expr())));
	    }
	    commands_.push_back(c);
	    command_modules_.push_back({});
	    command_modules_.back().insert(*mi);
	    command_modules_.back().insert(*mj);
	  }
	}
      }
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Model& m) {
  os << m.type();
  std::set<int>::const_iterator vi =
      m.global_variables().begin();
  if (vi != m.global_variables().end()) {
    os << std::endl;
    for (; vi != m.global_variables().end(); vi++) {
      const ParsedVariable& v = m.variables()[*vi];
      os << std::endl << "global " << v.name();
      os << " : [" << v.min_value() << ".." << v.max_value() << "]";
      if (v.min_value() != v.init_value()) {
	os << " init " << v.init_value();
      }
      os << ';';
    }
  }
  for (size_t i = 0; i < m.modules().size(); ++i) {
    const Module& module = *m.modules()[i];
    os << std::endl << std::endl << "module M" << i;
    vi = m.module_variables(i).begin();
    if (vi != m.module_variables(i).end()) {
      os << std::endl;
      for (; vi != m.module_variables(i).end(); vi++) {
	const ParsedVariable& v = m.variables()[*vi];
	os << std::endl << "  " << v.name();
	os << " : [" << v.min_value() << ".." << v.max_value() << "]";
	if (v.min_value() != v.init_value()) {
	  os << " init " << v.init_value();
	}
	os << ';';
      }
    }
    auto ci = module.commands().begin();
    if (ci != module.commands().end()) {
      os << std::endl;
      for (; ci != module.commands().end(); ci++) {
	os << std::endl << "  " << **ci << ';';
      }
    }
    os << std::endl << std::endl << "endmodule";
  }
  return os;
}

VariableProperties::VariableProperties(int min_value, int low_bit, int high_bit)
    : min_value_(min_value), low_bit_(low_bit), high_bit_(high_bit) {
  CHECK_LE(low_bit, high_bit);
}

DecisionDiagramModel::DecisionDiagramModel(
    const DecisionDiagramManager* manager,
    const ADD& rate_matrix, const BDD& reachable_states,
    const BDD& initial_state, int initial_state_index, ODDNode* odd)
    : manager_(manager),
      rate_matrix_(rate_matrix),
      reachable_states_(reachable_states),
      initial_state_(initial_state),
      initial_state_index_(initial_state_index),
      odd_(odd) {}

DecisionDiagramModel::~DecisionDiagramModel() {
  free_odd(odd_);
}

namespace {

int GetInitIndex(const DecisionDiagramManager& dd_man,
                 const BDD& initial_state, ODDNode* odd) {
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
    const DecisionDiagramManager* manager, size_t moments, const Model& model) {
  CHECK(manager);
  std::cout << "Building model...";
  /*
   * Precomute DDs for variables and modules.
   */
  std::map<std::string, VariableProperties> variable_properties;
  int low_bit = 0;
  for (auto i = model.variables().begin(); i != model.variables().end(); ++i) {
    const ParsedVariable& v = *i;
    int high_bit = low_bit + Log2(v.max_value() - v.min_value());
    variable_properties.insert(std::make_pair(
        v.name(), VariableProperties(v.min_value(), low_bit, high_bit)));
    low_bit = high_bit + 1;
  }
  /* BDD for initial state. */
  BDD init_bdd = manager->GetConstant(true);
  for (auto i = model.variables().rbegin();
       i != model.variables().rend(); ++i) {
    const ParsedVariable& v = *i;
    auto j = variable_properties.find(v.name());
    CHECK(j != variable_properties.end());
    const VariableProperties& p = j->second;
    BDD dds = variable_mtbdd(*manager, v.min_value(), p.low_bit(), p.high_bit())
        .Interval(v.init_value(), v.init_value());
    init_bdd = dds && init_bdd;
  }
  /*
   * Generate phase-type distributions for commands with
   * non-exponential delay.
   */
  size_t nvars = manager->GetVariableCount()/2;
  std::map<int, PHData> ph_commands;
  for (int i = model.commands().size() - 1; i >= 0; i--) {
    const Command& command = *model.commands()[i];
    const Distribution& dist = command.delay();
    if (typeid(dist) != typeid(Exponential)) {
      PHData data(*manager);
      switch (moments) {
	case 1:
	  match_first_moment(data.params, dist);
	  break;
	case 2:
	  dist.acph2(data.params2);
	  data.params.n = 0;
	  break;
	case 3:
	default:
	  dist.acph(data.params);
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
            (BDD(mtbdd(*manager, variable_properties, command.guard()))
             || ddv.Interval(0, 0)) &&
            (BDD(primed_mtbdd(*manager, variable_properties, command.guard()))
             || ddvp.Interval(0, 0)) &&
            (!BDD(primed_mtbdd(*manager, variable_properties, command.guard()))
             || ddid);
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
    BDD ddg = BDD(mtbdd(*manager, variable_properties, command.guard()));
    /* BDD for command. */
    std::set<std::string> updated_variables;
    BDD ddc = command_bdd(*manager, variable_properties, command,
                          &updated_variables);
    const Exponential* exp_delay =
	dynamic_cast<const Exponential*>(&command.delay());
    PHData* ph_data =
        (exp_delay != NULL) ? NULL : &ph_commands.find(i)->second;
    if (ph_data != NULL && ph_data->params.n == 0) {
      if (VLOG_IS_ON(2)) {
        LOG(INFO) << "n=" << ph_data->params2.n
                  << " p=" << ph_data->params2.p
                  << " r1=" << ph_data->params2.r1
                  << " r2=" << ph_data->params2.r2;
      }
      /*
       * Event 1: phi & s=0 => s'=1
       */
      ADD ddv = variable_mtbdd(
          *manager, 0, ph_data->low_bit, ph_data->high_bit);
      BDD dds = ddv.Interval(0, 0);
      ADD ddvp = variable_primed_mtbdd(
          *manager, 0, ph_data->low_bit, ph_data->high_bit);
      BDD ddu = dds && ddvp.Interval(1, 1) && ddg;
      ddu = variable_updates(*manager, ddu, model, variable_properties, {}, {},
                             i, ph_commands);
      ADD ddr = manager->GetConstant(ph_data->params2.p*ph_data->params2.r1);
      ADD ddq = ADD(ddu) * ddr;
      if (VLOG_IS_ON(2)) {
        Cudd_PrintDebug(manager->manager(),
                        ddq.get(), manager->GetVariableCount(), 2);
      }
      ddR = ddq +  ddR;
      /*
       * Event 2: phi & s=0 => s'=0 & effects
       */
      BDD ddp = ddvp.Interval(0, 0);
      ddu = ddc && dds && ddp;
      ddu = variable_updates(*manager, ddu, model, variable_properties,
                             model.command_modules()[i], updated_variables, i,
                             ph_commands);
      ddr = manager->GetConstant(
          (1.0 - ph_data->params2.p) * ph_data->params2.r1);
      ddq = ADD(ddu) * ddr;
      if (VLOG_IS_ON(2)) {
        Cudd_PrintDebug(manager->manager(),
                        ddq.get(), manager->GetVariableCount(), 2);
      }
      ddR = ddq + ddR;
      /*
       * Event 3: phi & s=n-1 => s'=0 & effects
       */
      dds = ddv.Interval(ph_data->params2.n - 1, ph_data->params2.n - 1);
      ddu = ddc && dds && ddp;
      ddu = variable_updates(*manager, ddu, model, variable_properties,
                             model.command_modules()[i], updated_variables, i,
                             ph_commands);
      ddr = manager->GetConstant(ph_data->params2.r2);
      ddq = ADD(ddu) * ddr;
      if (VLOG_IS_ON(2)) {
        Cudd_PrintDebug(manager->manager(),
                        ddq.get(), manager->GetVariableCount(), 2);
      }
      ddR = ddq + ddR;
      if (ph_data->params2.n > 2) {
        /*
         * Event 4: phi & s>=1 & s<=n-2 => s'=s+1
         */
        dds = ddv.Interval(1, ph_data->params2.n - 2);
        ddu = dds && ddvp == ddv + manager->GetConstant(1) && ddg;
        ddu = variable_updates(*manager, ddu, model, variable_properties, {},
                               {}, i, ph_commands);
        ddr = manager->GetConstant(ph_data->params2.r1);
        ddq = ADD(ddu) * ddr;
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(),
                          ddq.get(), manager->GetVariableCount(), 2);
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
        ADD ddv = variable_mtbdd(
            *manager, 0, ph_data->low_bit, ph_data->high_bit);
        BDD dds = ddv.Interval(0, ph_data->params.n - 3);
        ADD ddvp = variable_primed_mtbdd(
            *manager, 0, ph_data->low_bit, ph_data->high_bit);
        ADD ddp = ddv + manager->GetConstant(1);
        BDD ddu = dds && ddvp == ddp && ddg;
        ddu = variable_updates(*manager, ddu, model, variable_properties, {},
                               {}, i, ph_commands);
        ADD ddq = ADD(ddu) * manager->GetConstant(ph_data->params.re);
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(),
                          ddq.get(), manager->GetVariableCount(), 2);
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
        ADD ddv = variable_mtbdd(
            *manager, 0, ph_data->low_bit, ph_data->high_bit);
        BDD dds = ddv.Interval(ph_data->params.n - 2, ph_data->params.n - 2);
        ADD ddvp = variable_primed_mtbdd(
            *manager, 0, ph_data->low_bit, ph_data->high_bit);
        BDD ddu = ddvp.Interval(ph_data->params.n - 1, ph_data->params.n - 1);
        ddu = dds && ddu && ddg;
        ddu = variable_updates(*manager, ddu, model, variable_properties, {},
                               {}, i, ph_commands);
        ADD ddr = manager->GetConstant(ph_data->params.pc*ph_data->params.rc1);
        ADD ddq = ADD(ddu) * ddr;
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(),
                          ddq.get(), manager->GetVariableCount(), 2);
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
        ddu = variable_updates(*manager, ddu, model, variable_properties,
                               model.command_modules()[i], updated_variables, i,
                               ph_commands);
        ddr = manager->GetConstant(
            (1.0 - ph_data->params.pc)*ph_data->params.rc1);
        ddq = ADD(ddu) * ddr;
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(),
                          ddq.get(), manager->GetVariableCount(), 2);
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
        ddu = variable_updates(*manager, ddu, model, variable_properties,
                               model.command_modules()[i], updated_variables, i,
                               ph_commands);
        ddq = ADD(ddu) * manager->GetConstant(ph_data->params.rc2);
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(),
                          ddq.get(), manager->GetVariableCount(), 2);
        }
        ddR = ddq + ddR;
      } else {
        /*
         * Event for exponential (or 1-phase Coxian) distribution.
         */
        BDD dda = manager->GetConstant(false);
        if (ph_data != NULL && ph_data->low_bit >= 0) {
          /* Coxian: s=n-2 => s'=0 */
          BDD dds = variable_mtbdd(
              *manager, 0, ph_data->low_bit, ph_data->high_bit)
              .Interval(ph_data->params.n - 2, ph_data->params.n - 2);
          BDD ddp = variable_primed_mtbdd(
              *manager, 0, ph_data->low_bit, ph_data->high_bit)
              .Interval(0, 0);
          dda = dds && ddp;
          dda = variable_updates(*manager, dda, model, variable_properties,
                                 model.command_modules()[i], updated_variables,
                                 i, ph_commands);
        } else {
          dda = manager->GetConstant(true);
          dda = variable_updates(*manager, dda, model, variable_properties,
                                 model.command_modules()[i], updated_variables,
                                 i, ph_commands);
        }
        BDD ddu = ddc && dda;
        ADD ddr = (exp_delay != NULL)
            ? mtbdd(*manager, variable_properties, exp_delay->rate())
            : manager->GetConstant(ph_data->params.rc1);
        ADD ddq = ADD(ddu) * ddr;
        if (VLOG_IS_ON(2)) {
          Cudd_PrintDebug(manager->manager(),
                          ddq.get(), manager->GetVariableCount(), 2);
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

namespace {

ADD CompileVariable(const DecisionDiagramManager& manager,
                    int low, int low_bit, int high_bit, bool primed) {
  ADD result = manager.GetConstant(0);
  const int offset = primed ? 1 : 0;
  for (int i = high_bit; i >= low_bit; --i) {
    result = result + (manager.GetAddVariable(2*i + offset) *
                       manager.GetConstant(1 << (high_bit - i)));
  }
  return result + manager.GetConstant(low);
}

class ExpressionCompiler : public ExpressionVisitor {
 public:
  ExpressionCompiler(
      const DecisionDiagramManager* manager,
      const std::map<std::string, VariableProperties>* variable_properties,
      bool primed);

  ADD mtbdd() const { return mtbdd_; }

 private:
  virtual void DoVisitLiteral(const Literal& expr);
  virtual void DoVisitIdentifier(const Identifier& expr);
  virtual void DoVisitFunctionCall(const FunctionCall& expr);
  virtual void DoVisitUnaryOperation(const UnaryOperation& expr);
  virtual void DoVisitBinaryOperation(const BinaryOperation& expr);
  virtual void DoVisitConditional(const Conditional& expr);
  virtual void DoVisitProbabilityThresholdOperation(
      const ProbabilityThresholdOperation& expr);

  const DecisionDiagramManager* manager_;
  const std::map<std::string, VariableProperties>* variable_properties_;
  bool primed_;
  ADD mtbdd_;
};

ExpressionCompiler::ExpressionCompiler(
    const DecisionDiagramManager* manager,
    const std::map<std::string, VariableProperties>* variable_properties,
    bool primed)
    : manager_(manager), variable_properties_(variable_properties),
      primed_(primed), mtbdd_(manager->GetConstant(0)) {
}

void ExpressionCompiler::DoVisitLiteral(const Literal& expr) {
  mtbdd_ = manager_->GetConstant(expr.value().value<double>());
}

void ExpressionCompiler::DoVisitIdentifier(const Identifier& expr) {
  auto i = variable_properties_->find(expr.name());
  CHECK(i != variable_properties_->end());
  const VariableProperties& p = i->second;
  mtbdd_ = CompileVariable(
      *manager_, p.min_value(), p.low_bit(), p.high_bit(), primed_);
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

}  // namespace

ADD mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Expression& e) {
  ExpressionCompiler compiler(
      &manager, &variable_properties, false /* primed */);
  e.Accept(&compiler);
  return compiler.mtbdd();
}

ADD primed_mtbdd(
    const DecisionDiagramManager& manager,
    const std::map<std::string, VariableProperties>& variable_properties,
    const Expression& e) {
  ExpressionCompiler compiler(
      &manager, &variable_properties, true /* primed */);
  e.Accept(&compiler);
  return compiler.mtbdd();
}

ADD variable_mtbdd(const DecisionDiagramManager& manager,
                   int low, int low_bit, int high_bit) {
  return CompileVariable(manager, low, low_bit, high_bit, false /* primed */);
}

ADD variable_primed_mtbdd(const DecisionDiagramManager& manager,
                          int low, int low_bit, int high_bit) {
  return CompileVariable(manager, low, low_bit, high_bit, true /* primed */);
}
