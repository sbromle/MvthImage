/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Zoom a region of an image.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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
#include "base/images_types.h"

image_t * zoom_fltr(image_t *img,
		int x0, int y0, int x1, int y1,
		int w, int h)
{
	image_t *newimg=NULL;
	float *orig;
	int ih,iw;
	int bands;
	double xscale,yscale;
	int i,j,k;

	iw=img->w;
	ih=img->h;
	bands=img->bands;

	newimg=(image_t*)calloc(1,sizeof(image_t));
	newimg->data=(float*)calloc(w*h*bands,sizeof(float));
	newimg->w=w;
	newimg->h=h;
	newimg->bands=bands;

	orig=img->data;

	xscale=(double)(x1-x0)/(double)w;
	yscale=(double)(y1-y0)/(double)h;

	/* do the scaling */
	double v[3];
	for (j=0;j<h;j++)
	{
		int y;
		double yfrac;
		yfrac=(yscale*j+y0);
		y=(int)(yfrac);
		yfrac=yfrac-y;
		if (y<0 || y>ih-1) continue;
		for (i=0;i<w;i++)
		{
			int x;
			double xfrac;
			xfrac=(xscale*i+x0);
			x=(int)(xfrac);
			xfrac=xfrac-x;
			if (x<0 || x>iw-1) continue;
			if (x+1<iw && y+1<ih)
			{
				/* four neighbour interpolation */
				for (k=0;k<bands;k++)
					v[k]=(1.0-xfrac)*(1.0-yfrac)*orig[bands*(y*iw+x)+k]
						+(1.0-xfrac)*yfrac*orig[bands*((y+1)*iw+x)+k]
						+xfrac*yfrac*orig[bands*((y+1)*iw+(x+1))+k]
						+(xfrac)*(1.0-yfrac)*orig[bands*(y*iw+x+1)+k];
			}
			else if (x+1>=iw && y+1<ih)
			{
				/* then interpolate in y direction only */
				for (k=0;k<bands;k++)
					v[k]=(1.0-yfrac)*orig[bands*(y*iw+x)+k]
						+yfrac*orig[bands*((y+1)*iw+x)+k];
			}
			else if (x+1<iw && y+1>=ih)
			{
				/* then interpolate in x direction only */
				for (k=0;k<bands;k++)
					v[k]=(1.0-xfrac)*orig[bands*(y*iw+x)+k]
						+xfrac*orig[bands*(y*iw+(x+1))+k];
			}
			else 
			{
				/* no interpolation */
				for (k=0;k<bands;k++)
					v[k]=orig[bands*(y*iw+x)+k];
			}

			for (k=0;k<bands;k++)
				newimg->data[bands*(i+j*w)+k]=(float)v[k];
		}
	}

	return newimg;
}
