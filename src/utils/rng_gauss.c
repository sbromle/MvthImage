/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Uniform to gaussian RNG converter.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

/* Return a number chosen from a gaussian distribution with stddev sigma,
 * and zero mean.
 */
#include <stdio.h>
#include <math.h>
#include "mvth_rng.h"

double
rng_gauss(const double sigma)
{
	double x, y, r2;

	x=2.0*genrand()-1.0;
	/* get a y until (x,y) is inside unit circle */
	do {
		y=2.0*genrand()-1.0;
		r2=x*x+y*y;
	} while (r2>1.0);

	if (r2==0.0) return 0; /* very unlikely so we'll handle it trivially */

	return sigma*y*sqrt(-2.0*log(r2)/r2);
}
