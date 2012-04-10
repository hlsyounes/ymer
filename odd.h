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
 * $Id: odd.h,v 2.1 2004-01-25 12:38:55 lorens Exp $
 */
#ifndef ODD_H
#define ODD_H

//------------------------------------------------------------------------------

#include <util.h>
#include <cudd.h>

// odd definitions

struct ODDNode {
  DdNode *dd;
  ODDNode *next;
  ODDNode *e;
  ODDNode *t;
  long eoff;
  long toff;
};

// function prototypes

ODDNode *build_odd(DdManager *ddman, DdNode *dd, DdNode **vars, int num_vars);
int get_num_odd_nodes();

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
