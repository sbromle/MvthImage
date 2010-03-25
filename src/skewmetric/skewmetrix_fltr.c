/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 *  Project an image into an skew-symmetric view.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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
#include <string.h>
#include <math.h>
#include <assert.h>


#include "utils/getRemap.h"

extern int verbose;

void skewmetric_fltr(image_t *img,double theta)
{
	float *newimg;
	float *orig;
	int h,w;
	int bands;
	double **remaptable;

	w=img->w;
	h=img->h;
	bands=img->bands;
	if (img->d!=1) return;

	newimg=(float*)calloc(w*h*bands,sizeof(float));

	orig=img->data;
	remaptable=getSkewSymmetricMap(w,h,theta);

	/* do the remapping */
	remapImage(orig,w,h,newimg,w,h,bands,remaptable);
	free(remaptable[0]);
	free(remaptable);
	memcpy(orig,newimg,w*h*bands*sizeof(float));
	free(newimg);
}
