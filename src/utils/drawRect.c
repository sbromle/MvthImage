/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw, fill a rectangle in an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "drawRect.h"

void drawPDot(unsigned char *img, int w, int h, int bands,
		int x, int y, int s,
		unsigned char *rgb)
{
	int i,j,k;
	int so2;
	int offsety;
	int offset;

	so2=s/2;

	for (j=-so2+y;j<=so2+y;j++)
	{
		if (j<0||j>h-1) continue;
		offsety=j*w;
		for (i=-so2+x;i<=so2+x;i++)
		{
			if (i<0||i>w-1) continue;
			offset=bands*(offsety+i);
			for (k=0;k<bands;k++) img[offset+k]=rgb[k];
		}
	}
	return;
}

void drawPDotZbuffer(unsigned char *img, int w, int h, int bands,
		int x, int y, int s,
		unsigned char *rgb, float *zbuffer, float depth)
{
	int i,j,k;
	int so2;
	int offsety;
	int offset;

	so2=s/2;

	for (j=-so2+y;j<=so2+y;j++)
	{
		if (j<0||j>h-1) continue;
		offsety=j*w;
		for (i=-so2+x;i<=so2+x;i++)
		{
			if (i<0||i>w-1) continue;
			if (zbuffer[offsety+i]<depth) continue;
			zbuffer[offsety+i]=depth;
			offset=bands*(offsety+i);
			for (k=0;k<bands;k++) img[offset+k]=rgb[k];
		}
	}
	return;
}

void drawRect(unsigned char *img, int w, int h, int bands,
		int x1, int y1, int x2, int y2,
		unsigned char *rgb)
{
	int i,j,k;
	int offset1, offset2;

	if (x1<0) x1=0;
	if (x2>=w) x2=w-1;
	if (y1<0) y1=0;
	if (y2>=h) y2=h-1;

	/* draw the horizontal lines */
	offset1=y1*w*bands;
	offset2=y2*w*bands;
	for (i=x1;i<=x2;i++)
	{
		for (k=0;k<bands;k++)
		{
			img[offset1+bands*i+k]=rgb[k];
			img[offset2+bands*i+k]=rgb[k];
		}
	}
	/* draw vertical lines */
	for (j=y1;j<=y2;j++)
	{
		offset1=j*bands*w;
		for (k=0;k<bands;k++)
		{
			img[offset1+bands*x1+k]=rgb[k];
			img[offset1+bands*x2+k]=rgb[k];
		}
	}

	return;
}

void fillRect(unsigned char *img, int w, int h, int bands,
		Rect_t rect,
		unsigned char *rgb)
{
	int i,j,k;
	unsigned char *ptr;
	unsigned long offset;
	int x1,x2,y1,y2;

	x1=rect.x0;
	y1=rect.y0;
	x2=rect.x1;
	y2=rect.y1;

	if (x1<0) x1=0;
	if (y1<0) y1=0;
	if (x2>=w) x2=w-1;
	if (y2>=h) y2=h-1;

	/* draw all the horizontal lines */
	for (j=y1;j<y2;j++)
	{
		offset=w*j*bands;
		for (i=x1;i<x2;i++)
		{
			ptr=img+offset+bands*i;
			for (k=0;k<bands;k++)
			{
				*ptr=rgb[k];
				ptr++;
			}
		}
	}
	return;
}
