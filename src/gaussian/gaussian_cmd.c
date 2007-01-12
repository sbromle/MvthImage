/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Gaussian filter an image.
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


/* The following two functions are now both identical */
int gaussian_cmd(ClientData clientData, Tcl_Interp *interp,
		    int objc, Tcl_Obj *CONST objv[])
{
	double sigma;
	image_t *img;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	void (*gaussian_fltr)(image_t *img, float sigma)=NULL;

	if (objc!=3) {
		Tcl_WrongNumArgs(interp,1,objv,"?img? ?sigma in pixels?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;

	img=mimg->img;

	if (Tcl_GetDoubleFromObj(interp,objv[2],&sigma)==TCL_ERROR)
		return TCL_ERROR;

	if (sigma<0) {
		Tcl_AppendResult(interp,"Sigma must be >=0\n",NULL);
		return TCL_ERROR;
	}

	/* register with the undo substructure */
	//register_image_undo_var(name);

	gaussian_fltr=load_symbol(MVTHIMAGELIB,"gaussian_fltr",&libhandle);
	assert(gaussian_fltr!=NULL);
	/* do the filter */
	gaussian_fltr(img,(float)sigma);
	//stamp_image_t(img);
	
	release_handle(&libhandle);
	return TCL_OK;
}
