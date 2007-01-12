/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Write out an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "base/images_types.h"
#include "writeimage.h"

int writeimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	char *filename=NULL;
	image_t *img=NULL;
	int bpc=1;
	MvthImage *mimg=NULL;
	void *libhandle=NULL;
	int (*writeimage)(float *img,int w,int h,int bands,int bpc,char *filename)=NULL;


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
	writeimage=load_symbol(MVTHIMAGELIB,"writeimage",&libhandle);
	assert(writeimage!=NULL);
	writeimage(img->data, img->w,img->h,img->bands,bpc,filename);

	release_handle(&libhandle);
	return TCL_OK;
}
