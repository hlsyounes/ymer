/*
 * Interface to MT19937 random number generator.
 *
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
 * $Id: rng.h,v 1.1 2003-11-12 03:58:48 lorens Exp $
 */
#ifndef RNG_H
#define RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes state[N] with the given seed. */
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

#ifdef	__cplusplus
}
#endif

#endif /* RNG_H */
