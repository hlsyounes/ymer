/*
 * Models.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011 Google Inc
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
#include <set>
#include <stdexcept>


/* ====================================================================== */
/* SynchronizationMap */

/*
 * A synchronization map.
 */
struct SynchronizationMap
  : public std::multimap<std::pair<const Module*, int>, const Command*> {
};

/* Range for synchronization map. */
typedef std::pair<SynchronizationMap::const_iterator,
		  SynchronizationMap::const_iterator> SynchronizationMapRange;


/* ====================================================================== */
/* Model */

/* Deletes this model. */
Model::~Model() {
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    if ((*ci)->synch() != 0) {
      delete *ci;
    }
  }
  for (std::vector<const Constant<int>*>::const_iterator ci =
	 int_constants().begin();
       ci != int_constants().end(); ci++) {
    RCObject::deref(*ci);
  }
  for (std::vector<const Constant<double>*>::const_iterator ci =
	 double_constants().begin();
       ci != double_constants().end(); ci++) {
    RCObject::deref(*ci);
  }
  for (VariableList<int>::const_iterator vi = int_variables().begin();
       vi != int_variables().end(); vi++) {
    RCObject::deref(*vi);
  }
  for (VariableList<double>::const_iterator vi = double_variables().begin();
       vi != double_variables().end(); vi++) {
    RCObject::deref(*vi);
  }
  for (ModuleList::const_iterator mi = modules().begin();
       mi != modules().end(); mi++) {
    delete *mi;
  }
}


/* Adds a global integer-valued constant to this model. */
void Model::add_constant(const Constant<int>& constant) {
  int_constants_.push_back(&constant);
  RCObject::ref(&constant);
}


/* Adds a global double-valued constant to this model. */
void Model::add_constant(const Constant<double>& constant) {
  double_constants_.push_back(&constant);
  RCObject::ref(&constant);
}


/* Adds an integer-valued global variable to this model. */
void Model::add_variable(const Variable<int>& variable) {
  int_variables_.push_back(&variable);
  RCObject::ref(&variable);
}


/* Adds a double-valued global variable to this model. */
void Model::add_variable(const Variable<double>& variable) {
  double_variables_.push_back(&variable);
  RCObject::ref(&variable);
}


/* Adds a module to this model. */
void Model::add_module(const Module& module) {
  modules_.push_back(&module);
}


/* Compiles the commands of this model. */
void Model::compile() const {
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
  /*
   * Process the commands of each module.
   */
  std::set<int> synchs;
  SynchronizationMap synch_commands;
  for (ModuleList::const_reverse_iterator mi = modules().rbegin();
       mi != modules().rend(); mi++) {
    for (CommandList::const_iterator ci = (*mi)->commands().begin();
	 ci != (*mi)->commands().end(); ci++) {
      const Command& command = **ci;
      int synch = command.synch();
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
  for (std::set<int>::const_iterator si = synchs.begin();
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
	    if (&ci.delay() == &Exponential::STANDARD) {
	      c = new Command(*si, guard, cj.delay());
	    } else if (&cj.delay() == &Exponential::STANDARD) {
	      c = new Command(*si, guard, ci.delay());
	    } else {
	      throw std::logic_error("at least one command in a"
				     " synchronization pair must have rate 1");
	    }
	    for (UpdateList<int>::const_iterator ui = ci.int_updates().begin();
		 ui != ci.int_updates().end(); ui++) {
	      const Update<int>& u = **ui;
	      c->add_update(*new Update<int>(u.variable(), u.expr()));
	    }
	    for (UpdateList<double>::const_iterator ui =
		   ci.double_updates().begin();
		 ui != ci.double_updates().end(); ui++) {
	      const Update<double>& u = **ui;
	      c->add_update(*new Update<double>(u.variable(), u.expr()));
	    }
	    for (UpdateList<int>::const_iterator uj = cj.int_updates().begin();
		 uj != cj.int_updates().end(); uj++) {
	      const Update<int>& u = **uj;
	      c->add_update(*new Update<int>(u.variable(), u.expr()));
	    }
	    for (UpdateList<double>::const_iterator uj =
		   cj.double_updates().begin();
		 uj != cj.double_updates().end(); uj++) {
	      const Update<double>& u = **uj;
	      c->add_update(*new Update<double>(u.variable(), u.expr()));
	    }
	    commands_.push_back(c);
	  }
	}
      }
    }
  }
}


/* Output operator for models. */
std::ostream& operator<<(std::ostream& os, const Model& m) {
  os << "stochastic";
  std::vector<const Constant<int>*>::const_iterator ci =
    m.int_constants().begin();
  if (ci != m.int_constants().end()) {
    os << std::endl;
    for (; ci != m.int_constants().end(); ci++) {
      const Constant<int>& c = **ci;
      os << std::endl << "const " << c << " = " << c.value() << ';';
    }
  }
  std::vector<const Constant<double>*>::const_iterator cj =
    m.double_constants().begin();
  if (cj != m.double_constants().end()) {
    os << std::endl;
    for (; cj != m.double_constants().end(); cj++) {
      const Constant<double>& c = **cj;
      os << std::endl << "rate " << c << " = " << c.value() << ';';
    }
  }
  VariableList<int>::const_iterator vi = m.int_variables().begin();
  if (vi != m.int_variables().end()) {
    os << std::endl;
    for (; vi != m.int_variables().end(); vi++) {
      const Variable<int>& v = **vi;
      os << std::endl << "global " << v
	 << " : [" << v.low() << ".." << v.high() << "]"
	 << " init " << v.init() << ';';
    }
  }
  VariableList<double>::const_iterator vj = m.double_variables().begin();
  if (vj != m.double_variables().end()) {
    os << std::endl;
    for (; vj != m.double_variables().end(); vj++) {
      const Variable<double>& v = **vj;
      os << std::endl << "global " << v
	 << " : [" << v.low() << ".." << v.high() << "]"
	 << " init " << v.init() << ';';
    }
  }
  for (ModuleList::const_iterator mi = m.modules().begin();
       mi != m.modules().end(); mi++) {
    os << std::endl << std::endl << **mi;
  }
  return os;
}
