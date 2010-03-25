/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Difference two images.
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
 * (see the file named "COPYING), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
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
#include "base/mmap_handler.h"
#include "base/mvthimagestate.h"

int diffimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int w[3],h[3],bands[3];
	image_t *img[3]={NULL,NULL,NULL};
	MvthImage *mimg[3]={NULL,NULL,NULL};
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
		if (img[i]->d!=1) {
			Tcl_AppendResult(interp,"diffimage only supports 2D images.\n",NULL);
			return TCL_ERROR;
		}
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

	assert(DSYM(diff_fltr)!=NULL);
	assert(DSYM(new_image_t)!=NULL);

	/* make an image to hold the result */
	img[2]=DSYM(new_image_t)(w[1],h[1],bands[1]);

	/* so all that remains is that we do the difference */

	DSYM(diff_fltr)(img[0], img[1], img[2]);

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

	DSYM(stamp_image_t)(img[2]);
	
	return TCL_OK;
}
