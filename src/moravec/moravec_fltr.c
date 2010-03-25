/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Moravec interest operator filter.
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
#include <float.h>


#include "utils/mvth_convert.h"
#include "base/images_types.h"

#define MV_LEFTRIGHT
//#define MV_UPDOWN
//#define MV_FWDIAG
//#define MV_BWDIAG

extern int verbose;

void moravec_fltr(image_t *img, int winsize)
{
	float *orig;
	float *mvec[4]; /* resulting values from the morevec operator */
	float *mvecmin; /* min resulting values from the morevec operator */
	int i,j;
	//int k;
	int ii,jj;
	//int kk;
	int h,w;
	float tmp[4];

	if (img->bands!=1 || img->d!=1) return;

	w=img->w;
	h=img->h;

	/* for now, overwrite winsize with 2 */
	if (winsize!=2)
	{
		fprintf(stderr,"Overriding winsize=%d to be winsize=2\n",winsize);
		winsize=2;
	}

	/* make space for the new image */
	if (verbose) fprintf(stderr,"About to make a new image.\n");
	for (j=0;j<4;j++) mvec[j]=(float*)calloc(h*w,sizeof(float));

	mvecmin=(float*)malloc(h*w*sizeof(float));
	for (j=0;j<w*h;j++) mvecmin[j]=FLT_MAX;
	/* get an alias for the original image */
	orig=img->data;

	/* scan through the image
	 * Accumulate sums as we scan */

	/* scan over each location that a difference is evaluated */
	for (j=0;j<h-1;j++) /* must be h-1 since we will step to h-1 */
	{
		int minj,maxj;

		minj=j-winsize;
		maxj=j+winsize;
		if (minj<0) minj=0;
		if (maxj>h-1) maxj=h-1;

		for (i=0;i<w-1;i++)
		{
			int mini,maxi;
			mini=i-winsize;
			maxi=i+winsize;
			if (mini<0) mini=0;
			if (maxi>w-1) maxi=w-1;

			/* get the difference */
#ifdef MV_LEFTRIGHT
			tmp[0]=orig[j*w+i]-orig[j*w+i+1];
			tmp[0]*=tmp[0];
#endif
#ifdef MV_UPDOWN
			tmp[1]=orig[j*w+i]-orig[(j+1)*w+i];
			tmp[1]*=tmp[1];
#endif
#ifdef MV_BWDIAG
			tmp[2]=orig[j*w+i]-orig[(j+1)*w+i+1];
			tmp[2]*=tmp[2];
#endif
#ifdef MV_FWDIAG
			tmp[3]=orig[j*w+i+1]-orig[(j+1)*w+i];
			tmp[3]*=tmp[3];
#endif

			/* and assign it to all locations where it could contribute */
			for (jj=minj; jj<maxj; jj++)
			{
				for (ii=mini; ii<maxi; ii++)
				{
#ifdef MV_LEFTRIGHT
					mvec[0][jj*w+ii]+=tmp[0];
#endif
#ifdef MV_UPDOWN
					mvec[1][jj*w+ii]+=tmp[1];
#endif
#ifdef MV_BWDIAG
					mvec[2][jj*w+ii]+=tmp[2];
#endif
#ifdef MV_FWDIAG
					mvec[3][jj*w+ii]+=tmp[3];
#endif
				}
			}
		}
	}

	/* ok, copy over the values and to store minimums */
	for (j=0;j<w*h;j++)
	{
#ifdef MV_LEFTRIGHT
		if (mvec[0][j]<mvecmin[j]) mvecmin[j]=mvec[0][j];
#endif
#ifdef MV_UPDOWN
		if (mvec[1][j]<mvecmin[j]) mvecmin[j]=mvec[1][j];
#endif
#ifdef MV_BWDIAG
		if (mvec[2][j]<mvecmin[j]) mvecmin[j]=mvec[2][j];
#endif
#ifdef MV_FWDIAG
		if (mvec[3][j]<mvecmin[j]) mvecmin[j]=mvec[3][j];
#endif
	}

	/* make all the crazy large things zero */
	for (j=0;j<w*h;j++) if (mvecmin[j]==FLT_MAX) mvecmin[j]=0;

	memcpy(orig,mvecmin,w*h*sizeof(float));


	for (j=0;j<4;j++) free(mvec[j]);
	free(mvecmin);
}
