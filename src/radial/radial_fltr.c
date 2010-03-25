/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Remove radial lens distortion.
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


#include "base/images_types.h"

static int rootByBisection(double i, double j,
		double *x, double *y,
		double k1, double k2,
		double maxx, double maxy,
		double tol);

/* Takes WImage and params[4]={cx,cy,kappa1,kappa2} */
void radial_fltr(image_t *img, double *params)
{
	float *nimg;
	float *orig;
	int h,w;
	int bands;
	int k;
	int ix,iy; /* integer values for scan */
	double xfrac,yfrac; /* fractional values for accumutor */
	double cx,cy;
	double kappa1,kappa2;
#ifdef NORMALIZED
	double factor,ifactor;
	double xfactor,yfactor;
	double dmax;
	double dxmax,dymax;
#endif
	int nfail;
	int ret;

	int i,j;
	double x,y;

	w=img->w;
	h=img->h;
	bands=img->bands;
	if (img->d!=1) return;

	/* new image */
	nimg=(float*)calloc(w*h*bands,sizeof(float));

	orig=img->data;

	cx    =params[0];
	cy    =params[1];
	kappa1=params[2];
	kappa2=params[3];

#ifdef NORMALIZED
	dxmax= ( w-cx>cx     ? w-cx  : cx);
	dymax= ( h-cy>cy     ? h-cy  : cy);
	dmax = ( dxmax>dymax ? dxmax : dymax);
 
	factor=1.0/dmax;
	ifactor=1.0/factor;
	xfactor=1.0/w;
	yfactor=1.0/w; /* NOTE: TESTING width !!! */
	dymax*=factor;
	dxmax*=factor;
#endif

	nfail=0;
	/* do things using bisection! */
	for (j=0;j<h;j++)
	{
		for (i=0;i<w;i++)
		{
			double normx,normy;
#ifdef NORMALIZED
			normx=xfactor*(i-cx);
			normy=yfactor*(j-cy);
#else
			normx=(i-cx);
			normy=(j-cy);
#endif
			/* find the x/y coords that solve the inverse mapping */
			//rootByBisection(normx,normy,&x,&y,kappa1,kappa2,dxmax,dymax,0.001*factor);
#ifdef NORMALIZED
			ret=rootByBisection(normx,normy,&x,&y,kappa1,kappa2,1.0,1.0,0.0001);
#else
			ret=rootByBisection(normx,normy,&x,&y,kappa1,kappa2,w,h,0.1);
#endif
			if (ret==-1) nfail++;
			//fprintf(stderr,"Got roots %lf %lf\n",x,y);
			/* and do the weighting */
			/*
			x*=ifactor;
			y*=ifactor;
			*/
#ifdef NORMALIZED
			x/=xfactor;
			y/=yfactor;
#endif

			ix=(int)(x+cx);
			iy=(int)(y+cy);
			/*
			if (ix<0) ix=0;
			if (iy<0) iy=0;
			if (ix>=w-1) ix=w-2;
			if (iy>=h-1) iy=h-2;
			*/
			if (ix<0 || iy<0 || ix>=w-1 || iy>=h-1) continue;
			xfrac=x+cx-ix;
			yfrac=y+cy-iy;
			if (xfrac<0) xfrac+=1.0;
			if (yfrac<0) yfrac+=1.0;
			for (k=0;k<bands;k++)
			{
				nimg[bands*(j*w+i)+k] = (float)(
					  (1-xfrac)*(1-yfrac)   * orig[bands*(w*iy+ix)+k]
					+ (xfrac)*(1-yfrac)     * orig[bands*(w*iy+ix+1)+k]
					+ (1-xfrac)*(yfrac)     * orig[bands*(w*(iy+1)+ix)+k]
					+ (xfrac)*(yfrac)       * orig[bands*(w*(iy+1)+ix+1)+k]
						);
			}
		}
	}
	fprintf(stderr,"Failed to converge %d times out of %d\n",nfail,w*h);

	memcpy(img->data,nimg,w*h*bands*sizeof(float));
	free(nimg);
}

#include <float.h>
static double g(double p, double x, double k1, double k2);

static int rootByBisection(double i, double j,
		double *x, double *y,
		double k1, double k2,
		double maxx, double maxy,
		double tol)
{
	double xh,yh;
	double xl,yl;
	int maxN=1000;
	int k;
	double p;
	/* want to solve for x, y such that
	 * i=x*(1 + k1 r^2 + k2 r^4)
	 * j=y*(1 + k1 r^2 + k2 r^4)
	 * using bisection.
	 */
	/* DBL_EPSILON */

	if (i==0 && j==0 ) {
		*x=*y=0;
		return 0;
	}
	else if (i==0) /* and j!=0 */
	{
		*x=0;
		/* must find solution to j-y*(1+k1r^2+k2r^4)=0 */
		k=0;
		yl=-maxy; yh=maxy;
		while (k<maxN)
		{
			p=0.5*(yh+yl);
			if (g(j,p,k1,k2)==0 || 0.5*(yh-yl)<tol)
			{
				*y=p;
				return k;
			}
			k++;
			if (g(j,yl,k1,k2)*g(j,p,k1,k2)>0) yl=p;
			else yh=p;
		}
		*y=maxy;
		return -1;
	}
	else if (j==0)
	{
		*y=0;
		/* must find solution to j-y*(1+k1r^2+k2r^4)=0 */
		k=0;
		xl=-maxx; xh=maxx;
		while (k<maxN)
		{
			p=xl+0.5*(xh-xl);
			if (g(i,p,k1,k2)==0 || 0.5*(xh-xl)<tol)
			{
				*x=p;
				return k;
			}
			k++;
			if (g(i,xl,k1,k2)*g(i,p,k1,k2)>0) xl=p;
			else xh=p;
		}
		*x=maxx;
		return -1;
	}
	else
	{
		/* both i and j are nonzero */
		double frac;
		if (fabs(i)>fabs(j))
		{
			frac=(1+(j/i)*(j/i));
			k=0;
			xl=-maxx; xh=maxx;
			while (k<maxN)
			{
				p=0.5*(xh+xl);
				if (g(i,p,frac*k1,frac*frac*k2)==0 || 0.5*(xh-xl)<tol)
				{
					*x=p;
					*y=(j/i)*p;
					return k;
				}
				k++;
				if (g(i,xl,frac*k1,frac*frac*k2)*g(i,p,frac*k1,frac*frac*k2)>0) xl=p;
				else xh=p;
			}
		}
		else
		{
			frac=(1+(i/j)*(i/j));
			k=0;
			yl=-maxy; yh=maxy;
			while (k<maxN)
			{
				p=yl+0.5*(yh-yl);
				if (g(j,p,frac*k1,frac*frac*k2)==0 || 0.5*(yh-yl)<tol)
				{
					*y=p;
					*x=(i/j)*p;
					return k;
				}
				k++;
				if (g(j,yl,frac*k1,frac*frac*k2)*g(j,p,frac*k1,frac*frac*k2)>0) yl=p;
				else yh=p;
			}
		}

		*x=maxx;
		*y=maxy;
		return -1;
	}

}

static double g(double p, double x, double k1, double k2)
{
	return p-x*(1+x*x*(k1+k2*x*x));
}
		

