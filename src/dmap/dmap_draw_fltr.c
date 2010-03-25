/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a disparity map.
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
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include "base/images_types.h"
#include "utils/getColor.h"
#include "base/stereo_context_t.h"

extern int verbose;

/* if scale<0 use lookup table, otherwise just scale by scale */
int dmap_draw_fltr(image_t *wimg, SContext_t *sc, float scale)
{
	float *img;
	//Rect_t rect;
	//int inc;
	int i,j;
	float rgb[4];
	int h,w,range;
	int horopter;
	int iw,ih; /* image width*/
	float *r,*g,*b;
	int bands;
	float *dmap;
	int nvalid;


	/* confirm that the wimg can hold the dmap */
	if (sc->dmap==NULL)
	{
		fprintf(stderr,"dmap is NULL in stereo_context!\n");
		return 0;
	}
	else if (wimg->w<sc->w || wimg->h<sc->h)
	{
		fprintf(stderr,"WImage too small for stereo_context.\n");
		return 0;
	}

	if (wimg->d!=1) {
		fprintf(stderr,"dmap_draw_fltr only supports 2D images.\n");
		return 0;
	}

	h=sc->h;
	w=sc->w;
	iw=wimg->w;
	ih=wimg->h;
	range=sc->range;
	horopter=sc->horopter;
	if (verbose) fprintf(stderr,"DRAW_DMAP: h=%d, w=%d, range=%d, horopter=%d\n",
			w,h,range,horopter);
	dmap=sc->dmap;
	img=wimg->data;
	bands=wimg->bands;

	/* clear out the old values */
	memset(img,0,iw*ih*bands*sizeof(float));
	
	/* initialize the lookup table */
	if (scale<=0.0)
		init_dmap_lut(sc);
	/* draw the map */
	nvalid=0;
	if (bands==3)
	{
		for (j=0;j<h;j++)/* scan over the dmap rows */
		{
			r=&img[3*j*iw];
			g=r+1;
			b=r+2;
			for (i=0;i<w;i++) /* scan over the dmap columns */
			{
				if (*dmap>=0)
				{
					if (scale<=0.0)
					{
						getDisparityColor(*dmap-horopter,rgb);
					}
					else 
					{
						rgb[0]=rgb[1]=rgb[2]=(float)(scale*(*dmap-horopter));
						rgb[3]=0.0;
					}
					*r=rgb[0];
					*g=rgb[1];
					*b=rgb[2];
					nvalid++;
				}
				else
				{
					/* Don't like white */
					/*
					*r=255;
					*g=255;
					*b=255;
					*/
					*r=*g=*b=0; /* use black. */
				}
				r+=3;
				g+=3;
				b+=3;
				dmap++;
			}
		}
	}
	else /* bands==1 */
	{
		for (j=0;j<h;j++)
		{
			r=&img[j*iw];
			for (i=0;i<w;i++)
			{
				if (*dmap>=0)
				{
					if (scale<=0.0)
					{
						getDisparityColor(*dmap-horopter,rgb);
						*r=(float)((rgb[0]+rgb[1]+rgb[2])/3.0);
					}
					else
					{
						*r=(float)scale*(*dmap-horopter);
					}
					nvalid++;
				}
				else
				{
					/* *r=255;*/ /* use black instead */
					*r=0;
				}
				r++;
				dmap++;
			}
		}
	}
#if 0
	/* and draw a legend up the left column */
	rect.x0=0;
	rect.x1=3; /* 3 pixels wide */
	inc=(int)(ih/(float)(range+1)); /* number of pixels per disparity */
	for (i=0;i<=range;i++)
	{
		rect.y0=i*inc; 
		rect.y1=(i+1)*inc;
		getDisparityColor((float)i,rgb);
		if (bands==3) fillRect(img,iw,ih,3,rect,rgb);
		else fillRect(img,iw,ih,1,rect,rgb);
	}
#endif
	return nvalid;
}
