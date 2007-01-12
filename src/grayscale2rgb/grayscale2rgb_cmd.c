/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert a grayscale image to rgb.
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


int rgb_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *img=NULL;
	void *libhandle=NULL;
	image_t * (*grayscale2rgb_fltr)(image_t *src)=NULL;
	MvthImage *mimg=NULL;

	if (objc!=2)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;

	img=mimg->img;

	/* register with the undo substructure */
	//register_image_undo_var(name);

	if (img->bands!=1)
	{
		Tcl_AppendResult(interp,"Image is not grayscale.",NULL);
		return TCL_ERROR;
	}

	grayscale2rgb_fltr=load_symbol(MVTHIMAGELIB,"grayscale2rgb_fltr",&libhandle);
	assert(grayscale2rgb_fltr!=NULL);
	img=grayscale2rgb_fltr(img);
	mvthImageReplace(img,mimg);
	//register_image_var(img,name);
	//stamp_image_t(img);

	release_handle(&libhandle);
	return TCL_OK;
}
