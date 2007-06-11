/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Do canny edge detection.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <memUtils.h>
#include <float.h>
#include "utils/derivatives.h"
#include "utils/mvth_convert.h"
#include "base/images_types.h"

/* functional definitions. */

static int nonmaxSuppress(float *src, float *dst, char *dirs, int w, int h);
static int hysteresisThresh(float *src, float *dst, char *dirs,
		int w, int h, float thigh, float tlow);

extern int verbose;

/* canny edge detector assumes image is
 * already smoothed.
 */
void canny_fltr(image_t *img, float thigh, float tlow)
{
	float *orig;
	float *tmp=NULL,*dst=NULL;
	float *Jx=NULL, *Jy=NULL; /* will hold results of derivatives */
	char *directions=NULL; /* holds directional values from 0 to 7 */
	char dir;
	int w,h,bands;
	int register i;

	if (img->bands!=1)
	{
		fprintf(stderr,"canny_fltr: Must be grayscale.\n");
		return;
	}

	w=img->w;
	h=img->h;
	bands=img->bands;
	orig=img->data;

	tmp=(float*)calloc(w*h,sizeof(float));
	dst=(float*)calloc(w*h,sizeof(float));
	Jx=(float*)calloc(w*h,sizeof(float));
	Jy=(float*)calloc(w*h,sizeof(float));
	directions=(char*)calloc(w*h,sizeof(char));

	if (tmp==NULL || dst==NULL || Jx==NULL || Jy==NULL || directions==NULL)
	{
		fprintf(stderr,"canny16: Error. Cannot allocate workspace.\n");
		if (tmp!=NULL) free(tmp);
		if (dst!=NULL) free(dst);
		if (Jx!=NULL) free(Jx);
		if (Jy!=NULL) free(Jy);
		if (directions!=NULL) free(directions);
		return;
	}

	memcpy(tmp,img->data,w*h*img->bands*sizeof(float));

	/* do the gradients in each direction */
	gradX(tmp,Jx,w,h);
	gradY(tmp,Jy,w,h);

	/* and store the magnitudes back into tmp */
	for (i=0;i<w*h;i++)
	{
		tmp[i]=sqrtf(Jy[i]*Jy[i]+Jx[i]*Jx[i]);
		dir=(char)(atan2(Jy[i],Jx[i])*4/M_PI+4);
		if (dir==(char)8) dir=0;
		directions[i]=dir;
	}

	/* ok, so now we have magnitudes and directions stored */
	/* do the non-max suppression */
	nonmaxSuppress(tmp,dst,directions,w,h);

	/* and finally the hysterises thresh */
	hysteresisThresh(dst,tmp,directions,w,h,thigh,tlow);

	/* copy the result back into the image */
	memcpy(orig,tmp,w*h*img->bands*sizeof(float));

	if (tmp!=NULL) free(tmp);
	if (dst!=NULL) free(dst);
	if (Jx!=NULL) free(Jx);
	if (Jy!=NULL) free(Jy);
	if (directions!=NULL) free(directions);

	return;
}

static int nonmaxSuppress(float *src, float *dst, char *dirs, int w, int h)
{
	register int j,i;
	register int pos,y;
	/* do the non-max suppression */
	for (j=0;j<h;j++)
	{
		y=j*w;
		for (i=0;i<w;i++)
		{
			pos=y+i;
			switch(dirs[pos])
			{
				case 0:
				case 4:
					if (i>0 && i<w-1)
					{
						if (src[pos-1]>src[pos] || src[pos+1]>src[pos])
							dst[pos]=0;
						else dst[pos]=src[pos];
					}
					break;
				case 1:
				case 5:
					if (i>0 && i<w-1 && j>0 && j<h-1)
					{
						if (src[pos-1+w]>src[pos] || src[pos+1-w]>src[pos])
							dst[pos]=0;
						else dst[pos]=src[pos];
					}
					break;
				case 2:
				case 6:
					if (j>0 && j<h-1)
					{
						if (src[pos+w]>src[pos] || src[pos-w]>src[pos])
							dst[pos]=0;
						else dst[pos]=src[pos];
					}
					break;
				case 3:
				case 7:
					if (i>0 && i<w-1 && j>0 && j<h-1)
					{
						if (src[pos+1-w]>src[pos] || src[pos-1+w]>src[pos])
							dst[pos]=0;
						else dst[pos]=src[pos];
					}
					break;
				default:
					fprintf(stderr,"Canny: Non-max suppress: Invalid direction %d\n",
							(int)dirs[pos]);
					break;
			}
		}
	}
	return 0;
}

static int hysteresisThresh(float *src, float *dst, char *dirs,
		int w, int h,
		float thigh, float tlow)
{
	register int i,j;
	register int pos;
	register int y;
	register int offsetx=0;
	register int offsety=0;
	/* need an occupancy grid */
	char **done;

	memset(dst,0,w*h*sizeof(float));

	done=twoDchar(w,h);
	if (done==NULL)
	{
		fprintf(stderr,"hysteresisThresh16(): Cannot allocate occupancy grid.\n");
		return -1;
	}

	/* check to see if we need to do automatic thresholding */
	if (thigh==0 || tlow==0) /* then calculate good estimates */
	{
		float max=-FLT_MAX;
		for (j=0;j<h;j++)
		{
			pos=j*w;
			for (i=0;i<w;i++)
			{
				if (max<src[pos+i]) max=src[pos+i];
			}
		}
		thigh=(0.4*max);
		tlow=(0.1*max);
		if (verbose) fprintf(stderr,"canny: [max,thigh,tlow] [%g,%g,%g]\n",max,thigh,tlow);
	}


	for (j=0;j<h;j++)
	{
		y=j*w;
		for (i=0;i<w;i++)
		{
			pos=y+i;
			if (src[pos]<thigh || done[0][pos]==(char)1) continue;
			/* otherwise begin hysteresis */
			/* this position passed high threshold */
			/* get offset to new direction */
			switch(dirs[pos])
			{
				case 0: case 4:
					offsetx=1;
					offsety=0;
					break;
				case 1: case 5:
					offsetx=1;
					offsety=-w;
					break;
				case 2: case 6:
					offsetx=0;
					offsety=-w;
					break;
				case 3: case 7:
					offsetx=-1;
					offsety=w;
					break;
				default:
					fprintf(stderr,"hysteresisthresh: Invalid direction.\n");
					break;
			}
			while (src[pos]>tlow &&  done[0][pos]==0)
			{
				done[0][pos]=1;
				//dst[pos]=src[pos];
				dst[pos]=1.0;
				if (i+offsetx>w-1 || i+offsetx<0) break;
				if (j+offsety>h-1 || j+offsety<0) break;
				if (src[pos+offsetx+offsety]>tlow)
					pos+=offsetx+offsety;
			}
		}
	}
	free2D(done);
	return 1;
}
