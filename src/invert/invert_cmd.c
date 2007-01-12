/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Invert an image.
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

int invertimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *mi=NULL;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	void (*invert_fltr)(image_t *wimg)=NULL;

	if (objc!=2)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	mi=mimg->img;

	/* register with the undo substructure */
	//register_image_undo_var(name);

	invert_fltr=load_symbol(MVTHIMAGELIB,"invert_fltr",&libhandle);
	assert(invert_fltr!=NULL);
/* do the filter */
	invert_fltr(mi);
	stamp_image_t(mi);
	

	Tcl_ResetResult(interp);
	release_handle(&libhandle);
	return TCL_OK;
}
