/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Triangulate vertices in a sparse disparity map,
 * and then fill in the map.
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
#include "mvth_dynamic_symbols.h"
#include "base/mvthimagestate.h"
#include "base/stereo_context.h"

int delauney_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *img;
	MvthImage *mimg=NULL;

	if (objc!=2)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?image name?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK) return TCL_ERROR;

	img=mimg->img;

	/* test to see if a disparity map exists */
	if (stereo_context.dmap == NULL)
	{
		Tcl_AppendResult(interp,"Stereo context no yet valid.\n",NULL);
		return TCL_ERROR;
	}

	/* register with undo */
	//register_image_undo_var(name);

	/* load the helper commands */
	assert(delauney_fltr!=NULL);
	assert(resize_image_t!=NULL);

	if (img->w!=stereo_context.w || img->h!=stereo_context.h)
	{
		resize_image_t(img,stereo_context.w,stereo_context.h,img->bands);
		updateMvthImageDims(mimg,img->w,img->h,img->bands);
	}

	delauney_fltr(img,&stereo_context);
	stamp_image_t(img);
	
	return TCL_OK;
}
