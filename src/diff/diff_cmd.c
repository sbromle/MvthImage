/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Difference two images.
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
#include "base/mmap_handler.h"
#include "base/mvthimagestate.h"
#include "base/images_utils.h"
#include "utils/timestamp.h"

int diffimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int w[3],h[3],bands[3];
	image_t *img[3]={NULL,NULL,NULL};
	MvthImage *mimg[3]={NULL,NULL,NULL};
	void *libhandle=NULL;
	int (*diff_fltr)(image_t *img1, image_t *img2, image_t *imgD)=NULL;
	image_t * (*new_image_t)(int w, int h, int bands)=NULL;
	int i;

	if (objc!=3 && objc!=4)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?img1? ?img2? [?img3?]");
		return TCL_ERROR;
	}

	for (i=0;i<2;i++) {
		if (getMvthImageFromObj(interp,objv[i+1],&mimg[i])!=TCL_OK)
			return TCL_ERROR;
		img[i]=mimg[i]->img;
	}
	if (objc==4) 
	{
		if (getMvthImageFromObj(interp,objv[3],&mimg[2])!=TCL_OK)
			return TCL_ERROR;
	}
	
	for (i=0;i<2;i++)
	{
		w[i]=img[i]->w;
		h[i]=img[i]->h;
		bands[i]=img[i]->bands;
	}

	/* get the minimum dimentions */
	if (w[1]<w[0]) w[1]=w[0];
	if (h[1]<h[0]) h[1]=h[0];
	/* get the maximum channels */
	if (bands[1]>bands[0]) bands[1]=bands[0];

	diff_fltr=load_symbol(MVTHIMAGELIB,"diff_fltr",&libhandle);
	assert(diff_fltr!=NULL);
	new_image_t=load_symbol(MVTHIMAGELIB,"new_image_t",&libhandle);
	assert(new_image_t!=NULL);

	/* make an image to hold the result */
	img[2]=new_image_t(w[1],h[1],bands[1]);

	/* so all that remains is that we do the difference */

	diff_fltr(img[0], img[1], img[2]);

	/* where do we place the result? */
	if (mimg[2]==NULL)
	{
		mvthImageReplace(img[2],mimg[1]);
		Tcl_SetObjResult(interp,objv[2]);
	}
	else
	{
		mvthImageReplace(img[2],mimg[2]);
		Tcl_SetObjResult(interp,objv[3]);
	}

	stamp_image_t(img[2]);
	
	release_handle(&libhandle);
	return TCL_OK;
}