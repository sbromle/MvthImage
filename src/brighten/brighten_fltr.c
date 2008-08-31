/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Brighten an image.
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


#include "base/images_types.h"

void brighten_fltr(image_t *wimg,double factor)
{
	float *img;
	int j;
	int h,w;
	int bands;

	w=wimg->w;
	h=wimg->h;
	bands=wimg->bands;

	img=wimg->data;

	for (j=0;j<w*h*bands;j++)
		img[j]*=factor;

	return;
}
