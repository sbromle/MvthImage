/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Gaussian filter an image.
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


#include "base/images_types.h"

float get_gaussian_mask(float **mask_in, int *mw_in,
		float *norm_in, float sigma)
{
	float norm; /* mask normalisation */
	int mw;     /* mask width */
	float *mask; /* floating point representation of mask */
	int i; /* counters */
	double x;

	/******* construct the filter mask *********/
	/* get the mask width */
	mw=(int)(5.0*sigma+0.5); /* 5*sigma */
#ifdef DEBUG
	fprintf(stderr,"get_gaussian_mask(): mw=%d\n",mw);
#endif
	mw=(mw/2)*2+1;         /* and odd */
#ifdef DEBUG
	fprintf(stderr,"get_gaussian_mask(): mw=%d\n",mw);
#endif
		/* 5*sigma and odd */
	/* allocate space for the mask */
	mask=(float*)malloc(mw*sizeof(float));
	/* fill the floating point and integer masks */
	norm=0;
	for (i=0;i<mw;i++)
	{
		x=(double)((i-mw/2.0+0.5)*(i-mw/2.0+0.5))/(2*sigma*sigma);
		mask[i]=exp(-x);
		norm+=mask[i];
	}
	for (i=0;i<mw;i++) mask[i]/=norm;
	
#ifdef VERBOSE
	float sum=0;
	fprintf(stderr,"Gaussian mask:\n");
	for (i=0;i<mw;i++)
	{
		sum+=mask[i];
		fprintf(stderr,"%f ",mask[i]);
	}
	fprintf(stderr," Sum=%f\n",sum);
#endif

	

	/********** filter mask constructed */
#ifdef DEBUG
	fprintf(stderr,"mask: ");
	for (i=0;i<mw;i++) fprintf(stderr,"%d ",mask[i]);
	fprintf(stderr,"\n");
#endif
	*mask_in=mask;
	*norm_in=norm;
	*mw_in=mw;
	return norm;
}

void gaussian_fltr(image_t *img, float sigma)
{
	float *nimg;
	float *orig;
	int i,j,k;
	int bands;
	int kk;
	int h,w;
	int mw;
	float norm;
	float tmpsum;
	float *fmask=NULL;

#ifdef DEBUG
	fprintf(stderr,"Inside gaussian_fltr\n");
#endif

	if (img==NULL || img->data==NULL)
	{
		fprintf(stderr,"Something NULL with img!\n");
		return;
	}
	if (img->d!=1)
	{
		fprintf(stderr,"gaussian_fltr only supports 2D images!\n");
		return;
	}

#ifdef NO_MULTI_BAND
	if (wimg->bands!=1)
	{
		fprintf(stderr,"Error: Gaussian only defined for grayscale images.\n\n");
		return;
	}
#endif

	w=img->w;
	h=img->h;
	bands=img->bands;

	/******* first get the filter mask *********/
#ifdef DEBUG
	fprintf(stderr,"About to get the gaussian mask.\n");
#endif
	get_gaussian_mask(&fmask,&mw,&norm,sigma);

	/* make space for the new image */
#ifdef DEBUG
	fprintf(stderr,"About to make a new image.\n");
#endif
	nimg=(float *)calloc(w*h*bands,sizeof(float));
	/* get an alias for the original image */
	orig=img->data;

	/* for each band do the smooth */
	for (kk=0;kk<bands;kk++)
	{
		/* for each pixel in image, do the mask */
		/* do x direction */
		for (j=0;j<h;j++)
		{
			for (i=mw/2;i<w-mw/2;i++)
			{
				tmpsum=0;
				for (k=-mw/2;k<=mw/2;k++) tmpsum+=fmask[k+mw/2]*orig[bands*(w*j+i+k)+kk];
				nimg[bands*(w*j+i)+kk]=(float)tmpsum;
			}
		}
		/* fill in missing data at borders */
		for (j=0;j<h;j++)
		{
			for (i=0;i<mw/2;i++) nimg[bands*(w*j+i)+kk]=orig[bands*(w*j+i)+kk];
			for (i=w-mw/2;i<w;i++) nimg[bands*(w*j+i)+kk]=orig[bands*(w*j+i)+kk];
		}
	}
	/* and clear out orig */
	memset(orig,0,bands*w*h*sizeof(orig[0]));

	/* for each band */
	for (kk=0;kk<bands;kk++)
	{
		/* do y direction */
		for (j=mw/2;j<h-mw/2;j++)
		{
			for (i=0;i<w;i++)
			{
				tmpsum=0;
				for (k=-mw/2;k<=mw/2;k++) tmpsum+=fmask[k+mw/2]*nimg[bands*(w*(j+k)+i)+kk];
				orig[bands*(w*j+i)+kk]=(float)tmpsum;
			}
		}
	}

	free(fmask);
	free(nimg);
}
