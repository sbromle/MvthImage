/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Provide the Tcl package loading interface for the MvthImage
 * extension.
 *
 * Copyright (C) 2006 Samuel P. Bromley <sam@sambromley.com>
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
#include <tcl.h>
#include <tk.h>
#include "base/images_types.h"
#include "base/mvthimagestate.h"
#include "dynamic_symbols_mvth.h"

int verbose=0;

/* command function declarations */
extern int mireload_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int astereo_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int axes_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int brighten_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int circle_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int canny_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int delauney_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int detectcorners_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int diffimage_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int fillimage_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int draw_hmap_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int gaussian_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int feature_extract_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int grayscale_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int invertimage_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int isometric_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int kmeans_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
#if 0
extern int listimages_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int newimage_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int openimage_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
#endif
extern int pasteimage_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int visual_map_1d_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int rgb_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int roberts_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int rotate_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int hsv_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int inverse_hsv_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int text_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
#if 0
extern int undoimage_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
#endif
extern int writeimage_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
extern int plotpoly_cmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);




int Mvthimage_Init(Tcl_Interp *interp) {
	/* initialize the stub table interface */
	if (Tcl_InitStubs(interp,"8.1",0)==NULL) {
		return TCL_ERROR;
	}
	
	/* dynamically load all of the library functions */
	unload_all_mvth();
	load_all_mvth();

	/* Create all of the Tcl commands */
	Tcl_CreateObjCommand(interp,"mireload",mireload_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
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
	Tcl_CreateObjCommand(interp,"delauney",delauney_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"detectcorners",detectcorners_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"diffimages",diffimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"drawhmap",draw_hmap_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"fillimage",fillimage_cmd,
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
	Tcl_CreateObjCommand(interp,"undoimage",undoimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
#endif
	Tcl_CreateObjCommand(interp,"pasteimage",pasteimage_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"vmap",visual_map_1d_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"plotpoly",plotpoly_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"rgb",rgb_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"roberts",roberts_cmd,
			(ClientData)NULL,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateObjCommand(interp,"rotate",rotate_cmd,
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
	Tcl_VarEval(interp,
			"puts stdout {mvthimage Copyright (C) 2009 Sam Bromley};",
			"puts stdout {This software comes with ABSOLUTELY NO WARRANTY.};",
			"puts stdout {This is free software, and you are welcome to};",
			"puts stdout {redistribute it under certain conditions.};",
			"puts stdout {For details, see the GNU Lesser Public License V.3 <http://www.gnu.org/licenses>.};",
			NULL);
	/* Declare that we provide the mvthimage package */
	Tcl_PkgProvide(interp,"mvthimage","1.0");
	return TCL_OK;
}
