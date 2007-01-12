/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Triangulate vertices in a sparse disparity map,
 * and then fill in the map.
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
#include "base/stereo_context.h"
#include "utils/timestamp.h"


int delauney_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *img;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	void (*delauney_fltr)(image_t *,SContext_t *sc)=NULL;
	int (*resize_image_t)(image_t *img, int w, int h, int bands)=NULL;

	if (objc!=2)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?image name?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;

	img=mimg->img;

	/* test to see if a disparity map exists */
	if (stereo_context.dmap == NULL)
	{
		Tcl_AppendResult(interp,"Stereo context no yet valid.\n",NULL);
		return TCL_ERROR;
	}

	/* register with undo */
	//register_image_undo_var(name);

	/* load the helper commands */
	delauney_fltr=load_symbol(MVTHIMAGELIB,"delauney_fltr",&libhandle);
	assert(delauney_fltr!=NULL);
	resize_image_t=load_symbol(MVTHIMAGELIB,"resize_image_t",&libhandle);
	assert(resize_image_t!=NULL);

	if (img->w!=stereo_context.w || img->h!=stereo_context.h)
	{
		resize_image_t(img,stereo_context.w,stereo_context.h,img->bands);
		updateMvthImageDims(mimg,img->w,img->h,img->bands);
	}

	delauney_fltr(img,&stereo_context);
	stamp_image_t(img);
	

	release_handle(&libhandle);
	return TCL_OK;
}
