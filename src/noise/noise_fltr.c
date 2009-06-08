/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Moravec interest operator filter.
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
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
