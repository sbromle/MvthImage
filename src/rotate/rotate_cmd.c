/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Rotate an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
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
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"

int rotate_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *mimg=NULL;
	void *libhandle=NULL;

	void (*rotate_fltr)(image_t *wimg,
		const double alpha,
		const double beta,
		const double gamma,
		const double x, const double y, const double z)=NULL;

	double alpha=0.0, beta=0.0,gamma=0.0;
	double x=0.0, y=0.0,z=0.0;

	Tcl_ArgvInfo argTable[] = {
		{"-alpha"  ,TCL_ARGV_FLOAT,NULL,&alpha,"rotation about x-axis (degrees)"},
		{"-beta"  ,TCL_ARGV_FLOAT,NULL,&beta,"rotation about y-axis (degrees)"},
		{"-gamma",TCL_ARGV_FLOAT,NULL,&gamma,"rotation about z-axis (degrees)"},
		{"-x" ,TCL_ARGV_FLOAT,NULL,&x,"point of rotation"},
		{"-y" ,TCL_ARGV_FLOAT,NULL,&y,"point of rotation"},
		{"-z" ,TCL_ARGV_FLOAT,NULL,&z,"point of rotation"},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};

	Tcl_Obj **remObjv=NULL;

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	if (objc!=2) {
		Tcl_WrongNumArgs(interp,1,objv,"imgvar ?options?");
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	if (alpha==0.0 && beta==0.0 && gamma==0.0) return TCL_OK;

	if (getMvthImageFromObj(interp,remObjv[1],&mimg)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}
	if (remObjv!=NULL) free(remObjv);

	/* load the symbol */
	rotate_fltr=load_symbol(MVTHIMAGELIB,"rotate_fltr",&libhandle);
	assert(rotate_fltr!=NULL);

	/* do the filter */
	rotate_fltr(mimg->img,alpha,beta,gamma,x,y,z);

	release_handle(&libhandle);
	
	return TCL_OK;
}
