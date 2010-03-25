/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Brighten an image.
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


#include "base/images_types.h"

void brighten_fltr(image_t *wimg,double factor)
{
	float *img;
	int j;
	int h,w;
	int bands;

	if (wimg->d!=1) return;
	w=wimg->w;
	h=wimg->h;
	bands=wimg->bands;

	img=wimg->data;

	for (j=0;j<w*h*bands;j++)
		img[j]*=factor;

	return;
}
