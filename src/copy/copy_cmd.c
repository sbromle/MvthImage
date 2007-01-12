/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Copy an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "base/images_utils.h"
#include "base/mmap_handler.h"
#include "utils/timestamp.h"

int copyimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *simg;
	image_t *dimg=NULL;
	MvthImage *smimg=NULL;
	MvthImage *dmimg=NULL;
	void *libhandle=NULL;
	int (*copy_image_t)(image_t *src, image_t *dst)=NULL;
	image_t * (*new_image_t)(int w, int h, int bands)=NULL;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?srcname? ?dstname?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&smimg)!=TCL_OK) return TCL_ERROR;
	simg=smimg->img;
	if (getMvthImageFromObj(interp,objv[2],&dmimg)!=TCL_OK) return TCL_ERROR;

	if (smimg==dmimg) {
		Tcl_AppendResult(interp,"Source and Destination are the same!",NULL);
		return TCL_ERROR;
	}

	//if (dimg!=NULL) register_image_undo_var(dstname);

	/* load the helper functions */
	new_image_t=load_symbol(MVTHIMAGELIB,"new_image_t",&libhandle);
	assert(new_image_t!=NULL);
	copy_image_t=load_symbol(MVTHIMAGELIB,"copy_image_t",&libhandle);
	assert(copy_image_t!=NULL);

	/* make a copy */
	dimg=new_image_t(simg->w,simg->h,simg->bands);
	assert(dimg!=NULL);
	memcpy(dimg->name,simg->name,sizeof(dimg->name));
	copy_image_t(simg,dimg);
	//register_image_var(dimg,dstname);
	//stamp_image_t(dimg);
	mvthImageReplace(dimg,dmimg);

	Tcl_SetObjResult(interp,objv[2]);

	release_handle(&libhandle);
	return TCL_OK;
}
