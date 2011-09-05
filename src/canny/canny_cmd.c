/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Do canny edge detection.
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

int canny_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	double tlow,thigh;
	image_t *img=NULL;

	if (objc!=4)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name? ?high threshold? ?low threshold?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&img)!=TCL_OK) return TCL_ERROR;
	if (img->d!=1) {
		Tcl_AppendResult(interp,"canny_cmd only supports 2D images.\n",NULL);
		return TCL_ERROR;
	}

	if (Tcl_GetDoubleFromObj(interp,objv[2],&thigh)==TCL_ERROR)
		return TCL_ERROR;
	if (Tcl_GetDoubleFromObj(interp,objv[3],&tlow)==TCL_ERROR)
		return TCL_ERROR;

	if (thigh<0) {
		Tcl_AppendResult(interp,"High threshold must be > 0.",NULL);
		return TCL_ERROR;
	}
	if (tlow<0) {
		Tcl_AppendResult(interp,"Low threshold must be > 0.",NULL);
		return TCL_ERROR;
	}


	/* register it with the undo substructure */
	//register_image_undo_var(name);

	/* do the filter */
	assert(DSYM(canny_fltr)!=NULL);
	DSYM(canny_fltr)(img,thigh,tlow);
	DSYM(stamp_image_t)(img);

	return TCL_OK;
}
