/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Random number generator.
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

/* interface to the Mersenne-Twister Random number generator. */

#include <stdio.h>
#include "mersennetwister.h"

static int initialised=0;

void init_rng(unsigned long seed)
{
	sgenrand(seed);
	initialised=1;
	fprintf(stderr,"Initialized RNG!\n");
}

double genrand(void)
{
	if (!initialised)
	{
		sgenrand((unsigned long)748787919ul);
		initialised=1;
		return mt_genrand();
	}
	return mt_genrand();
}
