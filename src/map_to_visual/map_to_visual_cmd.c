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
 * Take an image holding plot-able data, and map it
 * to a 2D image for viewing.
 * The data is stored in an image_t structure, similar to
 * regular images, but the depth can be greater than 1,
 * and the number of bands can be much greater than 3.
 *
 * For example, if plotting temperature as a function of
 * time for farmlands in several provinces.
 * The dimensions of the data image could be as follows:
 *  w = 10 (10 provinces)
 *  h = 5 (5 zones within each province)
 *  d = 4 (one for each season)
 *  bands = 365 (one sample for each day of the year).
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
#include <tcl.h>
#include <tclArgv.h>
#include <assert.h>
#include "dynamic_symbols_mvth.h"
#include "base/mvthimagestate.h"

/* Interpret the data image to contain
 * number of samples equal to number of bands
 * (or half if each samples are stored as
 * x,y pairs.)
 */
int visual_map_1d_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	//int cutlen=INT_MAX;
	image_t *dimg=NULL;
	image_t *vimg=NULL;

	int i;
	int dots=0; /* use dots instead of lines */
	int dir=0;		/*plot horizontally*/
	int doErase=0;  /* if set to 1, erase the image before plotting */
	int w0=-1,w1=-1; /*range of data elements along width */
	int h0=-1,h1=-1; /*range of data elements along height */
	int d0=-1, d1=-1; /*range of data elements along depth */
	int t0=0, t1=INT_MAX; /* range of data elements along bands */

	int indexoffset=0; /* starting index to use for coloring*/
	ViewPort_t viewport= {
		{{1.0,5,0,100,"%6.3lg","",AXIS_LINEAR,"",0}, /* x axes */
		{1.0,5,0,0  ,"%6.3lg","",AXIS_LINEAR,"",0}, /* y axes */
		{1.0,5,0,0  ,"%6.3lg","",AXIS_LINEAR,"",0}}, /* z axes */
		0.05,0.95, /* x pixel range */
		0.05,0.95 /* y pixel range */ };

	Tcl_ArgvInfo argTable[] = {
		{"-portrait",TCL_ARGV_CONSTANT,(void*)1,&dir,
			"plot in portrait orientation (vertical channels)"},
		{"-erase",TCL_ARGV_CONSTANT,(void*)1,&doErase,
			"erase image before plotting"},
		{"-dots",TCL_ARGV_CONSTANT,(void*)1,&dots,
			"plot using dots instead of lines"},
		{"-colorindex",TCL_ARGV_INT,NULL,&indexoffset,
			"index to use for initial ping color lookup"},
		{"-w0",TCL_ARGV_INT,NULL,&w0,
			"starting index of width range of data elements to plot"},
		{"-w1",TCL_ARGV_INT,NULL,&w1,
			"final index of width range of data elements to plot"},
		{"-h0",TCL_ARGV_INT,NULL,&h0,
			"starting index of height range of data elements to plot"},
		{"-h1",TCL_ARGV_INT,NULL,&h1,
			"final index of height range of data elements to plot"},
		{"-d0",TCL_ARGV_INT,NULL,&d0,
			"starting index of depth range of data elements to plot"},
		{"-d1",TCL_ARGV_INT,NULL,&d1,
			"final index of depth range of data elements to plot"},
		{"-t0",TCL_ARGV_INT,NULL,&t0,
			"starting index of samples range of data elements to plot"},
		{"-t1",TCL_ARGV_INT,NULL,&t1,
			"final index of samples range of data elements to plot"},
		{"-x",TCL_ARGV_FLOAT,NULL,&viewport.xmin, "left x coordinate of viewport"},
		{"-X",TCL_ARGV_FLOAT,NULL,&viewport.xmax, "right x coordinate of viewport"},
		{"-y",TCL_ARGV_FLOAT,NULL,&(viewport.ymin), "bottom y coordinate of viewport"},
		{"-Y",TCL_ARGV_FLOAT,NULL,&(viewport.ymax), "top y coordinate of viewport"},
		{"-v",TCL_ARGV_FLOAT,NULL,&(viewport.axis[1].min), "lower voltage boundary"},
		{"-V",TCL_ARGV_FLOAT,NULL,&(viewport.axis[1].max), "upper voltage boundary"},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};
	
	Tcl_Obj **remObjv=NULL;

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	if (objc!=3) {
		Tcl_AppendResult(interp,"You must specify both a data mvthimage and a visual mvthimage variable!\n",NULL);
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}
	
	/* check to see if we have a bundle loaded in the gname slot */
	if (getMvthImageFromObj(interp,remObjv[1],&dimg)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,remObjv[2],&vimg)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}
	if (remObjv!=NULL) free(remObjv);

	if (vimg->d!=1) {
		Tcl_AppendResult(interp,"visual_map_1d only supports 2D images.\n",NULL);
		return TCL_ERROR;
	}

	if (t0<0) t0=0;
	if (t1<0) t1=INT_MAX;
	if (t1<=t0)
	{
		Tcl_AppendResult(interp,"Error: T_end must be greater than T_start.\n",NULL);
		return TCL_ERROR;
	}

	viewport.axis[0].min=t0;
	viewport.axis[0].max=t1;

	/* register with undo substructure */
	//register_image_undo_var(iname);

	if (doErase)
	{
		//unsigned char colour=0; /* default as black background */
		//if (img->bands==1) colour=255; /* greyscale image, make BG white */
		float colour=1.0; /* make BG white */
		int xlow=vimg->w*viewport.xmin;
		int xhigh=vimg->w*viewport.xmax;
		int ylow=vimg->h*(1-viewport.ymax);
		int yhigh=vimg->h*(1-viewport.ymin);
		int bands=vimg->bands;
		if (xlow<0) xlow=0;
		if (xhigh>vimg->w-1) xhigh=vimg->w-1;
		if (ylow<0) ylow=0;
		if (yhigh>vimg->h-1) yhigh=vimg->h-1;
		if (xlow<=xhigh && ylow<=yhigh)
		{
			int x,y,k;
			for (y=ylow;y<=yhigh;y++)
				for (x=xlow;x<=xhigh;x++)
					for(k=0;k<bands;k++)
						vimg->data[bands*(y*vimg->w+x)+k]=colour;
		}
	}

	if (w0<0 || w1<0) { w0=0; w1=dimg->w; }
	if (h0<0 || h1<0) { h0=0; h1=dimg->h; }
	if (d0<0 || d1<0) { d0=0; d1=dimg->d; }

	DSYM(visual_map_1d_fltr)(dimg,vimg,viewport,
			w0,w1,h0,h1,d0,d1,t0,t1,
			dir, &indexoffset,
			dots);

	Tcl_SetObjResult(interp,Tcl_NewIntObj(indexoffset));
	return TCL_OK;
}
