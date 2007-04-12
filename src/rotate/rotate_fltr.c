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

/* rotate an image counter clockwise by index*90 degrees. */
void rotate_90_fltr(image_t *img, int index) 
{
	float *newimg;
	float *orig;
	int nh,nw;
	int bands;
	int i,j,k;

	index=index%4;
	if (index==0) return;
	if (index<0) index+=4;


	newimg=(float*)calloc(img->w*img->h*img->bands,sizeof(float));
	orig=img->data;

	bands=img->bands;

	if (index==1) {
		nh=img->w;
		nw=img->h;
		/* 90 degrees counter-clockwise */
		fprintf(stderr,"90 counter-Clockwise\n");
		for (j=0;j<nh;j++)
		{
			for (i=0;i<nw;i++)
			{
				for (k=0;k<bands;k++)
				{
					newimg[bands*(j*nw+i)+k]
						= orig[bands*(i*img->w+(img->w-1-j))+k];
				}
			}
		}
	} else if (index==2) {
		nh=img->h;
		nw=img->w;
		/* 180 degrees */
		for (j=0;j<nh;j++)
		{
			for (i=0;i<nw;i++)
			{
				for (k=0;k<bands;k++)
				{
					newimg[bands*(j*nw+i)+k]
						= orig[bands*((img->h-1-j)*img->w+(img->w-1-i))+k];
				}
			}
		}
	} else {
		/* 270 degrees counter-clockwise (90 degrees clockwise) */
		nh=img->w;
		nw=img->h;
		for (j=0;j<nh;j++)
		{
			for (i=0;i<nw;i++)
			{
				for (k=0;k<bands;k++)
				{
					newimg[bands*(j*nw+i)+k]
						= orig[bands*((img->h-1-i)*img->w+j)+k];
				}
			}
		}
	}

	memcpy(orig,newimg,nw*nh*bands*sizeof(float));
	free(newimg);
	img->w=nw;
	img->h=nh;
	return;
}

/* rotate an image mask counter clockwise by index*90 degrees. */
void rotatemask_90_fltr(unsigned char *mask, int w, int h, int index) 
{
	unsigned char *newmask;
	int nh,nw;
	int i,j;

	index=index%4;
	if (index==0) return;
	if (index<0) index+=4;

	newmask=(unsigned char*)calloc(w*h,sizeof(unsigned char));

	if (index==1) {
		nh=w;
		nw=h;
		/* 90 degrees counter-clockwise */
		for (j=0;j<nh;j++)
		{
			for (i=0;i<nw;i++)
			{
				newmask[j*nw+i] = mask[i*w+w-1-j];
			}
		}
	} else if (index==2) {
		nh=h;
		nw=w;
		/* 180 degrees */
		for (j=0;j<nh;j++)
		{
			for (i=0;i<nw;i++)
			{
				newmask[j*nw+i] = mask[(h-1-j)*w+w-1-i];
			}
		}
	} else {
		/* 270 degrees counter-clockwise (90 degrees clockwise) */
		nh=w;
		nw=h;
		for (j=0;j<nh;j++)
		{
			for (i=0;i<nw;i++)
			{
				newmask[j*nw+i] = mask[(h-1-i)*w+j];
			}
		}
	}

	memcpy(mask,newmask,w*h*sizeof(unsigned char));
	free(newmask);
	return;
}
