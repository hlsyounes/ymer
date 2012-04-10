/*
 * Interface to MT19937 random number generator.
 *
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
 * $Id: rng.h,v 2.1 2004-01-25 12:41:35 lorens Exp $
 */
#ifndef RNG_H
#define RNG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned long aaa;
  unsigned long maskB, maskC;
  unsigned long *state;
  int left, initf;
  unsigned long *next;
} mt_struct;

/* Seeds the MT19937 random number generator. */
void init_genrand(unsigned long s);

/* Generates an integer random number between 0 and 0xffffffff (both
   inclusive). */
unsigned long genrand_int32(void);

/* Generates a random number in the interval [0,1]. */
double genrand_real1(void);

/* Generates a random number in the interval [0,1). */
double genrand_real2(void);

/* Generates a random number in the interval (0,1). */
double genrand_real3(void);

/* Construct id-specific MT521 random number generator. */
mt_struct *get_mt_parameter_id(unsigned short id);

/* Deletes the given id-specific random number generator. */
void free_mt_struct(mt_struct *mts);

/* Seeds the given id-specific random number generator. */
void init_genrand_id(unsigned long s, mt_struct *mts);

/* Generates an integer random number between 0 and 0xffffffff (both
   inclusive) using the given id-specific generator. */
unsigned long genrand_int32_id(mt_struct *mts);

/* Generates a random number in the interval [0,1] using the given
   id-specific generator. */
double genrand_real1_id(mt_struct *mts);

/* Generates a random number in the interval [0,1) using the given
   id-specific generator. */
double genrand_real2_id(mt_struct *mts);

/* Generates a random number in the interval (0,1) using the given
   id-specific generator. */
double genrand_real3_id(mt_struct *mts);

#ifdef	__cplusplus
}
#endif

#endif /* RNG_H */
