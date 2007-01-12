/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Moravec interest operator filter.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <memUtils.h>
#include "utils/mvth_rng.h"
#include "utils/rng_gauss.h"
#include "base/images_types.h"

void noise_fltr(image_t *img, double sigma)
{
	float *data;
	int j,k;
	//int k;
	int h,w;
	int bands;
	double rnd;

	w=img->w;
	h=img->h;
	bands=img->bands;

	/* get an alias for the original image */
	data=img->data;


	/* scan through the image */
	for (j=0;j<w*h;j++)
	{
		rnd=rng_gauss(sigma);

		for (k=0;k<bands;k++) data[bands*j+k]+=rnd;
	}

	return;
}
