/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 *  Project an image into an skew-symmetric view.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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
