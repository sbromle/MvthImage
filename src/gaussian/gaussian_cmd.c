/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Gaussian filter an image.
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
#include <tcl.h>
#include <assert.h>
#include "dynamic_symbols.h"
#include "base/mvthimagestate.h"

/* The following two functions are now both identical */
int gaussian_cmd(ClientData clientData, Tcl_Interp *interp,
		    int objc, Tcl_Obj *CONST objv[])
{
	double sigma;
	image_t *img;
	MvthImage *mimg=NULL;

	if (objc!=3) {
		Tcl_WrongNumArgs(interp,1,objv,"?img? ?sigma in pixels?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;

	img=mimg->img;

	if (Tcl_GetDoubleFromObj(interp,objv[2],&sigma)==TCL_ERROR)
		return TCL_ERROR;

	if (sigma<0) {
		Tcl_AppendResult(interp,"Sigma must be >=0\n",NULL);
		return TCL_ERROR;
	}

	/* register with the undo substructure */
	//register_image_undo_var(name);

	assert(gaussian_fltr!=NULL);
	/* do the filter */
	gaussian_fltr(img,(float)sigma);
	//stamp_image_t(img);
	
	return TCL_OK;
}
