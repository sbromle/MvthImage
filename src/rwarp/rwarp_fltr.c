/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Warp one image to another using radial coordinate transformation.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sbromley@guigne.com>
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
