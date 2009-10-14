/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Shade an image given a height map.
 *
 * Copyright (C) 2009 Samuel P. Bromley <sam@sambromley.com>
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
#include <math.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_symbols_mvth.h"
#include "base/mvthimagestate.h"

int draw_hmap_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *mimg=NULL;
	MvthImage *outimg=NULL;
	float angle=30.0;

	if (objc!=3 && objc!=4)
	{
		Tcl_WrongNumArgs(interp,1,objv,"grayscaleimage outimage ?angle(degrees)?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	if (getMvthImageFromObj(interp,objv[2],&outimg)!=TCL_OK) return TCL_ERROR;
	if (objc==4) {
		double a=30;
		if (Tcl_GetDoubleFromObj(interp,objv[3],&a)!=TCL_OK) return TCL_ERROR;
		if (a<0) a=0.0;
		else if (a>90) a=90;
		angle=a;
	}

	if (mimg->img->bands!=1) {
		Tcl_AppendResult(interp,"Input data image must be grayscale.\n",NULL);
		return TCL_ERROR;
	}

	assert(draw_hmap_fltr!=NULL);
	draw_hmap_fltr(outimg->img,
			mimg->img->data,mimg->img->w,mimg->img->h,
			angle*M_PI/180);
	stamp_image_t(outimg->img);
	
	Tcl_ResetResult(interp);
	return TCL_OK;
}
