/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Rectify an image.
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
#include "base/images_types.h"
#include "utils/getRemap.h"

void rectify_fltr(image_t *img, double fargs[9])
{
	float *nimg;
	float *orig;
	double **remaptable;
	R3Dmatrix_t rm;
	int h,w;
	int i;
	int bands;

	w=img->w;
	h=img->h;
	bands=img->bands;

	if (img->d!=1) return;

	nimg=(float*)calloc(w*h*bands,sizeof(float));

	orig=img->data;

	/* fill in the rm matrix */
	for (i=0;i<9;i++) rm[i]=fargs[i];
	//rm=(R3Dmatrix_t)fargs;	

	/* get the remap matrix */
	remaptable=getUnitMapping(w,h,rm);

	/* do the remapping */
	remapImage(orig,w,h,nimg,w,h,bands,remaptable);

	free(remaptable[0]);
	free(remaptable);

	memcpy(orig,nimg,w*h*bands*sizeof(float));
	free(nimg);
}
