/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert an rgb image to grayscale.
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
#include "base/images_utils.h"

image_t * rgb2grayscale_fltr(image_t *img)
{
	image_t *ans;
	float *orig;
	float *newimg;
	int i,j,k;
	int h,w,bands;
	int offset;
	int offset2;
	int rowstride;


	if (img->bands==1)
	{
		fprintf(stderr,"Error: Image already grayscale.\n\n");
		return img;
	}

	w=img->w;
	h=img->h;
	bands=img->bands;
	rowstride=bands*w;

	ans=new_image_t(w,h,1);

	orig=img->data;
	newimg=ans->data;

	for (j=0;j<h;j++)
	{
		offset=j*w;
		offset2=j*rowstride;
		for (i=0;i<w;i++)
		{
			double sum=0;
			for (k=0;k<bands;k++) {
				sum+=(double)orig[offset2+bands*i+k];
			}
			sum/=bands;
			newimg[offset+i]=(float)sum;
		}
	}
	return ans;
}
