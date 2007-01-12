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
#include <memUtils.h>

#include "base/images_types.h"

void green2gray_fltr(image_t *img)
{
	int j;
	int h,w;

	if (img->bands!=3)
	{
		fprintf(stderr,"Error: WImage already grayscale.\n\n");
		return;
	}

	w=img->w;
	h=img->h;

	for (j=0;j<w*h;j++)
	{
		img->data[3*j]=img->data[3*j+1];
		img->data[3*j+2]=img->data[3*j+1];
	}
	return;
}
