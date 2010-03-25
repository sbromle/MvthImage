/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Pyramid based stereo.
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
#include <unistd.h>


#include "utils/getMatch.h"
#include "utils/getColor.h"
#include "base/stereo_context_t.h"
#include "base/images_types.h"

static const float DMAP_ERROR_TOL=1.0F;

extern int verbose;


extern void resize_image_avg(float *src, int w, int h, int bands,
		float *dst, const float fract);

typedef struct PLevel_s {
	float *left;
	float *right;
	int w, h;
} PLevel_t;

static void getPyramidDisparities(float *origL,
		float *origR, SContext_t *sc,int level, int maxlevel);

/*
static void scaleUpDmap(float *dmap, int w, int h,
		int nw, int nh, float f);
		*/
static void doubleUpDmap(float *dmap, int w, int h,
		int nw, int nh);

void pyramid_stereo_fltr(image_t *imgL, image_t *imgR,
		int nlevels, SContext_t *sc)
{
	PLevel_t *level; /* pointers to each of the refinement levels */
	int h,w; /* width and height of top level */
	int i;
	int maxlevels;
	int range,window=3;

	if (imgL->d!=1 || imgR->d!=1) return;

	/* all error testing now done in astereo_cmd.c */

	w=imgL->w;
	h=imgL->h;

	/* save the stereo context */
	sc->horopter=0; /* nonzero doesn't make sense for pyramid */
	range=sc->range;
	if (sc->dmap==NULL ||
			sc->w*sc->h!=w*h)
	{
		if (sc->dmap!=NULL)
			free(sc->dmap);
		sc->dmap=(float*)malloc(w*h*sizeof(float));
		for (i=0;i<w*h;i++) sc->dmap[i]=-1;
	}
	else
	{
		/* zero out the dmap */
		for (i=0;i<w*h;i++) sc->dmap[i]=-1;
		//memset(stereo_context.dmap,0,w*h*sizeof(float));
	}
	sc->w=w;
	sc->h=h;


	/* find out the number of valid levels */
	i=(w>h?h:w);
	if ((i>>nlevels) < range+(window>>nlevels))
	{
		fprintf(stderr,"Too many levels specified for image size and parameters.\n");
		return;
	}

	maxlevels=ilogbf((float)i);

	/* Set up the multiple levels */
	level=(PLevel_t*)malloc((nlevels+1)*sizeof(PLevel_t));

	if (nlevels>=maxlevels)
	{
		fprintf(stderr,"Too many levels requested given size of image.\n");
		free(level);
		return;
	}
	
	level[0].left  = imgL->data;
	level[0].right = imgR->data;
	level[0].w = w;
	level[0].h = h;

	for (i=1;i<=nlevels;i++)
	{
		level[i].w=(int)(0.5*level[i-1].w+0.5); /* scale down to floor of w/2 */
		level[i].h=(int)(0.5*level[i-1].h+0.5); /* scale down to floor of h/2 */
		level[i].left=
			(float*)malloc(level[i].w*level[i].h*sizeof(float));
		level[i].right=
			(float*)malloc(level[i].w*level[i].h*sizeof(float));
		/* build the pyramid */
		resize_image_avg(level[i-1].left,
				level[i-1].w,level[i-1].h,1, level[i].left,0.5F);
		resize_image_avg(level[i-1].right,
				level[i-1].w,level[i-1].h,1,level[i].right,0.5F);
	}


	/* ok, so we now have the pyramids */
	/* starting at the lowest (coarsest) level, get the
	 * disparity and propogate it upwards.
	 */
	for (i=nlevels;i>0;i--)
	{
		SContext_t sc1;
		/* get the disparities at this level, using the existing
		 * disparity map as a starting location */
		sc1.w=level[i].w;
		sc1.h=level[i].h;
		sc1.range=range;
		sc1.horopter=0;
		sc1.dmap=sc->dmap;
		/* TODO: handle big windows on coarse scales problem. */
		//sc.window=window>>i;
		sc1.window=window;
		if (sc1.window<1) sc1.window=1;
		else if (sc1.window>sc1.w) sc1.window=1;

		if (verbose)
		{
			fprintf(stderr,"Level %d\n",i);
			fprintf(stderr,"\tsc.w = %d\n",sc1.w);
			fprintf(stderr,"\tsc.h = %d\n",sc1.h);
			fprintf(stderr,"\tsc.range = %d\n",sc1.range);
			fprintf(stderr,"\tsc.horopter = %d\n",sc1.horopter);
			fprintf(stderr,"\tsc.window = %d\n",sc1.window);
		}

		getPyramidDisparities(level[i].left, level[i].right,
				&sc1,nlevels-i,nlevels);

//#define DRAW_INTERMEDIATE_DMAP
#ifdef DRAW_INTERMEDIATE_DMAP
		/* THIS IS NOW BROKEN SINCE WE USING LOCAL "sc"
		 * RATHER THAN GLOBAL stereo_context ABOVE.
		 */
		/* fix stereo_context.range for color purposes */
		char filename[1024];
		extern int writeimage(float *img,int w,int h,
				int bands,char *filename);
		extern void dmap_draw_fltr(image_t *,SContext_t *sc, float);
		image_t tmpimg;
		tmpimg.data=(float*)malloc(sc1.w*sc1.h*3*sizeof(float));
		tmpimg.w=w;
		tmpimg.h=h;
		tmpimg.bands=3;
		sc1.range*=(1<<(nlevels-i));
		dmap_draw_fltr(&tmpimg,&sc1,16*(1<<i));/* draw the intermediate dmap for kicks */
		fprintf(stderr,"Drawing dmap (%dx%d)\n",sc1.w,sc1.h)
		sprintf(filename,"/tmp/dmap%02d.jpg",i);
		writeimage(tmpimg.data,tmpimg.w,tmpimg.h,3,filename);
		free(tmpimg.data);
		//sleep(1);        /* sleep for a second so we can see the new dmap */
#endif

		/* scale the disparity map up and interpolate */
		/*
		scaleUpDmap(sc->dmap,
				level[i].w,level[i].h,
				level[i-1].w,level[i-1].h,2.0);
				*/
		doubleUpDmap(sc->dmap,
				level[i].w,level[i].h,
				level[i-1].w,level[i-1].h);

		/* we are now done with that level */
		free(level[i].left);
		free(level[i].right);
		/* now ready for next run ...*/
	}

	//stereo_context.h=h;
	/* ok, now just do the finest level */
	fprintf(stderr,"FINEST LEVEL\n");
	getPyramidDisparities(level[0].left, level[0].right,sc,nlevels,nlevels);

	sc->range=range<<nlevels;
	/* draw the dmap */
	//dmap_draw_fltr(imgD,stereo_context,-1);
	/* free the level data structures */
	free(level);
}

/* scale the dmap up by factor f, and interpolate missing
 * pixels */
//#define LIN_INTERPOLATE_DMAP
#define MAX_DMAP
#ifdef MAX_DMAP

/* double the size of the dmap, up to at most nwXnh */
static void doubleUpDmap(float *dmap, int w, int h, int nw, int nh)
{
	int i,j;
	float *dmapnew;


	if ((nh>>1)>h || (nw>>1)>w)
	{
		fprintf(stderr,"ERROR SCALING DMAP. nh/2>h or nw/2>w \n");
		return;
	}

	dmapnew=(float*)calloc(nw*nh,sizeof(float));

	for (j=0;j<nh;j++)
	{
		for (i=0;i<nw;i++)
		{
			dmapnew[j*nw+i]=2*dmap[w*(j>>1)+(i>>1)];
		}
	}

	memcpy(dmap,dmapnew,nw*nh*sizeof(float));
	free(dmapnew);
}

#elif defined LIN_INTERPOLATE_DMAP

static void scaleUpDmap(float *dmap, int w, int h, float f)
{
	int i,j;
	int nw,nh;
	int jold,iold;
	float fi=1.0/f; 
	int yoffset;
	int offset;
	int nyoffset;
	int noffset;
	float yfrac,xfrac;
	float *dmapnew;
	float d[4];

	nh=(int)(h*f+0.5);
	nw=(int)(w*f+0.5);

	dmapnew=(float*)calloc(nw*nh,sizeof(float));
	
	for (j=0;j<nh-1;j++) /* must leave out uppermost boundary */
	{
		jold=(int)(j*fi);
		yfrac=j*fi-jold;
		yoffset=jold*w;
		nyoffset=j*nw;
		for (i=0;i<nw-1;i++)
		{
			iold=(int)(i*fi);
			offset=yoffset+iold;
			noffset=nyoffset+i;

			xfrac=i*fi-iold;
			d[0]=dmap[offset];
			d[1]=dmap[offset+1];
			d[2]=dmap[offset+w];
			d[3]=dmap[offset+w+1];
			if (d[0]>=0||d[1]>=0||d[2]>=0||d[2]>=0)
			{
				if (d[0]<0) d[0]=0;
				if (d[1]<0) d[1]=0;
				if (d[2]<0) d[2]=0;
				if (d[3]<0) d[3]=0;
			
				dmapnew[j*nw+i]=f*(
				  	d[0]*(1-xfrac)*(1-yfrac)
					+ d[1]*(xfrac)*(1-yfrac)
					+ d[2]*(1-xfrac)*(yfrac)
					+ d[3]*(xfrac)*(yfrac));
			}
		}
	}
	/* do last column */
	for (j=0;j<nh-1;j++)
	{
		//jold=(int)(j/f);
		//dmapnew[j*nw+nw-1]=f*dmap[jold*w+w-1];
		dmapnew[j*nw+nw-1]=dmapnew[j*nw+nw-2];
	}
	/* do last row */
	for (i=0;i<nw;i++)
	{
		dmapnew[(nh-1)*nw+i]=dmapnew[(nh-2)*nw+i];
	}

	memcpy(dmap,dmapnew,nw*nh*sizeof(float));
	free(dmapnew);
	return;
}
#endif

static void getPyramidDisparities(float *origL,
		float *origR,SContext_t *sc, int level, int maxlevel)
{
	int i; /* i will iterate over the overlapping region */
	int j;
	/* hmm. Let i iterate instead over the overlapping region. */
	int xL; /* xoffset in the left image */
	int sR; /* xoffset in the right image */
	float disp;
	float xR1;
	int y;
	int d;
	float *template;
	float *search;
	float *dmap;
	int win;
	int ir; /* total i range */
	int c; /* 1/2 the correlation window width */
	float offset1;
	int r1, r2; /* search ranges within image boundaries */
	            /* will search right image from xL-d-r1 to xL-d+r2 */

	int rmax; /* the maximum disparity attainable for the current level */
	int r;

	int w,h;

	/* set the local aliases */
	win=sc->window;
	w=sc->w;
	h=sc->h;
	r=sc->range;
	rmax=r<<level; /*NOTE: Coarsest=0, --> Finest=nlevels */
	if (verbose) fprintf(stderr,"Rmax=%d\n",rmax);


	dmap=sc->dmap;
	
	ir=w-win; /* overlap region */ /* CHANGED FROM w-win */
	c=win>>1;
	if (c<0)
	{
		fprintf(stderr,"WARNING: c<0?\n");
		c=0;
	}

	for (j=1;j<h-1;j++) /* need to leave a 1 pixel border for window search */
	{
		y=j*w; /* vertical offset into image */

		for (i=0;i<ir;i++) /* i ranges over overlapped region of right image */
		{
			xL=i+c; /* centers of start of search */

			/* now if we have an initial guess, we can
			 * use it to refine our search */
			d=(int)dmap[y+xL]; /* will store the dmap in the right image
															  *	reference frame.
																*/

			/* so our search for xL in the right image, should be
			 * centered about xL-d.
			 */
			sR=xL-d;

			/* so, we'll look to the left of sR and to the right
			 * of sR over a maximum distance of r on each side.
			 */
			if (sR<0)
			{
				fprintf(stderr,"WARNING: sR<0\n");
				dmap[y+xL]=xL;
				continue;
			}
			else if (sR-c<0)
			{
				/* then the predicted disparity would put
				 * us off the image! I.e. there is no real
				 * match here. So set the disparity to the
				 * edge of the image.
				 */
				d=i;
				dmap[y+xL]=i;
				continue;
			}
			else if (sR-c-r<0) r1=sR-c;
			else r1=r;

			if (sR+c>w-1)
			{
				dmap[y+xL]=w-1-sR-c;
				continue;
			}
			else if (sR+c+r>w-1)/*check that we won't step too far in comparison*/
			{
				r2=w-1-sR-c;
			}
			else r2=r;

			if (r2>r || r1>r || r1<0 || r2<0)
				fprintf(stderr,"r1=%d r2=%d r=%d sR=%d c=%d w=%d\n",
						r1,r2,r,sR,c,w);

			/* ok, so we can now search for the point xL,
			 * in the right image inside the window
			 * [xL-d-r1, xL-d+r2] with a correlation window
			 * of half width "c" on either side.
			 */

			/* "template" to match is left image
			 * at position xL. */
			template=&origL[y+i];
			/* search range is in right image, starting
			 * from our minimum boundary */
			search=&origR[y+sR-c-r1]; /* start at doffset */

			/* now searches over distance range */
			offset1=getWindowMatch(template-w,search-w,3,w,win,r1+r2+1);

			/* so we've found a match at xR1 */
			xR1=sR-r1+offset1;
			/* ok, so our new disparity is: */
			disp=d+r1-offset1;
			//disp=xL-xR1;

#if 1
			dmap[y+xL]=disp;
#else
			/* ok, so now we have a new starting position in the
			 * right image. */
			//int xR=xL-(int)disp;
			int xR=(int)xR1;
			float xfrac=xR1-xR;
			/* we need to now search the left image. so let's
			 * determine the range we can/should search */

			/* start search where we are, and then look around it*/
			int sL=xL;

			r1=r;
			r2=r;
			if (xR-c<0) /* beyond left edge of image*/
			{
				dmap[y+xL]=sL; /* disparity determined to lie farther than edge*/
				continue;
			}
			if (sL-c<0)
			{
				/* some kind of conflict,
				 * so just set disparity to distance to edge of image */
				dmap[y+xL]=sL;
				continue;
			}
			else if (sL-c-r<0) r1=sL-c;
			if (sL+c+r>w-1) r2=w-1-sL-c;

			/*** RIGHT TO LEFT SEARCH ******/
			template=&origR[y+xR-c];
			search=&origL[y+sL-r1-c];

			//offset2=getWindowMatch(tr,sr,3,window,2*range);
			offset2=getWindowMatch(template-w,search-w,3,w,win,r2+r1+1);

			//xL1=sL-r1+offset2;

			if (fabs(offset2-r1+xfrac)<=DMAP_ERROR_TOL)
			{
				dmap[y+xL]=disp;
			}
#if 1
			else if (xL-1>0 && dmap[y+xL]<dmap[y+xL-1]) 
			{
				dmap[y+xL]=dmap[y+xL-1];
				//fprintf(stderr,"diff disp = %+lf\n",disp2-disp);
				//dmap[y+xL-c]=0;
			}
#endif
#endif
		}
	}
	/* Since we are using 3 rows for getWindowMatch, we must 
	 * fill in top and bottom rows of dmap with its neighbours */
	for (i=0;i<w;i++)
	{
		dmap[i]=dmap[w+i];
		dmap[w*(h-1)+i]=dmap[w*(h-2)+i];
	}
	/* Now it is possible that we skipped some
	 * disparity calculations on the left edge,
	 * and the right edge.
	 * lets fill these in */
	for (j=0;j<h;j++)
	{
		for (i=0;i<c;i++)
		{
			dmap[w*j+i]=dmap[w*j+c+2];
			dmap[w*j+w-1-i]=dmap[w*j+w-1-c-1];
		}
	}
	return;
}
