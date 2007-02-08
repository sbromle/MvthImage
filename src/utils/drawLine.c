/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a line in an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "drawLine.h"

#define CLIPPED 0
#define ALL_OUTSIDE -1
static int clipLine(int *x, int *y, int *dx, int *dy, int w, int h);

void drawLine(float *img, int w, int h, int bands,
		int x, int y, int dx, int dy,
		float *rgb)
{
	float *p;
	int xinc, yinc;
	int i;
	int j;
	int error;
	int ret;

	/* do line clipping */
	if (x<0 || x>=w || x+dx<0 || x+dx>=w
	    || y<0 || y>=h || y+dy<0 || y+dy>=h)
	{
		ret=clipLine(&x,&y,&dx,&dy,w,h);
		if (ret==ALL_OUTSIDE) return;
	}

	p=&img[bands*(y*w+x)];

	if (dx>=0) xinc=bands;
	else 
	{
		xinc=-bands;
		dx=-dx;
	}

	if (dy>=0) yinc=w*bands;
	else
	{
		yinc=-w*bands;
		dy=-dy;
	}

	error=0;

	if (dx>dy)
	{
		for (i=0;i<=dx;i++)
		{
			for (j=0;j<bands;j++) p[j]=rgb[j];
			error+=dy;

			if (error>dx)
			{
				error-=dx;
				p+=yinc;
			}
			
			p+=xinc;
		}
	}
	else
	{
		for (i=0;i<=dy;i++)
		{
			for (j=0;j<bands;j++) p[j]=rgb[j];
			error+=dx;
			if (error>=dy)
			{
				error-=dy;
				p+=xinc;
			}
			p+=yinc;
		}
	}
	return;
}

void drawPolyLine(float *img, int w, int h, int bands,
		int x[], int y[], int n,
		float *rgb)
{
	int dx,dy;
	int i;
	if (n<2) return;

	for (i=0;i<n-1;i++)
	{
		dx=x[i+1]-x[i];
		dy=y[i+1]-y[i];
		drawLine(img,w,h,bands,x[i],y[i],dx,dy,rgb);
	}
	return;
}

static int clipLine(int *xi, int *yi, int *dxi, int *dyi, int w, int h)
{
	int x,y;
	double dx,dy;
	int x2,y2;
	x=*xi;
	y=*yi;
	dx=(double)*dxi;
	dy=(double)*dyi;
	x2=x+dx;
	y2=y+dy;

	//fprintf(stderr,"CLIPLINE: (%d,%d) -> (%d,%d)\n",x,y,x2,y2);

	/* first test for horizontal or vertical lines */
	if (dy==0)
	{
		/* line is horizontal */
		//fprintf(stderr,"Line was horizontal at y=%d\n",y);
		if (y<0 || y>=h) return ALL_OUTSIDE;
		if (x<0) x=0; if (x2<0) x2=0;
		if (x>=w) x=w-1; if (x2>=w) x2=w-1;
		*xi=x; *dxi=x2-x;
		return CLIPPED;
	}
	if (dx==0)
	{
		/* line is vertical */
		//fprintf(stderr,"Line was vertical at x=%d\n",x);
		if (x<0 || x>=w) return ALL_OUTSIDE;
		if (y<0) y=0; if (y2<0) y2=0;
		if (y>=h) y=h-1; if (y2>=h) y2=h-1;
		*yi=y; *dyi=y2-y;
		return CLIPPED;
	}


	/* there a several cases */
	/* case 1: Line is completely visible.
	 * case 2: One point is outside, other point is inside.
	 * case 3: Both points are outside, but line passes though image.
	 * case 4: Entirety of line is outside image.
	 */

	/* easiest case is case 1 */
	if (   x>=0    && x<w
			&& y>=0    && y<h
			&& x+dx>=0 && x+dx<w
			&& y+dy>=0 && y+dy<h)
	{
		//fprintf(stderr,"Line was all inside (%d,%d) -> (%d,%d)\n",x,y,x2,y2);
		return CLIPPED;
	}

	/* test most of case 4 */
	if ( (x<0 && x2<0) || /* all to the left */
			 (x>=w && x2>=w) || /* all to the right */
			 (y<0 && y2<0) || /* all above */
			 (y>=h && y2>=h)) /*all below */
	{
		//fprintf(stderr,"Line was entirely outside\n");
		return ALL_OUTSIDE;
	}

	/* two cases left */
	/* 1) Only one point is outside the image
	 * 2) Both points outside, but line intersects image.
	 */

	/* Can just handle each point that is outside */
	
	/* test point 1 to see if it is outside */
	if (x<0 || x>=w || y<0 || y>=h)
	{
		/* then point 1 is outside, and we should
		 * clip it to the appropriate boundaries */
		//fprintf(stderr,"Point 1 was outside (%d,%d)\n",x,y);

		/* test vertical boundaries first */
		if (x<0)
		{
			/* then clip to left edge */
			y=(int)(0.5+y2+(dy/dx)*x2);
			x=0;
		}
		else if (x>=w)
		{
			/* then clip to right edge */
			y=(int)(0.5+y2+(dy/dx)*(w-x2-1));
			x=w-1;
			//fprintf(stderr,"P1 right clipped to (%d,%d)\n",x,y);
		}

		/* now test the vertical boundaries */
		if (y<0)
		{
			/* then clip to top edge */
			x=(int)(0.5+x2-(dx/dy)*y2);
			y=0;
		}
		else if (y>=h)
		{
			/* then clip to bottom edge */
			x=(int)(0.5+x2+(dx/dy)*(h-1-y2));
			y=h-1;
		}
		//fprintf(stderr,"Point 1 clipped to (%d,%d)\n",x,y);
	}

	/* test point 2 to see if it is outside */
	if (x2<0 || x2>=w || y2<0 || y2>=h)
	{
		/* then point 2 is outside, and we should
		 * clip it to the appropriate boundaries */
		//fprintf(stderr,"Point 2 was outside (%d,%d)\n",x2,y2);

		/* test vertical boundaries first */
		if (x2<0)
		{
			/* then clip to left edge */
			y2=(int)(0.5+y+(dy/dx)*x);
			x2=0;
		}
		else if (x2>=w)
		{
			/* then clip to right edge */
			y2=(int)(0.5+y+(dy/dx)*(w-x-1));
			x2=w-1;
			//fprintf(stderr,"P2 clipped to right edge: (%d,%d)\n",x2,y2);
		}

		/* now test the vertical boundaries */
		if (y2<0)
		{
			/* then clip to top edge */
			x2=(int)(0.5+x+(dx/dy)*y);
			y2=0;
			//fprintf(stderr,"P2 clipped to top edge: (%d,%d)\n",x2,y2);
		}
		else if (y2>=h)
		{
			/* then clip to bottom edge */
			x2=(int)(0.5+x+(dx/dy)*(h-1-y));
			y2=h-1;
			//fprintf(stderr,"P2 clipped to bottom edge: (%d,%d)\n",x2,y2);
		}
		//fprintf(stderr,"Point 2 clipped to (%d,%d)\n",x2,y2);
	}

	*xi=x;
	*yi=y;
	*dxi=x2-x;
	*dyi=y2-y;

	return CLIPPED;
}

/* draw dots at specified locations */
void drawDots(float *img, int w, int h, int bands,
		int x[], int y[], int n,
		float *rgb)
{
	int i,k;
	for (i=0;i<n;i++)
	{
		if (x[i]<0 || x[i]>=w || y[i]<0 || y[i]>=h) continue;
		for (k=0;k<bands;k++)
			img[bands*(y[i]*w+x[i])+k]=rgb[k];
	}
	return;
}
