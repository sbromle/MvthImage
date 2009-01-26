/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Undo a command.
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
#include "base/images_types.h"

int undoimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int error_id=0;

	if (objc!=1)
	{
		Tcl_WrongNumArgs(interp,1,objv,NULL);
		return TCL_ERROR;
	}

	image_undo_var(&error_id);

	if (error_id!=0) {
		Tcl_AppendResult(interp,"Image Undo failed.\n",NULL);
		return TCL_ERROR;
	}

	return TCL_OK;
}

