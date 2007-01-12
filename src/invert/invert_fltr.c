/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Invert an image.
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
#include <memUtils.h>
#include <float.h>
#include "base/images_types.h"

void invert_fltr0(ImageBlock* b) {
	int i,j,k;
	int rgbsize=3; /* used to avoid flipping alpha channel */
	if (b->pixelSize<3) rgbsize=1; 
	unsigned char *c;

	for (j=0;j<b->height;j++) {
		for (i=0;i<b->width;i++) {
			for (k=0;k<rgbsize;k++) {
				c=&b->pixelPtr[j*b->pitch+i*b->pixelSize+b->offset[k]];
				*c=255-(*c);
			}
		}
	}
}

void invert_fltr(image_t *wimg)
{
	float *img;
	int j;
	int h,w;
	int bands;

	int nonscaled=0;
	float min,max;

	w=wimg->w;
	h=wimg->h;
	bands=wimg->bands;

	img=wimg->data;

	/* first, check if anything is above 1.0 or below 0 */
	min=FLT_MAX; 
	max=-FLT_MAX; 
	for (j=0;j<w*h*bands;j++) {
		if (min>img[j]) min=img[j];
		if (max<img[j]) max=img[j];
		if (img[j]<0.0f || img[j]>1.0f) {
			nonscaled=1;
			break;
		}
	}
	if (nonscaled==1) {
		for (j=0;j<w*h*bands;j++) {
			if (min!=max) img[j]=(img[j]-min)/(max-min);
			else img[j]=0.5;
		}
	}


	for (j=0;j<w*h*bands;j++)
		img[j]=1.0-img[j];

	return;
}
