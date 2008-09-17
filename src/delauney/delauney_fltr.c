/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Triangulate a sparse disparity map.
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
#include <assert.h>


#include "base/images_types.h"
#include "base/stereo_context_t.h"
#include "utils/getColor.h"
#include "utils/drawLine.h"
#include "defs.h"
#include "geometry.h"
#include "voronoi.h"
#include "vor_memory.h"

extern int verbose;

int np=0;
int *coords=NULL;

struct Site *getnextcoords();

void delauney_fltr(image_t *img,SContext_t *sc)
{
//	unsigned char *img;
//	Rect_t rect;
//	int inc;
	int i,j;
//	unsigned char rgb[3];
	int h,w,range;
//	int iw,ih; /* image width*/
//	unsigned char *r,*g,*b;
//	int bands;
	float *dmap;
	int ixmin,iymin,ixmax,iymax;
	int k;
	int *vertices=NULL;
	int nv;


	/* is the dmap valid ? */
	if (sc->dmap==NULL)
	{
		fprintf(stderr,"dmap is NULL in stereo_context!\n");
		return;
	}

	h=sc->h;
	w=sc->w;
	range=sc->range;
	dmap=sc->dmap;

	/* assemble the coordinates of the valid disparity points */
	np=0;
	for (i=0;i<w*h;i++) if (dmap[i]>=0) np++;
	if (np==0)
	{
		fprintf(stderr,"No valid points in dmap!\n");
		return;
	}
	else
	{
		fprintf(stderr,"Got %d valid points\n",np);
	}


	coords=(int*)malloc(2*np*sizeof(int));
	k=0;
	ixmax=iymax=0;
	ixmin=w-1;
	iymin=h-1;
	for (j=0;j<h;j++)
		for (i=0;i<w;i++)
			if (dmap[j*w+i]>=0) 
			{
				coords[2*k]=i;
				coords[2*k+1]=j;
				k++;
				if (ixmax<i) ixmax=i;
				if (ixmin>i) ixmin=i;
				if (iymax<j) iymax=j;
				if (iymin>j) iymin=j;
			}

	/* ok now we have the number of vertices, and their locations,
	 * triangulate them */
	nsites=np;
	xmax=(float)ixmax;
	xmin=(float)ixmin;
	ymax=(float)iymax;
	ymin=(float)iymin;
	siteidx=0;
	vor_freeinit(&sfl,sizeof *sites);
	geominit();
	vertices=voronoi(getnextcoords,&nv);

	for (i=0;i<nv;i++)
	{
		int x1,x2,x3;
		int y1,y2,y3;
		float rgb[3]={1.0,1.0,1.0};
		x1=coords[2*vertices[3*i]];
		y1=coords[2*vertices[3*i]+1];
		x2=coords[2*vertices[3*i+1]];
		y2=coords[2*vertices[3*i+1]+1];
		x3=coords[2*vertices[3*i+2]];
		y3=coords[2*vertices[3*i+2]+1];
		getDisparityColor(dmap[y1*w+x1],rgb);
		drawLine(img->data,img->w,img->h,img->bands,img->w*img->bands,x1,y1,x2-x1,y2-y1,rgb);
		getDisparityColor(dmap[y2*w+x2],rgb);
		drawLine(img->data,img->w,img->h,img->bands,img->w*img->bands,x2,y2,x3-x2,y3-y2,rgb);
		getDisparityColor(dmap[y3*w+x3],rgb);
		drawLine(img->data,img->w,img->h,img->bands,img->w*img->bands,x3,y3,x1-x3,y1-y3,rgb);
	}

	free(coords);
	free(vertices);
	return;
}

struct Site *getnextcoords()
{
	struct Site *s;
	if (siteidx==nsites) return (struct Site*)NULL;
	s=(struct Site *)vor_getfree(&sfl);
	s->refcnt=0;
	s->sitenbr=siteidx;
	s->coord.x=(float)coords[2*siteidx];
	s->coord.y=(float)coords[2*siteidx+1];
	siteidx++;
	return s;
}
