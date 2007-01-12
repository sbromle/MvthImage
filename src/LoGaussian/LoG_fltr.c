/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Laplacian of Gaussian filter.
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

#include "base/images_types.h"
#include "utils/xyfilters.h"

static int get_LoG_mw(float sigma)
{
	int mw;
	mw=(int)(5.0*sigma+0.5); /* 5*sigma */
#ifdef DEBUG
	fprintf(stderr,"get_LoG_mask(): mw=%d\n",mw);
#endif
	mw=(mw/2)*2+1;         /* and odd */
#ifdef DEBUG
	fprintf(stderr,"get_LoG_mask(): mw=%d\n",mw);
#endif
	return mw;
}

float g(float rsq,float one_sigma2)
{
	float ans;
	ans=0.5*M_1_PI*one_sigma2*exp(-0.5*rsq*one_sigma2);
	return ans;
}

/* assumes fmask is already preallocated */
static int get_LoG_mask(float **fmask, int mw,
		float *fnorm_in, float sigma)
{
	float fnorm;
	int j,i; /* counters */
	double x;
	float r2;
	double one_sigma2; /* one over sigma squared */

	/******* construct the filter mask *********/

	/* fill the floating point and integer masks */
	fnorm=0;
	one_sigma2=1.0/(sigma*sigma);
	for (j=0;j<mw;j++)
	{
		for (i=0;i<mw;i++)
		{
			r2=(float)((i-(mw>>1))*(i-(mw>>1))+(j-(mw>>1))*(j-(mw>>1)));
			x=g(r2,one_sigma2);
			x*=one_sigma2*(r2*one_sigma2-2);
			fmask[j][i]=x;
			fnorm+=fmask[j][i];
		}
	}
	for (i=0;i<mw*mw;i++) fmask[0][i]/=fnorm;
	
#ifdef DEBUG
	{
		float sum=0;
		fprintf(stderr,"Laplacian of Gaussian mask:\n");
		for (j=0;j<mw;j++)
		{
			for (i=0;i<mw;i++)
			{
				sum+=fmask[j][i];
				fprintf(stderr,"%d %d %f\n",i,j,fmask[j][i]);
			}
		}
		fprintf(stderr," Sum=%f\n",sum);
	}
#endif

	/********** filter mask constructed */
	*fnorm_in=fnorm;
	return 0;
}

void LoG_fltr(image_t *wimg, float sigma)
{
	float *img;
	float *orig;
	int i;
	int h,w;
	int mw;
	float fnorm;
	float **fmask=NULL;

	if (wimg->bands!=1)
	{
		fprintf(stderr,"Error: Log of Gaussian only defined for grayscale images.\n\n");
		return;
	}

	w=wimg->w;
	h=wimg->h;

	/* first get size of required mask */
	mw=get_LoG_mw(sigma);

	/* allocate space for the mask */
	fmask=(float**)malloc(mw*sizeof(float*));
	fmask[0]=(float*)malloc(mw*mw*sizeof(float));
	for (i=1;i<mw;i++) fmask[i]=fmask[0]+i*mw;

	/******* first get the filter mask *********/
	get_LoG_mask(fmask,mw,&fnorm,sigma);

	/* make space for the new image */
	img=(float*)calloc(w*h,sizeof(float));
	/* get an alias for the original image */
	orig=wimg->data;

	/* do the mask */
	do_2D_filterF(orig,img,w,h,fmask,mw,fnorm);

	/* copy result over to orig */
	memcpy(orig,img,w*h*sizeof(orig[0]));

	free(fmask[0]);
	free(fmask);
	free(img);
}
