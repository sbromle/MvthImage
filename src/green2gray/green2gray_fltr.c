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
 * (see the file named "COPYING), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "base/images_types.h"

void green2gray_fltr(image_t *img)
{
	int j;
	int h,w,d;

	w=img->w;
	h=img->h;
	d=img->d;

	if (img->bands>=3) {
		for (j=0;j<w*h*d;j++) {
			img->data[img->bands*j]=img->data[img->bands*j+1];
			img->data[img->bands*j+2]=img->data[img->bands*j+1];
		}
	}
	return;
}
