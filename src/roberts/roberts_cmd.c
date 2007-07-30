/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Roberts edge detection.
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
#include <assert.h>
#include <tcl.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"

int roberts_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *mimg=NULL;
	image_t *img=NULL;
	void *libhandle=NULL;
	void (*roberts_fltr)(image_t *img)=NULL;

	if (objc!=2) {
		Tcl_WrongNumArgs(interp,1,objv,"imagename");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	img=mimg->img;

	/* load the symbol */
	roberts_fltr=load_symbol(MVTHIMAGELIB,"roberts_fltr",&libhandle);
	assert(roberts_fltr!=NULL);
	/* do the filter */
	roberts_fltr(img);
	stamp_image_t(img);
	release_handle(&libhandle);
	return TCL_OK;
}
