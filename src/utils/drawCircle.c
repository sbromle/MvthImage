/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a circle.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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
#include <math.h>

#include "base/images_context.h"

void drawCircle(image_t *img,int cx, int cy, int r, float rgb[4])
{
	int x,y;
	int k;
	double angle, inc;
	int w,h,b;
	float *data=NULL;

	if (r<=0) return;

	if (img==NULL) return;

	w=img->w;
	h=img->h;
	b=img->bands;
	data=img->data;

	/* get the increment we need to not skip any pixels */
	inc=2.0/(double)(r*r);

	for (angle=0;angle<2*M_PI;angle+=inc)
	{
		x=(int)(r*cos(angle)+cx+0.5);
		y=(int)(r*sin(angle)+cy+0.5);
		if (y>=0 && y<h && x>=0 && x<w)
		for (k=0;k<b;k++) data[b*(y*w+x)+k]=rgb[k];
	}

	return;
}
