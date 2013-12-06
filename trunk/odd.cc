/*
 * Copyright (C) 2002 Dave Parker
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
 * $Id: odd.cc,v 2.1 2004-01-25 12:38:51 lorens Exp $
 */
#include "odd.h"

#include <cstdio>
#include <set>

#include "cudd.h"

// static variables
static int num_odd_nodes = 0;

// local prototypes
static ODDNode *build_odd_rec(const DecisionDiagramManager &ddman, DdNode *dd, size_t level, const VariableArray<BDD> &vars, ODDNode **tables);
static long add_offsets(const DecisionDiagramManager &ddman, ODDNode *dd, int level, int num_vars);

//------------------------------------------------------------------------------

ODDNode *build_odd(const DecisionDiagramManager &ddman, DdNode *dd, const VariableArray<BDD> &vars)
{
  size_t i;
  ODDNode **tables;
  ODDNode *res;

  // build tables to store odd nodes
  tables = new ODDNode*[vars.size()+1];
  for (i = 0; i < vars.size()+1; i++) {
    tables[i] = NULL;
  }
	
  // reset node counter
  num_odd_nodes = 0;
	
  // call recursive bit
  res = build_odd_rec(ddman, dd, 0, vars, tables);
	
  // add offsets to odd
  add_offsets(ddman, res, 0, vars.size());

  // free memory
  delete tables;
	
  return res;
}

//------------------------------------------------------------------------------

static ODDNode *build_odd_rec(const DecisionDiagramManager &ddman, DdNode *dd, size_t level, const VariableArray<BDD> &vars, ODDNode **tables)
{
  ODDNode *ptr;
	
  // see if we already have odd in the tables
  ptr = tables[level];
  while (ptr != NULL) {
    if (ptr->dd == dd) break;
    ptr = ptr->next;
  }
	
  // if not, add it
  if (ptr == NULL) {
    num_odd_nodes++;
    ptr = new ODDNode();
    ptr->dd = dd;		
    ptr->next = tables[level];
    tables[level] = ptr;
    // and recurse...
		
    // should be able to add this because will never traverse a path to the
    // zeros temrinal - always look at states that exist
    // can we assume this?
    //	if (dd == Cudd_ReadZero(ddman)) return;

    if (level == vars.size()) {
      ptr->e = NULL;
      ptr->t = NULL;
    }
    else if (vars.get()[level]->index < dd->index) {
      ptr->e = build_odd_rec(ddman, dd, level+1, vars, tables);
      ptr->t = ptr->e;
    }
    else {
      ptr->e = build_odd_rec(ddman, Cudd_E(dd), level+1, vars, tables);
      ptr->t = build_odd_rec(ddman, Cudd_T(dd), level+1, vars, tables);
    }
    ptr->eoff = -1;
    ptr->toff = -1;
  }
	
  return ptr;
}

//------------------------------------------------------------------------------

long add_offsets(const DecisionDiagramManager &ddman, ODDNode *odd, int level, int num_vars)
{
  if ((odd->eoff == -1) || (odd->toff == -1)) {
    if (level == num_vars) {
      if (odd->dd == Cudd_ReadZero(ddman.manager())) {
	odd->eoff = 0;
	odd->toff = 0;
      }
      else {
	odd->eoff = 0;
	odd->toff = 1;
      }
    }
    else {
      odd->eoff = add_offsets(ddman, odd->e, level+1, num_vars);
      odd->toff = add_offsets(ddman, odd->t, level+1, num_vars);
    }
  }
  return odd->eoff + odd->toff;
}

//------------------------------------------------------------------------------

int get_num_odd_nodes()
{
  return num_odd_nodes;
}

//------------------------------------------------------------------------------

namespace {

void free_odd_impl(ODDNode *odd, std::set<ODDNode*>* deleted) {
  if (deleted->find(odd) == deleted->end()) {
    deleted->insert(odd);
    free_odd_impl(odd->next, deleted);
    free_odd_impl(odd->e, deleted);
    free_odd_impl(odd->t, deleted);
    delete odd;
  }
}

}  // namespace

void free_odd(ODDNode *odd) {
  std::set<ODDNode*> deleted;
  deleted.insert(NULL);
  free_odd_impl(odd, &deleted);
}
