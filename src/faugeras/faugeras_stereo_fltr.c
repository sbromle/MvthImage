/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Calculate a disparity map using Area-based stereo vision.
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


#include "utils/getMatch.h"
#include "utils/getColor.h"
#include "base/images_types.h"
#include "base/stereo_context_t.h"

extern int verbose;

//#define DMAP_ERROR_TOL 2.0
static const float DMAP_ERROR_TOL=0.5F;

//#define DO_ONE_ROW

static void getDisparities(float *origL,
		float *origR,
		int w, int h, SContext_t *sc);

void faugeras_stereo_fltr(image_t *img1, image_t *img2,
		SContext_t *sc)
{
	int h,w;

	/* all error testing now done in astereo_cmd.c */
	if (img1->d!=1 || img2->d!=1) return;

	w=img1->w;
	h=img1->h;

	/* save the stereo context */
	if (sc->dmap==NULL ||
			sc->w*sc->h!=w*h)
	{
		if (sc->dmap!=NULL)
			free(sc->dmap);
		sc->dmap=(float*)calloc(w*h,sizeof(float));
	}
	sc->w=w;
	sc->h=h;

	/* zero out the image */
	//memset(wimgD->img[0],0,w*h*3*sizeof(char));

	/* get handles on the images */

	/* do the stereo */
	getDisparities(img1->data, img2->data, w, h, sc);

	/* draw the dmap */
	//dmap_draw_fltr(img3,stereo_context, -1.0);
}

/* given a 2D array A, do block summations over
 * blocks of size c. Return result as floats in B.
 * Workspace must be of size 2*w*h.
 */
void blocksum(float *A, float *workspace, int w, int h, int c, float *B)
{
	int i,j;
	int y;
	int size;

	float *Q;
	float *N;
	size=w*h;
	memset(workspace,0,2*size*sizeof(float));
	Q=workspace;
	N=workspace+size;

	for (j=0;j<h;j++)
	{
		y=j*w;
		for (i=0;i<c;i++)
		{
			Q[y]+=A[y+i]; /* initial column of Q */
		}
	}
	/* Do remaining columns of Q */
	for (j=0;j<h;j++)
	{
		y=j*w;
		for (i=1;i<w;i++)
		{
			if (y+i+c<size)
				Q[y+i]=Q[y+i-1] + A[y+i+c-1] - A[y+i-1];
			else
				Q[y+i]=Q[y+i-1] - A[y+i-1];
		}
	}
	/* do rows of N */
	for (i=0;i<w;i++)
	{
		for (j=0;j<c;j++)
		{
			N[i]+=Q[j*w+i]; /* initial row of N */
		}
	}

	/* Do remaining rows of N */
	for (i=0;i<w;i++)
	{
		for (j=1;j<h;j++)
		{
			if ((j+c)*w+i<size)
				N[j*w+i]=N[(j-1)*w+i] + Q[(j+c-1)*w+i] - Q[(j-1)*w+i];
			else
				N[j*w+i]=N[(j-1)*w+i] - Q[(j-1)*w+i];
		}
	}

	for (i=0;i<size;i++)
	{
		B[i]=(float)N[i];
	}
	return;
}

/* do blocksums for A, and B.
 * workspace must be 4*w*h.
 * return results in RA, and RB.
 */
void blocksum2(float *A, float *B, float *workspace,
		int w, int h, int c,
		float *RA,float *RB)
{
	int i,j;
	int y;
	int size;

	float *QA;
	float *NA;
	float *QB;
	float *NB;
	size=w*h;
	memset(workspace,0,4*size*sizeof(float));
	QA=workspace;
	NA=workspace+size;
	QB=workspace+2*size;
	NB=workspace+3*size;

	for (j=0;j<h;j++)
	{
		y=j*w;
		for (i=0;i<c;i++)
		{
			QA[y]+=A[y+i]; /* initial column of Q */
			QB[y]+=B[y+i]; /* initial column of Q */
		}
	}
	/* Do remaining columns of Q */
	for (j=0;j<h;j++)
	{
		y=j*w;
		for (i=1;i<w;i++)
		{
			if (y+i+c<size)
			{
				QA[y+i]=QA[y+i-1] + A[y+i+c-1] - A[y+i-1];
				QB[y+i]=QB[y+i-1] + B[y+i+c-1] - B[y+i-1];
			}
			else
			{
				QA[y+i]=QA[y+i-1] - A[y+i-1];
				QB[y+i]=QB[y+i-1] - B[y+i-1];
			}
		}
	}
	/* do rows of N */
	for (i=0;i<w;i++)
	{
		for (j=0;j<c;j++)
		{
			NA[i]+=QA[j*w+i]; /* initial row of N */
			NB[i]+=QB[j*w+i]; /* initial row of N */
		}
	}

	/* Do remaining rows of N */
	for (i=0;i<w;i++)
	{
		for (j=1;j<h;j++)
		{
			if ((j+c)*w+i<size)
			{
				NA[j*w+i]=NA[(j-1)*w+i] + QA[(j+c-1)*w+i] - QA[(j-1)*w+i];
				NB[j*w+i]=NB[(j-1)*w+i] + QB[(j+c-1)*w+i] - QB[(j-1)*w+i];
			}
			else
			{
				NA[j*w+i]=NA[(j-1)*w+i] - QA[(j-1)*w+i];
				NB[j*w+i]=NB[(j-1)*w+i] - QB[(j-1)*w+i];
			}
		}
	}

	for (i=0;i<size;i++)
	{
		RA[i]=(float)NA[i];
		RB[i]=(float)NB[i];
	}
	return;
}


static void getDisparities(float *origL,
		float *origR,
		int w, int h, SContext_t *sc)
{
	int i,j;
	int r;
	int offset;
	int wo2;
	float *dmap;
	int range,window,horopter;
	//unsigned int lrfail=0; /* number of pixels that fail the left-right check*/

	static int lastw=0,lasth=0;
	static float *workspace=NULL;
	static float *A=NULL;
	static float *B=NULL;
	static float *N1=NULL;
	static float *D1=NULL;
	static float *D2=NULL;
	static float *prev=NULL;
	float tmp;

	/* set the local aliases */
	range=sc->range;
	window=sc->window;
	horopter=sc->horopter;
	wo2=window;
	dmap=sc->dmap;
	for (j=0;j<w*h;j++) dmap[j]=-1;

	if (lastw!=w || lasth!=h)
	{
		if (A!=NULL)
		{
			free(prev);
			free(N1);
			free(D1);
			free(D2);
			free(A);
			free(B);
			free(workspace);
		}
		A=(float*)calloc(w*h,sizeof(float));
		B=(float*)calloc(w*h,sizeof(float));
		workspace=(float*)calloc(6*w*h,sizeof(float));
		N1=(float*)calloc(w*h,sizeof(float));
		D1=(float*)calloc(w*h,sizeof(float));
		D2=(float*)calloc(w*h,sizeof(float));
		prev=(float*)malloc(w*h*sizeof(float));
		lastw=w;
		lasth=h;
	}
	memset(prev,0,w*h*sizeof(float));

	/* build D1 (The denominator of right image) */
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			offset=j*w+i;
			A[offset]+=origR[offset]*origR[offset];
		}
	}
	blocksum(A,workspace,w,h,wo2,D1);
	/* take sqrts */
	for (i=0;i<w*h;i++)
	{
		D1[i]=(D1[i]>0.0f?1.0f/sqrtf(D1[i]):0.0f);
	}

	/* for each range */
	for (r=0;r<=range;r++)
	{
		/* build D2 (denominator of left image) */
		for (j=0;j<h;j++)
		{
			for (i=0;i<w-r;i++)
			{
				offset=j*w+i;
				A[offset]=origL[offset+r]*origL[offset+r];
				/* build N1 */
				B[offset]=origR[offset]*origL[offset+r];
			}
			for (i=w-r;i<w;i++)
			{
				offset=j*w+i;
				A[offset]=0;
				B[offset]=0;
			}
		}
		blocksum2(A,B,workspace,w,h,wo2,D2,N1);

		/* take sqrts */
		for (i=0;i<w*h;i++)
		{
			D2[i]=(D2[i]>0.0f?1.0f/sqrtf(D2[i]):0.0f);
		}

		/* find best matches */
		for (i=0;i<w*h;i++)
		{
			tmp=N1[i]*D1[i]*D2[i];
			if (prev[i]<tmp)
			{
				/* ensure peakiness */
				//if ((tmp-prev[i])/tmp>0.005)
					dmap[i]=r;
				prev[i]=tmp;
			}
		}
	}

	for (i=0;i<w*h;i++)
	{
		if (dmap[i]==(float)range) dmap[i]=-1;
	}


	return;
}
