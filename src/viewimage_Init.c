/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Provide the Tcl package to view Mvth Images in Tk.
 *
 * Copyright (C) 2006 Samuel P. Bromley <sam@sambromley.com>
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
#include <tcl.h>
#include <tk.h>
#include "base/mvthimagestate.h"

extern int blitImage(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);

int verbose=0;

int Viewimage_Init(Tcl_Interp *interp) {
	/* initialize the stub table interface */
	if (Tcl_InitStubs(interp,"8.1",0)==NULL) {
		return TCL_ERROR;
	}
	if (Tk_InitStubs(interp,"8.1",0)==NULL) {
		return TCL_ERROR;
	}
	if (Tcl_PkgRequire(interp,"Tk","8.1",0)==NULL) {
		return TCL_ERROR;
	}
	if (Tcl_PkgRequire(interp,"mvthimage","1.0",0)==NULL) {
		return TCL_ERROR;
	}
	
	/* Create all of the Tcl commands */
	Tcl_CreateObjCommand(interp,"blitimage",blitImage,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);

	/* initialize the new, alternative image context handling code */
	MvthImageState_Init(interp);

	/* Initialize the Tcl script for viewing images in a Tk window.*/
	char buff[1024];
	snprintf(buff,sizeof(buff),"%s/viewimage.tcl",TCLSCRIPTDIR);
	fprintf(stdout,"viewimage.tcl should be located at: %s\n",buff);
	Tcl_EvalFile(interp,buff);
	/* Declare that we provide the buriedtargets package */
	Tcl_PkgProvide(interp,"viewimage","1.0");
	return TCL_OK;
}
