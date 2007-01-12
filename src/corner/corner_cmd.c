/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Corner detection.
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


int detectcorners_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *img=NULL;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	void (*corner_fltr)(image_t *img, float thigh, float tlow)=NULL;

	if (objc!=2) {
		Tcl_WrongNumArgs(interp,1,objv,"?name?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	img=mimg->img;

	if (img->bands!=1)
	{
		Tcl_AppendResult(interp,"Sorry. Corner detection implemented for grayscale images only.",NULL);
		return TCL_ERROR;
	}

	/* register with the undo substructure */
	//register_image_undo_var(name);

	corner_fltr=load_symbol(MVTHIMAGELIB,"corner_fltr",&libhandle);
	assert(corner_fltr!=NULL);
	/* do the filter */
	corner_fltr(img,0,0);
	stamp_image_t(img);
	
	release_handle(&libhandle);
	return TCL_OK;
}
