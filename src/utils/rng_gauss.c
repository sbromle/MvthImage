/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Uniform to gaussian RNG converter.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * MVTH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License Version 3,
 * as published by the Free Software Foundation.
 *
 * MVTH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * (see the file named "COPYING"), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 *
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
