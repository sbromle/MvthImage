/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a rectangular random dot object into a stereo pair.
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
#include "utils/mvth_rng.h"
#include "base/images_types.h"
/*
#include "UTILS/rng.h"
*/
#include "base/stereo_context_t.h"

extern void paste_fltr(image_t *src, image_t *dst,int xoff, int yoff, float rgb[6], float alpha);

extern int verbose;

image_t mksprite(int ow, int oh);

void mkpair_fltr(image_t *img1, image_t *img2,
		int d, int x, int y, int ow, int oh)
{
	image_t spritet;
	float rgb[6]={0,0,0,0,0,0};
	if (img1->d!=1 || img2->d!=1) return;

	/* make the object we will draw */
	spritet=mksprite(ow,oh);
		
	/* paste the object to the images */
	paste_fltr(&spritet,img1,x,y,rgb,1.0);
	paste_fltr(&spritet,img2,x-d,y,rgb,1.0);
	if (spritet.data!=NULL) free(spritet.data);
}

image_t mksprite(int ow, int oh)
{
	image_t spr;
	float *data;
	int i;

	data=spr.data=(float *)malloc(ow*oh*sizeof(float));
	for (i=0;i<oh*ow;i++)
	{
		data[i]=(float)(0.5+genrand()*0.5);
	}
	/* draw a dark boundary around it */
	for (i=0;i<ow;i++)
		data[i]=data[ow*(oh-1)+i]=1.0/255.0;
	for (i=0;i<oh;i++)
		data[ow*i]=data[ow*(i+1)-1]=1.0/255;

	spr.w=ow;
	spr.h=oh;
	spr.d=1;
	spr.bands=1;

	return spr;
}
