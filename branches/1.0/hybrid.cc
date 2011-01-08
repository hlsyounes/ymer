/*
 * Copyright (C) 2002 Dave Parker
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
 * $Id: hybrid.cc,v 1.1 2003-11-07 04:24:35 lorens Exp $
 */
#include "hybrid.h"

// static variables
static int sb_max_mem = 1024;
static int num_sb_levels = -1;
static int num_hdd_matrix_nodes;
static int actual_num_sb_levels;
static int num_sparse_bits;
static double sparse_bits_memory;
static HDDNode *zero = NULL;

// local prototypes
static HDDNode *build_hdd_matrix_rowrec(DdManager *ddman, DdNode *dd, DdNode **rvars, DdNode **cvars, int num_vars, int level, ODDNode *row, ODDNode *col, HDDMatrix *hddm);
static HDDNode *build_hdd_matrix_colrec(DdManager *ddman, DdNode *dd, DdNode **rvars, DdNode **cvars, int num_vars, int level, ODDNode *row, ODDNode *col, HDDMatrix *hddm);
static int compute_n_and_nnz_rec(HDDNode *hdd, int level, int num_levels, ODDNode *row);
static SparseBit *build_sparse_bit(HDDNode *hdd, int level, int num_levels);
static void fill_sparse_bit_rec(HDDNode *hdd, int level, int num_levels, long row, long col, SparseBit *sb, int code);
static void hdd_negative_row_sums_rec(HDDNode *hdd, int level, int num_levels, long row, double *res);

//------------------------------------------------------------------------------
// hybrid utility functions
//------------------------------------------------------------------------------

// builds hybrid mtbdd matrix from mtbdd

HDDMatrix *build_hdd_matrix(DdManager *ddman, DdNode *matrix, DdNode **rvars, DdNode **cvars, int num_vars, ODDNode *odd)
{
	int i;
	HDDMatrix *res;
	HDDNode *ptr;

	// build tables to store hdd nodes
	res = new HDDMatrix();
	res->num_levels = num_vars;
	res->row_tables = new HDDNode*[num_vars+1];
	res->col_tables = new HDDNode*[num_vars];
	for (i = 0; i < num_vars; i++) {
		res->row_tables[i] = NULL;
		res->col_tables[i] = NULL;
	}
	// extra table for constants
	res->row_tables[num_vars] = NULL;
	
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
	res->top = build_hdd_matrix_rowrec(ddman, matrix, rvars, cvars, num_vars, 0, odd, odd, res);

	// go thru all nodes and
	// (1) store actual offset (int) not odd ptr
	// (2) set sparse bit pointer to null
	for (i = 0; i < num_vars+1; i++) {
		ptr = res->row_tables[i];
		while (ptr != NULL) {
			ptr->off = ((ODDNode*)(ptr->off))->eoff;
			ptr->sb = NULL;
			ptr = ptr->next;
		}
	}
	for (i = 0; i < num_vars; i++) {
		ptr = res->col_tables[i];
		while (ptr != NULL) {
			ptr->off = ((ODDNode*)(ptr->off))->eoff;
			ptr->sb = NULL;
			ptr = ptr->next;
		}
	}

//	int count;
//	int count2;

//	// count no of nodes at each level
//	for (i = 0; i < num_vars+1; i++) {
//		count = 0;
//		tmp = res->tables[i];
//		while (tmp != NULL) {
//			count++;
//			tmp = tmp->next;
//		}
//		printf("%d : %d\n", i, count);
//	}
//	printf("\n");
	
//	for (i = 0; i < num_vars+1; i++) {
//		count = 0;
//		count2 = 0;
//		tmp = res->tables[i];
//		while (tmp != NULL) {
//			count++;
//			if (tmp->ee != res->zero) count2++;
//			if (tmp->et != res->zero) count2++;
//			if (tmp->te != res->zero) count2++;
//			if (tmp->tt != res->zero) count2++;
//			tmp = tmp->next;
//		}
//		printf("%d : %f\n", i, (double)count2/count);
//	}
	
	// store odd too
	res->odd = odd;
	
	// initialise block storage to null
	res->blocks = NULL;
	
	// store num nodes
	res->num_nodes = num_hdd_matrix_nodes;
	
	// initialise other stats
	res->sbl = 0;
	res->num_sb = 0;
	res->sb_mem = 0;
	res->sorl = 0;
	
	return res;
}

//------------------------------------------------------------------------------

// recursive part of build_hdd_matrix

HDDNode *build_hdd_matrix_rowrec(DdManager *ddman, DdNode *dd, DdNode **rvars, DdNode **cvars, int num_vars, int level, ODDNode *row, ODDNode *col, HDDMatrix *hddm)
{
	HDDNode *ptr, *hdd_e, *hdd_t;
	DdNode *e, *t;

	// check for zero terminal
	if (dd == Cudd_ReadZero(ddman))
	{
		return hddm->zero;
	}
	
	// see if we already have the required node stored
	ptr = hddm->row_tables[level];
	while (ptr != NULL) {
		if (((DdNode*)(ptr->sb) == dd) && ((ODDNode*)(ptr->off) == row) && ((ODDNode*)(ptr->off2) == col)) break;
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
	if (level == num_vars) {
		num_hdd_matrix_nodes++;
		ptr = new HDDNode();
		ptr->type.val = Cudd_V(dd);
		ptr->off = (int)row;
		ptr->off2 = (int)col;
		ptr->sb = (SparseBit*)dd;
		ptr->next = hddm->row_tables[num_vars];
		hddm->row_tables[num_vars] = ptr;
		return ptr;
	}

	// if not, have to recurse before creation
	if (dd->index > rvars[level]->index) {
		e = t = dd;
	}
	else {
		e = Cudd_E(dd);
		t = Cudd_T(dd);
	}
	hdd_e = build_hdd_matrix_colrec(ddman, e, rvars, cvars, num_vars, level, row->e, col, hddm);
	hdd_t = build_hdd_matrix_colrec(ddman, t, rvars, cvars, num_vars, level, row->t, col, hddm);
	num_hdd_matrix_nodes++;
	ptr = new HDDNode();
	ptr->type.kids.e = hdd_e;
	ptr->type.kids.t = hdd_t;
	ptr->off = (int)row;
	ptr->off2 = (int)col;
	ptr->sb = (SparseBit*)dd;
	ptr->next = hddm->row_tables[level];
	hddm->row_tables[level] = ptr;
	return ptr;
}

HDDNode *build_hdd_matrix_colrec(DdManager *ddman, DdNode *dd, DdNode **rvars, DdNode **cvars, int num_vars, int level, ODDNode *row, ODDNode *col, HDDMatrix *hddm)
{
	HDDNode *ptr, *hdd_e, *hdd_t;
	DdNode *e, *t;

	// check for zero terminal
	if (dd == Cudd_ReadZero(ddman))
	{
		return hddm->zero;
	}
	
	// see if we already have the required node stored
	ptr = hddm->col_tables[level];
	while (ptr != NULL) {
		if (((DdNode*)(ptr->sb) == dd) && ((ODDNode*)(ptr->off) == col) && ((ODDNode*)(ptr->off2) == row)) break;
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
	if (dd->index > cvars[level]->index) {
		e = t = dd;
	}
	else {
		e = Cudd_E(dd);
		t = Cudd_T(dd);
	}
	hdd_e = build_hdd_matrix_rowrec(ddman, e, rvars, cvars, num_vars, level+1, row, col->e, hddm);
	hdd_t = build_hdd_matrix_rowrec(ddman, t, rvars, cvars, num_vars, level+1, row, col->t, hddm);
	num_hdd_matrix_nodes++;
	ptr = new HDDNode();
	ptr->type.kids.e = hdd_e;
	ptr->type.kids.t = hdd_t;
	ptr->off = (int)col;
	ptr->off2 = (int)row;
	ptr->sb = (SparseBit*)dd;
	ptr->next = hddm->col_tables[level];
	hddm->col_tables[level] = ptr;

	return ptr;
}

//------------------------------------------------------------------------------

// adds sparse bits on to the hdd

void add_sparse_bits(HDDMatrix *hddm)
{
	int i;
	double level_memory;
	bool sb_mem_out;
	HDDNode *ptr;
	
	// initialise variables
	num_sparse_bits = 0;
	sparse_bits_memory = 0;
	zero = hddm->zero;
	
	// first, we make sure that all sparse bit pointers are null
	// (they may have been used to store other things previously)
	for (i = 0; i < hddm->num_levels+1; i++) {
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

	// if user specified (valid) level, use that
	if ((num_sb_levels != -1) && (num_sb_levels >= 0) && (num_sb_levels <= hddm->num_levels)) {
		actual_num_sb_levels = num_sb_levels;
	}
	// otherwise we pick one...
	else {
		// level by level, from the bottom, compute how much memory
		// sparse bits would need for each level
		sb_mem_out = false;
		for (i = hddm->num_levels-1; i>=0; i--) {
			level_memory = 0;
			ptr = hddm->row_tables[i];
			while (ptr != NULL) {
				level_memory += ((ptr->off2)*12.0 + ((int)ptr->sb)*4.0 + 5*4.0);
				if (level_memory/1024.0 > sb_max_mem) {
					sb_mem_out = true;
					break;
				}
				ptr = ptr->next;
			}
//			printf("Level %d: %f KB %s\n", hddm->num_levels-(i), level_memory/1024.0, (sb_mem_out?"(mem out)":""));
			if (sb_mem_out) break;
		}
		// pick max level we can afford (memory-wise)
		if (sb_mem_out) {
			// (i+1)th is last one before we ran out of memory
			actual_num_sb_levels = hddm->num_levels-(i+1);
		}
		else {
			actual_num_sb_levels = hddm->num_levels;
		}
			
		// this was the old heuristic
		//actual_num_sb_levels = hddm->num_levels/3;

		// if matrix has been split into blocks for gauss-seidel/sor, make sure the two schemes don't overlap
		if (actual_num_sb_levels + hddm->sorl > hddm->num_levels) actual_num_sb_levels = hddm->num_levels - hddm->sorl;
	}

	// set all sparse bit pointers back to null
	// (except on level sparse bits will go)
	// (we will use n/nnz stats before overwriting them)
	for (i = 0; i < hddm->num_levels+1; i++) {
		// row nodes and terminals
		if ((i == hddm->num_levels - actual_num_sb_levels) && (actual_num_sb_levels> 0)) {
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
			ptr->sb = build_sparse_bit(ptr, hddm->num_levels - actual_num_sb_levels, hddm->num_levels);
			ptr = ptr->next;
		}
	}

	// store sparse bit stats
	hddm->sbl = actual_num_sb_levels;
	hddm->num_sb = num_sparse_bits;
	hddm->sb_mem = sparse_bits_memory;
}

//------------------------------------------------------------------------------

// compute the size (num states and nnz) of matrix corresponding to each hdd node
// (and store in sb and off2 pointers, respectively)

int compute_n_and_nnz_rec(HDDNode *hdd, int level, int num_levels, ODDNode *row)
{
	HDDNode *e, *t;
	
	// if it's the zero node... 
	if (hdd == zero) {
		hdd->sb = (SparseBit *)0;
		hdd->off2 = 0;
		return 0;
	}
	// if it's at the bottom...
	if (level == num_levels) {
		hdd->sb = (SparseBit *)0;
		hdd->off2 = 1;
		return 1;
	}
	
	// check if we've already done this node
	// (nb: important to check sb not off2 because we didn't reset the off2 pointers)
	if (hdd->sb != NULL) {
		return hdd->off2;
	}
	
	// store n
	hdd->sb = (SparseBit *)(row->eoff + row->toff);
	// recurse and store nnz
	hdd->off2 = 0;
	e = hdd->type.kids.e;
	if (e != zero) {
		hdd->off2 += compute_n_and_nnz_rec(e->type.kids.e, level+1, num_levels, row->e);
		hdd->off2 += compute_n_and_nnz_rec(e->type.kids.t, level+1, num_levels, row->e);
	}
	t = hdd->type.kids.t;
	if (t != zero) {
		hdd->off2 += compute_n_and_nnz_rec(t->type.kids.e, level+1, num_levels, row->t);
		hdd->off2 += compute_n_and_nnz_rec(t->type.kids.t, level+1, num_levels, row->t);
	}
	return hdd->off2;
}

//-----------------------------------------------------------------------------------

// build a sparse bit for a hdd node

SparseBit *build_sparse_bit(HDDNode *hdd, int level, int num_levels)
{
	int i, n, nnz;
	SparseBit *sb;
	
	// number of nonzeros is stored in off2
	nnz = hdd->off2;
	// size of submatrix is stored in sb pointer
	n = (int)hdd->sb;
	// work out memory running total
	sparse_bits_memory += (nnz*12.0 + n*4.0 + 5*4.0);
	// create the sparse bit
	num_sparse_bits++;
	sb = new SparseBit();
	sb->n = n;
	sb->nnz = nnz;
	sb->non_zeros = new double[nnz];
	sb->cols = new int[nnz];
	sb->row_starts = new int[n+1];
	// see how many nonzeros are in each row
	for (i = 0; i < n+1; i++) {
		sb->row_starts[i] = 0;
	}
	fill_sparse_bit_rec(hdd, level, num_levels, 0, 0, sb, 1);
	// and use this to compute the row_starts information
	for (i = 1; i < n+1; i++) {
		sb->row_starts[i] += sb->row_starts[i-1];
	}
	// then fill up the non_zeros/cols arrays
	fill_sparse_bit_rec(hdd, level, num_levels, 0, 0, sb, 2);
	// recompute the row_starts info because we've messed with it
	for (i = n; i > 0; i--) {
		sb->row_starts[i] = sb->row_starts[i-1];
	}
	sb->row_starts[0] = 0;
	
	return sb;
}

//------------------------------------------------------------------------------

// fill a sparse bit for a hdd node

void fill_sparse_bit_rec(HDDNode *hdd, int level, int num_levels, long row, long col, SparseBit *sb, int code)
{
	HDDNode *e, *t;
	
	// if it's the zero node...
	if (hdd == zero) {
		return;
	}
	// if it's at the bottom
	else if (level == num_levels) {
		// do something different depending on the code passed in
		switch (code) {
		
		case 1:
			sb->row_starts[row+1]++;
			break;
		
		case 2:
			sb->non_zeros[sb->row_starts[row]] = hdd->type.val;
			sb->cols[sb->row_starts[row]] = col;
			sb->row_starts[row]++;
			break;
		}
		return;
	}
	// otherwise recurse...
	e = hdd->type.kids.e;
	if (e != zero) {
		fill_sparse_bit_rec(e->type.kids.e, level+1, num_levels, row, col, sb, code);
		fill_sparse_bit_rec(e->type.kids.t, level+1, num_levels, row, col+e->off, sb, code);
	}
	t = hdd->type.kids.t;
	if (t != zero) {
		fill_sparse_bit_rec(t->type.kids.e, level+1, num_levels, row+hdd->off, col, sb, code);
		fill_sparse_bit_rec(t->type.kids.t, level+1, num_levels, row+hdd->off, col+t->off, sb, code);
	}
}

//------------------------------------------------------------------------------
// compute the negative row sums from the hdd of the rate matrix

double *hdd_negative_row_sums(HDDMatrix *hddm, int n)
{
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

void hdd_negative_row_sums_rec(HDDNode *hdd, int level, int num_levels, long row, double *diags)
{
	HDDNode *e, *t;
	
	// if it's the zero node
	if (hdd == zero) {
		return;
	}
	// if there's a sparse bit
	else if (hdd->sb) {
		int i, j, l, h, n;
		SparseBit *sb = hdd->sb;
		n = sb->n;
		double *non_zeros = sb->non_zeros;
		int *row_starts = sb->row_starts;
		for (i = 0; i < n; i++) {
			l = row_starts[i];
			h = row_starts[i+1];
			for (j = l; j < h; j++) {
				diags[row+i] -= non_zeros[j];
			}
		}
		return;
	}
	// or if we've reached the bottom
	else if (level == num_levels) {
		diags[row] -= hdd->type.val;
		return;
	}
	// otherwise recurse
	e = hdd->type.kids.e;
	if (e != zero) {
		hdd_negative_row_sums_rec(e->type.kids.e, level+1, num_levels, row, diags);
		hdd_negative_row_sums_rec(e->type.kids.t, level+1, num_levels, row, diags);
	}
	t = hdd->type.kids.t;
	if (t != zero) {
		hdd_negative_row_sums_rec(t->type.kids.e, level+1, num_levels, row+hdd->off, diags);
		hdd_negative_row_sums_rec(t->type.kids.t, level+1, num_levels, row+hdd->off, diags);
	}
}

//------------------------------------------------------------------------------

// free memory

void free_hdd_matrix(HDDMatrix *hddm)
{
	int i;
	HDDNode *ptr, *ptr2;
	
	// go thru all nodes
	for (i = 0; i < hddm->num_levels; i++) {
		ptr = hddm->row_tables[i];
		while (ptr) {
			// free sparse bit if there is one
			if (ptr->sb) {
				delete ptr->sb->non_zeros;
				delete ptr->sb->cols;
				delete ptr->sb->row_starts;
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
				delete ptr->sb->non_zeros;
				delete ptr->sb->cols;
				delete ptr->sb->row_starts;
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
	
	// free tables
	delete hddm->row_tables;
	delete hddm->col_tables;
	
	// free data structure
	delete hddm;
}

//------------------------------------------------------------------------------
