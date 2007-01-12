/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a circle.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/images_context.h"
#include "base/mvthimagestate.h"
#include "base/images_types.h"
#include "utils/drawCircle.h"
#include "utils/timestamp.h"

int circle_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int x, y, rad;
	double r,g,b;
	float rgb[3];
	image_t *img=NULL;
	MvthImage *mimg=NULL;
	Tcl_Obj **lobjv=NULL;
	int lobjc=0;
	void *libhandle=NULL;
	void (*drawCircle)(image_t *img,int cx, int cy, int r, float rgb[3])=NULL;


	if (objc!=4)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name? ?{x y radius}? ?{r g b}?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	img=mimg->img;

	if (Tcl_ListObjGetElements(interp,objv[2],&lobjc,&lobjv)==TCL_ERROR)
		return TCL_ERROR;
	if (lobjc!=3)
	{
		Tcl_AppendResult(interp,
				"List must contain x y coordinates and radius.", NULL);
		return TCL_ERROR;
	}
	if (Tcl_GetIntFromObj(interp,lobjv[0],&x)==TCL_ERROR)
		return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,lobjv[1],&y)==TCL_ERROR)
		return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,lobjv[2],&rad)==TCL_ERROR)
		return TCL_ERROR;

	if (Tcl_ListObjGetElements(interp,objv[3],&lobjc,&lobjv)==TCL_ERROR)
		return TCL_ERROR;
	if (lobjc!=3)
	{
		Tcl_AppendResult(interp,
				"List must contain r g and b values", NULL);
		return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp,lobjv[0],&r)==TCL_ERROR)
		return TCL_ERROR;
	if (Tcl_GetDoubleFromObj(interp,lobjv[1],&g)==TCL_ERROR)
		return TCL_ERROR;
	if (Tcl_GetDoubleFromObj(interp,lobjv[2],&b)==TCL_ERROR)
		return TCL_ERROR;

	/* do the filter */
	rgb[0]=(float)r;
	rgb[1]=(float)g;
	rgb[2]=(float)b;
	drawCircle=load_symbol(MVTHIMAGELIB,"drawCircle",&libhandle);
	assert(drawCircle!=NULL);
	drawCircle(img,x,y,rad,rgb);
	stamp_image_t(img);
	
	release_handle(&libhandle);
	return TCL_OK;
}
