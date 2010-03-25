/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Paste one image on top of another.
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
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "base/images_types.h"

#define ABLEND(x,y,a) ((x)*(1.0-(a))+(a)*(y))

void paste_fltr(image_t *src, image_t *dst,int xoff, int yoff, float RGB[6], float alpha)
{
	int xmin,xmax,ymin,ymax;
	int xskip,yskip; /* needed in case xoff < 0 or yoff < 0 */
	int i,j,k;
	int w,h,bands;
	int sw,sh,sbands;
	float *simg,*dimg;
	
	if (src->d!=1 || dst->d!=1) return;

	if (alpha<0) alpha=0;
	else if (alpha>1.0) alpha=1.0;

	simg=src->data;
	dimg=dst->data;

	w=dst->w;
	h=dst->h;
	bands=dst->bands;
	sw=src->w;
	sh=src->h;
	sbands=src->bands;

#ifdef VERBOSE
	fprintf(stderr,"[bands,sbands] = [ %d , %d ]\n",bands,sbands);
#endif

	/* get the ranges */
	xmin=(xoff<w?xoff:w-1);
	ymin=(yoff<h?yoff:h-1);
	
	/* allow for shifting negatively */
	xskip=yskip=0;
	if (xmin<0) {xskip=-xoff; xmin=0;}
	if (ymin<0) {yskip=-yoff; ymin=0;}

	xmax=xmin+sw-xskip;
	ymax=ymin+sh-yskip;

	if (xmax<0 || ymax<0)
	{
		fprintf(stderr,"Asked to paste image entirely outside image boundary.\n");
		return;
	}

	if (xmax>w-1) xmax=w-1;
	if (ymax>h-1) ymax=h-1;

	/* ok, now just do the looping */

	for (j=ymin;j<ymax;j++)
	{
		int voff;
		voff=j-ymin+yskip;
		if (voff>sh-1) break; /* actually, if we've done things correctly,
														 this should never break from here */
		for (i=xmin;i<xmax;i++)
		{
			int hoff;
			hoff=i-xmin+xskip;
			if (hoff>sw-1) break; /* ditto */
			if (bands==sbands)
			{
				if (bands==1)
				{
					if (simg[voff*sw+hoff]<RGB[0] || simg[voff*sw+hoff]>RGB[0]+RGB[3])
						dimg[(j*w+i)]=ABLEND(dimg[(j*w+i)],simg[(voff*sw+hoff)],alpha);
				}
				else
				{
					int transparent=1;
					for (k=0;k<bands;k++)
					{
						if (simg[bands*(voff*sw+hoff)+k]<RGB[k]
								|| simg[sbands*(voff*sw+hoff)+k]>RGB[k]+RGB[k+3])
							transparent=0;
					}
					if (!transparent) {
						for (k=0;k<bands;k++)
						{
							dimg[bands*(j*w+i)+k] =
								ABLEND(dimg[bands*(j*w+i)+k],simg[sbands*(voff*sw+hoff)+k],alpha);
						}
					}
				}
			}
			else if (bands==3 && sbands==1)
			{
				/* then we must replicate the grays */
				if (simg[(voff*sw+hoff)]<RGB[0] || simg[voff*sw+hoff]>RGB[0]+RGB[3])
				{
					for (k=0;k<bands;k++)
					{
						dimg[bands*(j*w+i)+k] =
								ABLEND(dimg[bands*(j*w+i)+k],simg[voff*sw+hoff],alpha);
					}
				}
			}
			else if (bands==1 && sbands==3)
			{
				/* then we must average the intensities */
				float val;
				val=0;
				int transparent=1;
				for (k=0;k<bands;k++)
				{
					val+=simg[sbands*(voff*sw+hoff)+k];
					if (simg[sbands*(voff*sw+hoff)+k]<RGB[k]
							|| simg[sbands*(voff*sw+hoff)+k]>RGB[k]+RGB[k+3])
						transparent=0;
				}
				val/=3.0;
				if (!transparent) {
					dimg[(j*w+i)]=ABLEND(dimg[j*w+i],val,alpha);
				}
			}
			else
			{
				fprintf(stderr,"Invalid bands mismatch detected!\n");
				return;
			}
		}
	}

#ifdef VERBOSE
	fprintf(stderr,"Paste Complete.\n");
#endif
	return;
}

void paste_with_mask_fltr(image_t *src, image_t *dst,int xoff, int yoff, unsigned char *mask)
{
	int xmin,xmax,ymin,ymax;
	int xskip,yskip; /* needed in case xoff < 0 or yoff < 0 */
	int i,j,k;
	int w,h,bands;
	int sw,sh,sbands;
	float *simg,*dimg;

	if (src->d!=1 || dst->d!=1) return;

	simg=src->data;
	dimg=dst->data;

	w=dst->w;
	h=dst->h;
	bands=dst->bands;
	sw=src->w;
	sh=src->h;
	sbands=src->bands;

#ifdef VERBOSE
	fprintf(stderr,"[bands,sbands] = [ %d , %d ]\n",bands,sbands);
#endif

	/* get the ranges */
	xmin=(xoff<w?xoff:w-1);
	ymin=(yoff<h?yoff:h-1);
	
	/* allow for shifting negatively */
	xskip=yskip=0;
	if (xmin<0) {xskip=-xoff; xmin=0;}
	if (ymin<0) {yskip=-yoff; ymin=0;}

	xmax=xmin+sw-xskip;
	ymax=ymin+sh-yskip;

	if (xmax<0 || ymax<0)
	{
		fprintf(stderr,"Asked to paste image entirely outside image boundary.\n");
		return;
	}

	if (xmax>w-1) xmax=w-1;
	if (ymax>h-1) ymax=h-1;

	/* ok, now just do the looping */

	for (j=ymin;j<ymax;j++)
	{
		int voff;
		voff=j-ymin+yskip;
		if (voff>sh-1) break; /* actually, if we've done things correctly,
														 this should never break from here */
		for (i=xmin;i<xmax;i++)
		{
			int hoff;
			hoff=i-xmin+xskip;
			if (hoff>sw-1) break; /* ditto */
			if (mask[voff*sw+hoff]==0) continue;
			/* otherwise paste the sucker! */
			if (bands==sbands) {
				for (k=0;k<bands;k++) 
					dimg[bands*(j*w+i)+k]=simg[bands*(voff*sw+hoff)+k];
			} else if (bands==3 && sbands==1) {
				/* then we must replicate the grays */
				for (k=0;k<bands;k++)
					dimg[bands*(j*w+i)+k]=simg[(voff*sw+hoff)];
			} else if (bands==1 && sbands==3) {
				/* then we must average the intensities */
				float val;
				val=0;
				for (k=0;k<sbands;k++) val+=simg[sbands*(voff*sw+hoff)+k];
				dimg[(j*w+i)]=(val/3.0);
			} else {
				fprintf(stderr,"Invalid bands mismatch detected!\n");
				return;
			}
		}
	}

#ifdef VERBOSE
	fprintf(stderr,"Paste Complete.\n");
#endif
	return;
}
