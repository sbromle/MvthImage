/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Provide the Tcl package loading interface for the MvthImage
 * extension.
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
#include "base/images_context.h"
#include "base/mvthimagestate.h"
#include "commands.h"

int verbose=0;

int Mvthimage_Init(Tcl_Interp *interp) {
	/* initialize the stub table interface */
	if (Tcl_InitStubs(interp,"8.1",0)==NULL) {
		return TCL_ERROR;
	}
	
	/* Create all of the Tcl commands */
	Tcl_CreateObjCommand(interp,"astereo",astereo_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"axes",axes_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"brighten",brighten_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"canny",canny_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"circle",circle_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"copyimage",copyimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"delauney",delauney_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"detectcorners",detectcorners_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"diffimages",diffimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"fillimage",fillimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"flushimage",flushimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"gaussian",gaussian_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"grayscale",grayscale_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"hsv",hsv_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"ihsv",inverse_hsv_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"invert",invertimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"isometric",isometric_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"kmeans",kmeans_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
#if 0
	Tcl_CreateObjCommand(interp,"listimages",listimages_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"newimage",newimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"openimage",openimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"undoimage",undoimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
#endif
	Tcl_CreateObjCommand(interp,"pasteimage",pasteimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"plotpoly",plotpoly_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"rgb",rgb_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"mvthtext",text_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"writeimage",writeimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);

	/* initialize the new, alternative image context handling code */
	MvthImageState_Init(interp);

	/* Initialize the state preserving commands */
	char buff[1024];
	snprintf(buff,sizeof(buff),"%s/preserve_mvthimage_state.tcl",TCLSCRIPTDIR);
	Tcl_EvalFile(interp,buff);
	Tcl_VarEval(interp,"lappend auto_path ",TCLSCRIPTDIR,NULL);
	/* Declare that we provide the buriedtargets package */
	Tcl_PkgProvide(interp,"mvthimage","1.0");
	return TCL_OK;
}
