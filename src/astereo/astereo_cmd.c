/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Calculate a disparity map using Area-based stereo vision.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "../base/stereo_context.h"

int astereo_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr=NULL;
	char buff[1024];
	image_t *imgL; /* left WImage  */
	image_t *imgR; /* right WIMage */
	MvthImage *mimgL,*mimgR;
	int range, window, horopter;

	void *libhandle=NULL;
	void (*astereo_fltr)(image_t *img1, image_t *img2, SContext_t *sc);

	if (objc!=6)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?left? ?right? ?range? ?window? ?horopter?");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&mimgL)!=TCL_OK) return TCL_ERROR;
	if (getMvthImageFromObj(interp,objv[1],&mimgR)!=TCL_OK) return TCL_ERROR;
	imgL=mimgL->img;
	imgR=mimgR->img;

	if (Tcl_GetIntFromObj(interp,objv[3],&range)==TCL_ERROR) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,objv[4],&window)==TCL_ERROR) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,objv[5],&horopter)==TCL_ERROR) return TCL_ERROR;

	if (range<2 || range>64) {
		resultPtr=Tcl_GetObjResult(interp);
		snprintf(buff,sizeof(buff),
				"Specified range (%d) out of range [2,64]\n", range);
		Tcl_SetStringObj(resultPtr,buff,strlen(buff)+1);
		return TCL_ERROR;
	}

	if (window<1 || window>32) {
		resultPtr=Tcl_GetObjResult(interp);
		snprintf(buff,sizeof(buff),
				"Specified window (%d) out of range [1,32]\n", window);
		Tcl_SetStringObj(resultPtr,buff,strlen(buff)+1);
		return TCL_ERROR;
	}


	if (imgR->bands != 1 || imgL->bands!=1)
	{
		resultPtr=Tcl_GetObjResult(interp);
		snprintf(buff,sizeof(buff),
			"Area-based stereo currently only implemented for grayscale images.\n");
		Tcl_SetStringObj(resultPtr,buff,strlen(buff)+1);
		return TCL_ERROR;
	}

	/* do the filter */
	astereo_fltr=load_symbol(MVTHIMAGELIB,"astereo_fltr",&libhandle);
	assert(astereo_fltr!=NULL);
	astereo_fltr(imgL,imgR,&stereo_context);

	release_handle(&libhandle);

	Tcl_ResetResult(interp);
	return TCL_OK;
}
