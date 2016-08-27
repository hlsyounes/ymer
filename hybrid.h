/*
 * Copyright (C) 2002 Dave Parker
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

#ifndef HYBRID_H_
#define HYBRID_H_

#include <cstddef>

#include "src/ddutil.h"

//------------------------------------------------------------------------------

// hdd data structure definitions

// hybrid mtbdd node (based on structure of CUDD nodes)

struct HDDNode;

struct HDDKids {
  HDDNode *e;
  HDDNode *t;
};

struct HDDNode {
  union {
    double val;	// for terminals
    HDDKids kids;	// for non-terminals
  } type;
  size_t off;
  size_t off2;
  SparseMatrix *sb;
  HDDNode *next;
};

// hybrid mtbdd matrix

struct HDDMatrix {
  // num levels
  int num_levels;
  // node tables
  HDDNode **row_tables;
  HDDNode **col_tables;
  // pointers to special nodes
  HDDNode *top;
  HDDNode *zero;
  // odd
  const OddNode *odd;
  // stats
  int num_nodes;
  int sbl;
  int num_sb;
};

//------------------------------------------------------------------------------

// function prototypes

HDDMatrix *build_hdd_matrix(const DecisionDiagramManager &ddman, const ADD &matrix, const OddNode *odd);
double *hdd_negative_row_sums(HDDMatrix *hddm, int n);
void free_hdd_matrix(HDDMatrix *hddm);

//------------------------------------------------------------------------------

#endif  // HYBRID_H_
