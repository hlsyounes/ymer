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
#include "formulas.h"
#include <iostream>
#include <stdexcept>
#include <typeinfo>

/* Verbosity level. */
extern int verbosity;


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
DdNode* reachability_bdd(const DecisionDiagramManager& dd_man,
                         DdNode* init, const ADD& rates,
                         DdNode** row_variables) {
  if (verbosity > 0) {
    std::cout << "Computing reachable states";
  }
  /*
   * Precompute variable permutations and cubes.
   */
  size_t nvars = dd_man.GetNumVariables()/2;
  int* row_to_col = new int[2*nvars];
  int* col_to_row = new int[2*nvars];
  for (size_t i = 0; i < nvars; i++) {
    row_to_col[2*i] = 2*i + 1;
    row_to_col[2*i + 1] = 2*i + 1;
    col_to_row[2*i] = 2*i;
    col_to_row[2*i + 1] = 2*i;
  }
  DdNode* row_cube =
      Cudd_bddComputeCube(dd_man.manager(), row_variables, NULL, nvars);
  Cudd_Ref(row_cube);
  /*
   * Fixpoint computation of reachability.
   */
  BDD trans = rates.StrictThreshold(0);
  DdNode* solr = init;
  Cudd_Ref(solr);
  DdNode* solc = Cudd_bddPermute(dd_man.manager(), solr, row_to_col);
  Cudd_Ref(solc);
  bool done = false;
  size_t iters = 0;
  while (!done) {
    iters++;
    if (verbosity > 0) {
      if (iters % 1000 == 0) {
	std::cout << ':';
      } else if (iters % 100 == 0) {
	std::cout << '.';
      }
    }
    DdNode* dda = Cudd_bddAnd(dd_man.manager(), trans.get(), solr);
    Cudd_Ref(dda);
    Cudd_RecursiveDeref(dd_man.manager(), solr);
    DdNode* dde = Cudd_bddExistAbstract(dd_man.manager(), dda, row_cube);
    Cudd_Ref(dde);
    Cudd_RecursiveDeref(dd_man.manager(), dda);
    DdNode* ddo = Cudd_bddOr(dd_man.manager(), solc, dde);
    if (ddo == solc) {
      done = true;
    } else {
      Cudd_Ref(ddo);
      Cudd_RecursiveDeref(dd_man.manager(), solc);
      solc = ddo;
    }
    Cudd_RecursiveDeref(dd_man.manager(), dde);
    solr = Cudd_bddPermute(dd_man.manager(), solc, col_to_row);
    Cudd_Ref(solr);
  }
  Cudd_RecursiveDeref(dd_man.manager(), solc);
  Cudd_RecursiveDeref(dd_man.manager(), row_cube);
  delete row_to_col;
  delete col_to_row;
  if (verbosity > 0) {
    std::cout << ' ' << iters << " iterations." << std::endl;
  }
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
BDD variable_identities(const DecisionDiagramManager& dd_man,
                        const BDD& dd_start,
                        const std::vector<const Variable*>& variables,
                        const std::set<const Variable*>& excluded) {
  BDD ddu = dd_start;
  for (std::vector<const Variable*>::const_reverse_iterator vi =
           variables.rbegin();
       vi != variables.rend(); vi++) {
    const Variable* v = *vi;
    if (excluded.find(v) == excluded.end()) {
      ddu = identity_bdd(dd_man, v->low(), v->low_bit(), v->high_bit()) && ddu;
    }
  }
  return ddu;
}

// Returns a BDD representation of the given update.
BDD update_bdd(const DecisionDiagramManager& manager, const Update& update) {
  return primed_mtbdd(manager, update.variable())
      == mtbdd(manager, update.expr());
}

// Returns a BDD representation of the given command and fills the provided
// set with variables updated by the command.
BDD command_bdd(const DecisionDiagramManager& manager, const Command& command,
                std::set<const Variable*>* updated_variables) {
  BDD ddu = manager.GetConstant(true);
  for (UpdateList::const_iterator ui = command.updates().begin();
       ui != command.updates().end(); ++ui) {
    const Update& update = **ui;
    ddu = update_bdd(manager, update) && ddu;
    updated_variables->insert(&update.variable());
  }
  return command.guard().bdd(manager) && ddu;
}


// Returns a BDD representing the conjunction of dd_start with the BDDs for
// updates of all variables for the given model not explicitly mentioned.
BDD variable_updates(const DecisionDiagramManager& manager,
                     const BDD& dd_start,
                     const Model& model,
                     const ModuleSet& touched_modules,
                     const std::set<const Variable*>& updated_variables,
                     int command_index,
                     const std::map<int, PHData>& ph_commands) {
  BDD ddu = dd_start;
  // Conjunction with identity BDD for untouched module variables.
  for (ModuleList::const_reverse_iterator mi = model.modules().rbegin();
       mi != model.modules().rend(); mi++) {
    const Module* module = *mi;
    if (touched_modules.find(module) != touched_modules.end()) {
      ddu = variable_identities(
          manager, ddu, module->variables(), updated_variables);
    } else {
      for (std::vector<const Variable*>::const_reverse_iterator vi =
               module->variables().rbegin();
           vi != module->variables().rend(); ++vi) {
        const Variable* v = *vi;
        ddu =
            identity_bdd(manager, v->low(), v->low_bit(), v->high_bit()) && ddu;
      }
    }
  }
  // Conjunction with identity BDD for untouched global variables.
  ddu = variable_identities(manager, ddu, model.variables(), updated_variables);
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
  : rate_mtbdd_(NULL), reach_bdd_(NULL), odd_(NULL), init_bdd_(NULL),
    init_index_(-1), row_variables_(NULL), column_variables_(NULL) {
}


/* Deletes this model. */
Model::~Model() {
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    if ((*ci)->synch() != 0) {
      delete *ci;
    }
  }
  for (std::vector<const Variable*>::const_iterator vi = variables().begin();
       vi != variables().end(); vi++) {
    Expression::destructive_deref(*vi);
  }
  for (ModuleList::const_iterator mi = modules().begin();
       mi != modules().end(); mi++) {
    delete *mi;
  }
}


/* Adds a global variable to this model. */
void Model::add_variable(const Variable& variable) {
  variables_.push_back(&variable);
  Expression::ref(&variable);
  if (variable.index() >= variable_names_.size()) {
    variable_names_.resize(variable.index() + 1);
  }
  variable_names_[variable.index()] = variable.name();
}


/* Adds a module to this model. */
void Model::add_module(const Module& module) {
  modules_.push_back(&module);
  for (int i = 0; i < module.variables().size(); ++i) {
    const Variable& variable = *module.variables()[i];
    if (variable.index() >= variable_names_.size()) {
      variable_names_.resize(variable.index() + 1);
    }
    variable_names_[variable.index()] = variable.name();
  }
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

}  // namespace

/* Compiles the commands of this model. */
void Model::compile() {
  /*
   * Clear currently compiled commands.
   */
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    if ((*ci)->synch() != 0) {
      delete *ci;
    }
  }
  commands_.clear();
  command_modules_.clear();
  /*
   * Process the commands of each module.
   */
  std::set<size_t> synchs;
  SynchronizationMap synch_commands;
  for (ModuleList::const_reverse_iterator mi = modules().rbegin();
       mi != modules().rend(); mi++) {
    for (CommandList::const_iterator ci = (*mi)->commands().begin();
	 ci != (*mi)->commands().end(); ci++) {
      const Command& command = **ci;
      size_t synch = command.synch();
      if (synch != 0) {
	/* Command is synchronized so store it for later processing. */
	synch_commands.insert(std::make_pair(std::make_pair(*mi, synch), *ci));
	synchs.insert(synch);
      } else { /* synch == 0 */
	/* Command is not synchronized. */
	commands_.push_back(*ci);
	command_modules_.push_back(ModuleSet());
	command_modules_.back().insert(*mi);
      }
    }
  }
  /*
   * Add synchronized commands.
   */
  for (std::set<size_t>::const_iterator si = synchs.begin();
       si != synchs.end(); si++) {
    for (ModuleList::const_reverse_iterator mi = modules().rbegin();
	 mi != modules().rend(); mi++) {
      SynchronizationMapRange sri =
	synch_commands.equal_range(std::make_pair(*mi, *si));
      for (SynchronizationMap::const_iterator smi = sri.first;
	   smi != sri.second; smi++) {
	const Command& ci = *(*smi).second;
	for (ModuleList::const_reverse_iterator mj = mi + 1;
	     mj != modules().rend(); mj++) {
	  SynchronizationMapRange srj =
	    synch_commands.equal_range(std::make_pair(*mj, *si));
	  for (SynchronizationMap::const_iterator smj = srj.first;
	       smj != srj.second; smj++) {
	    const Command& cj = *(*smj).second;
	    /*
	     * Synchronize ci with cj.
	     */
            std::map<std::string, const Variable*> empty_subst;
	    Conjunction* guard = new Conjunction();
	    guard->add_conjunct(ci.guard().substitution(empty_subst));
	    guard->add_conjunct(cj.guard().substitution(empty_subst));
	    Command* c;
	    if (IsUnitDistribution(ci.delay())) {
	      c = new Command(*si, guard, cj.delay().substitution(empty_subst));
	    } else if (IsUnitDistribution(cj.delay())) {
	      c = new Command(*si, guard, ci.delay().substitution(empty_subst));
	    } else {
	      throw std::logic_error("at least one command in a"
				     " synchronization pair must have rate 1");
	    }
	    for (UpdateList::const_iterator ui = ci.updates().begin();
		 ui != ci.updates().end(); ui++) {
	      const Update& u = **ui;
	      c->add_update(new Update(u.variable(), u.expr()));
	    }
	    for (UpdateList::const_iterator uj = cj.updates().begin();
		 uj != cj.updates().end(); uj++) {
	      const Update& u = **uj;
	      c->add_update(new Update(u.variable(), u.expr()));
	    }
	    commands_.push_back(c);
	    command_modules_.push_back(ModuleSet());
	    command_modules_.back().insert(*mi);
	    command_modules_.back().insert(*mj);
	  }
	}
      }
    }
  }
}


/* Caches DDs for for this model. */
void Model::cache_dds(const DecisionDiagramManager& dd_man,
                      size_t moments) const {
  if (rate_mtbdd_ == NULL) {
    if (verbosity > 0) {
      std::cout << "Buidling model...";
    }
    /*
     * Precomute DDs for variables and modules.
     */
    /* BDD for initial state. */
    init_bdd_ = Cudd_ReadOne(dd_man.manager());
    Cudd_Ref(init_bdd_);
    for (ModuleList::const_reverse_iterator mi = modules().rbegin();
	 mi != modules().rend(); mi++) {
      const Module& mod = **mi;
      for (std::vector<const Variable*>::const_reverse_iterator vi =
               mod.variables().rbegin();
	   vi != mod.variables().rend(); vi++) {
	const Variable& v = **vi;
        BDD dds = mtbdd(dd_man, v).Interval(v.start(), v.start());
	DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds.get(), init_bdd_);
	Cudd_Ref(dda);
	Cudd_RecursiveDeref(dd_man.manager(), init_bdd_);
	init_bdd_ = dda;
      }
    }
    for (std::vector<const Variable*>::const_reverse_iterator vi =
             variables().rbegin();
	 vi != variables().rend(); vi++) {
      const Variable& v = **vi;
      BDD dds = mtbdd(dd_man, v).Interval(v.start(), v.start());
      DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds.get(), init_bdd_);
      Cudd_Ref(dda);
      Cudd_RecursiveDeref(dd_man.manager(), init_bdd_);
      init_bdd_ = dda;
    }
    /*
     * Generate phase-type distributions for commands with
     * non-exponential delay.
     */
    size_t nvars = dd_man.GetNumVariables()/2;
    std::map<int, PHData> ph_commands;
    for (int i = commands().size() - 1; i >= 0; i--) {
      const Command& command = *commands_[i];
      const Distribution& dist = command.delay();
      if (typeid(dist) != typeid(Exponential)) {
        PHData data(dd_man);
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
	    Cudd_bddNewVar(dd_man.manager());
	    Cudd_bddNewVar(dd_man.manager());
	  }
	  ADD ddv = variable_mtbdd(dd_man, 0, data.low_bit, data.high_bit);
	  BDD dds = ddv.Interval(0, 0);
	  DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds.get(), init_bdd_);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), init_bdd_);
	  init_bdd_ = dda;
	  ADD ddvp =
              variable_primed_mtbdd(dd_man, 0, data.low_bit, data.high_bit);
	  BDD ddid = identity_bdd(dd_man, 0, data.low_bit, data.high_bit);
	  /*
	   * Constructs BDD representing phase update:
	   *
	   *   (!phi -> s=0) & (phi' -> s'=s) & (!phi' -> s'=0)
	   */
          data.update_bdd =
              (command.guard().bdd(dd_man) || ddv.Interval(0, 0)) &&
              (command.guard().primed_bdd(dd_man) || ddvp.Interval(0, 0)) &&
              (!command.guard().primed_bdd(dd_man) || ddid);
	}
        ph_commands.insert(std::make_pair(i, data));
      }
    }
    /*
     * Compute rate matrix for all commands.
     */
    ADD ddR = dd_man.GetConstant(0);
    for (int i = commands().size() - 1; i >= 0; i--) {
      const Command& command = *commands_[i];
      if (verbosity > 1) {
	std::cout << std::endl << "processing " << command << std::endl;
      }
      /* BDD for guard. */
      BDD ddg = command.guard().bdd(dd_man);
      /* BDD for command. */
      std::set<const Variable*> updated_variables;
      BDD ddc = command_bdd(dd_man, command, &updated_variables);
      const Exponential* exp_delay =
	dynamic_cast<const Exponential*>(&command.delay());
      PHData* ph_data =
          (exp_delay != NULL) ? NULL : &ph_commands.find(i)->second;
      if (ph_data != NULL && ph_data->params.n == 0) {
	if (verbosity > 1) {
	  std::cout << "n=" << ph_data->params2.n
		    << " p=" << ph_data->params2.p
		    << " r1=" << ph_data->params2.r1
		    << " r2=" << ph_data->params2.r2 << std::endl;
	}
	/*
	 * Event 1: phi & s=0 => s'=1
	 */
	ADD ddv = variable_mtbdd(
            dd_man, 0, ph_data->low_bit, ph_data->high_bit);
	BDD dds = ddv.Interval(0, 0);
	ADD ddvp = variable_primed_mtbdd(
            dd_man, 0, ph_data->low_bit, ph_data->high_bit);
	BDD ddu = dds && ddvp.Interval(1, 1) && ddg;
	ddu = variable_updates(dd_man, ddu, *this, ModuleSet(),
			       std::set<const Variable*>(), i, ph_commands);
	ADD ddr = dd_man.GetConstant(ph_data->params2.p*ph_data->params2.r1);
	ADD ddq = ADD(ddu) * ddr;
	if (verbosity > 1) {
	  Cudd_PrintDebug(dd_man.manager(),
                          ddq.get(), dd_man.GetNumVariables(), 2);
	}
	ddR = ddq +  ddR;
	/*
	 * Event 2: phi & s=0 => s'=0 & effects
	 */
	BDD ddp = ddvp.Interval(0, 0);
	ddu = ddc && dds && ddp;
	ddu = variable_updates(dd_man, ddu, *this, command_modules_[i],
			       updated_variables, i, ph_commands);
	ddr = dd_man.GetConstant(
            (1.0 - ph_data->params2.p) * ph_data->params2.r1);
	ddq = ADD(ddu) * ddr;
	if (verbosity > 1) {
	  Cudd_PrintDebug(dd_man.manager(),
                          ddq.get(), dd_man.GetNumVariables(), 2);
	}
	ddR = ddq + ddR;
	/*
	 * Event 3: phi & s=n-1 => s'=0 & effects
	 */
	dds = ddv.Interval(ph_data->params2.n - 1, ph_data->params2.n - 1);
	ddu = ddc && dds && ddp;
	ddu = variable_updates(dd_man, ddu, *this, command_modules_[i],
			       updated_variables, i, ph_commands);
	ddr = dd_man.GetConstant(ph_data->params2.r2);
	ddq = ADD(ddu) * ddr;
	if (verbosity > 1) {
	  Cudd_PrintDebug(dd_man.manager(),
                          ddq.get(), dd_man.GetNumVariables(), 2);
	}
	ddR = ddq + ddR;
	if (ph_data->params2.n > 2) {
	  /*
	   * Event 4: phi & s>=1 & s<=n-2 => s'=s+1
	   */
	  dds = ddv.Interval(1, ph_data->params2.n - 2);
	  ddu = dds && ddvp == ddv + dd_man.GetConstant(1) && ddg;
	  ddu = variable_updates(dd_man, ddu, *this, ModuleSet(),
				 std::set<const Variable*>(), i, ph_commands);
	  ddr = dd_man.GetConstant(ph_data->params2.r1);
	  ddq = ADD(ddu) * ddr;
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(),
                            ddq.get(), dd_man.GetNumVariables(), 2);
	  }
	  ddR = ddq + ddR;
	}
      } else {
	if (ph_data != NULL && verbosity > 1) {
	  std::cout << "n=" << ph_data->params.n;
	  if (ph_data->params.n > 2) {
	    std::cout << " re=" << ph_data->params.re;
	  }
	  std::cout << " pc=" << ph_data->params.pc
		    << " rc1=" << ph_data->params.rc1;
	  if (ph_data->params.pc > 0.0) {
	    std::cout << " rc2=" << ph_data->params.rc2;
	  }
	  std::cout << std::endl;
	}
	if (ph_data != NULL && ph_data->params.n > 2) {
	  /*
	   * Event for phase transition in Erlang part of phase-type
	   * distribution:
	   *
	   *   phi & s<n-2 => s'=s+1
	   */
	  ADD ddv = variable_mtbdd(
              dd_man, 0, ph_data->low_bit, ph_data->high_bit);
	  BDD dds = ddv.Interval(0, ph_data->params.n - 3);
	  ADD ddvp = variable_primed_mtbdd(
              dd_man, 0, ph_data->low_bit, ph_data->high_bit);
	  ADD ddp = ddv + dd_man.GetConstant(1);
	  BDD ddu = dds && ddvp == ddp && ddg;
	  ddu = variable_updates(dd_man, ddu, *this, ModuleSet(),
				 std::set<const Variable*>(), i, ph_commands);
	  ADD ddq = ADD(ddu) * dd_man.GetConstant(ph_data->params.re);
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(),
                            ddq.get(), dd_man.GetNumVariables(), 2);
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
              dd_man, 0, ph_data->low_bit, ph_data->high_bit);
	  BDD dds = ddv.Interval(ph_data->params.n - 2, ph_data->params.n - 2);
	  ADD ddvp = variable_primed_mtbdd(
              dd_man, 0, ph_data->low_bit, ph_data->high_bit);
	  BDD ddu = ddvp.Interval(ph_data->params.n - 1, ph_data->params.n - 1);
	  ddu = dds && ddu && ddg;
	  ddu = variable_updates(dd_man, ddu, *this, ModuleSet(),
				 std::set<const Variable*>(), i, ph_commands);
	  ADD ddr = dd_man.GetConstant(ph_data->params.pc*ph_data->params.rc1);
	  ADD ddq = ADD(ddu) * ddr;
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(),
                            ddq.get(), dd_man.GetNumVariables(), 2);
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
	  ddu = variable_updates(dd_man, ddu, *this, command_modules_[i],
				 updated_variables, i, ph_commands);
	  ddr = dd_man.GetConstant(
              (1.0 - ph_data->params.pc)*ph_data->params.rc1);
	  ddq = ADD(ddu) * ddr;
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(),
                            ddq.get(), dd_man.GetNumVariables(), 2);
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
	  ddu = variable_updates(dd_man, ddu, *this, command_modules_[i],
				 updated_variables, i, ph_commands);
	  ddq = ADD(ddu) * dd_man.GetConstant(ph_data->params.rc2);
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(),
                            ddq.get(), dd_man.GetNumVariables(), 2);
	  }
	  ddR = ddq + ddR;
	} else {
	  /*
	   * Event for exponential (or 1-phase Coxian) distribution.
	   */
	  BDD dda = dd_man.GetConstant(false);
	  if (ph_data != NULL && ph_data->low_bit >= 0) {
	    /* Coxian: s=n-2 => s'=0 */
	    BDD dds = variable_mtbdd(
                dd_man, 0, ph_data->low_bit, ph_data->high_bit)
                .Interval(ph_data->params.n - 2, ph_data->params.n - 2);
	    BDD ddp = variable_primed_mtbdd(
                dd_man, 0, ph_data->low_bit, ph_data->high_bit)
                .Interval(0, 0);
	    dda = dds && ddp;
	    dda = variable_updates(dd_man, dda, *this, command_modules_[i],
				   updated_variables, i, ph_commands);
	  } else {
	    dda = dd_man.GetConstant(true);
	    dda = variable_updates(dd_man, dda, *this, command_modules_[i],
				   updated_variables, i, ph_commands);
	  }
	  BDD ddu = ddc && dda;
	  ADD ddr = (exp_delay != NULL)
              ? mtbdd(dd_man, exp_delay->rate())
              : dd_man.GetConstant(ph_data->params.rc1);
	  ADD ddq = ADD(ddu) * ddr;
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(),
                            ddq.get(), dd_man.GetNumVariables(), 2);
	  }
	  ddR = ddq + ddR;
	}
      }
    }
    if (verbosity > 0) {
      std::cout << dd_man.GetNumVariables() << " variables." << std::endl;
    }
    /*
     * Collect row and column variables.
     */
    nvars = dd_man.GetNumVariables()/2;
    if (row_variables_ != NULL) {
      /* Row and column variables have been computed before, but
	 without phase variables.  Clear and recompute the arrays. */
      delete row_variables_;
      delete column_variables_;
    }
    row_variables_ = new DdNode*[nvars];
    column_variables_ = new DdNode*[nvars];
    for (size_t i = 0; i < nvars; i++) {
      row_variables_[i] = Cudd_bddIthVar(dd_man.manager(), 2*i);
      column_variables_[i] = Cudd_bddIthVar(dd_man.manager(), 2*i + 1);
    }
    /*
     * Reachability analysis.
     */
    reach_bdd_ = ::reachability_bdd(dd_man, init_bdd_, ddR, row_variables_);
    DdNode* reach_add = Cudd_BddToAdd(dd_man.manager(), reach_bdd_);
    Cudd_Ref(reach_add);
    DdNode* ddT =
        Cudd_addApply(dd_man.manager(), Cudd_addTimes, reach_add, ddR.get());
    Cudd_Ref(ddT);
    rate_mtbdd_ = ddT;
    /* Build ODD. */
    odd_ = build_odd(dd_man, reach_add, row_variables_, nvars);
    Cudd_RecursiveDeref(dd_man.manager(), reach_add);
  }
}


/* Returns an MTBDD representing the rate matrix for this model. */
DdNode* Model::rate_mtbdd(const DecisionDiagramManager& dd_man) const {
  if (rate_mtbdd_ == NULL) {
    throw std::logic_error("must cache DDs before use");
  }
  Cudd_Ref(rate_mtbdd_);
  return rate_mtbdd_;
}


/* Returns a reachability BDD for this model. */
DdNode* Model::reachability_bdd(const DecisionDiagramManager& dd_man) const {
  if (reach_bdd_ == NULL) {
    throw std::logic_error("must cache DDs before use");
  }
  Cudd_Ref(reach_bdd_);
  return reach_bdd_;
}


/* Returns an ODD for this model. */
ODDNode* Model::odd(const DecisionDiagramManager& dd_man) const {
  if (odd_ == NULL) {
    throw std::logic_error("must cache DDs before use");
  }
  return odd_;
}


/* Returns a BDD representing the initial state for this model. */
DdNode* Model::init_bdd(const DecisionDiagramManager& dd_man) const {
  if (init_bdd_ == NULL) {
    throw std::logic_error("must cache DDs before use");
  }
  Cudd_Ref(init_bdd_);
  return init_bdd_;
}


/* Returns the index associated with the initial state for this model. */
int Model::init_index(const DecisionDiagramManager& dd_man) const {
  if (init_index_ == -1) {
    if (init_bdd_ == NULL) {
      throw std::logic_error("must cache DDs before use");
    }
    DdNode* init_add_ = Cudd_BddToAdd(dd_man.manager(), init_bdd_);
    Cudd_Ref(init_add_);
    DdNode* d = init_add_;
    ODDNode* o = odd_;
    init_index_ = 0;
    size_t nvars = dd_man.GetNumVariables()/2;
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
	init_index_ += o->eoff;
	o = o->t;
      } else {
	o = o->e;
      }
    }
    Cudd_RecursiveDeref(dd_man.manager(), init_add_);
  }
  return init_index_;
}

BDD Model::state_bdd(const DecisionDiagramManager& dd_man,
                     const std::vector<int>& state) const {
  BDD dds = dd_man.GetConstant(true);
  for (ModuleList::const_reverse_iterator mi = modules().rbegin();
       mi != modules().rend(); mi++) {
    const Module& mod = **mi;
    for (std::vector<const Variable*>::const_reverse_iterator vi =
             mod.variables().rbegin();
         vi != mod.variables().rend(); vi++) {
      const Variable& v = **vi;
      const int value = state.at(v.index());
      dds = mtbdd(dd_man, v).Interval(value, value) && dds;
    }
  }
  for (std::vector<const Variable*>::const_reverse_iterator vi =
           variables().rbegin();
       vi != variables().rend(); vi++) {
    const Variable& v = **vi;
    const int value = state.at(v.index());
    dds = mtbdd(dd_man, v).Interval(value, value) && dds;
  }
  return dds;
}

/* Returns the row variables for this model. */
DdNode** Model::row_variables(const DecisionDiagramManager& dd_man) const {
  if (row_variables_ == NULL) {
    throw std::logic_error("must cache DDs before use");
  }
  return row_variables_;
}


/* Returns the column variables for this model. */
DdNode** Model::column_variables(const DecisionDiagramManager& dd_man) const {
  if (column_variables_ == NULL) {
    throw std::logic_error("must cache DDs before use");
  }
  return column_variables_;
}


/* Returns the number of states for this model. */
double Model::num_states(const DecisionDiagramManager& dd_man) const {
  if (reach_bdd_ == NULL) {
    throw std::logic_error("must cache DDs before use");
  }
  return Cudd_CountMinterm(dd_man.manager(),
                           reach_bdd_, dd_man.GetNumVariables()/2);
}


/* Returns the number of transitions for this model. */
double Model::num_transitions(const DecisionDiagramManager& dd_man) const {
  if (rate_mtbdd_ == NULL) {
    throw std::logic_error("must cache DDs before use");
  }
  return Cudd_CountMinterm(dd_man.manager(),
                           rate_mtbdd_, dd_man.GetNumVariables());
}


/* Releases all DDs cached for this model. */
void Model::uncache_dds(const DecisionDiagramManager& dd_man) const {
  if (rate_mtbdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), rate_mtbdd_);
    rate_mtbdd_ = NULL;
  }
  if (reach_bdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), reach_bdd_);
    reach_bdd_ = NULL;
  }
  free_odd(odd_);
  odd_ = NULL;
  if (init_bdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man.manager(), init_bdd_);
    init_bdd_ = NULL;
  }
  init_index_ = -1;
  if (row_variables_ != NULL) {
    delete row_variables_;
    row_variables_ = NULL;
  }
  if (column_variables_ != NULL) {
    delete column_variables_;
    column_variables_ = NULL;
  }
}


/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m) {
  os << "stochastic";
  std::vector<const Variable*>::const_iterator vi = m.variables().begin();
  if (vi != m.variables().end()) {
    os << std::endl;
    for (; vi != m.variables().end(); vi++) {
      const Variable& v = **vi;
      os << std::endl << "global " << v;
      os << " : [" << v.low() << ".." << v.high() << "]";
      if (v.low() != v.start()) {
	os << " init " << v.start();
      }
      os << ';';
    }
  }
  for (ModuleList::const_iterator mi = m.modules().begin();
       mi != m.modules().end(); mi++) {
    const Module& module = **mi;
    os << std::endl << std::endl << "module M" << (mi - m.modules().begin());
    vi = module.variables().begin();
    if (vi != module.variables().end()) {
      os << std::endl;
      for (; vi != module.variables().end(); vi++) {
	const Variable& v = **vi;
	os << std::endl << "  " << v;
	os << " : [" << v.low() << ".." << v.high() << "]";
	if (v.low() != v.start()) {
	  os << " init " << v.start();
	}
	os << ';';
      }
    }
    CommandList::const_iterator ci = module.commands().begin();
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
