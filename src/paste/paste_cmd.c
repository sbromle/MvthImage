/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Paste one image on top of another.
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
#include <tclArgv.h>
#include <assert.h>
#include "mvth_dynamic_symbols.h"
#include "base/mvthimagestate.h"

int pasteimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImage *smimg=NULL;
	MvthImage *dmimg=NULL;
	int xoff=0,yoff=0;
	Tcl_Obj *RGBtranslist=NULL;
	double alpha=1.0; /* alpha value alpha \in [0,1] for alpha blending */
	float RGB[6]; /* RGB and dR dG dB values for color-wise transparency */
	image_t *src_img=NULL;
	image_t *dst_img=NULL;

	Tcl_Obj **remObjv=NULL;
	Tcl_ArgvInfo argTable[] = {
		{"-alpha",TCL_ARGV_FLOAT,NULL,&alpha,
			"alpha value to use for blending"},
		{"-rgbtrans",TCL_ARGV_OBJ,NULL,&RGBtranslist,
			"6-tuple of R G B values and dR dG dB range for color-based transparency."},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	if (objc!=3 && objc!=5)
	{
		Tcl_WrongNumArgs(interp,1,objv,"srcname dstname ?xoffset yoffset?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,remObjv[1],&smimg)!=TCL_OK) return TCL_ERROR;
	if (getMvthImageFromObj(interp,remObjv[2],&dmimg)!=TCL_OK) return TCL_ERROR;
	if (objc==5) {
		if (Tcl_GetIntFromObj(interp,remObjv[3],&xoff)==TCL_ERROR) return TCL_ERROR;
		if (Tcl_GetIntFromObj(interp,remObjv[4],&yoff)==TCL_ERROR) return TCL_ERROR;
	}
	if (remObjv!=NULL) free(remObjv);

	if (alpha<0 || alpha>1.0) {
		Tcl_AppendResult(interp,"alpha must be between 0 and 1.0 ",NULL);
		return TCL_ERROR;
	}

	if (RGBtranslist!=NULL) {
		/* get the RGB values for color-based transparencey */
		int len;
		if (Tcl_ListObjLength(interp,RGBtranslist,&len)!=TCL_OK) return TCL_ERROR;
		if (len!=6) {
			Tcl_AppendResult(interp,"RGB transparency list must contain 6 elements. ",NULL);
			return TCL_ERROR;
		}
		for (len=0;len<6;len++) {
			Tcl_Obj *ptr=NULL;
			double rgb;
			if (Tcl_ListObjIndex(interp,RGBtranslist,len,&ptr)!=TCL_OK) return TCL_ERROR;
			if (Tcl_GetDoubleFromObj(interp,ptr,&rgb)!=TCL_OK) return TCL_ERROR;
			RGB[len]=(float)rgb;
		}
	}

	src_img=smimg->img;
	dst_img=dmimg->img;

	//register_image_undo_var(dstname);

	assert(paste_fltr!=NULL);
	paste_fltr(src_img,dst_img,xoff,yoff,RGB,(float)alpha);
	stamp_image_t(dst_img);

	Tcl_ResetResult(interp);
	return TCL_OK;
}
