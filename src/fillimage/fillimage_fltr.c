/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Fill an image with a colour.
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
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "base/images_types.h"

void fillimage_fltr0(ImageBlock *b, float rgba[4])
{
	int i,j,k;
	for (j=0;j<b->height;k++)
		for (i=0;i<b->width;i++)
			for (k=0;k<b->pixelSize;k++)
			{
				unsigned char v=(unsigned char)(255*rgba[k]);
				b->pixelPtr[j*b->pitch+i*b->pixelSize+b->offset[k]]=v;
			}
	return;
}

void fillimage_fltr(image_t *img,float val[4])
{
	float *data;
	int i,j,k,m;
	int h,w,d;
	int bands;

	w=img->w;
	h=img->h;
	d=img->d;
	bands=img->bands;
	data=img->data;


	for (j=0;j<h;j++) {
		for (i=0;i<w;i++) {
			for (k=0;k<d;k++) {
				for (m=0;m<bands;m++)
				{
					data[m+bands*(i+w*(j+k*h))]=val[m];
				}
			}
		}
	}

	return;
}

void fillimage_vp_fltr(image_t *img,float val[4],ViewPort_t viewport)
{
	float *data;
	int i,j,k;
	int h,w;
	int bands;

	w=img->w;
	h=img->h;
	bands=img->bands;
	data=img->data;
	if (img->d!=1) return;/* Viewport use only defined for 2D images. */

	int xmin=viewport.xmin*img->w;
	int xmax=viewport.xmax*img->w;
	int ymin=(1.0-viewport.ymax)*img->h;
	int ymax=(1.0-viewport.ymin)*img->h;

	if (xmax<0 || xmin>img->w-1 || ymax<0 || ymin>img->h-1) return;

	if (xmin<0) xmin=0;
	if (ymin<0) ymin=0;
	if (xmax>=img->w) xmax=img->w-1;
	if (ymax>=img->h) ymax=img->h-1;
	if (xmax<xmin || ymax<ymin) return;

	for (j=ymin;j<=ymax;j++)
	{
		for (i=xmin;i<=xmax;i++)
		{
			for (k=0;k<bands;k++)
			{
				data[bands*(j*w+i)+k]=val[k];
			}
		}
	}

	return;
}
