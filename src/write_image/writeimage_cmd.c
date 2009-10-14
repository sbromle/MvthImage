/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Write out an image.
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
#include "mvth_dynamic_symbols.h"
#include "base/mvthimagestate.h"
#include "base/images_types.h"

int writeimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	char *filename=NULL;
	image_t *img=NULL;
	int bpc=1;
	MvthImage *mimg=NULL;

	if (objc!=3 && objc!=4)
	{
		Tcl_WrongNumArgs(interp,1,objv,"img filename ?bpc?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	filename=Tcl_GetStringFromObj(objv[2],NULL);
	if (objc==4) {
		if (Tcl_GetIntFromObj(interp,objv[3],&bpc)!=TCL_OK) return TCL_ERROR;
		if (bpc<=0 || (bpc!=1 && bpc!=2 && bpc!=4)) {
			Tcl_AppendResult(interp,"bpc must be = 1, 2, or 4.\n",NULL);
			return TCL_ERROR;
		}
	}

	img=mimg->img;
	assert(writeimage!=NULL);
	writeimage(img->data, img->w,img->h,img->bands,bpc,filename);

	return TCL_OK;
}
