/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Flush an image to a file.
 *
 * Copyright (C) 2006 Samuel P. Bromley <sbromley@guigne.com>
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
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"

int flushimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	char *filename=NULL;
	image_t *img=NULL;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	int (*fflush_image_t)(image_t *m, char *filename)=NULL;

	/* make sure we still have an image specified */
	if (objc!=2 && objc!=3) {
		Tcl_WrongNumArgs(interp,1,objv,"?imgvar? [?tk image?]");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	img=mimg->img;

	if (objc==3) filename=Tcl_GetStringFromObj(objv[2],NULL);

	fflush_image_t=load_symbol(MVTHIMAGELIB,"fflush_image_t",&libhandle);
	assert(fflush_image_t!=NULL);
	fflush_image_t(img,filename);

	release_handle(&libhandle);
	return TCL_OK;
}
