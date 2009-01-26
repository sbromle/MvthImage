/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Request that an axes be drawn to an image.
 *
 * Copyright (C) 2004 Samuel P. Bromley <sbromley@guigne.com>
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
#include <errno.h>
#include <tcl.h>
#include <tclArgv.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"
#include "axes_fltr.h"

enum {MAXTICKS=500};

int axes_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *img=NULL;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	int (*axes_fltr)(image_t *img,ViewPort_t *viewport, float rgb[4])=NULL;

	ViewPort_t axes= {
		{1.0,9,0,1.0,"%lg","",AXIS_LINEAR,"a",0},
		{1.0,9,0,1.0,"%lg","",AXIS_LINEAR,"a",0},
		{1.0,9,0,1.0,"%lg","",AXIS_LINEAR,"a",0},
		0.1,0.9,
		0.1,0.9
	};
	char *xfmt=NULL;
	char *yfmt=NULL;
	char *xlabel=NULL;
	char *ylabel=NULL;
	int xlog=0;
	int ylog=0;

	char *xopt=NULL;
	char *yopt=NULL;

	float rgb[4]={1.0,1.0,1.0,1.0};
	double r=1.0,g=1.0,b=1.0;

	/* build the options table */
	Tcl_ArgvInfo argTable[] = {
		{"-xmin",TCL_ARGV_FLOAT,NULL,&axes.x_axis.min,
			"set minimum x value"},
		{"-xmax",TCL_ARGV_FLOAT,NULL,&axes.x_axis.max,
			"set maximum x value"},
		{"-ymin",TCL_ARGV_FLOAT,NULL,&axes.y_axis.min,
			"set minimum x value"},
		{"-ymax",TCL_ARGV_FLOAT,NULL,&axes.y_axis.max,
			"set maximum x value"},
		{"-xmajor",TCL_ARGV_FLOAT,NULL,&axes.x_axis.inc_major,
			"set x major increment value"},
		{"-ymajor",TCL_ARGV_FLOAT,NULL,&axes.y_axis.inc_major,
			"set y major increment value"},
		{"-nxminor",TCL_ARGV_INT,NULL,&axes.x_axis.nminor,
			"set number of minor ticks for x axes"},
		{"-nyminor",TCL_ARGV_INT,NULL,&axes.y_axis.nminor,
			"set number of minor ticks for y axes"},
		{"-xlog",TCL_ARGV_CONSTANT,(void*)1,&xlog, "set x axes as logarithmic"},
		{"-ylog",TCL_ARGV_CONSTANT,(void*)1,&ylog, "set x axes as logarithmic"},
		{"-xfmt",TCL_ARGV_STRING,NULL,&xfmt, "set x format string for tick labels"},
		{"-yfmt",TCL_ARGV_STRING,NULL,&yfmt, "set y format string for tick labels"},
		{"-xlabel",TCL_ARGV_STRING,NULL,&xlabel, "set x label"},
		{"-ylabel",TCL_ARGV_STRING,NULL,&ylabel, "set y label"},
		{"-i",TCL_ARGV_FLOAT,NULL,&axes.xmin,
			"set left x margin (fraction) of image"},
		{"-I",TCL_ARGV_FLOAT,NULL,&axes.xmax,
			"set right x margin (fraction) of image"},
		{"-j",TCL_ARGV_FLOAT,NULL,&axes.ymin,
			"set bottom y margin (fraction) of image"},
		{"-J",TCL_ARGV_FLOAT,NULL,&axes.ymax,
			"set top y margin (fraction) of image"},
		{"-xopt",TCL_ARGV_STRING,NULL,&xopt,
			"set x format string for x-axis options"},
		{"-yopt",TCL_ARGV_STRING,NULL,&yopt,
			"set y format string for y-axis options"},
		{"-red",TCL_ARGV_FLOAT,NULL,&r,
			"set red component of color to use for axes"},
		{"-green",TCL_ARGV_FLOAT,NULL,&g,
			"set green component of color to use for axes"},
		{"-blue",TCL_ARGV_FLOAT,NULL,&b,
			"set blue component of color to use for axes"},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};

	Tcl_Obj **remObjv=NULL;

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);;
		return TCL_ERROR;
	}

	/* make sure we still have an image specified */
	if (objc!=2) {
		Tcl_AppendResult(interp,"You must specify an image variable!\n",NULL);
		if (remObjv!=NULL) free(remObjv);;
		return TCL_ERROR;
	}
	if (getMvthImageFromObj(interp,remObjv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	if (remObjv!=NULL) free(remObjv);;

	/* set up the rgb colour array */
	rgb[0]=(float)r;
	rgb[1]=(float)g;
	rgb[2]=(float)b;
	rgb[3]=0;

	/* get format strings */
	if (xfmt!=NULL)
		strncpy(axes.x_axis.format,xfmt,32);
	if (yfmt!=NULL)
		strncpy(axes.y_axis.format,yfmt,32);

	if (xlabel!=NULL)
		strncpy(axes.x_axis.label,xlabel,256);
	if (ylabel!=NULL)
		strncpy(axes.y_axis.label,ylabel,256);

	/* get option strings*/
	if(xopt==NULL)
		strncpy(axes.x_axis.opt,"a",31);
	else
		strncpy(axes.x_axis.opt,xopt,31);
	if (yopt==NULL)
		strncpy(axes.y_axis.opt,"a",31);
	else
		strncpy(axes.y_axis.opt,xopt,31);

	
	/* select log or linear axes */
	if (xlog) {
		axes.x_axis.type=AXIS_LOGARITHMIC;
		if (axes.x_axis.inc_major<=1.0) axes.x_axis.inc_major=10;
		Tcl_Eval(interp,"puts \"Request for logaritmic x-axis.\n\"");
	}
	if (ylog) {
		axes.y_axis.type=AXIS_LOGARITHMIC;
		if (axes.y_axis.inc_major<=1.0) axes.y_axis.inc_major=10;
		Tcl_Eval(interp,"puts \"Request for logaritmic y-axis.\n\"");
	}

	/*check that xmin<xmax and ymin<ymax*/
	if((axes.y_axis.min>=axes.y_axis.max)||(axes.x_axis.min>=axes.x_axis.max)) {
		Tcl_AppendResult(interp,"axes(): Invalid axes limits\n",NULL);
		return TCL_ERROR;
	}

	/* check that user isn't requesting too many tick marks */
	int nmajx=(int)((axes.x_axis.max-axes.x_axis.min)/axes.x_axis.inc_major);
	int nmajy=(int)((axes.y_axis.max-axes.y_axis.min)/axes.y_axis.inc_major);
	if (nmajx>MAXTICKS || nmajy>MAXTICKS) {
		Tcl_AppendResult(interp,
				"Too many ticks. Try increasing major tick increment.\n",NULL);
		return TCL_ERROR;
	}

	/* get the ping */
	img=mimg->img;

	/* register with the undo substructure */
	//register_image_undo_var(name);

	axes_fltr=load_symbol(MVTHIMAGELIB,"axes_fltr",&libhandle);
	assert(axes_fltr!=NULL);
	axes_fltr(img,&axes,rgb);
	stamp_image_t(img);
	
	release_handle(&libhandle);

	Tcl_ResetResult(interp);
	return TCL_OK;
}
