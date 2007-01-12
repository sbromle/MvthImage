/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Do canny edge detection.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"


int canny_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	double tlow,thigh;
	image_t *img=NULL;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	void (*canny_fltr)(image_t *img, float thigh, float tlow)=NULL;

	if (objc!=4)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name? ?high threshold? ?low threshold?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	img=mimg->img;

	if (Tcl_GetDoubleFromObj(interp,objv[2],&thigh)==TCL_ERROR)
		return TCL_ERROR;
	if (Tcl_GetDoubleFromObj(interp,objv[3],&tlow)==TCL_ERROR)
		return TCL_ERROR;

	if (thigh<0) {
		Tcl_AppendResult(interp,"High threshold must be > 0.",NULL);
		return TCL_ERROR;
	}
	if (tlow<0) {
		Tcl_AppendResult(interp,"Low threshold must be > 0.",NULL);
		return TCL_ERROR;
	}


	/* register it with the undo substructure */
	//register_image_undo_var(name);

	/* do the filter */
	canny_fltr=load_symbol(MVTHIMAGELIB,"canny_fltr",&libhandle);
	assert(canny_fltr!=NULL);
	canny_fltr(img,thigh,tlow);
	stamp_image_t(img);

	release_handle(&libhandle);
	return TCL_OK;
}
