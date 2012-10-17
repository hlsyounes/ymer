/* seive.c */

/* Copyright (C) 2001 Makoto Matsumoto and Takuji Nishimura.       */
/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

#include <stdio.h>
#include <stdlib.h>
#include "rng.h"

#define N 17
#define M 8
#define R 23
#define UMASK 0xff800000UL
#define LMASK 0x007fffffUL
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v,a) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? (a) : 0UL))

#define SSS 7
#define TTT 15
#define S00 11
#define S01 18

#define WORD_LEN 32

#define LIMIT_IRRED_DEG 31
#define NIRREDPOLY 127
#define MAX_IRRED_DEG 9

/** for get_tempering_parameter_hard **/
#define LIMIT_V_BEST_OPT 15 

#define MIN_INFINITE (-2147483647-1)

#define TRNSTMP  tmp ^= ( (tmp>>S00) & 0xffffffffUL )
#define MASKTMP  tmp ^= ( (tmp<<SSS) & mask_b);tmp ^= ( (tmp<<TTT) & mask_c)
#define LSB(x) ( (x) & 0x1UL )

#define MAX_SEARCH 10000

static unsigned long matrix_a[10] = {
    0xa7750000UL, 0x8eaf0001UL, 0xcceb0002UL, 0xc07f0003UL, 0xd7ff0004UL,
    0xa6c90005UL, 0xe6380006UL, 0xedab0007UL, 0xe27f0008UL, 0xdd930009UL
};

/* list of irreducible polynomials whose degrees are less than 10 */
static int irredpolylist[NIRREDPOLY][MAX_IRRED_DEG+1] = {
    {0,1,0,0,0,0,0,0,0,0,},{1,1,0,0,0,0,0,0,0,0,},{1,1,1,0,0,0,0,0,0,0,},
    {1,1,0,1,0,0,0,0,0,0,},{1,0,1,1,0,0,0,0,0,0,},{1,1,0,0,1,0,0,0,0,0,},
    {1,0,0,1,1,0,0,0,0,0,},{1,1,1,1,1,0,0,0,0,0,},{1,0,1,0,0,1,0,0,0,0,},
    {1,0,0,1,0,1,0,0,0,0,},{1,1,1,1,0,1,0,0,0,0,},{1,1,1,0,1,1,0,0,0,0,},
    {1,1,0,1,1,1,0,0,0,0,},{1,0,1,1,1,1,0,0,0,0,},{1,1,0,0,0,0,1,0,0,0,},
    {1,0,0,1,0,0,1,0,0,0,},{1,1,1,0,1,0,1,0,0,0,},{1,1,0,1,1,0,1,0,0,0,},
    {1,0,0,0,0,1,1,0,0,0,},{1,1,1,0,0,1,1,0,0,0,},{1,0,1,1,0,1,1,0,0,0,},
    {1,1,0,0,1,1,1,0,0,0,},{1,0,1,0,1,1,1,0,0,0,},{1,1,0,0,0,0,0,1,0,0,},
    {1,0,0,1,0,0,0,1,0,0,},{1,1,1,1,0,0,0,1,0,0,},{1,0,0,0,1,0,0,1,0,0,},
    {1,0,1,1,1,0,0,1,0,0,},{1,1,1,0,0,1,0,1,0,0,},{1,1,0,1,0,1,0,1,0,0,},
    {1,0,0,1,1,1,0,1,0,0,},{1,1,1,1,1,1,0,1,0,0,},{1,0,0,0,0,0,1,1,0,0,},
    {1,1,0,1,0,0,1,1,0,0,},{1,1,0,0,1,0,1,1,0,0,},{1,0,1,0,1,0,1,1,0,0,},
    {1,0,1,0,0,1,1,1,0,0,},{1,1,1,1,0,1,1,1,0,0,},{1,0,0,0,1,1,1,1,0,0,},
    {1,1,1,0,1,1,1,1,0,0,},{1,0,1,1,1,1,1,1,0,0,},{1,1,0,1,1,0,0,0,1,0,},
    {1,0,1,1,1,0,0,0,1,0,},{1,1,0,1,0,1,0,0,1,0,},{1,0,1,1,0,1,0,0,1,0,},
    {1,0,0,1,1,1,0,0,1,0,},{1,1,1,1,1,1,0,0,1,0,},{1,0,1,1,0,0,1,0,1,0,},
    {1,1,1,1,1,0,1,0,1,0,},{1,1,0,0,0,1,1,0,1,0,},{1,0,1,0,0,1,1,0,1,0,},
    {1,0,0,1,0,1,1,0,1,0,},{1,0,0,0,1,1,1,0,1,0,},{1,1,1,0,1,1,1,0,1,0,},
    {1,1,0,1,1,1,1,0,1,0,},{1,1,1,0,0,0,0,1,1,0,},{1,1,0,1,0,0,0,1,1,0,},
    {1,0,1,1,0,0,0,1,1,0,},{1,1,1,1,1,0,0,1,1,0,},{1,1,0,0,0,1,0,1,1,0,},
    {1,0,0,1,0,1,0,1,1,0,},{1,0,0,0,1,1,0,1,1,0,},{1,0,1,1,1,1,0,1,1,0,},
    {1,1,0,0,0,0,1,1,1,0,},{1,1,1,1,0,0,1,1,1,0,},{1,1,1,0,1,0,1,1,1,0,},
    {1,0,1,1,1,0,1,1,1,0,},{1,1,1,0,0,1,1,1,1,0,},{1,1,0,0,1,1,1,1,1,0,},
    {1,0,1,0,1,1,1,1,1,0,},{1,0,0,1,1,1,1,1,1,0,},{1,1,0,0,0,0,0,0,0,1,},
    {1,0,0,0,1,0,0,0,0,1,},{1,1,1,0,1,0,0,0,0,1,},{1,1,0,1,1,0,0,0,0,1,},
    {1,0,0,0,0,1,0,0,0,1,},{1,0,1,1,0,1,0,0,0,1,},{1,1,0,0,1,1,0,0,0,1,},
    {1,1,0,1,0,0,1,0,0,1,},{1,0,0,1,1,0,1,0,0,1,},{1,1,1,1,1,0,1,0,0,1,},
    {1,0,1,0,0,1,1,0,0,1,},{1,0,0,1,0,1,1,0,0,1,},{1,1,1,1,0,1,1,0,0,1,},
    {1,1,1,0,1,1,1,0,0,1,},{1,0,1,1,1,1,1,0,0,1,},{1,1,1,0,0,0,0,1,0,1,},
    {1,0,1,0,1,0,0,1,0,1,},{1,0,0,1,1,0,0,1,0,1,},{1,1,0,0,0,1,0,1,0,1,},
    {1,0,1,0,0,1,0,1,0,1,},{1,1,1,1,0,1,0,1,0,1,},{1,1,1,0,1,1,0,1,0,1,},
    {1,0,1,1,1,1,0,1,0,1,},{1,1,1,1,0,0,1,1,0,1,},{1,0,0,0,1,0,1,1,0,1,},
    {1,1,0,1,1,0,1,1,0,1,},{1,0,1,0,1,1,1,1,0,1,},{1,0,0,1,1,1,1,1,0,1,},
    {1,0,0,0,0,0,0,0,1,1,},{1,1,0,0,1,0,0,0,1,1,},{1,0,1,0,1,0,0,0,1,1,},
    {1,1,1,1,1,0,0,0,1,1,},{1,1,0,0,0,1,0,0,1,1,},{1,0,0,0,1,1,0,0,1,1,},
    {1,1,0,1,1,1,0,0,1,1,},{1,0,0,1,0,0,1,0,1,1,},{1,1,1,1,0,0,1,0,1,1,},
    {1,1,0,1,1,0,1,0,1,1,},{1,0,0,0,0,1,1,0,1,1,},{1,1,0,1,0,1,1,0,1,1,},
    {1,0,1,1,0,1,1,0,1,1,},{1,1,0,0,1,1,1,0,1,1,},{1,1,1,1,1,1,1,0,1,1,},
    {1,0,1,0,0,0,0,1,1,1,},{1,1,1,1,0,0,0,1,1,1,},{1,0,0,0,0,1,0,1,1,1,},
    {1,0,1,0,1,1,0,1,1,1,},{1,0,0,1,1,1,0,1,1,1,},{1,1,1,0,0,0,1,1,1,1,},
    {1,1,0,1,0,0,1,1,1,1,},{1,0,1,1,0,0,1,1,1,1,},{1,0,1,0,1,0,1,1,1,1,},
    {1,0,0,1,1,0,1,1,1,1,},{1,1,0,0,0,1,1,1,1,1,},{1,0,0,1,0,1,1,1,1,1,},
    {1,1,0,1,1,1,1,1,1,1,},
};

typedef struct {int *x; int deg;} Polynomial;

typedef struct {
    unsigned long z;    /* integer part */
    unsigned long *cf;  /* fraction part */
    int start;     /* beginning of fraction part */
    int degree;    /* maximum degree */
    unsigned long bp;   /* bitpatterm (shifted&bitmasked) at the maximum degree */
}Vector;

typedef struct mask_node{
    unsigned long b,c;
    int v,leng;
    struct mask_node *next;
}MaskNode;

static unsigned long **modlist;
static Polynomial **preModPolys;

static unsigned long bitmask[WORD_LEN];
static unsigned long cur_bitmask[WORD_LEN];
static unsigned long mask_b, mask_c;
static unsigned long upper_v_bits;
static unsigned long aaa[2];
/** for optimize_v_hard **/
static int gcur_maxlengs[WORD_LEN];
static unsigned long gmax_b, gmax_c;

/********************* eqdeg **********************/

static void init_tempering(mt_struct *mts)
{
    int i;

    /* bits are filled in mts->aaa from MSB */
    aaa[0] = 0x0UL; aaa[1] = (mts->aaa);


    for( i=0; i<WORD_LEN; i++)
        bitmask[i] = cur_bitmask[i] = 0x80000000UL >> i;
}

static int push_mask(int l, int v, unsigned long b, unsigned long c, unsigned long *bbb, unsigned long *ccc)
{
    int i, j, k, nbv, nbvt;
    unsigned long bmask, bv_buf[2], bvt_buf[2];

    k = l;
    if( (SSS+v) >= WORD_LEN ){
        nbv = 1; bv_buf[0] = 0;
    }
    else if( (v>=TTT) && (c&bitmask[v-TTT] ) ){
        nbv = 1; bv_buf[0] = b&bitmask[v];
    }
    else {
        nbv = 2; bv_buf[0] = bitmask[v]; bv_buf[1] = 0;
    }

    if( ((v+TTT+SSS) < WORD_LEN) && (c&bitmask[v]) ){
        nbvt = 2; bvt_buf[0] = bitmask[v+TTT]; bvt_buf[1] = 0;
    }
    else {
        nbvt = 1; bvt_buf[0] = 0;
    }

    bmask = bitmask[v];
    if( (v+TTT) < WORD_LEN )
        bmask |= bitmask[v+TTT];
    bmask = ~bmask;
    for( i=0; i<nbvt; ++i){
        for( j=0; j<nbv; ++j){
            bbb[k] = (b&bmask) | bv_buf[j] | bvt_buf[i];
            ccc[k] = c;
            ++k;
        }
    }

    return k-l;
}

static int push_stack(unsigned long b, unsigned long c, int v, unsigned long *bbb, unsigned long *ccc)
{
    int i, ll, ncv;
    unsigned long cv_buf[2];

    ll = 0;

    if( (v+TTT) < WORD_LEN ){
        ncv = 2; cv_buf[0] = c | bitmask[v]; cv_buf[1] = c;
    }
    else {
        ncv = 1; cv_buf[0] = c;
    }

    for( i=0; i<ncv; ++i)
        ll += push_mask( ll, v, b, cv_buf[i], bbb, ccc);

    return ll;
}

static Vector *new_Vector(void)
{
    Vector *v;

    v = (Vector *)malloc( sizeof( Vector ) );
    if( v == NULL ){
        printf("malloc error in \"new_Vector()\"\n");
        exit(1);
    }

    v->cf = (unsigned long *)calloc( N, sizeof( unsigned long ) );
    if( v->cf == NULL ){
        printf("calloc error in \"new_Vector()\"\n");
        exit(1);
    }

    v->start = 0;

    return v;
}

static void free_Vector( Vector *v )
{
    if( NULL != v->cf ) free( v->cf );
    if( NULL != v ) free( v );
}

/* returns the max degree of v */
static int degree_of_vector( Vector *v)
{
    int i,j,k;
    int immm; /* immm = i + M */
    unsigned long tmp;
    Vector *u;

    if( v->z & upper_v_bits ){ /* if integer part is not 0 */
        v->bp = v->z;
        v->degree = 0;
        return 0;
    }

    for(k=v->start, j=0; j<N; j++,k++){

	if (k>=N) k -= N; /* same as k %= N (note 0<= k < 2*N) */

        tmp = v->cf[k];
	if (tmp) {
	    TRNSTMP;
	    MASKTMP;
	    if( tmp & upper_v_bits ) {
		v->bp = tmp;
		v->degree = -(j+1);
		return -(j+1);
	    }
	}
    }

    u = new_Vector(); /* copy v to u */
    for( j=0; j<N; j++) u->cf[j] = v->cf[j];
    u->z = v->z;
    u->start = v->start;


    k = N * (WORD_LEN-1) - R;
    i=u->start; 
    immm = i + M; /* note : immm < N+M < 2N */
    for(j=0; j<k; j++){ 

	  /* i = (u->start + j) % N */
	  if (i>=N) i -= N; /* same as i%=N: note always 0<=i<2*N */
	  if (immm>=N) immm -= N; /* same as immm %= N */

	  tmp = (u->cf[i] & UMASK) | (u->cf[(i+1)%N] & LMASK);
	  tmp = u->cf[immm] ^ ( (tmp>>1) ^ aaa[LSB(tmp)] );
	  tmp &= 0xffffffffUL;
	  u->cf[i] = tmp;

	  if (tmp) {
	      TRNSTMP;
	      MASKTMP;
	      if( tmp & upper_v_bits ) {
		  v->bp = tmp;
		  v->degree = -(j+N+1);
		  free_Vector(u);
		  return -(j+N+1);
	      }
	  }
	  
	  ++i;
	  ++immm;
    }

    free_Vector(u);
    v->bp = 0;
    v->degree = MIN_INFINITE;
    return MIN_INFINITE; /* if 0 inspite of  (nw-r) times of generation */
}

/* makes a initial lattice */
static Vector **make_lattice(int v)
{
    int i;
    unsigned long tmp;
    Vector **lattice, *top;

    lattice = (Vector **)malloc( (v+1) * sizeof( Vector *) );
    if( NULL == lattice ){
        printf("malloc error in \"make_lattice\"\n");
        exit(1);
    }

    for( i=1; i<=v; i++){ /* from 1st row to v-th row */
        lattice[i] = new_Vector();
        lattice[i]->z = bitmask[i-1];
        lattice[i]->start = 0;
        lattice[i]->bp = bitmask[i-1];
        lattice[i]->degree = 0;
    }


    top = new_Vector(); /* 0th row */
    for(i=0; i<N; i++) {
	  top->cf[i] = genrand_int32();
	  top->cf[i] &= 0xffffffffUL;
	}
    tmp = ( top->cf[0] & UMASK ) | ( top->cf[1] & LMASK );
    top->cf[0] = top->cf[M] ^ ( (tmp>>1) ^ aaa[LSB(tmp)] );
	top->cf[0] &= 0xffffffffUL;
    top->z = 0; top->start = 1; 
    degree_of_vector( top );
    lattice[0] = top;

    return lattice;
}

static void free_lattice( Vector **lattice, int v)
{
    int i;

    for( i=0; i<=v; i++)
        free_Vector( lattice[i] );
    free( lattice );
}

/* Vector v is multiplied by t^k */
static Vector *mult( Vector *v, int k)  
{
    int i, j, jmmm; /* jmmm = j + M */
    unsigned long tmp;
    Vector *u;

    u = new_Vector();
    for( i=0; i<N; i++) u->cf[i] = v->cf[i]; /* copy v to u */
    u->start = v->start;
    u->degree = v->degree;
    u->bp = v->bp;
    u->z = v->z;

    if( k == 0 ) return u; /* if k==0, only v is copied to u  */

    j=u->start; 
    jmmm = j+M; /* note : jmmm < N+M < 2N */
    for(i=0; i<k; i++){

	if (j>=N) j -= N; /* same as j%=N */
	if (jmmm>=N) jmmm -= N; /* same as jmmm %= N */

	u->z = u->cf[j];
	tmp =  (u->cf[j] & UMASK) | (u->cf[(j+1)%N] & LMASK) ;
	tmp = u->cf[jmmm] ^ ( (tmp>>1) ^ aaa[LSB(tmp)] );
	tmp &= 0xffffffffUL;
	u->cf[j] =  tmp;
	
	++j;
	++jmmm;
    }

    u->start += k; u->start %= N;

    /* integer part is shifted and bitmasked */
    tmp = u->z;
    TRNSTMP;
    MASKTMP;
    u->z = tmp;
    u->degree += k;

    return u;
}

/* adds v to u (then u will change) */
static void add( Vector *u, Vector *v) 
{
    int i, stu, stv;

    stu = u->start; stv = v->start;
    for( i=0; i<N; i++){

	/**  0 <= stu,stv < 2*N always holds          **/
	/** so, modulo N can be calculated as follows **/
	if (stu>=N) stu -= N; /* same as stu %= N  */
	if (stv>=N) stv -= N; /* same as stv %= N  */

	u->cf[stu] ^= v->cf[stv];
	stu++; stv++;
    }

    u->z ^=  v->z;
}

/* add t^k*(i-th row) to j-th row */
static void add_i_to_j( Vector **lattice, int i, int j, int k)
{
    Vector *ith;

    ith = mult( lattice[i], k);
    add( lattice[j], ith);
    free_Vector( ith );
}

/* exchange columns so that i-th element of variable vec */
/* gives the norm of vec */
static void pull_max( int i, int v, Vector *vec)
{
    int j;
    unsigned long tmp;

    if( vec->bp & cur_bitmask[i] ) return;

    for( j=i+1; j<v; j++){
        if( vec->bp & cur_bitmask[j] ){
            tmp = cur_bitmask[i];
            cur_bitmask[i] = cur_bitmask[j];
            cur_bitmask[j] = tmp;
            break;
        }
    }

}

/* puts i-th row be the minimum one in i-th row ... v-th row */
static void pull_min_row( int i, int v, Vector **lattice)
{
    int j, min_deg, min_j;
    Vector *vtmp;

    min_deg = lattice[i]->degree;
    min_j = i;
    for( j=i+1; j<=v; j++){
        if( min_deg > lattice[j]->degree ){
            min_deg = lattice[j]->degree;
            min_j = j;
        }
    }

    vtmp = lattice[min_j]; lattice[min_j] = lattice[i];
    lattice[i] = vtmp;
}

/* sweeps out k-th row with 0th ... (k-1)th rows */
static void hakidasi( int k, Vector **lattice)
{
  int i;
  
  for( i=0; i<k; i++){
	if( lattice[k]->bp & cur_bitmask[i] ){
	  add_i_to_j( lattice, i, k, lattice[k]->degree - lattice[i]->degree);
	  lattice[k]->bp ^= lattice[i]->bp;
        }
    }
}

static int lenstra(int v)
{
    Vector **lattice, *ltmp;
    int i, j, deg, max_row;

    upper_v_bits = 0;
    for( i=0; i<v; i++) {
        cur_bitmask[i] = bitmask[i];
        upper_v_bits |= bitmask[i];
    }

    lattice = make_lattice( v );

    i = -1; max_row=v;
    while( i<max_row ){ /* normalized until i-th row */

        pull_min_row( i+1, max_row, lattice );
        hakidasi( i+1, lattice);

        if( lattice[i+1]->bp & upper_v_bits ) {
            pull_max( i+1, v, lattice[i+1] );
            ++i;
        }
        else {
            deg = degree_of_vector( lattice[i+1]); 

            if(deg==MIN_INFINITE){
            /* if deg==MIN_INFINITE, */ 
            /* exchange (i+1)-th row and v-th row */
                ltmp = lattice[i+1]; lattice[i+1] = lattice[v];
                lattice[v] = ltmp;
                --max_row; 
            }
            else {
                for( j=i; j>=0; j--){
                    if( lattice[j]->degree <= deg ) break;
                }
                i = j;
            }
        }

    }

    i = lattice[max_row]->degree;
    free_lattice( lattice, v );
    return -i;
}

static MaskNode *cons_MaskNode(MaskNode *head, unsigned long b, unsigned long c, int leng)
{
    MaskNode *t;

    t = (MaskNode*)malloc(sizeof(MaskNode));
    if (t == NULL) {
	printf("malloc error in \"cons_MaskNode\"\n");
        exit(1);
    }

    t->b = b;
    t->c = c;
    t->leng = leng;
    t->next = head;

    return t;
}

static void delete_MaskNodes(MaskNode *head)
{
    MaskNode *t;

    while(head != NULL) {
	t = head->next;
	free(head);
	head = t;
    }
}

static MaskNode *delete_lower_MaskNodes(MaskNode *head, int l)
{
    MaskNode *s, *t, *tail;

    s = head;
    while(1) { /* heading */
	if (s == NULL)
	    return NULL;
	if (s->leng >= l)
	    break;
	t = s->next;
	free(s);
	s = t;
    }

    head = tail = s;
    
    while (head != NULL) {
	t = head->next;
	if (head->leng < l) {
	    free(head);
	}
	else {
	    tail->next = head;
	    tail = head;
	}
	head = t;
    }
	
    tail->next = NULL;
    return s;
}

/* (v-1) bitmasks of b,c */
static MaskNode *optimize_v_hard(int v, MaskNode *prev_masks)
{
    int i, ll, t;
    unsigned long bbb[8], ccc[8];
    MaskNode *cur_masks;

    cur_masks = NULL;

    while (prev_masks != NULL) {

	ll = push_stack(prev_masks->b,prev_masks->c,v,bbb,ccc);

	for (i=0; i<ll; ++i) {
	    mask_b = bbb[i];
	    mask_c = ccc[i];
	    t = lenstra(v+1);
	    if (t >= gcur_maxlengs[v]) {
		gcur_maxlengs[v] = t;
		gmax_b = mask_b;
		gmax_c = mask_c;
		cur_masks = cons_MaskNode(cur_masks, mask_b, mask_c, t);
	    }
	}
	prev_masks = prev_masks->next;
    }

    cur_masks = delete_lower_MaskNodes(cur_masks, gcur_maxlengs[v]);

    return cur_masks;
}

/* (v-1) bitmasks of b,c */
static void optimize_v(unsigned long b, unsigned long c, int v)
{
    int i, max_len, max_i, ll, t;
    unsigned long bbb[8], ccc[8];

    ll = push_stack(b,c,v,bbb,ccc);

    max_len = max_i = 0;
    if (ll > 1) { 
	for (i=0; i<ll; ++i) {
	    mask_b = bbb[i];
	    mask_c = ccc[i];
	    t = lenstra(v+1);
	    if (t > max_len) {
		max_len = t;
		max_i = i;
	    }
	}
    }

    if ( v >= WORD_LEN-1 ) {
	mask_b = bbb[max_i];
	mask_c = ccc[max_i];
	return;
    }

    optimize_v(bbb[max_i], ccc[max_i], v+1);
}

static void _get_tempering_parameter_hard_dc(mt_struct *mts)
{
    int i;
    MaskNode mn0, *cur, *next;

    init_tempering(mts);

    for (i=0; i<WORD_LEN; i++) 
	gcur_maxlengs[i] = -1;

    mn0.b = mn0.c = mn0.leng = 0;
    mn0.next = NULL;
	
    cur = &mn0;
    for (i=0; i<LIMIT_V_BEST_OPT; i++) {
	next = optimize_v_hard(i, cur);
	if (i > 0) 
	    delete_MaskNodes(cur);
	cur = next;
    }
    delete_MaskNodes(cur);

    optimize_v(gmax_b, gmax_c,i);
    mts->maskB = mask_b;
    mts->maskC = mask_c;
}

/********* prescreening functions *********/

static Polynomial *NewPoly(int degree)
{
    Polynomial *p;
    
    p = (Polynomial *)calloc( 1, sizeof(Polynomial));
    if( p==NULL ){
	printf("calloc error in \"NewPoly()\"\n");
	exit(1);
    }
    p->deg = degree;

    if (degree < 0) {
	p->x = NULL;
	return p;
    }
	
    p->x = (int *)calloc( degree + 1, sizeof(int));
    if( p->x == NULL ){
	printf("calloc error\n");
	exit(1);
    }

    return p;
}

static void FreePoly( Polynomial *p)
{
    if (p->x != NULL)
	free( p->x );
    free( p );
}

static Polynomial *PolynomialDup(Polynomial *pl)
{
    Polynomial *pt;
    int i;

    pt = NewPoly(pl->deg);
    for (i=pl->deg; i>=0; i--)
	pt->x[i] = pl->x[i];

    return pt;
}

/** wara mod waru **/
/** the result is stored in wara ********/
static void PolynomialMod( Polynomial *wara, const Polynomial *waru)
{
    int i;
    int deg_diff;

    while( wara->deg >= waru->deg  ){
	deg_diff = wara->deg - waru->deg;
	for( i=0; i<=waru->deg; i++){
	    wara->x[ i+deg_diff ] ^= waru->x[i];
	}
	
	for( i=wara->deg; i>=0; i--){
	    if( wara->x[i] ) break;
	}
	wara->deg=i;	
	
    }
}

/** multiplication **/
static Polynomial *PolynomialMult(Polynomial *p0,Polynomial *p1)
{
    int i, j;
    Polynomial *p;

    /* if either p0 or p1 is 0, return 0 */
    if ( (p0->deg < 0) || (p1->deg < 0) ) {
	p = NewPoly(-1);
	return p;
    }

    p = NewPoly(p0->deg + p1->deg);
    for( i=0; i<=p1->deg; i++){
	if( p1->x[i] ){
	    for( j=0; j<=p0->deg; j++){
		p->x[i+j] ^= p0->x[j];
	    }
	}
    }

    return p;
}

/** make the polynomial  "t**n + t**m"  **/
static Polynomial *make_tntm(int n, int m)
{
    Polynomial *p;

    p = NewPoly(n);
    p->x[n] = p->x[m] = 1;

    return p;
}

static void MakepreModPolys()
{
    Polynomial *t, *t0, *t1, *s, *s0, *s1;
    int i,j;

    j = 0;
    t = NewPoly(0);
    t->deg = 0;
    t->x[0] = 1;
    preModPolys[j++] = t;

    t = make_tntm (N, M);
    t0 = make_tntm (N, M);
    s = make_tntm (N-1, M-1);

    for( i=1; i<(WORD_LEN - R); i++){
	preModPolys[j++] = PolynomialDup(t0);
	t1 = t0; 
	t0 = PolynomialMult(t0, t); 
	FreePoly(t1);
    }

    preModPolys[j++] = PolynomialDup(t0);

    s0 =PolynomialMult( t0, s);
    FreePoly(t0);	FreePoly(t);
    for( i=(R-2); i>=0; i--){
	preModPolys[j++] = PolynomialDup(s0);
	s1 = s0; 
	s0 = PolynomialMult( s0, s); 
	FreePoly(s1);
    }
	
    preModPolys[j++] = PolynomialDup(s0);

    FreePoly(s0); FreePoly(s); 
}

/* aaa = (a_{w-1}a_{w-2}...a_1a_0 */   
static int IsReducible(unsigned long aaa, unsigned long *polylist)
{
    int i;
    unsigned long x;

    x = polylist[WORD_LEN];
    for (i=WORD_LEN-1; i>=0; i--) {
	if (aaa & 0x1UL) 
	    x ^= polylist[i];
	aaa >>= 1;
    }

    if ( x == 0U ) return 1;
    else return 0;
}

/* Pack Polynomial into a word */
static unsigned long word2bit(Polynomial *pl)
{
    int i;
    unsigned long bx;

    bx = 0UL;
    for (i=pl->deg; i>0; i--) {
	if (pl->x[i]) bx |= 0x1UL;
	bx <<= 1;
    }
    if (pl->x[0]) bx |= 0x1UL;
      
    return bx;
}

static void makemodlist(Polynomial *pl, int nPoly)
{
    Polynomial *tmpPl;
    int i;
    
    for (i=0; i<=WORD_LEN; i++) {
	tmpPl = PolynomialDup(preModPolys[i]);
	PolynomialMod(tmpPl,pl);
	modlist[nPoly][i] = word2bit(tmpPl);
	FreePoly(tmpPl);
    }
}

static void NextIrredPoly(Polynomial *pl, int nth)
{
    int i, max_deg;
    
    for (max_deg=0,i=0; i<=MAX_IRRED_DEG; i++) {
	if ( irredpolylist[nth][i] ) 
	    max_deg = i;
	pl->x[i] = irredpolylist[nth][i];
    }

    pl->deg = max_deg;

}

static int _prescreening_dc(unsigned long aaa)
{
    
    int i;

    for (i=0; i<NIRREDPOLY; i++) {
	if (IsReducible(aaa,modlist[i])) 
	    return 0;
    }
    return 1;
}

static void _InitPrescreening_dc(void)
{
    int i;
    Polynomial *pl;

    preModPolys = (Polynomial **)malloc((WORD_LEN+1)*(sizeof(Polynomial*)));
    if (NULL == preModPolys) {
	printf ("malloc error in \"InitPrescreening\"\n");
	exit(1);
    }
    MakepreModPolys();

    modlist = (unsigned long**)malloc(NIRREDPOLY * sizeof(unsigned long*));
    if (NULL == modlist) {
	printf ("malloc error in \"InitPrescreening()\"\n");
	exit(1);
    }
    for (i=0; i<NIRREDPOLY; i++) {
	modlist[i] = (unsigned long*)malloc( (WORD_LEN + 1) * (sizeof(unsigned long)) );
	if (NULL == modlist[i]) {
	    printf ("malloc error in \"InitPrescreening()\"\n");
	    exit(1);
	}
    }


    for (i=0; i<NIRREDPOLY; i++) {
	pl = NewPoly(MAX_IRRED_DEG);
	NextIrredPoly(pl,i); 
	makemodlist(pl, i);
	FreePoly(pl);
    }

    for (i=WORD_LEN; i>=0; i--)
	FreePoly(preModPolys[i]);
    free(preModPolys);

}

static void _EndPrescreening_dc(void)
{
    int i;

    for (i=0; i<NIRREDPOLY; i++) 
      free(modlist[i]);
    free(modlist);
}

/************ deterministic seive ************/

static int _CheckPeriod_dc(unsigned long a)
{
    int i, j, p, pp;
    unsigned long y, *x, *init, mat[2];

 
    p = N*WORD_LEN - R;

    x = (unsigned long*) malloc (2*p*sizeof(unsigned long));
    if (NULL==x) {
	printf("malloc error in \"_CheckPeriod_dc()\"\n");
	exit(1);
    }

    init = (unsigned long*) malloc (N*sizeof(unsigned long));
    if (NULL==init) {
	printf("malloc error \"_CheckPeriod_dc()\"\n");
	free(x);
	exit(1);
    }

    /* set initial values */
    for (i=0; i<N; ++i) 
	x[i] = init[i] = (0xffffffffUL & genrand_int32());
    /* it is better that LSBs of x[2] and x[3] are different */
    if ( (x[2] & 0x1UL) == (x[3] & 0x1UL) ) {
	x[3] ^= 0x1UL;
	init[3] ^= 0x1UL;
    }

    pp = 2*p-N;
    mat[0] = 0x0U; mat[1] = a;
    for (j=0; j<p; ++j) {
		
	/* generate */
	for (i=0; i<pp; ++i){
	    y = (x[i] & UMASK) | (x[i+1] & LMASK);
	    x[i+N] = x[i+M] ^ ( (y>>1) ^ mat[y & 0x1UL] );
	}
		
	/* pick up odd subscritpt elements */
	for (i=2; i<=p; ++i)
	    x[i] = x[(i<<1)-1];
		
	/* reverse generate */
	for (i=p-N; i>=0; --i) {
	    y = x[i+N] ^ x[i+M] ^ mat[ x[i+1] & 0x1UL ];
	    y <<=1; y |= x[i+1] & 0x1UL;

	    x[i+1] = (x[i+1] & UMASK) | (y & LMASK);
	    x[i] = (y & UMASK) | (x[i] & LMASK);
	}
		
    }

    if ((x[0] & UMASK)==(init[0] & UMASK)) {
	for (i=1; i<N; ++i) {
	    if (x[i] != init[i])
		break;
	}
	if (i==N) {
	    free(x); free(init);
	    return 1;
	}
    }

    free(x); free(init);
    return 0;
}

/************************************************************/

static int get_irred_param(mt_struct *mts, unsigned short id)
{
    int i;
    unsigned long a;

    for (i=0; i<MAX_SEARCH; i++) {
	a = ((genrand_int32() & 0xffff0000UL)
	     | 0x80000000UL | (unsigned long)id);
	if (_prescreening_dc(a)) {
	    if (_CheckPeriod_dc(a)) {
		mts->aaa = a;
		return 1;
	    }
	}
    }

    return 0;
}

static mt_struct *alloc_mt_struct(void)
{
    mt_struct *mts;

    mts = (mt_struct*)malloc(sizeof(mt_struct));
    if (NULL == mts) return NULL;
    mts->state = (unsigned long*)malloc(N*sizeof(unsigned long));
    if (NULL == mts->state) {
	free(mts);
	return NULL;
    }
    mts->left = 1;
    mts->initf = 0;

    return mts;
}

/* id <= 0xffff */
mt_struct *get_mt_parameter_id(unsigned short id)
{
    mt_struct *mts;

    if (id > 0xffff) {
	fprintf(stderr, "\"id\" must be less than 65536\n");
	return NULL;
    }

    mts = alloc_mt_struct();
    if (mts == NULL) return NULL;

    if (id < 10) {
      mts->aaa = matrix_a[id];
      return mts;
    }

    _InitPrescreening_dc();

    if ( !get_irred_param(mts, id) ) {
	free_mt_struct(mts);
	_EndPrescreening_dc();
	return NULL;
    }
    _get_tempering_parameter_hard_dc(mts);
    _EndPrescreening_dc();
    
    return mts;
}

void free_mt_struct(mt_struct *mts)
{
    free(mts->state);
    free(mts);
}

void init_genrand_id(unsigned long s, mt_struct *mts) 
{
    int j;

    if (mts == NULL) {
	init_genrand(s);
	return;
    }

    mts->state[0]= s & 0xffffffffUL;
    for (j=1; j<N; j++) {
        mts->state[j] = (1812433253UL
			 * (mts->state[j-1] ^ (mts->state[j-1] >> 30)) + j);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array state[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mts->state[j] &= 0xffffffffUL;  /* for >32 bit machines */
    }
    mts->left = 1; mts->initf = 1;
}

static void next_state(mt_struct *mts)
{
    unsigned long *p=mts->state;
    int j;

    /* if init_genrand() has not been called, */
    /* a default initial seed is used         */
    if (mts->initf==0) init_genrand_id(5489UL, mts);

    mts->left = N;
    mts->next = mts->state;
    
    for (j=N-M+1; --j; p++) 
        *p = p[M] ^ TWIST(p[0], p[1], mts->aaa);

    for (j=M; --j; p++) 
        *p = p[M-N] ^ TWIST(p[0], p[1], mts->aaa);

    *p = p[M-N] ^ TWIST(p[0], mts->state[0], mts->aaa);
}

unsigned long genrand_int32_id(mt_struct *mts) 
{
    unsigned long y;

    if (mts == NULL) {
	return genrand_int32();
    }

    if (--mts->left == 0) next_state(mts);
    y = *mts->next++;

    /* Tempering */
    y ^= (y >> S00);
    y ^= (y << SSS) & mts->maskB;
    y ^= (y << TTT) & mts->maskC;
    y ^= (y >> S01);

    return y;
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2_id(mt_struct *mts)
{
    return (double)genrand_int32_id(mts) * (1.0/4294967296.0);
    /* divided by 2^32 */
}
