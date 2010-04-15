/*
 * This file is part of MVTH - the Multipurpose Versatile Test Harness.
 * Copyright (C) 2010 Samuel P. Bromley <sam@sambromley.com>
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
 * -----
 * Take a PObj1D_t structure holding plot-able data, and map it
 * to a 2D image for viewing.
 * The data is stored in an PObj1D_t structure.
 *
 * For example, if plotting temperature as a function of
 * time for farmlands in several provinces.
 * The dimensions of the data image could be as follows:
 *  w = 10 (10 provinces)
 *  h = 5 (5 zones within each province)
 *  d = 4 (one for each season)
 *  bands = 1 (temperature is a scalar)
 *  len = 265 (one sample for each day of the year).
 *
 *
 *
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#include "images_types.h"
#include "mvth_colors.h"

/* Interpret the data image to contain
 * number of samples equal to number of bands
 */
int visual_map_1d_fltr(PObj1D_t *dimg, image_t *vimg, ViewPort_t viewport,
			int w0,int w1,int h0,int h1,int d0,int d1, int t0, int t1,
			int dir, int *indexoffset, int dots)
{
	double vmin,vmax;
	/* local aliases */
	int w,h,d,t;
	float *x=NULL;
	float *volts=NULL;

	//fprintf(stderr,"On enter indexoffset=%d\n",*indexoffset);

	if (w0<0) w0=0;
	if (w1>=dimg->w) w1=dimg->w-1;
	if (h0<0) h0=0;
	if (h1>=dimg->h) h1=dimg->h-1;
	if (d0<0) d0=0;
	if (d1>=dimg->d) d1=dimg->d-1;
	if (t0<0) t0=0;
	if (t1>=dimg->len) t1=dimg->len-1;
	/* for now, just support y-data, can add x-data later */

	fprintf(stderr,"w0=%d w1=%d w=%d\n", w0,w1,dimg->w);
	fprintf(stderr,"h0=%d h1=%d h=%d\n", h0,h1,dimg->h);
	fprintf(stderr,"d0=%d d1=%d d=%d\n", d0,d1,dimg->d);
	fprintf(stderr,"t0=%d t1=%d t=%d\n", t0,t1,dimg->bands);

	if (viewport.axis[0].max==(double)INT_MAX)
	{
		viewport.axis[0].max=(double)dimg->bands-1;
	}
	
	x=(float*)malloc(dimg->bands*sizeof(float));
	for (t=0;t<(int)dimg->bands;t++) x[t]=(float)t;

	vmin=FLT_MAX;
	vmax=-FLT_MAX;
	float vtmp;
	for (w=w0;w<=w1;w++) {
		for (h=h0;h<=h1;h++) {
			for (d=d0;d<=d1;d++) {
				for (t=t0;t<=t1;t++) {
					vtmp=POBJ_3D_GET_VALUE(dimg,w,h,d,t);
					if (vmin>vtmp) vmin=vtmp;
					if (vmax<vtmp) vmax=vtmp;
				}
			}
		}
	}

	if (viewport.axis[1].max==viewport.axis[1].min) {
		viewport.axis[1].min=vmin;
		viewport.axis[1].max=vmax;
	}

	fprintf(stderr,"Measured [vmin,vmax]=[%lg,%lg]\n",vmin,vmax);
	fprintf(stderr,"Using     [vmin,vmax]=[%lg,%lg]\n",
			viewport.axis[1].min,
			viewport.axis[1].max);

	unsigned int totalplotted=0;
	for (w=w0;w<=w1;w++) {
		for (h=h0;h<=h1;h++) {
			for (d=d0;d<=d1;d++) {
				unsigned int cindex;
				float rgb[4];
				if (vimg->bands!=1) {
					cindex=(totalplotted+(*indexoffset)) % mvth_num_colors;
					rgb[0]=mvth_colors[cindex][0];
					rgb[1]=mvth_colors[cindex][1];
					rgb[2]=mvth_colors[cindex][2];
					rgb[3]=1.0;
				} else {
					cindex=(totalplotted+(*indexoffset)) % mvth_num_grays;
					rgb[0]=mvth_grays[cindex][0];
					rgb[1]=mvth_grays[cindex][1];
					rgb[2]=mvth_grays[cindex][2];
					rgb[3]=1.0;
				}
				totalplotted++;
				if (dir==0)
				{
					if (!dots)
						plotxyF(vimg->data,vimg->w,vimg->h,vimg->bands,
								x,POBJ_3D_GET_PTR(dimg,w,h,d,0),(size_t)dimg->bands,rgb,&viewport);
					else
						plotxydotsF(vimg->data,vimg->w,vimg->h,vimg->bands,
								x,POBJ_3D_GET_PTR(dimg,w,h,d,0),(size_t)dimg->bands,rgb,&viewport);
				} else { /* PLOT VERTICALLY */
					double tmpx,tmpy;
					/* swap the p->v x and y axes */
					tmpx=viewport.axis[0].min;
					tmpy=viewport.axis[1].min;
					viewport.axis[0].min=tmpy;
					viewport.axis[1].min=tmpx;
					tmpx=viewport.axis[0].max;
					tmpy=viewport.axis[1].max;
					viewport.axis[0].max=tmpy;
					viewport.axis[1].max=tmpx;
					/* plot the line */
					if (!dots)
						plotxyF(vimg->data,vimg->w,vimg->h,vimg->bands,
								x,POBJ_3D_GET_PTR(dimg,w,h,d,0),(size_t)dimg->bands,rgb,&viewport);
					else
						plotxydotsF(vimg->data,vimg->w,vimg->h,vimg->bands,
								x,POBJ_3D_GET_PTR(dimg,w,h,d,0),(size_t)dimg->bands,rgb,&viewport);
				}
			}
		}
	}
	*indexoffset+=totalplotted;
	
	free(x);
	return 1;
}
