/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert an rgb image to grayscale.
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
