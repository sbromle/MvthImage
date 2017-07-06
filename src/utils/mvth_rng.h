#ifndef MVTH_RNG_H
#define MVTH_RNG_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Random number generator.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

/* interface to the Mersenne-Twister Random number generator. */

extern void init_rng(unsigned long seed);
extern double genrand(void);

#endif
