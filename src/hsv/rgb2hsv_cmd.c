/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert an rgb image to pseudo hsv image (The resultant
 * image is still RGB, but R,G,and B, channels are set
 * to scaled H,S,V values,respectively.
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


int hsv_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *mimg=NULL;
	image_t *img=NULL;
	void *libhandle=NULL;
	int (*rgb2hsv_fltr)(image_t *img)=NULL;

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

	rgb2hsv_fltr=load_symbol(MVTHIMAGELIB,"rgb2hsv_fltr",&libhandle);
	assert(rgb2hsv_fltr!=NULL);
	rgb2hsv_fltr(img);
	//register_image_var(img,name);
	//stamp_image_t(img);

	//mvthImageReplace(img,mimg);
	
	release_handle(&libhandle);
	return TCL_OK;
}

/* take an image that has been converted to pseudo HSV
 * using the above routine, and map it back to RGB.
 */
int inverse_hsv_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *mimg=NULL;
	image_t *img=NULL;
	void *libhandle=NULL;
	int (*hsv2rgb_fltr)(image_t *img)=NULL;

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

	hsv2rgb_fltr=load_symbol(MVTHIMAGELIB,"hsv2rgb_fltr",&libhandle);
	assert(hsv2rgb_fltr!=NULL);
	hsv2rgb_fltr(img);
	//register_image_var(img,name);
	//stamp_image_t(img);

	//mvthImageReplace(img,mimg);
	
	release_handle(&libhandle);
	return TCL_OK;
}
