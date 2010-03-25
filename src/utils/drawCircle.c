/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a circle.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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

#include "base/images_types.h"

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
	if (img->d!=1) return;
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

void fillCircle(image_t *img,int cx, int cy, int r, float rgb[4])
{
	int x,y;
	int k;
	int w,h,b;
	float *data=NULL;
	int i,j;

	if (r<=0) return;

	if (img==NULL) return;
	if (img->d!=1) return;

	w=img->w;
	h=img->h;
	b=img->bands;
	data=img->data;

	for (j=-r;j<=r;j++) {
		y=cy+j;
		if (y<0 || y>h-1) continue;
		for (i=-r;i<=r;i++) {
			x=cx+i;
			if (x<0 || x>w-1) continue;
			if (j*j+i*i<=r*r) {
				for (k=0;k<b;k++) data[b*(y*w+x)+k]=rgb[k];
			}
		}
	}

	return;
}
