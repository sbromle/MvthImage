/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a disparity map.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
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
