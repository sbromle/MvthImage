/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Warp one image to another using radial coordinate transformation.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sbromley@guigne.com>
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
#include "base/images_types.h"
#include "utils/getRemap.h"

int rwarp_fltr(image_t *src, image_t *dst, int ycenter)
{
	double **addresses=NULL;
	if (src->bands!=dst->bands) return -1;
	addresses=getRadialMap(src->w,src->h,dst->w,dst->h,ycenter);
	remapImage(src->data,src->w,src->h,
			dst->data,dst->w,dst->h,
			dst->bands,
			addresses);
	free(addresses[0]);
	free(addresses);
	return 0;
}
