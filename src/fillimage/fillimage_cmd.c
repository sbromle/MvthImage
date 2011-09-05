/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Fill an image with a color.
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
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_symbols_mvth.h"
#include "base/mvthimagestate.h"

int fillimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	float val[4]; /* r,g,b color values */
	int w,h,d,bands;
	image_t *mi=NULL;
	int lobjc=0;
	Tcl_Obj **lobjv=NULL;
	double tmp;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name? ?rgb list?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mi)!=TCL_OK) return TCL_ERROR;
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

	assert(DSYM(fillimage_fltr)!=NULL);
	DSYM(fillimage_fltr)(mi,val);
	DSYM(stamp_image_t)(mi);
	
	Tcl_ResetResult(interp);
	return TCL_OK;
}
