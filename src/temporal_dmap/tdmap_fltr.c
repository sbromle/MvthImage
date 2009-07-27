/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a disparity map.
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

typedef struct TDMAP_s {
	SContext_t sc;
	int *age;
} TDmap_t;

static TDmap_t tdmap;

/* add the current dmap info to the temporal one */
SContext_t tdmap_fltr(SContext_t *sc, int age)
{
	int j;
	int w,h;
	float *dmap;
	float *t;
	int *a;

	/* confirm that the wimg can hold the dmap */
	if (sc->dmap==NULL)
	{
		fprintf(stderr,"dmap is NULL in stereo_context!\n");
		return *sc;
	}

	w=sc->w;
	h=sc->h;
	dmap=sc->dmap;

	if (tdmap.sc.w==0 || tdmap.sc.h==0 ||
			tdmap.sc.w!=sc->w || tdmap.sc.h!=sc->h || tdmap.sc.dmap==NULL
			|| tdmap.age==NULL)
	{
		if (tdmap.sc.dmap!=NULL) free(tdmap.sc.dmap);
		tdmap.sc=*sc;
		tdmap.sc.dmap=(float*)malloc(w*h*sizeof(float));
		if (tdmap.age!=NULL) free(tdmap.age);
		tdmap.age=(int*)malloc(w*h*sizeof(int));
		for (j=0;j<w*h;j++)
		{
			tdmap.sc.dmap[j]=-1;
			tdmap.age[j]=-1;
		}
	}

	t=tdmap.sc.dmap;
	a=tdmap.age;
	/* ok, so now everything is set up and the correct size. */

	/* copy over the depth map */
	for (j=0;j<w*h;j++)
	{
		if (*a<0)
		{
			/* then this was a bad one from before */
			/* the dmap can be changed elsewhere, but the age
			 * is not updated elsewhere. Therefore, by setting
			 * expired points to have -1 age (done below) and then
			 * incrementing, we mark all invalid points as
			 * age 0 the next time around. While valid
			 * points will have ages >=1.
			 * We should reset those invalid points that have
			 * been filled in elsewhere (perhaps with linfill_dmap
			 * with invalid disparity values. */
			*t=-1;
			*a=-1;
		}

		if (*dmap>=0)
		{
			*t=*dmap;
			*a=0;
		}
		else
		{
			(*a)++;
			if (*a>age)
			{
				*t=-1;
				*a=-1;
			}
		}
		a++;
		t++;
		dmap++;
	}
	return tdmap.sc;
}
