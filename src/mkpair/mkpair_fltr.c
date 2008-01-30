/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a rectangular random dot object into a stereo pair.
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
#include "utils/mvth_rng.h"
#include "base/images_types.h"
/*
#include "UTILS/images_context.h"
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

	/* make the object we will draw */
	spritet=mksprite(ow,oh);
		
	/* paste the object to the images */
	paste_fltr(&spritet,img1,x,y,rgb,1.0);
	paste_fltr(&spritet,img2,x-d,y,rgb,1.0);
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
	spr.bands=1;

	return spr;
}
