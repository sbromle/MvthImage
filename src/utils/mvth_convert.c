/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert from one image type to another representation.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "mvth_convert.h"

/* convert an unsigned (possibly multibanded) image to a single band int
 * intensity image */
int cimg2iimg(unsigned char* src, int *dst, int w, int h, int dp, int lw)
{
	int i,j,k;
	int tmp,max;
	max=0;
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			tmp=0;
			for (k=0;k<dp;k++) tmp+=src[j*lw+i*dp+k];
			if (max<tmp) max=tmp;
			dst[j*w+i]=tmp;
		}
	}
	fprintf(stdout,"cimg2iimg: max = %d\n",max);
	return max;
}

int cimg2dimg(unsigned char* src, double *dst, int w, int h, int dp, double norm)
{
	int i,j,k;
	int lw;
	lw=w*dp;
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			for (k=0;k<dp;k++) dst[j*lw+i*dp+k]=norm*src[j*lw+i*dp+k];
		}
	}
	return 0;
}

int dimg2cimg(double* src, unsigned char *dst, int w, int h, int dp, double norm)
{
	int i,j,k;
	int lw;
	lw=w*dp;
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			for (k=0;k<dp;k++) dst[j*lw+i*dp+k]=(unsigned char)(norm*src[j*lw+i*dp+k]);
		}
	}
	return 0;
}




/* convert a signed integer image to a single band unsigned char
 * intensity image */
int iimg2cimg(int* src, unsigned char *dst, int w, int h, int lw)
{
	int i,j;
	int tmp,max,min;
	double norm;
	double bscale;

	memset(dst,0,h*lw*sizeof(unsigned char));

	/* find the maximum range */
	max=-99999999; min=999999999;
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			tmp=src[j*lw+i];
			if (tmp>max) max=tmp;
			if (tmp<min) min=tmp;
		}
	}
	fprintf(stdout,"iimg2cimg: [min,max] = [%d,%d]\n",min,max);
	if (max==min)
	{
		fprintf(stderr,"iimg2cimg: Warning! max=min=%d\n",min);
		return 0;
	}

	/* calculate the normal */
	norm=254.9/(max-min);
	bscale=-norm*min;
	/* fill in the array */
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			tmp=src[j*lw+i];
			dst[j*lw+i]=(unsigned char)(norm*tmp+bscale);
		}
	}
	return (int)norm;
}

/* convert an unsigned (possibly multibanded) image to a single band
 * short int (16 bit) intensity image */
int cimg2simg(unsigned char* src, short int *dst, int w, int h, int dp, int lw)
{
	int i,j,k;
	short int tmp,max;
	max=0;
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			tmp=0;
			for (k=0;k<dp;k++) tmp+=src[j*lw+i*dp+k];
			if (max<tmp) max=tmp;
			dst[j*w+i]=tmp;
		}
	}
	fprintf(stdout,"cimg2iimg: max = %d\n",max);
	return (int)max;
}

/* convert a signed short integer image to a single band unsigned char
 * intensity image */
int simg2cimg(short int* src, unsigned char *dst, int w, int h, int lw)
{
	int i,j;
	short int tmp,max,min;
	double norm;
	double offset;

	memset(dst,0,h*lw*sizeof(unsigned char));

	/* find the maximum range */
	max=SHRT_MIN; min=SHRT_MAX;
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			tmp=src[j*lw+i];
			if (tmp>max) max=tmp;
			if (tmp<min) min=tmp;
		}
	}
	fprintf(stdout,"simg2cimg: [min,max] = [%d,%d]\n",min,max);
	if (max==min)
	{
		fprintf(stderr,"simg2cimg: Warning! max=min=%d\n",min);
		return 0;
	}

	/* calculate the normal */
	norm=254.9/(max-min);
	offset=-norm*min;
	/* fill in the array */
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			tmp=src[j*lw+i];
			dst[j*lw+i]=(unsigned char)(norm*tmp+offset);
		}
	}
	return (int)norm;
}
