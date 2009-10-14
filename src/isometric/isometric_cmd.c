/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Project image into an isometric view.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * MVTH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License Version 3,
 * as published by the Free Software Foundation.
 *
 * MVTH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * (see the file named "COPYING"), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 *
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
#include "dynamic_symbols.h"
#include "base/mvthimagestate.h"

int isometric_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *img; /* must find this */
	MvthImage *mimg=NULL;
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
	assert(isometric_fltr!=NULL);

	isometric_fltr(img,theta);

	return TCL_OK;
}
