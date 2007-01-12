/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Generic filter routines.
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
#include "xyfilters.h"

int do_x_filter(float *src, float *dst,
		int w, int h,
		float *mask, int mw, float norm)
{
	int i,j,s;
	double tmp;

	if (src==NULL || dst==NULL) return 0;

	/* clear dst */
	memset(dst,0,h*w*sizeof(float));

	/* loop over rows */
	for (j=0;j<h;j++)
	{
		/* loop over columns */
		for (i=mw/2;i<w-mw/2;i++)
		{
			/* loop over mask width */
			tmp=0;
			for (s=0;s<mw;s++)
			{
				tmp+=mask[s]*src[j*w+(i-mw/2+s)];
			}
			dst[j*w+i]=(float)(tmp/norm);
		}
	}
	return 1;
}

int do_x_filter32(int *src, int *dst,
		int w, int h,
		int *mask, int mw, int norm)
{
	int i,j,s;
	int tmp;

	if (src==NULL || dst==NULL) return 0;

	/* clear dst */
	memset(dst,0,h*w*sizeof(int));

	/* loop over rows */
	for (j=0;j<h;j++)
	{
		/* loop over columns */
		for (i=mw/2;i<w-mw/2;i++)
		{
			/* loop over mask width */
			tmp=0;
			for (s=0;s<mw;s++)
			{
				tmp+=mask[s]*src[j*w+(i-mw/2+s)];
			}
			dst[j*w+i]=(int)(tmp/norm);
		}
	}
	return 1;
}

int do_x_filter16(short *src, short *dst,
		int w, int h,
		short *mask, int mw, int norm)
{
	int i,j,s;
	int tmp;

	if (src==NULL || dst==NULL) return 0;

	/* clear dst */
	memset(dst,0,h*w*sizeof(short));

	/* loop over rows */
	for (j=0;j<h;j++)
	{
		/* loop over columns */
		for (i=mw/2;i<w-mw/2;i++)
		{
			/* loop over mask width */
			tmp=0;
			for (s=0;s<mw;s++)
			{
				tmp+=mask[s]*src[j*w+(i-mw/2+s)];
			}
			dst[j*w+i]=(short)(tmp/norm);
		}
	}
	return 1;
}

int do_y_filter(float *src, float *dst,
		int w, int h, 
		float *mask, int mw, float norm)
{
	int i,j,s;
	double tmp;

	if (src==NULL || dst==NULL) return 0;

	/* clear dst */
	memset(dst,0,h*w*sizeof(float));

	/* loop over columns */
	for (i=0;i<w;i++)
	{
		/* loop over rows */
		for (j=mw/2;j<h-mw/2;j++)
		{
			/* loop over mask width */
			tmp=0;
			for (s=0;s<mw;s++)
			{
				tmp+=mask[s]*src[(j-mw/2+s)*w+i];
			}
			dst[j*w+i]=(float)(tmp/norm);
		}
	}
	return 1;
}

int do_y_filter32(int *src, int *dst,
		int w, int h, 
		int *mask, int mw, int norm)
{
	int i,j,s;
	int tmp;

	if (src==NULL || dst==NULL) return 0;

	/* clear dst */
	memset(dst,0,h*w*sizeof(int));

	/* loop over columns */
	for (i=0;i<w;i++)
	{
		/* loop over rows */
		for (j=mw/2;j<h-mw/2;j++)
		{
			/* loop over mask width */
			tmp=0;
			for (s=0;s<mw;s++)
			{
				tmp+=mask[s]*src[(j-mw/2+s)*w+i];
			}
			dst[j*w+i]=(int)(tmp/norm);
		}
	}
	return 1;
}

int do_y_filter16(short *src, short *dst,
		int w, int h, 
		short *mask, int mw, int norm)
{
	int i,j,s;
	int tmp;

	if (src==NULL || dst==NULL) return 0;

	/* clear dst */
	memset(dst,0,h*w*sizeof(short));

	/* loop over columns */
	for (i=0;i<w;i++)
	{
		/* loop over rows */
		for (j=mw/2;j<h-mw/2;j++)
		{
			/* loop over mask width */
			tmp=0;
			for (s=0;s<mw;s++)
			{
				tmp+=mask[s]*src[(j-mw/2+s)*w+i];
			}
			dst[j*w+i]=(short)(tmp/norm);
		}
	}
	return 1;
}

/* note, no guarantee yet that values will fit in unsigned char */
void do_2D_filterF(float *src,
		float *dst,
		int w, int h, float **fmask, int mw, float fnorm)
{

	int i,j;
	int ii,jj;
	float sum;
	long offset;

	for (jj=0;jj<h-mw;jj++)
	{
		for (ii=0;ii<w-mw;ii++)
		{
			sum=0;
			offset=(jj*w+ii);
			/* do the mask */
			for (j=0;j<mw;j++)
			{
				for (i=0;i<mw;i++)
				{
					sum+=fmask[j][i]*src[offset+j*w+i];
				}
			}

			dst[offset+w*(mw>>1)+(mw>>1)]=(unsigned char)(0.5*sum+0.5); /* fmask already normalized */
			//dst[offset]=(unsigned char)(sum/fnorm);
		}
	}
	return;
}


