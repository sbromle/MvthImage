/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert a disparity into a RGB color representation.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "base/stereo_context_t.h"

/* lookup table for logarithms of disparities. */
unsigned char logtable[64];

void init_dmap_lut(SContext_t *sc)
{
	int i;
	float scale;
	static int oldrange=0;

	if (oldrange!=0 && sc->range==oldrange) return;

	if (sc->range<=1)
	{
		fprintf(stderr,"Initialization of dmap lookup table failed (range<=1)\n");
		return;
	}
	else if (sc->range>64)
	{
		fprintf(stderr,"init_dmap_lut(): Not enough room in lookup table. recompile.\n");
		return;
	}

	oldrange=sc->range;

	scale=1.0f/log10f((float)sc->range);

	for (i=0;i<sc->range;i++)
	{
		logtable[i]=(float)(log10f((float)i+1)*scale);
	}
	return;
}


void getDisparityColor(float d, float rgb[4])
{
	if (d<=0)
	{
		rgb[0]=rgb[1]=rgb[2]=rgb[3]=0;
		return;
	}

	rgb[0]=0;
	rgb[1]=(float)(logtable[(int)d]);
	rgb[2]=0;

	return;
}

