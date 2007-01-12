/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Fill an image with a color.
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
#include "fillimage_fltr.h"


int fillimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	float val[4]; /* r,g,b color values */
	int w,h,bands;
	MvthImage *mimg=NULL;
	image_t *mi=NULL;
	int lobjc=0;
	Tcl_Obj **lobjv=NULL;
	double tmp;
	void *libhandle=NULL;
	void (*fillimage_fltr)(image_t *img,float val[3])=NULL;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name? ?rgb list?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	if (Tcl_ListObjGetElements(interp,objv[2],&lobjc,&lobjv)==TCL_ERROR)
		return TCL_ERROR;

	if (lobjc==1)
	{
		if (Tcl_GetDoubleFromObj(interp,lobjv[0],&tmp)==TCL_ERROR) return TCL_ERROR;
		val[0]=val[1]=val[2]=(float)tmp;
	} else if (lobjc==2) {
		if (Tcl_GetDoubleFromObj(interp,lobjv[0],&tmp)==TCL_ERROR) return TCL_ERROR;
		val[0]=(float)tmp;
		if (Tcl_GetDoubleFromObj(interp,lobjv[1],&tmp)==TCL_ERROR) return TCL_ERROR;
		val[1]=(float)tmp;
		val[2]=0;
	} else if (lobjc>=3) {
		if (Tcl_GetDoubleFromObj(interp,lobjv[0],&tmp)==TCL_ERROR) return TCL_ERROR;
		val[0]=(float)tmp;
		if (Tcl_GetDoubleFromObj(interp,lobjv[1],&tmp)==TCL_ERROR) return TCL_ERROR;
		val[1]=(float)tmp;
		if (Tcl_GetDoubleFromObj(interp,lobjv[2],&tmp)==TCL_ERROR) return TCL_ERROR;
		val[2]=(float)tmp;
	}
	val[3]=0;

	mi=mimg->img;

	/* register with the undo substructure */
	//register_image_undo_var(name);

	w=mi->w;
	h=mi->h;
	bands=mi->bands;

	fillimage_fltr=load_symbol(MVTHIMAGELIB,"fillimage_fltr",&libhandle);
	assert(fillimage_fltr!=NULL);
	fillimage_fltr(mi,val);
	stamp_image_t(mi);
	
	Tcl_ResetResult(interp);
	release_handle(&libhandle);
	return TCL_OK;
}