/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Create a new image.
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
#include <string.h>
#include <tcl.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/images_utils.h"
#include "utils/timestamp.h"

int newimage_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	Tcl_Obj *resultPtr=NULL;
	char *name=NULL;
	int w,h,bands;
	int namelen;
	char buff[1024];
	image_t *mi=NULL;
	image_t *old=NULL;
	void *libhandle=NULL;
	image_t * (*new_image_t)(int w, int h, int bands)=NULL;

	if (objc!=4 && objc!=5)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?name? ?width? ?height? [?bands?]");
		return TCL_ERROR;
	}

	name=Tcl_GetStringFromObj(objv[1],&namelen);
	if (Tcl_GetIntFromObj(interp,objv[2],&w)==TCL_ERROR) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,objv[3],&h)==TCL_ERROR) return TCL_ERROR;
	if (objc==5) {
		if (Tcl_GetIntFromObj(interp,objv[4],&bands)==TCL_ERROR) return TCL_ERROR;
	} else {
		bands=4;
	}

	if (w<1 || w>10000) {
		resultPtr=Tcl_GetObjResult(interp);
		snprintf(buff,sizeof(buff),
				"Specified width (%d) out of range [1:10000]\n", w);
		Tcl_SetStringObj(resultPtr,buff,strlen(buff)+1);
		return TCL_ERROR;
	}
	if (h<1 || h>10000) {
		resultPtr=Tcl_GetObjResult(interp);
		snprintf(buff,sizeof(buff),
				"Specified height (%d) out of range [1:10000]\n", h);
		Tcl_SetStringObj(resultPtr,buff,strlen(buff)+1);
		return TCL_ERROR;
	}
	if (bands!=1 && bands!=3 && bands!=4) {
		resultPtr=Tcl_GetObjResult(interp);
		snprintf(buff,sizeof(buff),
				"Specified bands (%d) out of range {1,3,4}\n", bands);
		Tcl_SetStringObj(resultPtr,buff,strlen(buff)+1);
		return TCL_ERROR;
	}


	new_image_t=load_symbol(MVTHIMAGELIB,"new_image_t",&libhandle);
	assert(new_image_t!=NULL);
	
	/* make the new image */
	mi=new_image_t(w,h,bands);
	mi->name[0]='\0';

	old=get_image_var(name);
	if (old!=NULL) register_image_undo_var(name);
	register_image_var(mi,name);
	stamp_image_t(mi);
	

	resultPtr=Tcl_GetObjResult(interp);
	snprintf(buff,sizeof(buff),"%s",name);
	Tcl_SetStringObj(resultPtr,buff,strlen(buff)+1);
	release_handle(&libhandle);
	return TCL_OK;
}
