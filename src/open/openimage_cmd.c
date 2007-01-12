/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Open an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "base/images_utils.h"
#include "base/mmap_handler.h"
#include "utils/timestamp.h"
#include "readimages.h"

int openimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	char *name=NULL;
	MvthImage *mimg=NULL;
	char *filename=NULL;
	image_t *img=NULL;
	void *libhandle=NULL;
	image_t * (*readimage)(char *filename)=NULL;

	if (objc!=3 && objc!=2)
	{
		Tcl_WrongNumArgs(interp,1,objv,"filename ?img?");
		return TCL_ERROR;
	}

	filename=Tcl_GetStringFromObj(objv[1],NULL);
	if (objc==3) {
		/* then we should lo
	}

	/* register with the undo substructure */
	//img=get_image_var(name);
	//if (img!=NULL) register_image_undo_var(name);


	readimage=load_symbol(MVTHIMAGELIB,"readimage",&libhandle);
	assert(readimage!=NULL);
	/* make a new image_t */
	img=readimage(filename);

	if (img==NULL || img->data==NULL)
	{
		if (img!=NULL) free(img);
		Tcl_AppendResult(interp,"Trouble reading image `",filename,"'",NULL);
		return TCL_ERROR;
	}

	register_image_var(img,name);
	stamp_image_t(img);

	release_handle(&libhandle);
	return TCL_OK;
}
