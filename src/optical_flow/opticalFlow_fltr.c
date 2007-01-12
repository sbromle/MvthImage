/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Compute optical flow between two images.
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
#include <limits.h>
#include <memUtils.h>

#include "base/images_types.h"
#include "utils/drawLine.h"

extern int verbose;

/* function to look up match */

static void getOpFlowMatch(float *template,
		float *search,
		int x, int y,
		int w, int h,
		int corrsize, /* correlation window size */
		int swindow, /* size of search area */
		int *dx, int *dy);

static void fill_flow(image_t *wimg, int range);

/* let dx position be encoded as red,
 * let dy position be encodes as blue.
 * Offset is 128, so that (0,0) gets encodes as (128<<16+128).
 */
#define color_encode(a,b) (((128+(127/range)*(a))<<16) + ((128+(127/range)*(b))))

static char *matched=NULL; /* array holding 1 if a match already found, 0 if not */

void opticalFlow_fltr(image_t *wimg1, image_t *wimg2, image_t *wimg_dst,
		int corrsize,
		int range, int fillonly)
{
	float *img1;
	float *img2;
	float *flow;
	int *dir; /* will hold the direction of flow */
	int h,w,bands;
	int dx,dy;
	int i,j;
	float rgb[4]; /* color to draw line */

	if (fillonly)
	{
		fill_flow(wimg_dst,range);
		return;
	}

	if (wimg1==NULL || wimg2==NULL) return;

	w=wimg1->w;
	h=wimg1->h;
	bands=wimg1->bands; /* wimg may not be RGB, though wimg_dst must be */

	/* actually, to make things simpler for now,
	 * insist on having bands==1 */
	if (bands!=1 || wimg2->bands!=1)
	{
		fprintf(stderr,"Sorry, for now we must insist on bands=1.\n");
		return;
	}
	img1=wimg1->data;
	img2=wimg2->data;


	/* check to make sure we have somewhere to put this thing. */
	if (wimg_dst!=NULL)
	{
		if (wimg_dst->bands!=3 || wimg_dst->w!=w || wimg_dst->h!=h)
		{
			fprintf(stderr,"WImage size mismatch.\n");
			return;
		}
	}

	flow=wimg_dst->data;
	/* clear out flow */
	memset(flow,0,w*h*3*sizeof(float));

	/****************
	 * BETTER METHOD FOR OPTICAL FLOW.
	 * TO BE IMPLEMENTED.
	 **********************
	 * 1) Get x gradient of images, store them.
	 * 2) Get y gradient of images, store them.
	 * 3) Get time derivative of images, store it.
	 * For each NxN patch (say N=5):
	 * i) Form A = [ E(p1)_x E(p1)_y]
	 *             [ E(p2)_x E(p2)_y] ...
	 * ii) Form b = [ E(p1)_t , E(p2)_t, ..., E(pNxN)_t]^T
	 * iii) Compute optical vflow (v=[vx,vy]^T) for the
	 *      point at the center of the NxN window, as
	 *
	 *      v = (A^T A)^(-1) (A^T)b.
	 *
	 * Do this for each point in the image.
	 * Result: Dense optical flow field.
	 **************************************/
	
	/* result will hold colors that represent the encoded displacements
	 * to the optical flow match */

	/* for now do brute force search for best correlation */

	/* allocate directional array */
	dir=(int*)calloc(2*w*h,sizeof(int));
	/* allocate space for the "matched" mask */
	matched=(char*)calloc(w*h,sizeof(char));

	for (j=range;j<h-range;j++)
	{
		for (i=range; i<w-range;i++)
		{
			int offset;
			offset=2*(j*w+i);
			getOpFlowMatch(img1,img2,i,j,w,h,corrsize,range,&dx,&dy);
			dir[offset]=dx;
			dir[offset+1]=dy;
		}
	}

#define SMOOTH_OPT_FLOW
#ifdef SMOOTH_OPT_FLOW
	for (j=0;j<h-3;j++)
	{
		for (i=0;i<w-3;i++)
		{
			int offset;
			int offset2;
			int ii,jj;
			double vavg,havg;
			/* get the 3x3 average of directions */
			offset=2*(j*w+i);
			vavg=havg=0;
			for (jj=0;jj<3;jj++)
			{
				for (ii=0;ii<3;ii++)
				{
					offset2=2*w*jj+ii;
					havg+=dir[offset+offset2];
					vavg+=dir[offset+offset2+1];
				}
			}
			dir[offset+2*(w+1)]=havg/9;
			dir[offset+2*(w+1)+1]=vavg/9;
		}
	}
#endif

//#define COLORIZE
#ifdef COLORIZE
	for (j=range;j<h-range;j++)
	{
		for (i=range; i<w-range;i++)
		{
			int tmp;
			int offset;
			dx=dir[2*(j*w+i)];
			dy=dir[2*(j*w+i)+1];
			tmp=(int)color_encode(dx,dy);
			c=(unsigned char)((tmp>>16)&255);
			//fprintf(stderr,"%03d ",c);
			offset=3*(j*w+i);
			flow[offset]=(float)c/255.0;
			//fprintf(stderr,"%03d ",0);
			flow[offset+1]=0;
			c=(unsigned char)(tmp&255);
			//fprintf(stderr,"%03d\n",c);
			flow[offset+2]=(float)c/255.0; 
		}
	}
#else 
	/* draw little arrows instead */
	rgb[0]=rgb[1]=rgb[2]=1.0;
	for (j=range;j<h-range;j+=7)
	{
		for (i=range; i<w-range;i+=7)
		{
			dx=dir[2*(j*w+i)];
			dy=dir[2*(j*w+i)+1];
			drawLine(flow,w,h,3,i,j,dx,dy,rgb);
			/* make a red dot to show tail of arrow */
			flow[3*(j*w+i)]=1.0;
			flow[3*(j*w+i)+1]=0;
			flow[3*(j*w+i)+2]=0;
		}
	}
#endif

	free(dir);
	free(matched);
	matched=NULL;
	return;
}

/* function to just fill in the flow field with
 * optical flow colors according to distance and
 * direction from center */
static void fill_flow(image_t *wimg, int range)
{
	float *img;
	unsigned char c; 
	int h,w,bands;
	double ddx,ddy;
	int dx,dy;
	int i,j;

	if (wimg==NULL) return;

	w=wimg->w;
	h=wimg->h;
	bands=wimg->bands; /* must be = 3 */
	img=wimg->data;

	for (j=0;j<h;j++)
	{
		int tmp;
		ddy=j-0.5*h;
		ddy*=(double)range/(double)h;
		dy=(int)ddy;
		for (i=0;i<w;i++)
		{
			ddx=i-w/2;
			ddx*=(double)range/(double)w;
			dx=(int)ddx;
			tmp=(int)color_encode(dx,dy);
			c=(unsigned char)((tmp>>16)&255);
			//fprintf(stderr,"%03d ",c);
			img[3*(j*w+i)]=(float)c/255.0;
			//fprintf(stderr,"%03d ",0);
			img[3*(j*w+i)+1]=0;
			c=(unsigned char)(tmp&255);
			//fprintf(stderr,"%03d\n",c);
			img[3*(j*w+i)+2]=(float)c/255.0; 
		}
	}
	return;
}

static float getCorrelation(float *tvec, float tavg, float *svec, float savg, int len)
{
	float ta,sa;
	float dt,ds;
	float sum;
	int k;

	ta=sa=sum=0;
	for (k=0;k<len;k++)
	{
		dt=tvec[k]-tavg;
		ds=svec[k]-savg;
		ta+=dt*dt;
		sa+=ds*ds;
		sum+=dt*ds;
	}
	if (sa==0 || ta==0) return -1;

	return sum/(sqrt(ta)*sqrt(sa));
}

static void pack_vector(float *img, int w, int h,
		int x, int y, int dx, int dy, float *vec, float *avg);

/* ASSUMES THAT IMAGE IS GRAYSCALE!! */
static void getOpFlowMatch(float *template,
		float *search,
		int x, int y,
		int w, int h,
		int corrsize, /* correlation window size */
		int swindow, /* size of search area */
		int *dx, int *dy)
{
	/* a vector to hold the template window */
	float *tvec;
	/* the average of the elements of tvec*/
	float tavg;
	/* a vector to hold a search window */
	float *svec;
	/* the average of the elements of svec*/
	float savg;
	//int i,j;
	int ii,jj;
	//int k;
	int hc; /* half of the corrsize */
	int hs; /* half of the search window size */
	int bx, by;
	float cmax,cval;
	float diff;

	hc=corrsize/2;
	hs=swindow/2;

	/* make space for the vectors */
	tvec=(float*)malloc(corrsize*corrsize*sizeof(float));
	svec=(float*)malloc(corrsize*corrsize*sizeof(float));

	/* fill tvec, and compute tavg */
	pack_vector(template,w,h,x-hc,y-hc,corrsize,corrsize,tvec,&tavg);

	/* ok so now we need to loop over the search area
	 * for each of the possible locations */

	/* first get the correlation at the center location */
	/* pack the svec */
	pack_vector(search,w,h,x-hc,y-hc,corrsize,corrsize,svec,&savg);
	/* find the correlation value for this location */
	cmax=getCorrelation(tvec,tavg,svec,savg,corrsize*corrsize);
	bx=0;
	by=0;

	/* and first, let's test to see if there are any differences
	 * at all within our correlation window. (Since if there are not,
	 * there is no need to look for a better match.)
	 */
	diff=0;
	for (jj=0;jj<corrsize*corrsize;jj++)
	{
		float tmp;
		tmp=tvec[jj]-svec[jj];
		diff+=(tmp>0?tmp:-tmp);
	}
	if (diff<(float)(1.5*corrsize*corrsize)) /* a reasonable minimum difference? */
	{
		*dx=0;
		*dy=0;
		free(tvec);
		free(svec);
		return;
	}
	else
	{
		if (verbose) fprintf(stderr,".");
	}



	/* now get the correlation values for each possible non-zero offset */
	for (jj=0;jj<swindow;jj++) 
	{
		for (ii=0;ii<swindow;ii++)
		{
			if (jj==0 && ii==0) continue; /* already did this one */

			if (!matched[w*(y-jj)-ii])
			{
 				/* search left/up */
				/* pack the svec */
				pack_vector(search,w,h,x-hc-ii,y-hc-jj,corrsize,corrsize,svec,&savg);
				/* find the correlation value for this location */
				cval=getCorrelation(tvec,tavg,svec,savg,corrsize*corrsize);
				if (cval>cmax)
				{
					cmax=cval;
					bx=-ii;
					by=-jj;
				}
			}
			if (!matched[w*(y+jj)+ii])
			{
 				/* search right/down */
				/* pack the svec */
				pack_vector(search,w,h,x-hc+ii,y-hc+jj,corrsize,corrsize,svec,&savg);
				/* find the correlation value for this location */
				cval=getCorrelation(tvec,tavg,svec,savg,corrsize*corrsize);
				if (cval>cmax)
				{
					cmax=cval;
					bx=+ii;
					by=+jj;
				}
			}
		}
	}
	//fprintf(stderr,"Found cmax=%f\n",cmax);

	*dx=bx;
	*dy=by;
	matched[w*(y+by)+x+bx]=1;
	free(tvec);
	free(svec);

	return;
}

/* takes a 2d array of unsigned chars, and repacks a rectangle
 * of size dx by dy with top corner located at (x,y) into
 * a 1d vector, vec. Stores the average in avg.
 * vec array must be PRE-ALLOCATED.
 */
static void pack_vector(float *img, int w, int h,
		int x, int y, int dx, int dy, float *vec, float *avg)
{
	int i,j,k;
	int ox,oy; /* offsets in x and y directions */
	float vavg;

	k=0;
	vavg=0;
	for (j=0;j<dy;j++)
	{
		oy=j+y;
		for (i=0;i<dx;i++)
		{
			ox=i+x;
			if (ox<0 || ox>=w
					|| oy<0 || oy>=h)
			{
				vec[k]=0;
			}
			else
			{
				vec[k]=(float)img[oy*w+ox];
				vavg+=vec[k];
			}
			k++;
		}
	}
	/* set up tavg vector */
	vavg/=(dx*dy);

	*avg=vavg;
	return;
}

