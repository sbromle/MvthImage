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
