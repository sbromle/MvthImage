/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Flush an image to a file.
 *
 * Copyright (C) 2006 Samuel P. Bromley <sbromley@guigne.com>
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
#include <errno.h>
#include <tcl.h>
#include <assert.h>
#include "mvth_dynamic_symbols.h"
#include "base/mvthimagestate.h"

int flushimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	char *filename=NULL;
	image_t *img=NULL;
	MvthImage *mimg=NULL;

	/* make sure we still have an image specified */
	if (objc!=2 && objc!=3) {
		Tcl_WrongNumArgs(interp,1,objv,"?imgvar? [?tk image?]");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	img=mimg->img;

	if (objc==3) filename=Tcl_GetStringFromObj(objv[2],NULL);

	assert(fflush_image_t!=NULL);
	fflush_image_t(img,filename);

	return TCL_OK;
}
