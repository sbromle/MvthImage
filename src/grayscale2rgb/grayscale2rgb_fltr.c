/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert a grayscale image to rgb.
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

int grayscale2rgb_fltr(image_t *src, image_t **dst_ptr)
{
	image_t *dst=NULL;
	int i,j;
	int in_place = 0;
	
	if (dst_ptr==NULL || src==NULL) return -1;

	if (*dst_ptr==NULL) {
		/* then we need to allocate a new image */
		dst = new_3d_image_t(src->w,src->h,src->d,3);
		*dst_ptr=dst;
	} else if (*dst_ptr==src) {
		in_place=1;
		dst=*dst_ptr;
	} else {
		dst = *dst_ptr;
		if (dst->w*dst->h*dst->d*dst->bands != src->w*src->h*src->d*3) {
			/* then we need to realloc dst */
			dst->data=(float*)malloc(src->w*src->h*src->d*3*sizeof(float));
		}
		dst->w=src->w;
		dst->h=src->h;
		dst->d=src->d;
		dst->bands=3;
	}

	if (src->bands==3) {
		/* need only copy */
		if (!in_place) {
			memcpy(dst->data,src->data,dst->w*dst->h*dst->d*sizeof(float));
		}
		return 0;
	} else if (src->bands==4) {
		/* we have an alpha channel that we should throw away */
		if (in_place) {
			/* repack without the alpha channel */
			for (i=0;i<src->w*src->h*src->d;i++) {
				dst->data[dst->bands*i+0]=src->data[src->bands*i+0];
				dst->data[dst->bands*i+1]=src->data[src->bands*i+1];
				dst->data[dst->bands*i+2]=src->data[src->bands*i+2];
			}
			dst->data=(float*)realloc(dst->data,dst->w*dst->h*dst->d*dst->bands*sizeof(float));
			return 0;
		}
	}

	/* otherwise, we need to convert grayscale to RGB */
	if (in_place) {
		/* we need to make room */
		dst->data = (float*)realloc(dst->data,dst->w*dst->h*dst->d*3*sizeof(float));
	}
	double sum;
	for (i=dst->w*dst->h*dst->d-1;i>=0;i--) {
		sum=0;
		for (j=0;j<src->bands;j++) {
			sum+=src->data[src->bands*i+j];
		}
		sum/=src->bands;
		dst->data[3*i+0]=sum;
		dst->data[3*i+1]=sum;
		dst->data[3*i+2]=sum;
	}
	if (in_place) {
		dst->bands=3;
	}

	return 0;
}
