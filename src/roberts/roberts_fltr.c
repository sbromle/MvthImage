/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Roberts edge detection.
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
#include <math.h>
#include <limits.h>
#include <float.h>

#include "base/images_types.h"

extern int verbose;

void roberts_fltr(image_t *img)
{
	int mask1[2][2]={{1,-1},{-1,1}};
	int mask2[2][2]={{-1,1},{1,-1}};
	int mw=2;
	int w,h;
	//float norm=2.0*sqrt(2.0);/* no longer need to normalize */
	float *I1,*I2;
	float *tmp;
	float max,min;
	int i,j,m1,m2;
	int pos,pos2;
	float *orig;

	orig=img->data;
	w=img->w;
	h=img->h;

	/* allocate the separate images */
	I1=(float*)calloc(h*w,sizeof(float));
	I2=(float*)calloc(h*w,sizeof(float));
	tmp=(float*)calloc(h*w,sizeof(float));

	for (j=mw/2;j<h-mw/2;j++)/* rows */
	{
		for (i=mw/2;i<w-mw/2;i++)/*columns */
		{
			pos=j*w+i;
			I1[pos]=0;
			I2[pos]=0;
			for (m1=0;m1<mw;m1++)/* mask rows */
			{
				for (m2=0;m2<mw;m2++)/* mask cols */
				{
					pos2=(j+m1-mw/2)*w+(i+m2-mw/2);
					/* accumulate contributions */
					I1[pos]+=mask1[m1][m2]*orig[pos2];
					I2[pos]+=mask2[m1][m2]*orig[pos2];
				}
			}
		}
	}
	/* ok, so now we have individual gradient images */
	/* calculate gradient magnitudes and normalise to 0-255*/
	/* this filter has maximums at 2sqrt(2)*255, so must
	 * divide by 2sqrt(2)=2.8284271247 */
	min=FLT_MAX;
	max=-FLT_MAX;
	for (j=0;j<h*w;j++)
	{
		tmp[j]=(float)(sqrt((double)I1[j]*I1[j]+I2[j]*I2[j]));
		if (max<tmp[j]) max=tmp[j];
		if (min>tmp[j]) min=tmp[j];
	}
	if (verbose) fprintf(stderr,"roberts_fltr(): Found [min,max]=[%g,%g]\n",min,max);
	/* then rebuild image using a normlize tmp */
	for (i=0;i<w*h;i++)
		orig[i]=(float)(((tmp[i]-min)/(max-min)));
	
	free(I1);
	free(I2);
	free(tmp);
	return;
}
