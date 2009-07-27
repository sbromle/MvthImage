/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Corner detection.
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
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#include "base/images_types.h"
#include "utils/derivatives.h"
#include "utils/mvth_convert.h"

/* corner detector. Uses eigenvalues of derivatives
 * in a subwindow */
#define HALF_CWINDOW 2

extern int verbose;

void corner_fltr(image_t *img, float thigh, float tlow)
{
	float *orig;
	float *tmp=NULL;
	float *Jx=NULL, *Jy=NULL; /* will hold results of derivatives */
	int w,h,bands;
	int register i,j;
	double lambda1,lambda2;
	double **eigenvals=NULL;
	double Cxx,Cxy,Cyy; /* entries of the matrix C (see below) */
	double det;
	double maxlambda;

	if (verbose) fprintf(stderr,"Inside corner_fltr()\n");

	w=img->w;
	h=img->h;
	bands=img->bands;
	orig=img->data;

	tmp=(float*)calloc(w*h,sizeof(float));
	Jx=(float*)calloc(w*h,sizeof(float));
	Jy=(float*)calloc(w*h,sizeof(float));
	eigenvals=(double**)malloc(h*sizeof(double**));
	eigenvals[0]=(double*)calloc(h*w,sizeof(double));
	for (i=1;i<h;i++) eigenvals[i]=eigenvals[0]+i*w;

	if (tmp==NULL || Jx==NULL || Jy==NULL || eigenvals==NULL)
	{
		fprintf(stderr,"canny16: Error. Cannot allocate workspace.\n");
		if (tmp!=NULL) free(tmp);
		if (Jx!=NULL) free(Jx);
		if (Jy!=NULL) free(Jy);
		if (eigenvals!=NULL) {
			free(eigenvals[0]);
			free(eigenvals);
		}
		return;
	}

	if (verbose) fprintf(stderr,"Inside corner_fltr(): Converting to short.\n");
	/* convert the initial image to an image of short ints */
	memcpy(tmp,orig,w*h*bands*sizeof(float));

	if (verbose) fprintf(stderr,"Inside corner_fltr(): Computing Gradients.\n");
	/* do the gradients in each direction */
	gradX(tmp,Jx,w,h);
	gradY(tmp,Jy,w,h);

	/* so now we have the image gradient */
	/* for each position, compute the matrix:
	 *      [ sum(ExEx)  sum(ExEy) ]
	 *  C = [ sum(ExEy)  sum(EyEy) ]
	 *
	 * and get its smallest eigenvalue */

	if (verbose) fprintf(stderr,"Inside corner_fltr(): Getting Eigenvalues.\n");
	for (j=HALF_CWINDOW;j<h-HALF_CWINDOW;j++)
	{
		int jj;
		int offset;
		for (i=HALF_CWINDOW;i<w-HALF_CWINDOW;i++)
		{
			int ii;
			Cxx=Cyy=Cxy=0;
			for (jj=-HALF_CWINDOW;jj<=HALF_CWINDOW;jj++)
			{
				for (ii=-HALF_CWINDOW;ii<=HALF_CWINDOW;ii++)
				{
					offset=(j+jj)*w+i+ii;
					Cxx+=Jx[offset]*Jx[offset];
					Cxy+=Jx[offset]*Jy[offset];
					Cyy+=Jy[offset]*Jy[offset];
				}
			}
			/* get the eigen values */
			det=Cxx*Cxx-2*Cxx*Cyy+Cyy*Cyy+4*Cxy*Cxy;
			if (det<0)
			{
				if (verbose) fprintf(stderr,"-");
				continue;
			}
			lambda1=fabs(0.5*Cxx+0.5*Cyy+0.5*sqrt(det));
			lambda2=fabs(0.5*Cxx+0.5*Cyy-0.5*sqrt(det));
			/* get the smallest one */
			eigenvals[j][i]=(lambda1<lambda2)?lambda1:lambda2;
		}
	}
	if (verbose) fprintf(stderr,"\n");

	/* normalize the eigenvalues */

	if (verbose) fprintf(stderr,"Inside corner_fltr(): Normalizing Eigenvalues.\n");
	maxlambda=0;
	for (i=0;i<w*h;i++)
	{
		if (maxlambda<eigenvals[0][i]) maxlambda=eigenvals[0][i];
	}
	if (maxlambda>0)
	{
		for (i=0;i<w*h;i++)
		{
			eigenvals[0][i]/=maxlambda;
		}
	}

	if (verbose) fprintf(stderr,"Inside corner_fltr(): Converting to image.\n");
	for (i=0;i<w*h*bands;i++) orig[i]=(float)eigenvals[0][i];

	if (tmp!=NULL) free(tmp);
	if (Jx!=NULL) free(Jx);
	if (Jy!=NULL) free(Jy);
	if (eigenvals!=NULL) {
		free(eigenvals[0]);
		free(eigenvals);
	}

	return;
}
