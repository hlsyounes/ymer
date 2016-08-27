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
#include "hybrid.h"

#include "cudd.h"

// static variables
static const int sb_max_mem = 1024;
static int num_hdd_matrix_nodes;
static int num_sparse_bits;
static HDDNode *zero = NULL;

// local prototypes
static HDDNode *build_hdd_matrix_rowrec(const DecisionDiagramManager &ddman,
                                        DdNode *dd, int level,
                                        const OddNode *row, const OddNode *col,
                                        HDDMatrix *hddm);
static HDDNode *build_hdd_matrix_colrec(const DecisionDiagramManager &ddman,
                                        DdNode *dd, int level,
                                        const OddNode *row, const OddNode *col,
                                        HDDMatrix *hddm);
static void add_sparse_bits(HDDMatrix *hddm);
static int compute_n_and_nnz_rec(HDDNode *hdd, int level, int num_levels,
                                 const OddNode *row);
static SparseMatrix *build_sparse_bit(HDDNode *hdd, int level, int num_levels);
static void fill_sparse_bit_rec(HDDNode *hdd, int level, int num_levels,
                                long row, long col, std::vector<double> *values,
                                std::vector<size_t> *columns,
                                std::vector<size_t> *row_starts, int code);
static void hdd_negative_row_sums_rec(HDDNode *hdd, int level, int num_levels,
                                      long row, double *res);

//------------------------------------------------------------------------------
// hybrid utility functions
//------------------------------------------------------------------------------

// builds hybrid mtbdd matrix from mtbdd

HDDMatrix *build_hdd_matrix(const DecisionDiagramManager &ddman,
                            const ADD &matrix, const OddNode *odd) {
  int i;
  HDDMatrix *res;
  HDDNode *ptr;

  // build tables to store hdd nodes
  res = new HDDMatrix();
  res->num_levels = ddman.GetVariableCount() / 2;
  res->row_tables = new HDDNode *[res->num_levels + 1];
  res->col_tables = new HDDNode *[res->num_levels];
  for (i = 0; i < res->num_levels; i++) {
    res->row_tables[i] = NULL;
    res->col_tables[i] = NULL;
  }
  // extra table for constants
  res->row_tables[res->num_levels] = NULL;

  // reset node counter
  num_hdd_matrix_nodes = 0;

  // create zero constant (special case)
  num_hdd_matrix_nodes++;
  res->zero = new HDDNode();
  res->zero->type.kids.e = NULL;
  res->zero->type.kids.t = NULL;
  res->zero->off = 0;
  res->zero->off2 = 0;
  res->zero->sb = NULL;
  res->zero->next = NULL;

  // call recursive bit
  res->top = build_hdd_matrix_rowrec(ddman, matrix.get(), 0, odd, odd, res);

  // go thru all nodes and
  // (1) store actual offset (int) not odd ptr
  // (2) set sparse bit pointer to null
  for (i = 0; i < res->num_levels + 1; i++) {
    ptr = res->row_tables[i];
    while (ptr != NULL) {
      ptr->off = ((const OddNode *)(ptr->off))->eoff;
      ptr->sb = NULL;
      ptr = ptr->next;
    }
  }
  for (i = 0; i < res->num_levels; i++) {
    ptr = res->col_tables[i];
    while (ptr != NULL) {
      ptr->off = ((const OddNode *)(ptr->off))->eoff;
      ptr->sb = NULL;
      ptr = ptr->next;
    }
  }

  // store odd too
  res->odd = odd;

  // store num nodes
  res->num_nodes = num_hdd_matrix_nodes;

  // initialise other stats
  res->sbl = 0;
  res->num_sb = 0;
  add_sparse_bits(res);

  return res;
}

//------------------------------------------------------------------------------

// recursive part of build_hdd_matrix

HDDNode *build_hdd_matrix_rowrec(const DecisionDiagramManager &ddman,
                                 DdNode *dd, int level, const OddNode *row,
                                 const OddNode *col, HDDMatrix *hddm) {
  HDDNode *ptr, *hdd_e, *hdd_t;
  DdNode *e, *t;

  // check for zero terminal
  if (dd == Cudd_ReadZero(ddman.manager())) {
    return hddm->zero;
  }

  // see if we already have the required node stored
  ptr = hddm->row_tables[level];
  while (ptr != NULL) {
    if (((DdNode *)(ptr->sb) == dd) && ((const OddNode *)(ptr->off) == row) &&
        ((const OddNode *)(ptr->off2) == col))
      break;
    // use this instead to check effect on node increase
    // if (((DdNode*)(ptr->sb) == dd)) break;
    ptr = ptr->next;
  }
  // if so, return it
  if (ptr != NULL) {
    return ptr;
  }

  // otherwise go on and create it...

  // if it's a terminal node, it's easy...
  int nvars = ddman.GetVariableCount() / 2;
  if (level == nvars) {
    num_hdd_matrix_nodes++;
    ptr = new HDDNode();
    ptr->type.val = Cudd_V(dd);
    ptr->off = (size_t)row;
    ptr->off2 = (size_t)col;
    ptr->sb = (SparseMatrix *)dd;
    ptr->next = hddm->row_tables[nvars];
    hddm->row_tables[nvars] = ptr;
    return ptr;
  }

  // if not, have to recurse before creation
  if (dd->index > ddman.GetBddVariable(2 * level).get()->index) {
    e = t = dd;
  } else {
    e = Cudd_E(dd);
    t = Cudd_T(dd);
  }
  hdd_e = build_hdd_matrix_colrec(ddman, e, level, row->e, col, hddm);
  hdd_t = build_hdd_matrix_colrec(ddman, t, level, row->t, col, hddm);
  num_hdd_matrix_nodes++;
  ptr = new HDDNode();
  ptr->type.kids.e = hdd_e;
  ptr->type.kids.t = hdd_t;
  ptr->off = (size_t)row;
  ptr->off2 = (size_t)col;
  ptr->sb = (SparseMatrix *)dd;
  ptr->next = hddm->row_tables[level];
  hddm->row_tables[level] = ptr;
  return ptr;
}

HDDNode *build_hdd_matrix_colrec(const DecisionDiagramManager &ddman,
                                 DdNode *dd, int level, const OddNode *row,
                                 const OddNode *col, HDDMatrix *hddm) {
  HDDNode *ptr, *hdd_e, *hdd_t;
  DdNode *e, *t;

  // check for zero terminal
  if (dd == Cudd_ReadZero(ddman.manager())) {
    return hddm->zero;
  }

  // see if we already have the required node stored
  ptr = hddm->col_tables[level];
  while (ptr != NULL) {
    if (((DdNode *)(ptr->sb) == dd) && ((const OddNode *)(ptr->off) == col) &&
        ((const OddNode *)(ptr->off2) == row))
      break;
    // use this instead to check effect on node increase
    // if (((DdNode*)(ptr->sb) == dd)) break;
    ptr = ptr->next;
  }
  // if so, return it
  if (ptr != NULL) {
    return ptr;
  }

  // otherwise go on and create it...

  // can't be a terminal node so recurse before creation
  if (dd->index > ddman.GetBddVariable(2 * level + 1).get()->index) {
    e = t = dd;
  } else {
    e = Cudd_E(dd);
    t = Cudd_T(dd);
  }
  hdd_e = build_hdd_matrix_rowrec(ddman, e, level + 1, row, col->e, hddm);
  hdd_t = build_hdd_matrix_rowrec(ddman, t, level + 1, row, col->t, hddm);
  num_hdd_matrix_nodes++;
  ptr = new HDDNode();
  ptr->type.kids.e = hdd_e;
  ptr->type.kids.t = hdd_t;
  ptr->off = (size_t)col;
  ptr->off2 = (size_t)row;
  ptr->sb = (SparseMatrix *)dd;
  ptr->next = hddm->col_tables[level];
  hddm->col_tables[level] = ptr;

  return ptr;
}

//------------------------------------------------------------------------------

// adds sparse bits on to the hdd

void add_sparse_bits(HDDMatrix *hddm) {
  int i;
  double level_memory;
  bool sb_mem_out;
  HDDNode *ptr;

  // initialise variables
  num_sparse_bits = 0;
  zero = hddm->zero;

  // first, we make sure that all sparse bit pointers are null
  // (they may have been used to store other things previously)
  for (i = 0; i < hddm->num_levels + 1; i++) {
    // row nodes and terminals
    ptr = hddm->row_tables[i];
    while (ptr != NULL) {
      ptr->sb = NULL;
      ptr = ptr->next;
    }
  }
  for (i = 0; i < hddm->num_levels; i++) {
    // column nodes
    ptr = hddm->col_tables[i];
    while (ptr != NULL) {
      ptr->sb = NULL;
      ptr = ptr->next;
    }
  }

  // store size (num states and nnz) of each node's matrix
  // (putting them in the sb and off2 pointers, respectively)
  compute_n_and_nnz_rec(hddm->top, 0, hddm->num_levels, hddm->odd);

  // level by level, from the bottom, compute how much memory
  // sparse bits would need for each level
  sb_mem_out = false;
  for (i = hddm->num_levels - 1; i >= 0; i--) {
    level_memory = 0;
    ptr = hddm->row_tables[i];
    while (ptr != NULL) {
      level_memory += ((ptr->off2) * 12.0 + ((size_t)ptr->sb) * 4.0 + 5 * 4.0);
      if (level_memory / 1024.0 > sb_max_mem) {
        sb_mem_out = true;
        break;
      }
      ptr = ptr->next;
    }
    if (sb_mem_out) break;
  }
  // pick max level we can afford (memory-wise)
  const int actual_num_sb_levels =
      (sb_mem_out) ? hddm->num_levels - (i + 1) : hddm->num_levels;

  // set all sparse bit pointers back to null
  // (except on level sparse bits will go)
  // (we will use n/nnz stats before overwriting them)
  for (i = 0; i < hddm->num_levels + 1; i++) {
    // row nodes and terminals
    if ((i == hddm->num_levels - actual_num_sb_levels) &&
        (actual_num_sb_levels > 0)) {
      // skip this one
      continue;
    }
    ptr = hddm->row_tables[i];
    while (ptr != NULL) {
      ptr->sb = NULL;
      ptr = ptr->next;
    }
  }
  for (i = 0; i < hddm->num_levels; i++) {
    // column nodes
    ptr = hddm->col_tables[i];
    while (ptr != NULL) {
      ptr->sb = NULL;
      ptr = ptr->next;
    }
  }

  // now actually add the sparse bits
  if (actual_num_sb_levels > 0) {
    ptr = hddm->row_tables[hddm->num_levels - actual_num_sb_levels];
    while (ptr != NULL) {
      ptr->sb = build_sparse_bit(ptr, hddm->num_levels - actual_num_sb_levels,
                                 hddm->num_levels);
      ptr = ptr->next;
    }
  }

  // store sparse bit stats
  hddm->sbl = actual_num_sb_levels;
  hddm->num_sb = num_sparse_bits;
}

//------------------------------------------------------------------------------

// compute the size (num states and nnz) of matrix corresponding to each hdd
// node
// (and store in sb and off2 pointers, respectively)

int compute_n_and_nnz_rec(HDDNode *hdd, int level, int num_levels,
                          const OddNode *row) {
  HDDNode *e, *t;

  // if it's the zero node...
  if (hdd == zero) {
    hdd->sb = (SparseMatrix *)0;
    hdd->off2 = 0;
    return 0;
  }
  // if it's at the bottom...
  if (level == num_levels) {
    hdd->sb = (SparseMatrix *)0;
    hdd->off2 = 1;
    return 1;
  }

  // check if we've already done this node
  // (nb: important to check sb not off2 because we didn't reset the off2
  // pointers)
  if (hdd->sb != NULL) {
    return hdd->off2;
  }

  // store n
  hdd->sb = (SparseMatrix *)(row->eoff + row->toff);
  // recurse and store nnz
  hdd->off2 = 0;
  e = hdd->type.kids.e;
  if (e != zero) {
    hdd->off2 +=
        compute_n_and_nnz_rec(e->type.kids.e, level + 1, num_levels, row->e);
    hdd->off2 +=
        compute_n_and_nnz_rec(e->type.kids.t, level + 1, num_levels, row->e);
  }
  t = hdd->type.kids.t;
  if (t != zero) {
    hdd->off2 +=
        compute_n_and_nnz_rec(t->type.kids.e, level + 1, num_levels, row->t);
    hdd->off2 +=
        compute_n_and_nnz_rec(t->type.kids.t, level + 1, num_levels, row->t);
  }
  return hdd->off2;
}

//-----------------------------------------------------------------------------------

// build a sparse bit for a hdd node

SparseMatrix *build_sparse_bit(HDDNode *hdd, int level, int num_levels) {
  // number of nonzeros is stored in off2
  const size_t nnz = hdd->off2;
  // size of submatrix is stored in sb pointer
  const size_t n = (size_t)hdd->sb;
  // create the sparse bit
  num_sparse_bits++;
  std::vector<double> values(nnz);
  std::vector<size_t> columns(nnz);
  std::vector<size_t> row_starts(n + 1);
  // see how many nonzeros are in each row
  fill_sparse_bit_rec(hdd, level, num_levels, 0, 0, &values, &columns,
                      &row_starts, 1);
  // and use this to compute the row_starts information
  for (size_t i = 1; i < n + 1; i++) {
    row_starts[i] += row_starts[i - 1];
  }
  // then fill up the non_zeros/cols arrays
  fill_sparse_bit_rec(hdd, level, num_levels, 0, 0, &values, &columns,
                      &row_starts, 2);
  // recompute the row_starts info because we've messed with it
  for (size_t i = n; i > 0; i--) {
    row_starts[i] = row_starts[i - 1];
  }
  row_starts[0] = 0;

  return new SparseMatrix(std::move(values), std::move(columns),
                          std::move(row_starts));
}

//------------------------------------------------------------------------------

// fill a sparse bit for a hdd node

void fill_sparse_bit_rec(HDDNode *hdd, int level, int num_levels, long row,
                         long col, std::vector<double> *values,
                         std::vector<size_t> *columns,
                         std::vector<size_t> *row_starts, int code) {
  // if it's the zero node...
  if (hdd == zero) {
    return;
  }
  // if it's at the bottom
  if (level == num_levels) {
    // do something different depending on the code passed in
    switch (code) {
      case 1:
        (*row_starts)[row + 1]++;
        break;
      case 2:
        (*values)[(*row_starts)[row]] = hdd->type.val;
        (*columns)[(*row_starts)[row]] = col;
        (*row_starts)[row]++;
        break;
    }
    return;
  }
  // otherwise recurse...
  const HDDNode *e = hdd->type.kids.e;
  if (e != zero) {
    fill_sparse_bit_rec(e->type.kids.e, level + 1, num_levels, row, col, values,
                        columns, row_starts, code);
    fill_sparse_bit_rec(e->type.kids.t, level + 1, num_levels, row,
                        col + e->off, values, columns, row_starts, code);
  }
  const HDDNode *t = hdd->type.kids.t;
  if (t != zero) {
    fill_sparse_bit_rec(t->type.kids.e, level + 1, num_levels, row + hdd->off,
                        col, values, columns, row_starts, code);
    fill_sparse_bit_rec(t->type.kids.t, level + 1, num_levels, row + hdd->off,
                        col + t->off, values, columns, row_starts, code);
  }
}

//------------------------------------------------------------------------------
// compute the negative row sums from the hdd of the rate matrix

double *hdd_negative_row_sums(HDDMatrix *hddm, int n) {
  HDDNode *hdd;
  double *diags;
  int i;

  // get top hdd
  hdd = hddm->top;

  // allocate/initialise array
  diags = new double[n];
  for (i = 0; i < n; i++) {
    diags[i] = 0.0;
  }

  // do row summing recursively
  hdd_negative_row_sums_rec(hdd, 0, hddm->num_levels, 0, diags);

  return diags;
}

//------------------------------------------------------------------------------

void hdd_negative_row_sums_rec(HDDNode *hdd, int level, int num_levels,
                               long row, double *diags) {
  HDDNode *e, *t;

  // if it's the zero node
  if (hdd == zero) {
    return;
  }
  // if there's a sparse bit
  if (hdd->sb) {
    for (auto element : *hdd->sb) {
      diags[row + element.row()] -= element.value();
    }
    return;
  }
  // or if we've reached the bottom
  if (level == num_levels) {
    diags[row] -= hdd->type.val;
    return;
  }
  // otherwise recurse
  e = hdd->type.kids.e;
  if (e != zero) {
    hdd_negative_row_sums_rec(e->type.kids.e, level + 1, num_levels, row,
                              diags);
    hdd_negative_row_sums_rec(e->type.kids.t, level + 1, num_levels, row,
                              diags);
  }
  t = hdd->type.kids.t;
  if (t != zero) {
    hdd_negative_row_sums_rec(t->type.kids.e, level + 1, num_levels,
                              row + hdd->off, diags);
    hdd_negative_row_sums_rec(t->type.kids.t, level + 1, num_levels,
                              row + hdd->off, diags);
  }
}

//------------------------------------------------------------------------------

// free memory

void free_hdd_matrix(HDDMatrix *hddm) {
  int i;
  HDDNode *ptr, *ptr2;

  // go thru all nodes
  for (i = 0; i < hddm->num_levels; i++) {
    ptr = hddm->row_tables[i];
    while (ptr) {
      // free sparse bit if there is one
      if (ptr->sb) {
        delete ptr->sb;
      }
      // get next node
      ptr2 = ptr->next;
      // free node
      delete ptr;
      // get next node
      ptr = ptr2;
    }
    ptr = hddm->col_tables[i];
    while (ptr) {
      // free sparse bit if there is one
      if (ptr->sb) {
        delete ptr->sb;
      }
      // get next node
      ptr2 = ptr->next;
      // free node
      delete ptr;
      // get next node
      ptr = ptr2;
    }
  }
  ptr = hddm->row_tables[hddm->num_levels];
  while (ptr) {
    // get next node
    ptr2 = ptr->next;
    // free node
    delete ptr;
    // get next node
    ptr = ptr2;
  }

  // free zero constant
  delete hddm->zero;

  // free tables
  delete hddm->row_tables;
  delete hddm->col_tables;

  // free data structure
  delete hddm;
}

//------------------------------------------------------------------------------
