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
 *
 * $Id: models.cc,v 2.1 2004-01-25 12:28:15 lorens Exp $
 */
#include "models.h"
#include "formulas.h"
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
/* Model */

/* Returns the number of phases of the given PH distribution. */
static int ph_phases(const PHData& data) {
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
static void match_first_moment(ECParameters& params,
			       const Distribution& dist) {
  params.n = 2;
  std::vector<double> mu;
  dist.moments(mu, 1);
  params.rc1 = 1.0/mu[0];
  params.pc = 0.0;
}


/* Returns a reachability BDD for the given initial state and rate
   matrix. */
static DdNode* reachability_bdd(const DecisionDiagramManager& dd_man,
				DdNode* init, DdNode* rates,
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
  DdNode* trans = Cudd_addBddStrictThreshold(dd_man.manager(), rates, 0);
  Cudd_Ref(trans);
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
    DdNode* dda = Cudd_bddAnd(dd_man.manager(), trans, solr);
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
  Cudd_RecursiveDeref(dd_man.manager(), trans);
  Cudd_RecursiveDeref(dd_man.manager(), row_cube);
  delete row_to_col;
  delete col_to_row;
  if (verbosity > 0) {
    std::cout << ' ' << iters << " iterations." << std::endl;
  }
  return solr;
}


/*
 * Returns a BDD representing equality of the given MTBDDs.
 */
static DdNode* equality_bdd(const DecisionDiagramManager& dd_man,
                            DdNode* dd1, DdNode* dd2) {
  DdNode* ddm = Cudd_addApply(dd_man.manager(), Cudd_addMinus, dd1, dd2);
  Cudd_Ref(ddm);
  DdNode* dde = Cudd_addBddInterval(dd_man.manager(), ddm, 0, 0);
  Cudd_Ref(dde);
  Cudd_RecursiveDeref(dd_man.manager(), ddm);
  return dde;
}


/* Returns a BDD representing the conjunction of dd_start with the
   identity BDDs for the given variables. */
static DdNode* variable_identities(const DecisionDiagramManager& dd_man,
                                   DdNode* dd_start,
				   const VariableList& variables,
				   const VariableSet& excluded) {
  DdNode* ddu = dd_start;
  for (VariableList::const_reverse_iterator vi = variables.rbegin();
       vi != variables.rend(); vi++) {
    if (excluded.find(*vi) == excluded.end()) {
      DdNode* ddv = (*vi)->identity_bdd(dd_man);
      DdNode* dda = Cudd_bddAnd(dd_man.manager(), ddv, ddu);
      Cudd_Ref(dda);
      Cudd_RecursiveDeref(dd_man.manager(), ddv);
      Cudd_RecursiveDeref(dd_man.manager(), ddu);
      ddu = dda;
    }
  }
  return ddu;
}


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
  for (VariableList::const_iterator vi = variables().begin();
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
}


/* Adds a module to this model. */
void Model::add_module(const Module& module) {
  modules_.push_back(&module);
}


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
	    Conjunction& guard = *new Conjunction();
	    guard.add_conjunct(ci.guard());
	    guard.add_conjunct(cj.guard());
	    Command* c;
	    if (&ci.delay() == &Distribution::EXP1) {
	      c = new Command(*si, guard, cj.delay());
	    } else if (&cj.delay() == &Distribution::EXP1) {
	      c = new Command(*si, guard, ci.delay());
	    } else {
	      throw std::logic_error("at least one command in a"
				     " synchronization pair must have rate 1");
	    }
	    for (UpdateList::const_iterator ui = ci.updates().begin();
		 ui != ci.updates().end(); ui++) {
	      const Update& u = **ui;
	      c->add_update(*new Update(u.variable(), u.expr()));
	    }
	    for (UpdateList::const_iterator uj = cj.updates().begin();
		 uj != cj.updates().end(); uj++) {
	      const Update& u = **uj;
	      c->add_update(*new Update(u.variable(), u.expr()));
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
      for (VariableList::const_reverse_iterator vi = mod.variables().rbegin();
	   vi != mod.variables().rend(); vi++) {
	const Variable& v = **vi;
	DdNode* ddv = v.mtbdd(dd_man.manager());
	DdNode* dds =
            Cudd_addBddInterval(dd_man.manager(), ddv, v.start(), v.start());
	Cudd_Ref(dds);
	DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds, init_bdd_);
	Cudd_Ref(dda);
	Cudd_RecursiveDeref(dd_man.manager(), dds);
	Cudd_RecursiveDeref(dd_man.manager(), init_bdd_);
	init_bdd_ = dda;
	v.primed_mtbdd(dd_man.manager());
	v.identity_bdd(dd_man);
      }
      mod.identity_bdd(dd_man);
    }
    for (VariableList::const_reverse_iterator vi = variables().rbegin();
	 vi != variables().rend(); vi++) {
      const Variable& v = **vi;
      DdNode* ddv = v.mtbdd(dd_man.manager());
      DdNode* dds =
          Cudd_addBddInterval(dd_man.manager(), ddv, v.start(), v.start());
      Cudd_Ref(dds);
      Cudd_RecursiveDeref(dd_man.manager(), ddv);
      DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds, init_bdd_);
      Cudd_Ref(dda);
      Cudd_RecursiveDeref(dd_man.manager(), dds);
      Cudd_RecursiveDeref(dd_man.manager(), init_bdd_);
      init_bdd_ = dda;
      v.primed_mtbdd(dd_man.manager());
      v.identity_bdd(dd_man);
    }
    /* BDD for variable ranges. */
    DdNode* range = range_bdd(dd_man);
    /*
     * Generate phase-type distributions for commands with
     * non-exponential delay.
     */
    size_t nvars = dd_man.GetNumVariables()/2;
    std::map<size_t, PHData> ph_commands;
    for (int i = commands().size() - 1; i >= 0; i--) {
      const Command& command = *commands_[i];
      const Distribution& dist = command.delay();
      if (typeid(dist) != typeid(Exponential)) {
	PHData& data = ph_commands[i];
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
	  data.s = new Variable(0, high, 0, nvars);
	  Expression::ref(data.s);
	  nvars = data.s->high_bit() + 1;
	  int nbits = data.s->high_bit() - data.s->low_bit() + 1;
	  for (int b = 0; b < nbits; b++) {
	    Cudd_bddNewVarAtLevel(dd_man.manager(), 2*b);
	    Cudd_bddNewVarAtLevel(dd_man.manager(), 2*b + 1);
	  }
	  DdNode* ddv = data.s->mtbdd(dd_man.manager());
	  DdNode* dds =
              Cudd_addBddInterval(dd_man.manager(), ddv, data.s->start(),
                                  data.s->start());
	  Cudd_Ref(dds);
	  DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds, init_bdd_);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), dds);
	  Cudd_RecursiveDeref(dd_man.manager(), init_bdd_);
	  init_bdd_ = dda;
	  DdNode* ddvp = data.s->primed_mtbdd(dd_man.manager());
	  DdNode* ddid = data.s->identity_bdd(dd_man);
	  DdNode* ddr = data.s->range_bdd(dd_man.manager());
	  dda = Cudd_bddAnd(dd_man.manager(), ddr, range);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), ddr);
	  Cudd_RecursiveDeref(dd_man.manager(), range);
	  range = dda;
	  /*
	   * Constructs BDD representing phase update:
	   *
	   *   (!phi -> s=0) & (phi' -> s'=s) & (!phi' -> s'=0)
	   */
	  DdNode* ddg = command.guard().bdd(dd_man);
	  dds = Cudd_addBddInterval(dd_man.manager(), ddv, 0, 0);
	  Cudd_Ref(dds);
	  DdNode* ddo = Cudd_bddOr(dd_man.manager(), ddg, dds);
	  Cudd_Ref(ddo);
	  Cudd_RecursiveDeref(dd_man.manager(), ddg);
	  Cudd_RecursiveDeref(dd_man.manager(), dds);
	  data.update_bdd = ddo;
	  ddg = command.guard().primed_bdd(dd_man);
	  dds = Cudd_addBddInterval(dd_man.manager(), ddvp, 0, 0);
	  Cudd_Ref(dds);
	  ddo = Cudd_bddOr(dd_man.manager(), ddg, dds);
	  Cudd_Ref(ddo);
	  Cudd_RecursiveDeref(dd_man.manager(), dds);
	  dda = Cudd_bddAnd(dd_man.manager(), data.update_bdd, ddo);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), data.update_bdd);
	  Cudd_RecursiveDeref(dd_man.manager(), ddo);
	  data.update_bdd = dda;
	  DdNode* ddn = Cudd_Not(ddg);
	  Cudd_Ref(ddn);
	  Cudd_RecursiveDeref(dd_man.manager(), ddg);
	  ddo = Cudd_bddOr(dd_man.manager(), ddn, ddid);
	  Cudd_Ref(ddo);
	  Cudd_RecursiveDeref(dd_man.manager(), ddn);
	  dda = Cudd_bddAnd(dd_man.manager(), data.update_bdd, ddo);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), data.update_bdd);
	  Cudd_RecursiveDeref(dd_man.manager(), ddo);
	  data.update_bdd = dda;
	} else {
	  data.s = NULL;
	}
      }
    }
    /*
     * Compute rate matrix for all commands.
     */
    DdNode* ddR = Cudd_ReadZero(dd_man.manager());
    Cudd_Ref(ddR);
    for (int i = commands().size() - 1; i >= 0; i--) {
      const Command& command = *commands_[i];
      if (verbosity > 1) {
	std::cout << std::endl << "processing " << command << std::endl;
      }
      /* BDD for guard. */
      DdNode* ddg = command.guard().bdd(dd_man);
      /* BDD for command. */
      VariableSet updated_variables;
      DdNode* ddc = command.bdd(updated_variables, dd_man);
      const Exponential* exp_delay =
	dynamic_cast<const Exponential*>(&command.delay());
      PHData* ph_data = (exp_delay != NULL) ? NULL : &ph_commands[i];
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
	DdNode* ddv = ph_data->s->mtbdd(dd_man.manager());
	DdNode* dds = Cudd_addBddInterval(dd_man.manager(), ddv, 0, 0);
	Cudd_Ref(dds);
	DdNode* ddvp = ph_data->s->primed_mtbdd(dd_man.manager());
	DdNode* ddu = Cudd_addBddInterval(dd_man.manager(), ddvp, 1, 1);
	Cudd_Ref(ddu);
	DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds, ddu);
	Cudd_Ref(dda);
	Cudd_RecursiveDeref(dd_man.manager(), ddu);
	ddu = Cudd_bddAnd(dd_man.manager(), dda, ddg);
	Cudd_Ref(ddu);
	Cudd_RecursiveDeref(dd_man.manager(), dda);
	ddu = variable_updates(dd_man, ddu, ModuleSet(),
			       VariableSet(), ph_data->s, ph_commands);
	DdNode* ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	Cudd_Ref(ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddu);
	DdNode* ddr = Cudd_addConst(dd_man.manager(),
				    ph_data->params2.p*ph_data->params2.r1);
	Cudd_Ref(ddr);
	DdNode* ddq = Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	Cudd_Ref(ddq);
	Cudd_RecursiveDeref(dd_man.manager(), ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddr);
	if (verbosity > 1) {
	  Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	}
	ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	Cudd_Ref(ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddq);
	Cudd_RecursiveDeref(dd_man.manager(), ddR);
	ddR = ddt;
	/*
	 * Event 2: phi & s=0 => s'=0 & effects
	 */
	DdNode* ddp = Cudd_addBddInterval(dd_man.manager(), ddvp, 0, 0);
	Cudd_Ref(ddp);
	dda = Cudd_bddAnd(dd_man.manager(), dds, ddp);
	Cudd_Ref(dda);
	Cudd_RecursiveDeref(dd_man.manager(), dds);
	ddu = Cudd_bddAnd(dd_man.manager(), ddc, dda);
	Cudd_Ref(ddu);
	Cudd_RecursiveDeref(dd_man.manager(), dda);
	ddu = variable_updates(dd_man, ddu, command_modules_[i],
			       updated_variables, ph_data->s, ph_commands);
	ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	Cudd_Ref(ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddu);
	ddr = Cudd_addConst(dd_man.manager(),
			    (1.0 - ph_data->params2.p)*ph_data->params2.r1);
	Cudd_Ref(ddr);
	ddq = Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	Cudd_Ref(ddq);
	Cudd_RecursiveDeref(dd_man.manager(), ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddr);
	if (verbosity > 1) {
	  Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	}
	ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	Cudd_Ref(ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddq);
	Cudd_RecursiveDeref(dd_man.manager(), ddR);
	ddR = ddt;
	/*
	 * Event 3: phi & s=n-1 => s'=0 & effects
	 */
	dds = Cudd_addBddInterval(dd_man.manager(), ddv, ph_data->params2.n - 1,
				  ph_data->params2.n - 1);
	Cudd_Ref(dds);
	dda = Cudd_bddAnd(dd_man.manager(), dds, ddp);
	Cudd_Ref(dda);
	Cudd_RecursiveDeref(dd_man.manager(), dds);
	Cudd_RecursiveDeref(dd_man.manager(), ddp);
	ddu = Cudd_bddAnd(dd_man.manager(), ddc, dda);
	Cudd_Ref(ddu);
	Cudd_RecursiveDeref(dd_man.manager(), dda);
	ddu = variable_updates(dd_man, ddu, command_modules_[i],
			       updated_variables, ph_data->s, ph_commands);
	ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	Cudd_Ref(ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddu);
	ddr = Cudd_addConst(dd_man.manager(), ph_data->params2.r2);
	Cudd_Ref(ddr);
	ddq = Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	Cudd_Ref(ddq);
	Cudd_RecursiveDeref(dd_man.manager(), ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddr);
	if (verbosity > 1) {
	  Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	}
	ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	Cudd_Ref(ddt);
	Cudd_RecursiveDeref(dd_man.manager(), ddq);
	Cudd_RecursiveDeref(dd_man.manager(), ddR);
	ddR = ddt;
	if (ph_data->params2.n > 2) {
	  /*
	   * Event 4: phi & s>=1 & s<=n-2 => s'=s+1
	   */
	  DdNode* dds = Cudd_addBddInterval(dd_man.manager(), ddv, 1,
					    ph_data->params2.n - 2);
	  Cudd_Ref(dds);
	  DdNode* dd1 = Cudd_addConst(dd_man.manager(), 1);
	  Cudd_Ref(dd1);
	  DdNode* ddp = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddv, dd1);
	  Cudd_Ref(ddp);
	  Cudd_RecursiveDeref(dd_man.manager(), dd1);
	  DdNode* ddu = equality_bdd(dd_man, ddvp, ddp);
	  Cudd_RecursiveDeref(dd_man.manager(), ddp);
	  DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds, ddu);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), dds);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  ddu = Cudd_bddAnd(dd_man.manager(), dda, ddg);
	  Cudd_Ref(ddu);
	  Cudd_RecursiveDeref(dd_man.manager(), dda);
	  ddu = variable_updates(dd_man, ddu, ModuleSet(),
				 VariableSet(), ph_data->s, ph_commands);
	  DdNode* ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  DdNode* ddr = Cudd_addConst(dd_man.manager(), ph_data->params2.r1);
	  Cudd_Ref(ddr);
	  DdNode* ddq =
              Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	  Cudd_Ref(ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddr);
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	  }
	  ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddR);
	  ddR = ddt;
	}
	Cudd_RecursiveDeref(dd_man.manager(), ddv);
	Cudd_RecursiveDeref(dd_man.manager(), ddvp);
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
	  DdNode* ddv = ph_data->s->mtbdd(dd_man.manager());
	  DdNode* dds = Cudd_addBddInterval(dd_man.manager(), ddv, 0,
					    ph_data->params.n - 3);
	  Cudd_Ref(dds);
	  DdNode* ddvp = ph_data->s->primed_mtbdd(dd_man.manager());
	  DdNode* dd1 = Cudd_addConst(dd_man.manager(), 1);
	  Cudd_Ref(dd1);
	  DdNode* ddp = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddv, dd1);
	  Cudd_Ref(ddp);
	  Cudd_RecursiveDeref(dd_man.manager(), ddv);
	  Cudd_RecursiveDeref(dd_man.manager(), dd1);
	  DdNode* ddu = equality_bdd(dd_man, ddvp, ddp);
	  Cudd_RecursiveDeref(dd_man.manager(), ddvp);
	  Cudd_RecursiveDeref(dd_man.manager(), ddp);
	  DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds, ddu);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), dds);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  ddu = Cudd_bddAnd(dd_man.manager(), dda, ddg);
	  Cudd_Ref(ddu);
	  Cudd_RecursiveDeref(dd_man.manager(), dda);
	  ddu = variable_updates(dd_man, ddu, ModuleSet(),
				 VariableSet(), ph_data->s, ph_commands);
	  DdNode* ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  DdNode* ddr = Cudd_addConst(dd_man.manager(), ph_data->params.re);
	  Cudd_Ref(ddr);
	  DdNode* ddq =
              Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	  Cudd_Ref(ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddr);
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	  }
	  ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddR);
	  ddR = ddt;
	}
	if (ph_data != NULL && ph_data->params.pc > 0.0) {
	  /*
	   * Event 1 for phase transition in Coxian part of phase-type
	   * distribution:
	   *
	   *   phi & s=n-2 => s'=n-1
	   */
	  DdNode* ddv = ph_data->s->mtbdd(dd_man.manager());
	  DdNode* dds = Cudd_addBddInterval(dd_man.manager(), ddv,
                                            ph_data->params.n - 2,
					    ph_data->params.n - 2);
	  Cudd_Ref(dds);
	  DdNode* ddvp = ph_data->s->primed_mtbdd(dd_man.manager());
	  DdNode* ddu = Cudd_addBddInterval(dd_man.manager(), ddvp,
					    ph_data->params.n - 1,
					    ph_data->params.n - 1);
	  Cudd_Ref(ddu);
	  DdNode* dda = Cudd_bddAnd(dd_man.manager(), dds, ddu);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  ddu = Cudd_bddAnd(dd_man.manager(), dda, ddg);
	  Cudd_Ref(ddu);
	  Cudd_RecursiveDeref(dd_man.manager(), dda);
	  ddu = variable_updates(dd_man, ddu, ModuleSet(),
				 VariableSet(), ph_data->s, ph_commands);
	  DdNode* ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  DdNode* ddr = Cudd_addConst(dd_man.manager(),
				      ph_data->params.pc*ph_data->params.rc1);
	  Cudd_Ref(ddr);
	  DdNode* ddq =
              Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	  Cudd_Ref(ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddr);
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	  }
	  ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddR);
	  ddR = ddt;
	  /*
	   * Event 2 for phase transition in Coxian part of phase-type
	   * distribution:
	   *
	   *   phi & s=n-2 => s'=0 & effects
	   */
	  DdNode* ddp = Cudd_addBddInterval(dd_man.manager(), ddvp, 0, 0);
	  Cudd_Ref(ddp);
	  Cudd_RecursiveDeref(dd_man.manager(), ddvp);
	  dda = Cudd_bddAnd(dd_man.manager(), dds, ddp);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), dds);
	  ddu = Cudd_bddAnd(dd_man.manager(), ddc, dda);
	  Cudd_Ref(ddu);
	  Cudd_RecursiveDeref(dd_man.manager(), dda);
	  ddu = variable_updates(dd_man, ddu, command_modules_[i],
				 updated_variables, ph_data->s, ph_commands);
	  ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  ddr = Cudd_addConst(dd_man.manager(),
			      (1.0 - ph_data->params.pc)*ph_data->params.rc1);
	  Cudd_Ref(ddr);
	  ddq = Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	  Cudd_Ref(ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddr);
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	  }
	  ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddR);
	  ddR = ddt;
	  /*
	   * Event 3 for phase transition in Coxian part of phase-type
	   * distribution:
	   *
	   *   phi & s=n-1 => s'=0 & effects
	   */
	  dds = Cudd_addBddInterval(dd_man.manager(), ddv,
                                    ph_data->params.n - 1,
				    ph_data->params.n - 1);
	  Cudd_Ref(dds);
	  Cudd_RecursiveDeref(dd_man.manager(), ddv);
	  dda = Cudd_bddAnd(dd_man.manager(), dds, ddp);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man.manager(), dds);
	  Cudd_RecursiveDeref(dd_man.manager(), ddp);
	  ddu = Cudd_bddAnd(dd_man.manager(), ddc, dda);
	  Cudd_Ref(ddu);
	  Cudd_RecursiveDeref(dd_man.manager(), dda);
	  ddu = variable_updates(dd_man, ddu, command_modules_[i],
				 updated_variables, ph_data->s, ph_commands);
	  ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  ddr = Cudd_addConst(dd_man.manager(), ph_data->params.rc2);
	  Cudd_Ref(ddr);
	  ddq = Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	  Cudd_Ref(ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddr);
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	  }
	  ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddR);
	  ddR = ddt;
	} else {
	  /*
	   * Event for exponential (or 1-phase Coxian) distribution.
	   */
	  DdNode* dda;
	  if (ph_data != NULL && ph_data->s != NULL) {
	    /* Coxian: s=n-2 => s'=0 */
	    DdNode* ddv = ph_data->s->mtbdd(dd_man.manager());
	    DdNode* dds = Cudd_addBddInterval(dd_man.manager(), ddv,
					      ph_data->params.n - 2,
					      ph_data->params.n - 2);
	    Cudd_Ref(dds);
	    Cudd_RecursiveDeref(dd_man.manager(), ddv);
	    DdNode* ddvp = ph_data->s->primed_mtbdd(dd_man.manager());
	    DdNode* ddp = Cudd_addBddInterval(dd_man.manager(), ddvp, 0, 0);
	    Cudd_Ref(ddp);
	    Cudd_RecursiveDeref(dd_man.manager(), ddvp);
	    dda = Cudd_bddAnd(dd_man.manager(), dds, ddp);
	    Cudd_Ref(dda);
	    Cudd_RecursiveDeref(dd_man.manager(), dds);
	    Cudd_RecursiveDeref(dd_man.manager(), ddp);
	    dda = variable_updates(dd_man, dda, command_modules_[i],
				   updated_variables, ph_data->s, ph_commands);
	  } else {
	    dda = Cudd_ReadOne(dd_man.manager());
	    Cudd_Ref(dda);
	    dda = variable_updates(dd_man, dda, command_modules_[i],
				   updated_variables, NULL, ph_commands);
	  }
	  DdNode* ddu = Cudd_bddAnd(dd_man.manager(), ddc, dda);
	  Cudd_Ref(ddu);
	  Cudd_RecursiveDeref(dd_man.manager(), dda);
	  DdNode* ddt = Cudd_BddToAdd(dd_man.manager(), ddu);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddu);
	  DdNode* ddr;
	  if (exp_delay != NULL) {
	    ddr = exp_delay->rate().mtbdd(dd_man.manager());
	  } else {
	    ddr = Cudd_addConst(dd_man.manager(), ph_data->params.rc1);
	    Cudd_Ref(ddr);
	  }
	  DdNode* ddq =
              Cudd_addApply(dd_man.manager(), Cudd_addTimes, ddt, ddr);
	  Cudd_Ref(ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddr);
	  if (verbosity > 1) {
	    Cudd_PrintDebug(dd_man.manager(), ddq, dd_man.GetNumVariables(), 2);
	  }
	  ddt = Cudd_addApply(dd_man.manager(), Cudd_addPlus, ddq, ddR);
	  Cudd_Ref(ddt);
	  Cudd_RecursiveDeref(dd_man.manager(), ddq);
	  Cudd_RecursiveDeref(dd_man.manager(), ddR);
	  ddR = ddt;
	}
      }
      Cudd_RecursiveDeref(dd_man.manager(), ddg);
      Cudd_RecursiveDeref(dd_man.manager(), ddc);
    }
    /* Release BDD for variable ranges. */
    Cudd_RecursiveDeref(dd_man.manager(), range);
    /*
     * Release DDs for phase variables.
     */
    for (std::map<size_t, PHData>::const_iterator ci = ph_commands.begin();
	 ci != ph_commands.end(); ci++) {
      if ((*ci).second.s != NULL) {
	(*ci).second.s->uncache_dds(dd_man.manager());
	Expression::destructive_deref((*ci).second.s);
	Cudd_RecursiveDeref(dd_man.manager(), (*ci).second.update_bdd);
      }
    }
    /*
     * Release DDs for modules.
     */
    for (ModuleList::const_reverse_iterator mi = modules().rbegin();
	 mi != modules().rend(); mi++) {
      (*mi)->uncache_dds(dd_man);
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
        Cudd_addApply(dd_man.manager(), Cudd_addTimes, reach_add, ddR);
    Cudd_Ref(ddT);
    Cudd_RecursiveDeref(dd_man.manager(), ddR);
    rate_mtbdd_ = ddT;
    /* Build ODD. */
    odd_ = build_odd(dd_man.manager(), reach_add, row_variables_, nvars);
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
    for (ModuleList::const_reverse_iterator mi = modules().rbegin();
	 mi != modules().rend(); mi++) {
      const Module& mod = **mi;
      for (VariableList::const_reverse_iterator vi = mod.variables().rbegin();
	   vi != mod.variables().rend(); vi++) {
	const Variable& v = **vi;
	v.uncache_dds(dd_man.manager());
      }
    }
    for (VariableList::const_reverse_iterator vi = variables().rbegin();
	 vi != variables().rend(); vi++) {
      const Variable& v = **vi;
      v.uncache_dds(dd_man.manager());
    }
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


/* Returns a BDD representing the range for all model variables. */
DdNode* Model::range_bdd(const DecisionDiagramManager& dd_man) const {
  DdNode* dd = Cudd_ReadOne(dd_man.manager());
  Cudd_Ref(dd);
  for (ModuleList::const_reverse_iterator mi = modules().rbegin();
       mi != modules().rend(); mi++) {
    const VariableList& mod_variables = (*mi)->variables();
    for (VariableList::const_reverse_iterator vi = mod_variables.rbegin();
	 vi != mod_variables.rend(); vi++) {
      DdNode* ddv = (*vi)->range_bdd(dd_man.manager());
      DdNode* ddr = Cudd_bddAnd(dd_man.manager(), ddv, dd);
      Cudd_Ref(ddr);
      Cudd_RecursiveDeref(dd_man.manager(), ddv);
      Cudd_RecursiveDeref(dd_man.manager(), dd);
      dd = ddr;
    }
  }
  for (VariableList::const_reverse_iterator vi = variables().rbegin();
       vi != variables().rend(); vi++) {
    DdNode* ddv = (*vi)->range_bdd(dd_man.manager());
    DdNode* ddr = Cudd_bddAnd(dd_man.manager(), ddv, dd);
    Cudd_Ref(ddr);
    Cudd_RecursiveDeref(dd_man.manager(), ddv);
    Cudd_RecursiveDeref(dd_man.manager(), dd);
    dd = ddr;
  }
  return dd;
}


/* Returns a BDD representing the conjunction of dd_start with the
   BDDs for updates of all variables not explicitly mentioned. */
DdNode*
Model::variable_updates(const DecisionDiagramManager& dd_man, DdNode* dd_start,
			const ModuleSet& touched_modules,
			const VariableSet& updated_variables,
			const Variable* phase_variable,
			const std::map<size_t, PHData>& ph_commands) const {
  DdNode* ddu = dd_start;
  /*
   * Conjunction with identity BDD for untouched module variables.
   */
  for (ModuleList::const_reverse_iterator mi = modules().rbegin();
       mi != modules().rend(); mi++) {
    if (touched_modules.find(*mi) != touched_modules.end()) {
      ddu = variable_identities(dd_man, ddu, (*mi)->variables(),
				updated_variables);
    } else {
      DdNode* ddm = (*mi)->identity_bdd(dd_man);
      DdNode* dda = Cudd_bddAnd(dd_man.manager(), ddm, ddu);
      Cudd_Ref(dda);
      Cudd_RecursiveDeref(dd_man.manager(), ddm);
      Cudd_RecursiveDeref(dd_man.manager(), ddu);
      ddu = dda;
    }
  }
  /* Conjunction with identity BDD for untouched global variables. */
  ddu = variable_identities(dd_man, ddu, variables(), updated_variables);
  /*
   * Conjunction with update rules for phase variables.
   */
  for (std::map<size_t, PHData>::const_reverse_iterator ci =
	 ph_commands.rbegin();
       ci != ph_commands.rend(); ci++) {
    const Variable* var = (*ci).second.s;
    if (var != phase_variable && var != NULL) {
      DdNode* dda = Cudd_bddAnd(dd_man.manager(), (*ci).second.update_bdd, ddu);
      Cudd_Ref(dda);
      Cudd_RecursiveDeref(dd_man.manager(), ddu);
      ddu = dda;
    }
  }
  return ddu;
}


/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m) {
  os << "stochastic";
  VariableList::const_iterator vi = m.variables().begin();
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
