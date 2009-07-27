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


#include "base/images_types.h"
#include "base/stereo_context_t.h"
#include "utils/getMatch.h"

extern int verbose;

//#define DMAP_ERROR_TOL 2.0
static const float DMAP_ERROR_TOL=0.5F;

//#define DO_ONE_ROW

static void getDisparities(float *origL,
		float *origR,
		int w, int h, SContext_t *sc);

void astereo_fltr(image_t *img1, image_t *img2,
		SContext_t *sc)
		//int range, int window, int horopter)
{
	int h,w;

	/* all error testing now done in astereo_cmd.c */

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

static void getDisparities(float *origL,
		float *origR,
		int w, int h, SContext_t *sc)
{
	int i,j;
	float disp;
	float disp2;
	int offsety;
	int offset;
	int wo2;
	float *template;
	float *search;
	float *dmap;
	float tmp;
	int range,window,horopter;
	unsigned int lrfail=0; /* number of pixels that fail the left-right check*/

	/* set the local aliases */
	range=sc->range;
	window=sc->window;
	horopter=sc->horopter;

	wo2=window;
	//memset(img,(unsigned char)255,3*w*h);

	dmap=sc->dmap;
	//dmap=(float*)malloc(w*h*sizeof(float));
	for (j=0;j<w*h;j++) dmap[j]=-1;

	for (j=1;j<h-1;j++)
	{
		offsety=j*w; /* vertical offset into image */
		/* must start at least range+wo2 x offset in right image */
		for (i=range+wo2+horopter;i<w-wo2;i++)
		{
			offset=offsety+i; /* offset into linear array */
			template=&origL[offset-wo2];
			search=&origR[offset-wo2-range-horopter];
			/* disp holds disparity between left and right */
#ifdef DO_ONE_ROW
			tmp=range-getWindowMatch(template,search,1,w,window,range);
#else
			tmp=range-getWindowMatch(template-w,search-w,3,w,window,range);
#endif
			disp=tmp;

			if (tmp==range) /* no match. so skip it */
			{
				lrfail++;
				continue;
			}

			assert(disp<=range);

			/* search right to left */
			template=&origR[offset-wo2-horopter-(int)disp];
			search=&origL[offset-wo2-(int)disp];
#ifdef DO_ONE_ROW
			tmp=getWindowMatch(template,search,1,w,window,range);
#else
			tmp=getWindowMatch(template-w,search-w,3,w,window,range);
#endif
			disp2=tmp;

			if (disp2>range)
			{
				if (verbose) fprintf(stderr,"Error disp2=%lf > range\n",disp2);
				assert(disp2<=range);
			}
			if (fabs(disp2-disp)<=DMAP_ERROR_TOL)
			{
				dmap[offset-wo2]=disp2+horopter;
			}
			else
			{
				lrfail++;
			}
		}
	}
	if (verbose)
	{
		fprintf(stderr,"%u of %d pixels (%f %%) failed L-R check.\n",lrfail,w*h,100.0F*(float)lrfail/(float)(w*h));
		lrfail=0;
		for (j=0;j<w*h;j++) if (dmap[j]<0) lrfail++;
		fprintf(stderr,"But %d have negative disp?\n",lrfail-2*w);
	}

	return;
}
