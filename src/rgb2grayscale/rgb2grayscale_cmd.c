/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert an rgb image to grayscale.
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
#include "base/images_utils.h"
#include "utils/timestamp.h"


int grayscale_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *mimg=NULL;
	image_t *img=NULL;
	void *libhandle=NULL;
	image_t * (*rgb2grayscale_fltr)(image_t *img)=NULL;

	if (objc!=2)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	img=mimg->img;

	/* register with the undo substructure */
	//register_image_undo_var(name);

	if (img->bands==1)
	{
		Tcl_AppendResult(interp,"Image is already grayscale.",NULL);
		return TCL_ERROR;
	}

	rgb2grayscale_fltr=load_symbol(MVTHIMAGELIB,"rgb2grayscale_fltr",&libhandle);
	assert(rgb2grayscale_fltr!=NULL);
	img=rgb2grayscale_fltr(img);
	//register_image_var(img,name);
	stamp_image_t(img);

	mvthImageReplace(img,mimg);
	

	release_handle(&libhandle);
	return TCL_OK;
}
