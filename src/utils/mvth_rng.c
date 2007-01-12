/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Random number generator.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
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
