/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Plot a graph of xy data within a viewport.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "base/images_types.h"
#include "drawLine.h"

int plotxy(float *img, int w, int h, int bands,
		double x[], double y[], size_t len, float *rgb,
		ViewPort_t *viewport)
{
	int *ix=NULL, *iy=NULL;
	double xscale,yscale; /* scale from data to image coords */
	double xmin,ymin;
	int xlow,ylow;
	int xhigh,yhigh;
	int i;
	if (x==NULL || y==NULL || len==0 || img==NULL || viewport==NULL) return -1;

	xmin=viewport->x_axis.min;
	ymin=viewport->y_axis.min;
	xscale=w*(viewport->xmax-viewport->xmin)/(viewport->x_axis.max-viewport->x_axis.min);
	yscale=h*(viewport->ymax-viewport->ymin)/(viewport->y_axis.max-viewport->y_axis.min);
	xlow=(int)(w*viewport->xmin);
	ylow=(int)(h*viewport->ymin);
	xhigh=(int)(w*viewport->xmax);
	yhigh=(int)(h*viewport->ymax);

	/* allocate memory for the drawPolyline arrays */
	ix=(int*)malloc(len*sizeof(int));
	iy=(int*)malloc(len*sizeof(int));
	for (i=0;i<len;i++)
	{
		int xtmp=(int)(xlow+(x[i]-xmin)*xscale+0.5);
		int ytmp=(int)(ylow+(y[i]-ymin)*yscale+0.5);
		if (xtmp<xlow) xtmp=xlow;
		if (xtmp>xhigh) xtmp=xhigh;
		if (ytmp<ylow) ytmp=ylow;
		if (ytmp>yhigh) ytmp=yhigh;
		ix[i]=xtmp;
		iy[i]=h-ytmp;
	}
	drawPolyLine(img,w,h,bands,ix,iy,len,rgb);
	free(ix);
	free(iy);

	return 0;
}
