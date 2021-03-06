/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Gaussian Random Number Generator.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef RNGGAUSSIAN
#define RNGGAUSSIAN
/* Returns a random number chosen from a gaussian dist with stddev=sigma.
 */

double rng_gauss(double sigma);

#endif
