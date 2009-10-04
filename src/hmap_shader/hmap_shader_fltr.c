/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Generate an image given a 2D height map array.
 *
 * Copyright (C) 2009 Samuel P. Bromley <sam@sambromley.com>
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
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "base/images_types.h"

#define DOT(x1,y1,x2,y2) ((x1)*(x2)+(y1)*(y2))

void draw_hmap_fltr(image_t *img,
		float *data, unsigned int dw, unsigned int dh,
		float angle)
{
	float *p;
	int i,j,k;
	int h,w;
	int bands;

	w=img->w;
	h=img->h;
	bands=img->bands;
	p=img->data;

	float ys=dh/(float)h;
	float xs=dw/(float)w;
	float drop=tan(angle);
	float shadow=-FLT_MAX;
	float t;
	float vx=cos(angle);
	float vy=sin(angle);

	/* scan each line and render it to the image */
	for (j=0;j<h-1;j++) {
		unsigned int y = ys*j;
		if (y>=dh) continue;
		for (i=0;i<w-1;i++) {
			unsigned int x = xs*i;
			if (x>=dw-1) continue;
			t=data[y*dw+x+1];
			if (t>shadow) shadow=t;
			float d=data[y*dw+x+1]-data[y*dw+x];
			float dn=1.0/sqrt(d*d+1);
			if (t<shadow || d<=0) {
				for (k=0;k<bands;k++) p[bands*(j*w+i)+k]=0.0;
			} else {
				/* otherwise we must color the pixel */
				float dot=DOT(-d*dn,1*dn,vx,vy); /* dot prod of two unit vectors */
				for (k=0;k<bands;k++) p[bands*(j*w+i)+k]=dot;
			}
			shadow-=drop;
		}
	}
}
