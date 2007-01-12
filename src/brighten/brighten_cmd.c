/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Brighten an image.
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
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"

int brighten_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	char *name=NULL;
	int namelen;
	double factor;
	image_t *img=NULL;
	void *libhandle=NULL;
	void (*brighten_fltr)(image_t *wimg,double factor)=NULL;
	MvthImage *mimg=NULL;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name? ?factor?");
		return TCL_ERROR;
	}

	name=Tcl_GetStringFromObj(objv[1],&namelen);
	if (Tcl_GetDoubleFromObj(interp,objv[2],&factor)==TCL_ERROR)
		return TCL_ERROR;

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;

	img=mimg->img;
	//register_image_undo_var(name);

	/* do the filter */
	brighten_fltr=load_symbol(MVTHIMAGELIB,"brighten_fltr",&libhandle);
	assert(brighten_fltr!=NULL);
	brighten_fltr(img,factor);
	stamp_image_t(img);
	
	release_handle(&libhandle);
	return TCL_OK;
}
