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

int rgb2grayscale_fltr(image_t *src, image_t **dst_ptr)
{
	int j,k;
	double sum;
	image_t *dst=NULL;
	int in_place=0;

	if (dst_ptr==NULL || src==NULL) return -1;

	if (*dst_ptr==NULL) {
		/* then we need to allocate a new image */
		dst=new_3d_image_t(src->w,src->h,src->d,1);
		*dst_ptr=dst;
	} else if (*dst_ptr==src) {
		in_place=1;
		dst=*dst_ptr;
	} else {
		dst = *dst_ptr;
		if (dst->w*dst->h*dst->d*dst->bands != src->w*src->h*src->d) {
			/* then we need to realloc dst */
			dst->data=(float*)malloc(src->w*src->h*src->d*sizeof(float));
		}
		dst->w=src->w;
		dst->h=src->h;
		dst->d=src->d;
		dst->bands=1;
	}

	if (src->bands==1)
	{
		if (!in_place) {
			/* then just do a straight copy */
			memcpy(dst->data,src->data,dst->w*dst->h*dst->d*sizeof(float));
		}
		/* otherwise, we have a no-op */
		return 0;
	}
	
	/* otherwise, we need to convert from RGB to grayscale */
	for (j=0;j<dst->w*dst->h*dst->d;j++) {
		double sum=0;
		for (k=0;k<src->bands;k++) {
			sum+=(double)src->data[j*src->bands+k];
		}
		dst->data[j]=sum/src->bands;
	}
	if (in_place) {
		/* then we need to resize the memory allocated for this image */
		dst->data=(float*)realloc(dst->data,dst->w*dst->h*dst->d*sizeof(float));
		dst->bands=1;
	}
	return 0;
}
