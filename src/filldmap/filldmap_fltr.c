/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Calculate a disparity map using Area-based stereo vision.
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


#include "utils/getColor.h"
#include "base/stereo_context_t.h"

extern int verbose;

//#define DO_ONE_ROW

void filldmap_fltr(SContext_t *sc)
{
	int h,w;
	int i,j;
	float val,oldval;
	float *d;
	int y;

	if (sc->dmap==NULL)
	{
		fprintf(stderr,"dmap does not exist\n");
	}

	d=sc->dmap;
	w=sc->w;
	h=sc->h;

	for (j=0;j<h;j++)
	{
		y=j*w;
		oldval=d[y];
		for (i=0;i<w;i++)
		{
			val=d[y+i];
			if (val<0.0)
			{
				if (oldval<0 && y>=0 && d[y-w+i]>0) oldval=d[y-w+i];
				d[y+i]=oldval;
			}
			else oldval=val;
		}
	}
}

