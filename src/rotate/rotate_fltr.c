/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Rotate an image.
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
#include <math.h>
#include <memUtils.h>

#include "utils/getRemap.h"
#include "utils/Tmatrix.h"

extern int verbose;

void rotate_fltr(image_t *img,
		const double alpha,
		const double beta,
		const double gamma,
		const double x, const double y, const double z)
{
	float *newimg;
	float *orig;
	int h,w;
	int bands;
	Tmatrix tm;
	double **remaptable;
	const double deg2rad = M_PI/180.0;

	w=img->w;
	h=img->h;
	bands=img->bands;

	newimg=(float*)calloc(w*h*bands,sizeof(float));

	orig=img->data;

	/* build the transform matrix */
	makeTmatrix(deg2rad*alpha,deg2rad*beta,deg2rad*gamma,x,y,z,tm);

	/* construct the address remap table */
#ifdef DEBUG
	if (verbose) fprintf(stderr,"Passing getMapping() [w,h]=[%d,%d]\n",w,h);
#endif
	remaptable=getMapping3d(w,h,tm);

	/* do the remapping */
	remapImage(orig,w,h,newimg,w,h,bands,remaptable);
	free(remaptable[0]);
	free(remaptable);
	memcpy(orig,newimg,w*h*bands*sizeof(float));
	free(newimg);
}
