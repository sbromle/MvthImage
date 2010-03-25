/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Provide the Tcl package to view Mvth Images in Tk.
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
	Tcl_VarEval(interp,
			"puts stdout {viewimage Copyright (C) 2009 Sam Bromley};",
			"puts stdout {This software comes with ABSOLUTELY NO WARRANTY.};",
			"puts stdout {This is free software, and you are welcome to};",
			"puts stdout {redistribute it under certain conditions.};",
			"puts stdout {For details, see the GNU Lesser Public License V.3 <http://www.gnu.org/licenses>.};",
			NULL);
	Tcl_VarEval(interp,
			"proc miexpand {w} {"
				"foreach {wo ho do bo} [mi size $w] break;"
				"set c [::viewimage::canvasNameFromImg $w];"
				"set wi [winfo width $c];"
				"set hi [winfo height $c];"
				"set wi [expr {$wi-3}];"
				"set hi [expr {$hi-3}];"
				"if {$wi<=0} {set wi 10};"
				"if {$hi<=0} {set hi 10};"
				"mi size $w [list $wi $hi $bo];"
				"xblitimage $w;"
			"}",NULL);
	/* Declare that we provide the buriedtargets package */
	Tcl_PkgProvide(interp,"viewimage","1.0");
	return TCL_OK;
}
