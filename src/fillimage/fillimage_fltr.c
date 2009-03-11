/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Fill an image with a colour.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
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
	int i,j,k;
	int h,w;
	int bands;

	w=img->w;
	h=img->h;
	bands=img->bands;
	data=img->data;


	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			for (k=0;k<bands;k++)
			{
				data[bands*(j*w+i)+k]=val[k];
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
