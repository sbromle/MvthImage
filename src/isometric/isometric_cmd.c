/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Project image into an isometric view.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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
#include <assert.h>
#include <tcl.h>
#include <tclArgv.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"

int isometric_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *img; /* must find this */
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	void (*isometric_fltr)(image_t *img, double theta)=NULL;
	double theta=30;
	Tcl_Obj **remObjv=NULL;

	Tcl_ArgvInfo argTable[] = {
		{"-theta",TCL_ARGV_FLOAT,NULL,&theta,
			"angle of projection in degrees"},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	if (objc!=2) {
		Tcl_WrongNumArgs(interp,1,objv,"img ?-theta projection_angle_in_degrees?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	img=mimg->img;

	/* load any symbols that we need */
	isometric_fltr=load_symbol(MVTHIMAGELIB,"isometric_fltr",&libhandle);
	assert(isometric_fltr!=NULL);

	isometric_fltr(img,theta);

	release_handle(&libhandle);
	
	return TCL_OK;
}
