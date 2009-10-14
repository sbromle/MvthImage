/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Compute k-means grouping of values on an image.
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
 * (see the file named "COPYING), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <tcl.h>
#include <tclArgv.h>
#include "base/mvthimagestate.h"
#include "dynamic_symbols_mvth.h"

/* Arguments are:
 * kmeans image ?-nclasses numclasses? ?-channel R|G|B? 
 */
int kmeans_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *mimg=NULL;
	image_t *img=NULL;
	int nclasses=10;
	int channel=0; /*  0 = red channel
											1 = green channel
											2 = blue channel */

	Tcl_ArgvInfo argTable[] = {
		{"-channel",TCL_ARGV_INT,NULL,&channel,
			"Channel to use [-1|0|1|2]"},
		{"-nclasses",TCL_ARGV_INT,NULL,&nclasses,
			"number of classes to use"},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};
	
	Tcl_Obj **remObjv=NULL;

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);;
		return TCL_ERROR;
	}

	if (objc!=2) {
		Tcl_AppendResult(interp,"You must specify an image!\n",NULL);
		if (remObjv!=NULL) free(remObjv);;
		return TCL_ERROR;
	}
	
	if (nclasses<=1 || nclasses>255)
	{
		Tcl_AppendResult(interp,"Error: There must be more than 1 and less than 255 classes.\n",NULL);
		return TCL_ERROR;
	}

	if (channel<0 || channel>2) {
		Tcl_AppendResult(interp,"Error: Channel must be <2.\n",NULL);
		return TCL_ERROR;
	}

  if (getMvthImageFromObj(interp,remObjv[1],&mimg)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);;
		return TCL_ERROR;
	}

	if (remObjv!=NULL) free(remObjv);;
	img=mimg->img;

	if (channel>=img->bands) {
		Tcl_AppendResult(interp,"Error: No such channel in image.\n",NULL);
		return TCL_ERROR;
	}

	/* load any symbols that we may need */
	assert(kmeans_fltr!=NULL);

	kmeans_fltr(img,nclasses,channel);
	/* release the library */

	return TCL_OK;
}
