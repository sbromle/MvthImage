/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert an rgb image to rgb where channels are pseudo HSV
 * (that is, R,G,B values are reset to scaled H,S,V values
 * respectively.)
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
#include "utils/hsv_utils.h"

int rgb2hsv_fltr(image_t *img)
{
	float *ptr;
	int j;
	float h,s,v;

	if (img->d!=1) return -1;
	ptr=img->data;
	for (j=0;j<img->w*img->h;j++)
	{
		RGB2HSV(*ptr,*(ptr+1),*(ptr+2),&h,&s,&v);
		*ptr=h;
		*(ptr+1)=s;
		*(ptr+2)=v;
		ptr+=img->bands;
	}
	return 0;
}

/* convert a pseudo hsv file back to true RGB */
int hsv2rgb_fltr(image_t *img)
{
	float *ptr;
	int j;
	float h,s,v;

	if (img->d!=1) return -1;

	ptr=img->data;
	for (j=0;j<img->w*img->h;j++)
	{
		h=(*ptr);
		s=(*(ptr+1));
		v=(*(ptr+2));
		HSV2RGB(h,s,v,ptr,ptr+1,ptr+2);
		ptr+=img->bands;
	}
	return 0;
}
