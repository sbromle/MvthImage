/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Request that an axes be drawn to an image.
 *
 * Copyright (C) 2004 Samuel P. Bromley <sbromley@guigne.com>
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
 * (see the file named "COPYING), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
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
#include "dynamic_symbols_mvth.h"
#include "base/mvthimagestate.h"

enum {MAXTICKS=500};

int axes_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *img=NULL;
	MvthImage *mimg=NULL;

	ViewPort_t axes= {
		{{1.0,9,0,1.0,"%lg","",AXIS_LINEAR,"a",0},
		{1.0,9,0,1.0,"%lg","",AXIS_LINEAR,"a",0},
		{1.0,9,0,1.0,"%lg","",AXIS_LINEAR,"a",0}},
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
	        // ignored variables for compatibility with ::pup::axes
	        {"-axesopts",TCL_ARGV_IGNORE_ARG,NULL,NULL,
			"options for drawing both axes"},
		{"-axesrgb",TCL_ARGV_IGNORE_ARG,NULL,NULL,
			"rgb-triple for axes color"},
		{"-box", TCL_ARGV_IGNORE_ARG,NULL,NULL,
	                "draw a box around the plotting area"},
	        {"-labelrgb",TCL_ARGV_IGNORE_ARG,NULL,NULL,
			"rgb-triple for label color"},
		{"-title",TCL_ARGV_IGNORE_ARG,NULL,NULL,
			"set title label"},
	        //end of custom commands
		{"-xmin",TCL_ARGV_FLOAT,NULL,&axes.axis[0].min,
			"set minimum x value"},
		{"-xmax",TCL_ARGV_FLOAT,NULL,&axes.axis[0].max,
			"set maximum x value"},
		{"-ymin",TCL_ARGV_FLOAT,NULL,&axes.axis[1].min,
			"set minimum x value"},
		{"-ymax",TCL_ARGV_FLOAT,NULL,&axes.axis[1].max,
			"set maximum x value"},
		{"-xmajor",TCL_ARGV_FLOAT,NULL,&axes.axis[0].inc_major,
			"set x major increment value"},
		{"-ymajor",TCL_ARGV_FLOAT,NULL,&axes.axis[1].inc_major,
			"set y major increment value"},
		{"-nxminor",TCL_ARGV_INT,NULL,&axes.axis[0].nminor,
			"set number of minor ticks for x axes"},
		{"-nyminor",TCL_ARGV_INT,NULL,&axes.axis[1].nminor,
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
		strncpy(axes.axis[0].format,xfmt,32);
	if (yfmt!=NULL)
		strncpy(axes.axis[1].format,yfmt,32);

	if (xlabel!=NULL)
		strncpy(axes.axis[0].label,xlabel,256);
	if (ylabel!=NULL)
		strncpy(axes.axis[1].label,ylabel,256);

	/* get option strings*/
	if(xopt==NULL)
		strncpy(axes.axis[0].opt,"a",31);
	else
		strncpy(axes.axis[0].opt,xopt,31);
	if (yopt==NULL)
		strncpy(axes.axis[1].opt,"a",31);
	else
		strncpy(axes.axis[1].opt,xopt,31);

	
	/* select log or linear axes */
	if (xlog) {
		axes.axis[0].type=AXIS_LOGARITHMIC;
		if (axes.axis[0].inc_major<=1.0) axes.axis[0].inc_major=10;
		Tcl_Eval(interp,"puts \"Request for logaritmic x-axis.\n\"");
	}
	if (ylog) {
		axes.axis[1].type=AXIS_LOGARITHMIC;
		if (axes.axis[1].inc_major<=1.0) axes.axis[1].inc_major=10;
		Tcl_Eval(interp,"puts \"Request for logaritmic y-axis.\n\"");
	}

	/*check that xmin<xmax and ymin<ymax*/
	if((axes.axis[1].min>=axes.axis[1].max)||(axes.axis[0].min>=axes.axis[0].max)) {
		Tcl_AppendResult(interp,"axes(): Invalid axes limits\n",NULL);
		return TCL_ERROR;
	}

	/* check that user isn't requesting too many tick marks */
	int nmajx=(int)((axes.axis[0].max-axes.axis[0].min)/axes.axis[0].inc_major);
	int nmajy=(int)((axes.axis[1].max-axes.axis[1].min)/axes.axis[1].inc_major);
	if (nmajx>MAXTICKS || nmajy>MAXTICKS) {
		Tcl_AppendResult(interp,
				"Too many ticks. Try increasing major tick increment.\n",NULL);
		return TCL_ERROR;
	}

	/* get the ping */
	img=mimg->img;

	/* register with the undo substructure */
	//register_image_undo_var(name);

	assert(DSYM(axes_fltr)!=NULL);
	DSYM(axes_fltr)(img,&axes,rgb);
	DSYM(stamp_image_t)(img);
	
	Tcl_ResetResult(interp);
	return TCL_OK;
}
