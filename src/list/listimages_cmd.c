/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * List all the images we have registered as variables.
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
#include "base/images_types.h"
#include "base/images_utils.h"
#include "base/mmap_handler.h"

int listimages_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	char buff[1024];
	int nvars;
	char *iname=NULL;
	char **names;
	char **curname;
	Tcl_Obj *list_of_names=NULL;
	void *libhandle=NULL;
	int (*sprint_image_t)(char *buff, image_t *img)=NULL;


	if (objc!=1 && objc!=2)
	{
		Tcl_WrongNumArgs(interp,1,objv,"[?img var?]");
		return TCL_ERROR;
	} 
	if (objc==1) /* then we will return a list of names */
		list_of_names=Tcl_NewListObj(0,NULL); /* create the list */
	else /* else we need to get the name to match */
		iname=Tcl_GetStringFromObj(objv[1],NULL);

	sprint_image_t=load_symbol(MVTHIMAGELIB,"sprint_image_t",&libhandle);
	assert(sprint_image_t!=NULL);

	/* get a list of all variables */
	names=get_image_vars(&nvars);
	curname=names;
	while (curname!=NULL && *curname!=NULL)
	{
		if (objc==1) {
			if (Tcl_ListObjAppendElement(interp,list_of_names,
						Tcl_NewStringObj(*curname,-1))!=TCL_OK)
				return TCL_ERROR;
		} else {
			if (strcmp(*curname,iname)==0)
			{
				sprintf(buff,"%s\t",*curname);
				sprint_image_t(&buff[strlen(buff)],get_image_var(*curname));
				if (Tcl_VarEval(interp,"puts {", buff,"}",NULL)!=TCL_OK)
					return TCL_ERROR;
				return TCL_OK;
			}
		}
		curname++;
	}
	if (objc==1)
		Tcl_SetObjResult(interp,list_of_names);

	release_handle(&libhandle);
	return TCL_OK;
}
