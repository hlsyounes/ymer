/*
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: models.cc,v 1.7 2003-11-07 22:00:05 lorens Exp $
 */
#include "models.h"
#include "formulas.h"


/* Verbosity level. */
extern int verbosity;


/* ====================================================================== */
/* Model */

/* Returns a reachability BDD for the given initial state and rate
   matrix. */
static DdNode* reachability_bdd(DdManager* dd_man,
				DdNode* init, DdNode* rates,
				DdNode** row_variables) {
  if (verbosity > 0) {
    std::cout << "Computing reachable states...";
  }
  /*
   * Precompute variable permutations and cubes.
   */
  size_t nvars = Cudd_ReadSize(dd_man);
  int* row_to_col = new int[nvars];
  int* col_to_row = new int[nvars];
  for (size_t i = 0; i < nvars; i++) {
    if (i % 2 == 0) {
      row_to_col[i] = i + 1;
      col_to_row[i] = i;
    } else {
      row_to_col[i] = i;
      col_to_row[i] = i - 1;
    }
  }
  nvars /= 2;
  DdNode* row_cube = Cudd_bddComputeCube(dd_man, row_variables, NULL, nvars);
  Cudd_Ref(row_cube);
  /*
   * Fixpoint computation of reachability.
   */
  DdNode* trans = Cudd_addBddStrictThreshold(dd_man, rates, 0);
  Cudd_Ref(trans);
  DdNode* solr = init;
  Cudd_Ref(solr);
  // NOTE: use bddSwapVariables instead!
  DdNode* solc = Cudd_bddPermute(dd_man, solr, row_to_col);
  Cudd_Ref(solc);
  bool done = false;
  size_t iter = 0;
  while (!done) {
    iter++;
    DdNode* dda = Cudd_bddAnd(dd_man, trans, solr);
    Cudd_Ref(dda);
    Cudd_RecursiveDeref(dd_man, solr);
    DdNode* dde = Cudd_bddExistAbstract(dd_man, dda, row_cube);
    Cudd_Ref(dde);
    Cudd_RecursiveDeref(dd_man, dda);
    DdNode* ddo = Cudd_bddOr(dd_man, solc, dde);
    if (ddo == solc) {
      done = true;
    } else {
      Cudd_Ref(ddo);
      Cudd_RecursiveDeref(dd_man, solc);
      solc = ddo;
    }
    Cudd_RecursiveDeref(dd_man, dde);
    solr = Cudd_bddPermute(dd_man, solc, col_to_row);
    Cudd_Ref(solr);
  }
  Cudd_RecursiveDeref(dd_man, solc);
  Cudd_RecursiveDeref(dd_man, trans);
  Cudd_RecursiveDeref(dd_man, row_cube);
  delete row_to_col;
  delete col_to_row;
  if (verbosity > 0) {
    std::cout << iter << " iterations." << std::endl;
  }
  return solr;
}


/* Returns a BDD representing the conjunction of dd_start with the
   identity BDDs for the given variables. */
static DdNode* variable_identities(DdManager* dd_man, DdNode* dd_start,
				   const VariableList& variables,
				   const VariableSet& excluded) {
  DdNode* ddu = dd_start;
  for (VariableList::const_reverse_iterator vi = variables.rbegin();
       vi != variables.rend(); vi++) {
    if (excluded.find(*vi) == excluded.end()) {
      DdNode* ddv = (*vi)->identity_bdd(dd_man);
      DdNode* dda = Cudd_bddAnd(dd_man, ddv, ddu);
      Cudd_Ref(dda);
      Cudd_RecursiveDeref(dd_man, ddv);
      Cudd_RecursiveDeref(dd_man, ddu);
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
    Expression::unregister_use(*vi);
  }
  for (ModuleList::const_iterator mi = modules().begin();
       mi != modules().end(); mi++) {
    delete *mi;
  }
}


/* Adds a global variable to this model. */
void Model::add_variable(const Variable& variable) {
  variables_.push_back(&variable);
  Expression::register_use(&variable);
}


/* Adds a module to this model. */
void Model::add_module(const Module& module) {
  modules_.push_back(&module);
}


/* Caches commands for this model. */
void Model::cache_commands() const {
  commands_.clear();
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
	    Command* c =
	      new Command(*si, guard,
			  Multiplication::make(ci.rate(), cj.rate()));
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
	  }
	}
      }
    }
  }
}


/* Caches DDs for for this model. */
void Model::cache_dds(DdManager* dd_man) const {
  if (rate_mtbdd_ == NULL) {
    if (verbosity > 0) {
      std::cout << "Buidling model...";
    }
    /*
     * Precomute DDs for variables and modules.
     */
    init_bdd_ = Cudd_ReadOne(dd_man);
    Cudd_Ref(init_bdd_);
    for (ModuleList::const_reverse_iterator mi = modules().rbegin();
	 mi != modules().rend(); mi++) {
      const Module& mod = **mi;
      for (VariableList::const_reverse_iterator vi = mod.variables().rbegin();
	   vi != mod.variables().rend(); vi++) {
	const Variable& v = **vi;
	DdNode* ddv = v.mtbdd(dd_man);
	DdNode* dds = Cudd_addBddInterval(dd_man, ddv, v.start(), v.start());
	Cudd_Ref(dds);
	DdNode* dda = Cudd_bddAnd(dd_man, dds, init_bdd_);
	Cudd_Ref(dda);
	Cudd_RecursiveDeref(dd_man, dds);
	Cudd_RecursiveDeref(dd_man, init_bdd_);
	init_bdd_ = dda;
	v.primed_mtbdd(dd_man);
	v.identity_bdd(dd_man);
      }
      mod.identity_bdd(dd_man);
    }
    for (VariableList::const_reverse_iterator vi = variables().rbegin();
	 vi != variables().rend(); vi++) {
      const Variable& v = **vi;
      DdNode* ddv = v.mtbdd(dd_man);
      DdNode* dds = Cudd_addBddInterval(dd_man, ddv, v.start(), v.start());
      Cudd_Ref(dds);
      Cudd_RecursiveDeref(dd_man, ddv);
      DdNode* dda = Cudd_bddAnd(dd_man, dds, init_bdd_);
      Cudd_Ref(dda);
      Cudd_RecursiveDeref(dd_man, dds);
      Cudd_RecursiveDeref(dd_man, init_bdd_);
      init_bdd_ = dda;
      v.primed_mtbdd(dd_man);
      v.identity_bdd(dd_man);
    }
    /* BDD for variable ranges. */
    DdNode* range = range_bdd(dd_man);
    /*
     * Compute rate matrix for non-synchronized commands.
     */
    DdNode* ddR = Cudd_ReadZero(dd_man);
    Cudd_Ref(ddR);
    SynchronizationMap synch_commands;
    std::set<size_t> synchs;
    for (ModuleList::const_reverse_iterator mi = modules().rbegin();
	 mi != modules().rend(); mi++) {
      const VariableList& mod_variables = (*mi)->variables();
      const CommandList& mod_commands = (*mi)->commands();
      for (CommandList::const_iterator ci = mod_commands.begin();
	   ci != mod_commands.end(); ci++) {
	const Command& command = **ci;
	size_t synch = command.synch();
	if (synch != 0) {
	  /* Command is synchronized so store it for later processing. */
	  synch_commands.insert(std::make_pair(std::make_pair(*mi, synch),
					       *ci));
	  synchs.insert(synch);
	} else { /* Command is not synchronized. */
	  /* BDD for command. */
	  VariableSet updated_variables;
	  DdNode* ddu = command.bdd(updated_variables, dd_man);
	  /* Conjunction with identity BDD for untouched global variables. */
	  ddu = variable_identities(dd_man, ddu, variables(),
				    updated_variables);
	  /* Conjunction with identity BDD for untouched module variables. */
	  ddu = variable_identities(dd_man, ddu, mod_variables,
				    updated_variables);
	  /*
	   * Conjunction with identity BDD for other modules.
	   */
	  for (ModuleList::const_reverse_iterator mj = modules().rbegin();
	       mj != modules().rend(); mj++) {
	    if (*mj != *mi) {
	      DdNode* ddm = (*mj)->identity_bdd(dd_man);
	      DdNode* dda = Cudd_bddAnd(dd_man, ddm, ddu);
	      Cudd_Ref(dda);
	      Cudd_RecursiveDeref(dd_man, ddm);
	      Cudd_RecursiveDeref(dd_man, ddu);
	      ddu = dda;
	    }
	  }
	  /*
	   * Conjunction with BDD for variable ranges.
	   */
	  DdNode* dda = Cudd_bddAnd(dd_man, range, ddu);
	  Cudd_Ref(dda);
	  Cudd_RecursiveDeref(dd_man, ddu);
	  ddu = dda;
	  /*
	   * Multiply BDD with MTBDD for rate.
	   */
	  DdNode* ddU = Cudd_BddToAdd(dd_man, ddu);
	  Cudd_Ref(ddU);
	  Cudd_RecursiveDeref(dd_man, ddu);
	  DdNode* ddL = command.rate().mtbdd(dd_man);
	  DdNode* ddC = Cudd_addApply(dd_man, Cudd_addTimes, ddL, ddU);
	  Cudd_Ref(ddC);
	  Cudd_RecursiveDeref(dd_man, ddL);
	  Cudd_RecursiveDeref(dd_man, ddU);
	  /*
	   * Add MTBDD for command to MTBDD for model.
	   */
	  DdNode* ddT = Cudd_addApply(dd_man, Cudd_addPlus, ddC, ddR);
	  Cudd_Ref(ddT);
	  Cudd_RecursiveDeref(dd_man, ddC);
	  Cudd_RecursiveDeref(dd_man, ddR);
	  ddR = ddT;
	}
      }
    }
    /*
     * Add MTBDDs for synchronized commands.
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
	      VariableSet updated_variables;
	      DdNode* dd1 = ci.bdd(updated_variables, dd_man);
	      DdNode* dd2 = cj.bdd(updated_variables, dd_man);
	      DdNode* ddu = Cudd_bddAnd(dd_man, dd1, dd2);
	      Cudd_Ref(ddu);
	      Cudd_RecursiveDeref(dd_man, dd1);
	      Cudd_RecursiveDeref(dd_man, dd2);
	      /* Conjunction with identity BDD for untouched global
		 variables. */
	      ddu = variable_identities(dd_man, ddu, variables(),
					updated_variables);
	      /* Conjunction with identity BDD for untouched module
		 variables. */
	      ddu = variable_identities(dd_man, ddu, (*mi)->variables(),
					updated_variables);
	      ddu = variable_identities(dd_man, ddu, (*mj)->variables(),
					updated_variables);
	      /*
	       * Conjunction with identity BDD for other modules.
	       */
	      for (ModuleList::const_reverse_iterator mk = modules().rbegin();
		   mk != modules().rend(); mk++) {
		if (*mk != *mi && *mk != *mj) {
		  DdNode* ddm = (*mk)->identity_bdd(dd_man);
		  DdNode* dda = Cudd_bddAnd(dd_man, ddm, ddu);
		  Cudd_Ref(dda);
		  Cudd_RecursiveDeref(dd_man, ddm);
		  Cudd_RecursiveDeref(dd_man, ddu);
		  ddu = dda;
		}
	      }
	      /*
	       * Conjunction with BDD for variable ranges.
	       */
	      DdNode* dda = Cudd_bddAnd(dd_man, range, ddu);
	      Cudd_Ref(dda);
	      Cudd_RecursiveDeref(dd_man, ddu);
	      ddu = dda;
	      /*
	       * Multiply BDD with MTBDD for rate.
	       */
	      DdNode* ddC = Cudd_BddToAdd(dd_man, ddu);
	      Cudd_Ref(ddC);
	      Cudd_RecursiveDeref(dd_man, ddu);
	      DdNode* ddL = ci.rate().mtbdd(dd_man);
	      DdNode* ddU = Cudd_addApply(dd_man, Cudd_addTimes, ddL, ddC);
	      Cudd_Ref(ddU);
	      Cudd_RecursiveDeref(dd_man, ddL);
	      Cudd_RecursiveDeref(dd_man, ddC);
	      ddL = cj.rate().mtbdd(dd_man);
	      ddC = Cudd_addApply(dd_man, Cudd_addTimes, ddL, ddU);
	      Cudd_Ref(ddC);
	      Cudd_RecursiveDeref(dd_man, ddL);
	      Cudd_RecursiveDeref(dd_man, ddU);
	      /*
	       * Add MTBDD for command to MTBDD for model.
	       */
	      DdNode* ddT = Cudd_addApply(dd_man, Cudd_addPlus, ddC, ddR);
	      Cudd_Ref(ddT);
	      Cudd_RecursiveDeref(dd_man, ddC);
	      Cudd_RecursiveDeref(dd_man, ddR);
	      ddR = ddT;
	    }
	  }
	}
      }
    }
    /* Release BDD for variable ranges. */
    Cudd_RecursiveDeref(dd_man, range);
    /*
     * Release DDs for modules.
     */
    for (ModuleList::const_reverse_iterator mi = modules().rbegin();
	 mi != modules().rend(); mi++) {
      (*mi)->uncache_dds(dd_man);
    }
    if (verbosity > 0) {
      std::cout << Cudd_ReadSize(dd_man) << " variables." << std::endl;
    }
    /*
     * Collect row and column variables.
     */
    size_t nvars = Cudd_ReadSize(dd_man) / 2;
    row_variables_ = new DdNode*[nvars];
    column_variables_ = new DdNode*[nvars];
    for (size_t i = 0; i < nvars; i++) {
      row_variables_[i] = Cudd_bddIthVar(dd_man, 2*i);
      column_variables_[i] = Cudd_bddIthVar(dd_man, 2*i + 1);
    }
    /*
     * Reachability analysis.
     */
    reach_bdd_ = ::reachability_bdd(dd_man, init_bdd_, ddR, row_variables_);
    DdNode* reach_add = Cudd_BddToAdd(dd_man, reach_bdd_);
    Cudd_Ref(reach_add);
    DdNode* ddT = Cudd_addApply(dd_man, Cudd_addTimes, reach_add, ddR);
    Cudd_Ref(ddT);
    Cudd_RecursiveDeref(dd_man, ddR);
    rate_mtbdd_ = ddT;
    /* Build ODD. */
    odd_ = build_odd(dd_man, reach_add, row_variables_, nvars);
    Cudd_RecursiveDeref(dd_man, reach_add);
  }
}


/* Returns an MTBDD representing the rate matrix for this model. */
DdNode* Model::rate_mtbdd(DdManager* dd_man) const {
  if (rate_mtbdd_ == NULL) {
    cache_dds(dd_man);
  }
  Cudd_Ref(rate_mtbdd_);
  return rate_mtbdd_;
}


/* Returns a reachability BDD for this model. */
DdNode* Model::reachability_bdd(DdManager* dd_man) const {
  if (reach_bdd_ == NULL) {
    cache_dds(dd_man);
  }
  Cudd_Ref(reach_bdd_);
  return reach_bdd_;
}


/* Returns an ODD for this model. */
ODDNode* Model::odd(DdManager* dd_man) const {
  if (odd_ == NULL) {
    cache_dds(dd_man);
  }
  return odd_;
}


/* Returns a BDD representing the initial state for this model. */
DdNode* Model::init_bdd(DdManager* dd_man) const {
  if (init_bdd_ == NULL) {
    cache_dds(dd_man);
  }
  Cudd_Ref(init_bdd_);
  return init_bdd_;
}


/* Returns the index associated with the initial state for this model. */
int Model::init_index(DdManager* dd_man) const {
  if (init_index_ == -1) {
    cache_dds(dd_man);
    DdNode* init_add_ = Cudd_BddToAdd(dd_man, init_bdd_);
    Cudd_Ref(init_add_);
    DdNode* d = init_add_;
    ODDNode* o = odd_;
    init_index_ = 0;
    size_t nvars = Cudd_ReadSize(dd_man) / 2;
    for (size_t i = 1; i <= nvars; i++) {
      bool bit;
      if (i == nvars) {
	bit = (Cudd_T(d) == Cudd_ReadOne(dd_man));
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
    Cudd_RecursiveDeref(dd_man, init_add_);
  }
  return init_index_;
}


/* Returns the row variables for this model. */
DdNode** Model::row_variables(DdManager* dd_man) const {
  if (row_variables_ == NULL) {
    cache_dds(dd_man);
  }
  return row_variables_;
}


/* Returns the column variables for this model. */
DdNode** Model::column_variables(DdManager* dd_man) const {
  if (column_variables_ == NULL) {
    cache_dds(dd_man);
  }
  return column_variables_;
}


/* Returns the number of states for this model. */
double Model::num_states(DdManager* dd_man) const {
  if (reach_bdd_ == NULL) {
    cache_dds(dd_man);
  }
  return Cudd_CountMinterm(dd_man, reach_bdd_, Cudd_ReadSize(dd_man) / 2);
}


/* Returns the number of transitions for this model. */
double Model::num_transitions(DdManager* dd_man) const {
  if (rate_mtbdd_ == NULL) {
    cache_dds(dd_man);
  }
  return Cudd_CountMinterm(dd_man, rate_mtbdd_, Cudd_ReadSize(dd_man));
}


/* Releases all DDs cached for this model. */
void Model::uncache_dds(DdManager* dd_man) const {
  if (rate_mtbdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man, rate_mtbdd_);
    rate_mtbdd_ = NULL;
    for (ModuleList::const_reverse_iterator mi = modules().rbegin();
	 mi != modules().rend(); mi++) {
      const Module& mod = **mi;
      for (VariableList::const_reverse_iterator vi = mod.variables().rbegin();
	   vi != mod.variables().rend(); vi++) {
	const Variable& v = **vi;
	v.uncache_dds(dd_man);
      }
    }
    for (VariableList::const_reverse_iterator vi = variables().rbegin();
	 vi != variables().rend(); vi++) {
      const Variable& v = **vi;
      v.uncache_dds(dd_man);
    }
  }
  if (reach_bdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man, reach_bdd_);
    reach_bdd_ = NULL;
  }
  odd_ = NULL;
  if (init_bdd_ != NULL) {
    Cudd_RecursiveDeref(dd_man, init_bdd_);
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
DdNode* Model::range_bdd(DdManager* dd_man) const {
  DdNode* dd = Cudd_ReadOne(dd_man);
  Cudd_Ref(dd);
  for (ModuleList::const_reverse_iterator mi = modules().rbegin();
       mi != modules().rend(); mi++) {
    const VariableList& mod_variables = (*mi)->variables();
    for (VariableList::const_reverse_iterator vi = mod_variables.rbegin();
	 vi != mod_variables.rend(); vi++) {
      DdNode* ddv = (*vi)->range_bdd(dd_man);
      DdNode* ddr = Cudd_bddAnd(dd_man, ddv, dd);
      Cudd_Ref(ddr);
      Cudd_RecursiveDeref(dd_man, ddv);
      Cudd_RecursiveDeref(dd_man, dd);
      dd = ddr;
    }
  }
  for (VariableList::const_reverse_iterator vi = variables().rbegin();
       vi != variables().rend(); vi++) {
    DdNode* ddv = (*vi)->range_bdd(dd_man);
    DdNode* ddr = Cudd_bddAnd(dd_man, ddv, dd);
    Cudd_Ref(ddr);
    Cudd_RecursiveDeref(dd_man, ddv);
    Cudd_RecursiveDeref(dd_man, dd);
    dd = ddr;
  }
  return dd;
}


/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m) {
  os << "stochastic";
  VariableList::const_iterator vi = m.variables().begin();
  if (vi != m.variables().end()) {
    os << std::endl;
    for (; vi != m.variables().end(); vi++) {
      const Variable& v = **vi;
      os << std::endl << "global ";
      v.print(os);
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
	os << std::endl << "  ";
	v.print(os);
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
	const Command& c = **ci;
	os << std::endl << "  [";
	if (c.synch() != 0) {
	  os << 's' << c.synch();
	}
	os << "] ";
	c.guard().print(os);
	os << " -> ";
	c.rate().print(os);
	os << " : ";
	UpdateList::const_iterator ui = c.updates().begin();
	if (ui != c.updates().end()) {
	  const Update* u = *ui;
	  u->variable().print(os);
	  os << "\'=";
	  u->expr().print(os);
	  for (ui++; ui != c.updates().end(); ui++) {
	    u = *ui;
	    os << " & ";
	    u->variable().print(os);
	    os << "\'=";
	    u->expr().print(os);
	  }
	}
	os << ';';
      }
    }
    os << std::endl << std::endl << "endmodule";
  }
  return os;
}
