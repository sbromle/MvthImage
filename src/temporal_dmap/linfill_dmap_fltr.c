/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Fill in a sparse disparity map using linear interpolation.
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
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>


#include "base/stereo_context_t.h"

extern int verbose;

/* add the current dmap info to the temporal one */
int linfill_dmap_fltr(SContext_t *sc)
{
	int i,j;
	int ii;
	int w,h;
	float *dmap;
	int a,b;
	int n;

	/* confirm that the wimg can hold the dmap */
	if (sc->dmap==NULL)
	{
		fprintf(stderr,"dmap is NULL in stereo_context!\n");
		return -1;
	}

	w=sc->w;
	h=sc->h;
	dmap=sc->dmap;

	n=0;
	for (j=0;j<h;j++)
	{
		/* skip initial problem sights */
		i=sc->range;
		while (dmap[j*w+i]==-1 && i<w) i++;
		if (i==w) continue;
		for (;i<w;i++)
		{
			if (dmap[j*w+i]==-1)
			{
				a=i; /* save the first problem sight */
				while (i<w && dmap[j*w+i]<0) /* find the next valid match */
					i++;
				if (i==w) b=w-1;
				else b=i;
				n+=(b-a);
				/* fill between a and b with value at b */
				for (ii=a;ii<b;ii++)
				{
					dmap[j*w+ii]=dmap[j*w+b];
				}
			}
		}
	}

	if (verbose) fprintf(stderr,"Filled %d invalid points.\n",n);
	return n;
}
