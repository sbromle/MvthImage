/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert an rgb image to rgb where channels are pseudo HSV
 * (that is, R,G,B values are reset to scaled H,S,V values
 * respectively.)
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
#include "utils/hsv_utils.h"

int rgb2hsv_fltr(image_t *img)
{
	float *ptr;
	int j;
	float h,s,v;

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
