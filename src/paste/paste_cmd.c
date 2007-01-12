/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Paste one image on top of another.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"
#include "paste_fltr.h"

int pasteimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *smimg=NULL;
	MvthImage *dmimg=NULL;
	int xoff,yoff;
	double t=0, dt=0;
	image_t *src_img=NULL;
	image_t *dst_img=NULL;
	void *libhandle=NULL;
	void (*paste_fltr)(image_t *src, image_t *dst,int xoff, int yoff, float t, float dt)=NULL;

	if (objc!=5 && objc!=7)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?srcname? ?dstname? ?xoffset? ?yoffset? [?t dt?]");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&smimg)!=TCL_OK) return TCL_ERROR;
	if (getMvthImageFromObj(interp,objv[2],&dmimg)!=TCL_OK) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,objv[3],&xoff)==TCL_ERROR) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,objv[4],&yoff)==TCL_ERROR) return TCL_ERROR;
	if (objc==7)
	{
		if (Tcl_GetDoubleFromObj(interp,objv[5],&t)==TCL_ERROR) return TCL_ERROR;
		if (Tcl_GetDoubleFromObj(interp,objv[6],&dt)==TCL_ERROR) return TCL_ERROR;
	}

	src_img=smimg->img;
	dst_img=dmimg->img;

	//register_image_undo_var(dstname);

	paste_fltr=load_symbol(MVTHIMAGELIB,"paste_fltr",&libhandle);
	assert(paste_fltr!=NULL);
	paste_fltr(src_img,dst_img,xoff,yoff,t,dt);
	stamp_image_t(dst_img);

	Tcl_ResetResult(interp);
	release_handle(&libhandle);
	return TCL_OK;
}
