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
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"


int draw_hmap_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *mimg=NULL;
	MvthImage *outimg=NULL;
	void *libhandle=NULL;
	void (*draw_hmap_fltr)(image_t *img,float *data, unsigned int dw, unsigned int dh, float angle)=NULL;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"grayscaleimage outimage");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;
	if (getMvthImageFromObj(interp,objv[2],&outimg)!=TCL_OK) return TCL_ERROR;

	if (mimg->img->bands!=1) {
		Tcl_AppendResult(interp,"Input data image must be grayscale.\n",NULL);
		return TCL_ERROR;
	}

	draw_hmap_fltr=load_symbol(MVTHIMAGELIB,"draw_hmap_fltr",&libhandle);
	assert(draw_hmap_fltr!=NULL);
	draw_hmap_fltr(outimg->img,
			mimg->img->data,mimg->img->w,mimg->img->h,
			30*M_PI/180);
	stamp_image_t(outimg->img);
	
	Tcl_ResetResult(interp);
	release_handle(&libhandle);
	return TCL_OK;
}
