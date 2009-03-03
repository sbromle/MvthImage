/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Plot a polynomial to an images within a viewport.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sbromley@guigne.com>
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
#include <float.h>

#include "base/images_types.h"
#include "utils/plotxy.h"

int plotpoly(image_t *img, ViewPort_t *viewport,
		int order, double *coefs, float rgb[4])
{
	int i,j;
	int len=0;
	double *xdata,*ydata;
	/* local aliases */
	double xmin;
	double xscale;

	/* get number of pixels in x direction */
	len=(int)((img->w)*(viewport->xmax-viewport->xmin));
	/* get x range */
	xdata=(double*)malloc(len*sizeof(double));
	ydata=(double*)malloc(len*sizeof(double));

	xmin=viewport->axis[0].min;
	/* x scaling from pixels to data */
	xscale=(viewport->axis[0].max-viewport->axis[0].min)/(img->w*(viewport->xmax-viewport->xmin));
  
	/* generate the data */
	for (i=0;i<len;i++)
	{
		double x=xscale*i+xmin;;
		xdata[i]=x;
		ydata[i]=coefs[order];
		for (j=order-1;j>=0;j--)
		{
			ydata[i]=ydata[i]*x+coefs[j];
		}
	}
	
	plotxy(img->data,img->w,img->h,img->bands,xdata,ydata,len,rgb,viewport);

	free(xdata);
	free(ydata);
	return 0;
}
