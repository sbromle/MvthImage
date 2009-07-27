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
