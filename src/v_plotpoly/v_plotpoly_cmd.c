/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Plot a polynomial within a viewport.
 *
 * Copyright (C) 2005, 2006 Samuel P. Bromley <sbromley@guigne.com>
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
#include <limits.h>
#include <float.h>
#include <math.h>
#include <assert.h>
/* include the GNU command argument handling header */
#include <tcl.h>
#include <tclArgv.h>
#include <memUtils.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"
#include "v_plotpoly_fltr.h"

int plotpoly_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int i; 
	//int cutlen=INT_MAX;
	image_t *mi=NULL;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	int (*plotpoly)(image_t *img, ViewPort_t *viewport,
		int order, double *coefs, float rgb[4])=NULL;
	
	int doErase=0;
	double r=1.0,g=1.0,b=1.0; /* color to draw in */
	int order=1; /* default to a line plot */
	double *coefs=NULL; /* coefficients of polynomial */
	ViewPort_t viewport= {
		{10,5,0,100,"%6.3lg",AXIS_LINEAR,"",0}, /* x axes */
		{1.0,5,0,1.0,"%6.3lg",AXIS_LINEAR,"",0}, /* y axes */
		{1.0,5,0,1.0,"%6.3lg",AXIS_LINEAR,"",0}, /* z axes */
		0.05,0.95, /* x pixel range */
		0.05,0.95 /* y pixel range */ };
	Tcl_ArgvInfo argTable[] = {
		{"-erase",TCL_ARGV_CONSTANT,(void*)1,&doErase,"erase image before plotting"},
		{"-red"  ,TCL_ARGV_FLOAT,NULL,&r,"red value of plot"},
		{"-green",TCL_ARGV_FLOAT,NULL,&g,"green value of plot"},
		{"-blue" ,TCL_ARGV_FLOAT,NULL,&b,"blue value of plot"},
		{"-ilow",TCL_ARGV_FLOAT,NULL,&viewport.xmin,
			"left image coordinate of viewport"},
		{"-ihigh",TCL_ARGV_FLOAT,NULL,&viewport.xmax,
			"right image coordinate of viewport"},
		{"-jlow",TCL_ARGV_FLOAT,NULL,&viewport.ymin,
			"bottom image coordinate of viewport"},
		{"-jhigh",TCL_ARGV_FLOAT,NULL,&viewport.ymax,
			"top image coordinate of viewport"},
		{"-xlow",TCL_ARGV_FLOAT,NULL,&viewport.x_axis.min,
			"left data coordinate of viewport"},
		{"-xhigh",TCL_ARGV_FLOAT,NULL,&viewport.x_axis.max,
			"right data coordinate of viewport"},
		{"-ylow",TCL_ARGV_FLOAT,NULL,&(viewport.y_axis.min),
			"bottom data coordinate of viewport"},
		{"-yhigh",TCL_ARGV_FLOAT,NULL,&(viewport.y_axis.max),
			"top data coordinate of viewport"},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};

	Tcl_Obj **remObjv=NULL;

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	/* make sure we still have an image specified */
	if (objc!=3) {
		Tcl_WrongNumArgs(interp,1,objv,"[options] ?imgvar? ?list of coefficients?");
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}
	if (getMvthImageFromObj(interp,remObjv[1],&mimg)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}
	
	/* is the coeeficients object a list? */
	Tcl_Obj **carray=NULL; /* array of elements of the coefficients list */
	if (Tcl_ListObjGetElements(interp,remObjv[2],&order,&carray)!=TCL_OK) {
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	if (order==0) {
		Tcl_AppendResult(interp,"Empty coeeficients array!\n",NULL);
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	/* allocate space for coefficients */
	coefs=(double*)malloc(order*sizeof(double));
	/* get the coefficients */
	for (i=0;i<order;i++)
	{
		if (Tcl_GetDoubleFromObj(interp,carray[i],&coefs[i])!=TCL_OK) {
			if (remObjv!=NULL) free(remObjv);
			return TCL_ERROR;
		}
	}
	if (remObjv!=NULL) free(remObjv);

	/* make sure an image exists at specified location */
	mi=mimg->img;

	/* register with undo substructure */
	//register_image_undo_var(iname);

	if (doErase)
	{
		float *pixels=mi->data;
		int w=mi->w;
		int h=mi->h;
		int bands=mi->bands;
		/* blank the image */
		memset(pixels, 0, w*h*bands*sizeof(float));
	}

	/* ok, do the plotting */
	float rgb[4];
	rgb[0]=(float)r;
	rgb[1]=(float)g;
	rgb[2]=(float)b;
	rgb[3]=0.0;
	plotpoly=load_symbol(MVTHIMAGELIB,"plotpoly",&libhandle);
	assert(plotpoly!=NULL);

	/* do the filter */
	plotpoly(mi,&viewport,order,coefs,rgb);
	stamp_image_t(mi);
	

	free(coefs);

	release_handle(&libhandle);
	return TCL_OK;
}