/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Get a remapping table for image transforms.
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

#include "getRemap.h"

double **getMapping(int w, int h, R3Dmatrix_t rm)
{
	int i,j;
	int centerx,centery; /* offsets to center of image */
	int xoffset,yoffset; /* offsets for indicing */
	double x,y,z;
  double **address=NULL;
	/* address with be a 2*w*h list of coords ((x,y) pairs)
	 * for each position (i,j) of the original image */

#ifdef DEBUG
	fprintf(stderr,"getMapping(): [w,h]=[%d,%d]\n",w,h);
#endif

	/* allocate the space for the remap table */
	address=(double**)malloc(h*sizeof(double*));
	assert(address!=NULL);
	address[0]=(double*)malloc(2*h*w*sizeof(double));
	assert(address[0]!=NULL);
	for (j=1;j<h;j++) address[j]=address[0]+2*w*j;

	/* build the address lookup table */
	/* do it this way because we can remap things faster
	 * in subsequent calls */

	centerx=w/2;
	centery=h/2;
	for (j=0;j<h;j++)
	{
		yoffset=j-centery;
		for (i=0;i<w;i++)
		{
			xoffset=i-centerx;
			x=(rm[0]*xoffset+rm[1]*yoffset+rm[2]);
			y=(rm[3]*xoffset+rm[4]*yoffset+rm[5]);
			z=(rm[6]*xoffset+rm[7]*yoffset+rm[8]);
			x=x/z+0.5+centerx;
			y=y/z+0.5+centery;
			if (x<0 || y<0 || x>w-1 || y>h-1)
			{
				address[j][2*i]=-1;
				address[j][2*i+1]=-1;
			}
			else
			{
				address[j][2*i]=x;
				address[j][2*i+1]=y;
			}
		}
	}
	return address;
}

#ifdef REMAP_DO_INVERSE 
int tm_inverse(Tmatrix t, Tmatrix i)
{
	int j;
	double d=t[0]*t[5]*t[10]-t[0]*t[9]*t[6]
		-t[4]*t[1]*t[10]+t[4]*t[2]*t[9]
		+t[1]*t[8]*t[6]-t[8]*t[5]*t[2];
	if (d==0)
	{
		for (j=0;j<12;j++) i[j]=0;
		return -1;
	}
	i[0]=t[5]*t[10]-t[9]*t[6];
	i[1]=t[2]*t[9]-t[1]*t[10];
	i[2]=t[1]*t[6]-t[5]*t[2];
	i[3]=-t[1]*t[6]*t[11] + t[1]*t[7]*t[10] + t[5]*t[2]*t[11]
		- t[5]*t[3]*t[10] - t[9]*t[7]*t[2] + t[9]*t[3]*t[6];
	i[4]=t[8]*t[6]-t[4]*t[10];
	i[5]=t[0]*t[10]-t[8]*t[2];
	i[6]=t[4]*t[2]-t[0]*t[6];
	i[7]=t[0]*t[6]*t[11] - t[0]*t[7]*t[10] - t[4]*t[2]*t[11] + t[4]*t[3]*t[10]
		+ t[8]*t[7]*t[2] - t[8]*t[3]*t[6];
	i[8]=t[4]*t[9]-t[8]*t[5];
	i[9]=t[1]*t[8]-t[0]*t[9];
	i[10]=t[0]*t[5]-t[4]*t[1];
	i[11]=-t[0]*t[5]*t[11] + t[0]*t[7]*t[9] + t[4]*t[1]*t[11]
		- t[4]*t[3]*t[9] - t[8]*t[1]*t[7] + t[8]*t[5]*t[3];

	for (j=0;j<12;j++) i[j]/=d;

	return 0;
}
#endif

double **getMapping3d(int w, int h, Tmatrix tm)
{
	int i,j;
	double centerx,centery; /* offsets to center of image */
	double xoffset,yoffset; /* offsets for indicing */
	double x,y;
	double x1,y1;
	double numer;
  double **address=NULL;
#ifdef REMAP_DO_INVERSE
	Tmatrix I; /* we want to do reverse lookup */
#endif

	/* address with be a 2*w*h list of coords ((x,y) pairs)
	 * for each position (i,j) of the original image */

#ifdef DEBUG
	fprintf(stderr,"getMapping(): [w,h]=[%d,%d]\n",w,h);
#endif

	/* allocate the space for the remap table */
	address=(double**)malloc(h*sizeof(double*));
	assert(address!=NULL);
	address[0]=(double*)malloc(2*h*w*sizeof(double));
	assert(address[0]!=NULL);
	for (j=1;j<h;j++) address[j]=address[0]+2*w*j;

#ifdef REMAP_DO_INVERSE
	/* build the inverse */
	tm_inverse(tm,I);
#endif
	/* build the address lookup table */
	/* do it this way because we can remap things faster
	 * in subsequent calls */

	/* we will try back-projection and plane intersection */
	numer=-(tm[2]*tm[3]+tm[6]*tm[7]+tm[10]*(tm[11]+1));

	centerx=0.5*w;
	centery=0.5*h;
	for (j=0;j<h;j++)
	{
		yoffset=j-centery;
		for (i=0;i<w;i++)
		{
			xoffset=i-centerx;
#ifdef REMAP_DO_INVERSE
			x=(I[0]*xoffset+I[1]*yoffset+I[ 2] +I[ 3]);
			y=(I[4]*xoffset+I[5]*yoffset+I[ 6] +I[ 7]);
			z=(I[8]*xoffset+I[9]*yoffset+I[10] +I[11]);
			x=x/z+0.5+centerx;
			y=y/z+0.5+centery;
#else
			double denom=-(xoffset*tm[2]+yoffset*tm[6]+tm[10]);
			if (denom==0) {x=1.0E9;y=1.0E9;}
			else
			{
				x1=xoffset*numer/denom;
				y1=yoffset*numer/denom;
				/* do rotation about z */
				x=tm[0]*x1+tm[1]*y1;
				y=tm[4]*x1+tm[5]*y1;
			}
			x+=centerx;
			y+=centery;
#endif
			if (x<0 || y<0 || x>w-1 || y>h-1)
			{
				address[j][2*i]=-1;
				address[j][2*i+1]=-1;
			}
			else
			{
				address[j][2*i]=x;
				address[j][2*i+1]=y;
			}
		}
	}
	return address;
}


/* like getMapping, but scales image width and height to [0,1] first */
double **getUnitMapping(int w, int h, R3Dmatrix_t rm)
{
	int i,j;
	int centerx,centery; /* offsets to center of image */
	double xoffset,yoffset; /* offsets for indicing */
	double x,y,z;
  double **address=NULL;
	/* address with be a 2*w*h list of coords ((x,y) pairs)
	 * for each position (i,j) of the original image */

#ifdef DEBUG
	fprintf(stderr,"getMapping(): [w,h]=[%d,%d]\n",w,h);
#endif

	/* allocate the space for the remap table */
	address=(double**)malloc(h*sizeof(double*));
	assert(address!=NULL);
	address[0]=(double*)malloc(2*h*w*sizeof(double));
	assert(address[0]!=NULL);
	for (j=1;j<h;j++) address[j]=address[0]+2*w*j;

	/* build the address lookup table */
	/* do it this way because we can remap things faster
	 * in subsequent calls */

	centerx=w/2;
	centery=h/2;
	for (j=0;j<h;j++)
	{
		yoffset=(j-centery)/(double)h;
		for (i=0;i<w;i++)
		{
			xoffset=(i-centerx)/(double)w;
			x=(rm[0]*xoffset+rm[1]*yoffset+rm[2]);
			y=(rm[3]*xoffset+rm[4]*yoffset+rm[5]);
			z=(rm[6]*xoffset+rm[7]*yoffset+rm[8]);
			x=x/z;
			y=y/z;
			x*=w;
			y*=h;
			x+=centerx;
			y+=centery;
			if (x<0 || y<0 || x>w-1 || y>h-1)
			{
				address[j][2*i]=-1;
				address[j][2*i+1]=-1;
			}
			else
			{
				address[j][2*i]=x;
				address[j][2*i+1]=y;
			}
		}
	}
	return address;
}

void remapImage(float *src, int sw, int sh,
		float *dst, int dw, int dh, int bands,
		double **table)
{
	int i,j,k;
	int x,y;
	int xp1,yp1; /* x and y plus 1 */
	double val;
	double xfrac,yfrac;
	int line;
#ifdef INTERP
	fprintf(stderr,"Remap using bilinear interpolation.\n");
#else
	fprintf(stderr,"Remap using truncation.\n");
#endif

	line=dw*bands;
	for (j=0;j<dh;j++)
	{
		for (i=0;i<dw;i++)
		{
			if (table[j][2*i]<0.0 || table[j][2*i+1] <0.0
					|| table[j][2*i]>sw-1 || table[j][2*i+1]>sh-1)
				continue;
			x=(int)table[j][2*i];
			xfrac=table[j][2*i]-x;
			y=(int)table[j][2*i+1];
			yfrac=table[j][2*i+1]-y;
			xp1=x+1;
			yp1=y+1;
			if (xp1>=sw) xp1=sw-1;
			if (yp1>=sh) yp1=sh-1;
			for (k=0;k<bands;k++)
			{
#ifdef INTERP
				val=(1-xfrac)*(1-yfrac)   * src[bands*(sw*y+x)+k]
					+ (xfrac)*(1-yfrac)     * src[bands*(sw*y+xp1)+k]
					+ (1-xfrac)*(yfrac)     * src[bands*(sw*(yp1)+x)+k]
					+ (xfrac)*(yfrac)   * src[bands*(sw*(yp1)+xp1)+k];
#else
				val=src[bands*(sw*y+x)+k];
#endif
				dst[j*line+bands*i+k]=(float)val;
			}
		}
	}
	return;
}

/* map a region of size wXh to an isometric field of size WXH */
double **getIsometricMap2(double w, double h, int W, int H,double theta)
{
	double **address=NULL;
	int i,j;
	double trad=theta*M_PI/180;
	double ct=1.0/tan(2*trad); 
	double c=cos(trad);
	double s=sin(trad);
	double sy=1.0/sin(2*trad);
	double a; /* overall scale factor */
	double x,y; /* shift */
	double nw,nh;/* new, unscaled width and height */
	double x1,y1;
	nw=(w+h)*c;
	nh=(w+h)*s;
	a=(nw/nh>(double)W/(double)H ? nw/W : nh/H);
	x=h*c;
	y=0;
	/* allocate memory for map */
	address=(double**)malloc(H*sizeof(double*));
	assert(address!=NULL);
	address[0]=(double*)malloc(2*H*W*sizeof(double));
	assert(address[0]!=NULL);
	for (j=1;j<H;j++) address[j]=address[0]+2*W*j;

	for (j=0;j<H;j++)
	{
		for (i=0;i<W;i++)
		{
			x1=a*( (c-s*ct)*i + (c*ct+s)*j ) + x*(s*ct-c);
			y1=a*( (-s*sy)*i + (sy*c)*j ) + x*(sy*s);
			if (x1<0||x1>w-1 || y1<0||y1>h-1)
			{
				address[j][2*i]=-1;
				address[j][2*i+1]=-1;
			}
			else
			{
				address[j][2*i]=x1;
				address[j][2*i+1]=y1;
			}
		}
	}
	return address;
}

/* map a region of size wXh to an isometric field of size WXH */
double **getIsometricMap(int w, int h,double theta)
{
	return getIsometricMap2((double)w,(double)h,w,h,theta);
}

#if 0
/* map a region of size wXh to an skewmetric field of size WXH */
double **getSkewSymmetricMap(double w, double h, int W, int H,double theta)
{
	double **address=NULL;
	int i,j;
	double trad=theta*M_PI/180;
	double ct=1.0/tan(2*trad); 
	double sy=1.0/sin(2*trad);
	double a; /* overall scale factor */
	double x,y; /* shift */
	double nw,nh;/* new, unscaled width and height */
	double x1,y1;
	nw=w+h*c;
	nh=h;
	a=(nw/nh>(double)W/(double)H ? nw/W : nh/H);
	x=h*c;
	y=0;
	/* allocate memory for map */
	address=(double**)malloc(H*sizeof(double*));
	assert(address!=NULL);
	address[0]=(double*)malloc(2*H*W*sizeof(double));
	assert(address[0]!=NULL);
	for (j=1;j<H;j++) address[j]=address[0]+2*W*j;

	for (j=0;j<H;j++)
	{
		for (i=0;i<W;i++)
		{
			x1=a*( (c-s*ct)*i + (c*ct+s)*j ) + x*(s*ct-c);
			y1=a*( (-s*sy)*i + (sy*c)*j ) + x*(sy*s);
			if (x1<0||x1>w-1 || y1<0||y1>h-1)
			{
				address[j][2*i]=-1;
				address[j][2*i+1]=-1;
			}
			else
			{
				address[j][2*i]=x1;
				address[j][2*i+1]=y1;
			}
		}
	}
	return address;
}
#endif

/* map measurements in radial coordinates to a conic profile
 * in cartesian coordinates */
double **getRadialMap(theta,rlen,W,H,ycenter)
	int theta; /* total number of data pixels in angular direction */
	int rlen; /* total number of data pixels in radial direction */
	int W; /* total width of image we are mapping to */
	int H; /* total height of image we are mapping to */
	int ycenter; /* height above image representing center of radial spreading*/
{
	double **address=NULL;
	int i,j;
	double dmax,amax;
	double r,t;
	/* allocate memory for map */
	address=(double**)malloc(H*sizeof(double*));
	assert(address!=NULL);
	address[0]=(double*)malloc(2*H*W*sizeof(double));
	assert(address[0]!=NULL);
	for (j=1;j<H;j++) address[j]=address[0]+2*W*j;

	dmax=(double)(H+ycenter);
	amax=2*asin(0.5*W/dmax);

	for (j=0;j<H;j++)
	{
		for (i=0;i<W;i++)
		{
			r=rlen*sqrt((i-0.5*W)*(i-0.5*W)+(j+ycenter)*(j+ycenter))/dmax;
			/* angle relative to left edge. */
			if ((double)i==0.5*W) t=0.5*theta;
			else t=theta*(atan((i-0.5*W)/(j+ycenter))+0.5*amax)/amax; 
			if (r<0.0 || (int)r>=rlen || t<0.0 || (int)t>=theta) {
				r=-1.0;
				t=-1.0;
			}
			address[j][2*i]=t;
			address[j][2*i+1]=r;
		}
	}
	return address;
}
