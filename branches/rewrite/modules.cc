/*
 * Modules.
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
#include "modules.h"


/* ====================================================================== */
/* Update */

/* Constructs a variable update. */
template<typename T>
Update<T>::Update(const Variable<T>& variable, const Expression<T>& expr)
  : variable_(&variable), expr_(&expr) {
  RCObject::ref(variable_);
  RCObject::ref(expr_);
}


/* Deletes this variable update. */
template<typename T>
Update<T>::~Update() {
  RCObject::deref(variable_);
  RCObject::deref(expr_);
}


/* Returns this update subject to the given substitutions. */
template<typename T>
const Update<T>& Update<T>::substitution(const Substitutions& subst) const {
  return *new Update(variable().substitution(subst),
		     expr().substitution(subst));
}


/* Explicit instantiations. */
template struct Update<int>;
template struct Update<double>;


/* ====================================================================== */
/* Command */

/* Constructs a command. */
Command::Command(int synch, const StateFormula& guard,
		 const Distribution& delay)
  : synch_(synch), guard_(&guard), delay_(&delay) {
  RCObject::ref(guard_);
  RCObject::ref(delay_);
}


/* Deletes this command. */
Command::~Command() {
  RCObject::deref(guard_);
  RCObject::deref(delay_);
  for (UpdateList<int>::const_iterator ui = int_updates().begin();
       ui != int_updates().end(); ui++) {
    delete *ui;
  }
  for (UpdateList<double>::const_iterator ui = double_updates().begin();
       ui != double_updates().end(); ui++) {
    delete *ui;
  }
}


/* Adds an integer-valued update to this command. */
void Command::add_update(const Update<int>& update) {
  int_updates_.push_back(&update);
}


/* Adds a double-valued update to this command. */
void Command::add_update(const Update<double>& update) {
  double_updates_.push_back(&update);
}


/* Returns this command subject to the given substitutions. */
const Command&
Command::substitution(const Substitutions& subst,
		      const SynchSubstitutions& synchs) const {
  SynchSubstitutions::const_iterator si = synchs.find(synch());
  int s = (si != synchs.end()) ? (*si).second : synch();
  Command* subst_comm = new Command(s, guard().substitution(subst),
				    delay().substitution(subst));
  for (UpdateList<int>::const_iterator ui = int_updates().begin();
       ui != int_updates().end(); ui++) {
    subst_comm->add_update((*ui)->substitution(subst));
  }
  for (UpdateList<double>::const_iterator ui = double_updates().begin();
       ui != double_updates().end(); ui++) {
    subst_comm->add_update((*ui)->substitution(subst));
  }
  return *subst_comm;
}


/* Output operator for commands. */
std::ostream& operator<<(std::ostream& os, const Command& c) {
  os << "[";
  if (c.synch() != 0) {
    os << 's' << c.synch();
  }
  os << "] " << c.guard() << " -> " << c.delay() << " : ";
  UpdateList<int>::const_iterator ui = c.int_updates().begin();
  if (ui != c.int_updates().end()) {
    const Update<int>* u = *ui;
    os << u->variable() << "\'=" << u->expr();
    for (ui++; ui != c.int_updates().end(); ui++) {
      u = *ui;
      os << " & " << u->variable() << "\'=" << u->expr();
    }
  }
  UpdateList<double>::const_iterator uj = c.double_updates().begin();
  if (uj != c.double_updates().end()) {
    const Update<double>* u = *uj;
    if (!c.int_updates().empty()) {
      os << " & ";
    }
    os << u->variable() << "\'=" << u->expr();
    for (uj++; uj != c.double_updates().end(); uj++) {
      u = *uj;
      os << " & " << u->variable() << "\'=" << u->expr();
    }
  }
  return os;
}


/* ====================================================================== */
/* Module */

/* Deletes this module. */
Module::~Module() {
  for (VariableList<int>::const_iterator vi = int_variables().begin();
       vi != int_variables().end(); vi++) {
    RCObject::deref(*vi);
  }
  for (VariableList<double>::const_iterator vi = double_variables().begin();
       vi != double_variables().end(); vi++) {
    RCObject::deref(*vi);
  }
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    delete *ci;
  }
}


/* Adds an integer-valued variable to this module. */
void Module::add_variable(const Variable<int>& variable) {
  int_variables_.push_back(&variable);
  RCObject::ref(&variable);
}


/* Adds a double-valued variable to this module. */
void Module::add_variable(const Variable<double>& variable) {
  double_variables_.push_back(&variable);
  RCObject::ref(&variable);
}


/* Adds a command to this module. */
void Module::add_command(const Command& command) {
  commands_.push_back(&command);
}


/* Returns this module subject to the given substitutions. */
Module& Module::substitution(const std::string& new_name,
			     const Substitutions& subst,
			     const SynchSubstitutions& synchs) const {
  Module* subst_mod = new Module(new_name);
  for (VariableList<int>::const_iterator vi = int_variables().begin();
       vi != int_variables().end(); vi++) {
    subst_mod->add_variable((*vi)->substitution(subst));
  }
  for (VariableList<double>::const_iterator vi = double_variables().begin();
       vi != double_variables().end(); vi++) {
    subst_mod->add_variable((*vi)->substitution(subst));
  }
  for (CommandList::const_iterator ci = commands().begin();
       ci != commands().end(); ci++) {
    subst_mod->add_command((*ci)->substitution(subst, synchs));
  }
  return *subst_mod;
}


/* Output operator for modules. */
std::ostream& operator<<(std::ostream& os, const Module& m) {
  os << "module " << m.name();
  VariableList<int>::const_iterator vi = m.int_variables().begin();
  if (vi != m.int_variables().end()) {
    os << std::endl;
    for (; vi != m.int_variables().end(); vi++) {
      const Variable<int>& v = **vi;
      os << std::endl << "  " << v
	 << " : [" << v.low() << ".." << v.high() << "]"
	 << " init " << v.init() << ';';
    }
  }
  VariableList<double>::const_iterator vj = m.double_variables().begin();
  if (vj != m.double_variables().end()) {
    os << std::endl;
    for (; vj != m.double_variables().end(); vj++) {
      const Variable<double>& v = **vj;
      os << std::endl << "  " << v
	 << " : [" << v.low() << ".." << v.high() << "]"
	 << " init " << v.init() << ';';
    }
  }
  CommandList::const_iterator ci = m.commands().begin();
  if (ci != m.commands().end()) {
    os << std::endl;
    for (; ci != m.commands().end(); ci++) {
      os << std::endl << "  " << **ci << ';';
    }
  }
  os << std::endl << std::endl << "endmodule";
  return os;
}
