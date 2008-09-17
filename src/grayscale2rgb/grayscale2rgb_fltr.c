/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert a grayscale image to rgb.
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

image_t * grayscale2rgb_fltr(image_t *img)
{
	image_t *ans=NULL;
	float *orig;
	float *r,*g,*b;
	float *cur;
	float sum;
	int i,k;
	int h,w,bands;

	if (img->bands>=3)
	{
		fprintf(stderr,"Image already rgb.\n\n");
		return img;
	}

	w=img->w;
	h=img->h;
	bands=img->bands;

	ans=new_image_t(w,h,3);

	orig=img->data;

	r=ans->data;
	g=r+1;
	b=g+1;
	cur=orig;
	bands=img->bands;

	for (i=0;i<w*h;i++)
	{
		sum=0;
		for (k=0;k<bands;k++) {
			sum+=(*cur);
			cur++;
		}
		sum/=bands;
		*r=sum;
		*g=sum;
		*b=sum;
		r+=3;
		g+=3;
		b+=3;
	}
	
	/* ok to free, a COPY was given to undo sublayer */

	return ans;
}
