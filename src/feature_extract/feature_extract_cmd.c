/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Highlight features in an image.
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
 * 
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <tclArgv.h>
#include <assert.h>
#include "dynamic_symbols_mvth.h"
#include "base/images_utils.h"

//extern image_t *feature_extract_fltr(image_t *img, int ci, int cj, int r, int b, int sgn, int procID);

int feature_extract_cmd(ClientData clientData, Tcl_Interp *interp,
		    int objc, Tcl_Obj *CONST objv[])
{
	char *sname;
	char *dname;
	image_t *simg;
	image_t *dimg;
	int procID = 0;		// procID = 0 first call on this image (initialize mem, image gradient, ...)
						// procID = 1 return call on this image (continue animation with no re-initialization)
						// procID = 2 free up all dynamically allocated memory
	int ci = 0;			// center of initial front (circle)
	int cj = 0;			// center of initial front (circle)
	int r = 4;			// initial front radius 
	int b = 2;			// tube radius around front where all computations take place
	int sgn = 1;		// "1" -> expand, "-1" -> shrink
	
	Tcl_Obj *resultPtr=NULL;
	char buff[1024];
	static int count = 0;
	int i;
	Tcl_Obj **_objv=NULL;

	/*
	Tcl_ArgvInfo argTable[] = {
		 {"-cr",TCL_ARGV_INT,NULL,&ci,
			"initial front center index (row)"},
		 {"-cc",TCL_ARGV_INT,NULL,&cj,
			"initial front center index (column)"},
		{"-r",TCL_ARGV_INT,NULL,&r,
			"initial front radius (pixels)"},
		{"-s",TCL_ARGV_INT,NULL,&sgn,
			"expand if s=1, shrink if s=-1"},
		{"-pid",TCL_ARGV_INT,NULL,&procID,
			"ending sample index for region of trace to use"},
		{(char*)NULL,TCL_ARGV_END,NULL,NULL,(char*)NULL}
	};

	_objv=(Tcl_Obj**)calloc(objc+1,sizeof(Tcl_Obj*));
	for (i=0;i<objc;i++) _objv[i]=objv[i];

	if (Tcl_ParseArgsObjv(interp,&objc,_objv,argTable,0)!=TCL_OK)
	{
		free(_objv);
		return TCL_ERROR;
	}
	*/
	Tcl_ArgvInfo argTable[] = {
		 {"-cr",TCL_ARGV_INT,NULL,&ci,
			"initial front center index (row)"},
		 {"-cc",TCL_ARGV_INT,NULL,&cj,
			"initial front center index (column)"},
		{"-r",TCL_ARGV_INT,NULL,&r,
			"initial front radius (pixels)"},
		{"-s",TCL_ARGV_INT,NULL,&sgn,
			"expand if s=1, shrink if s=-1"},
		{"-pid",TCL_ARGV_INT,NULL,&procID,
			"ending sample index for region of trace to use"},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};

	Tcl_Obj **remObjv=NULL;

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
			return TCL_ERROR;
	}

	fprintf(stdout, "ci=%d\n", ci);
	fprintf(stdout, "cj=%d\n", cj);
	fprintf(stdout, "r=%d\n", r);
	fprintf(stdout, "sgn=%d\n", sgn);
	fprintf(stdout, "procID=%d\n", procID);
	//if (objc!=3) {
	if (objc<3) {
		Tcl_WrongNumArgs(interp,1,objv,"?simg? ?dimg? [options]");
		return TCL_ERROR;
	}
	// sort out initial tube radius
	b = (r/2 < 6) ? r/2 : 6;
	//make sure sgn is set correctly
	if (sgn != -1) sgn = 1;

	if (getMvthImageFromObj(interp,remObjv[1],&simg)!=TCL_OK) return TCL_ERROR;
	sname=Tcl_GetStringFromObj(remObjv[1],NULL);
	dname=Tcl_GetStringFromObj(remObjv[2],NULL);
	if (strcmp(sname,dname)==0)
	{
		resultPtr=Tcl_GetObjResult(interp);
		snprintf(buff,sizeof(buff),"srcname and dstname are the same.\n");
		Tcl_SetStringObj(resultPtr,buff,strlen(buff)+1);
		return TCL_ERROR;
	}

	// source image
	simg=get_image_var(sname);
	if (simg==NULL)
	{
		Tcl_AppendResult(interp,"No image named '",sname,"'",NULL);
		return TCL_ERROR;
	}
	if (simg->bands != 1)
	{
		fprintf(stderr,"Feature_extract only works for source grayscale images.\n\n");
		return TCL_ERROR;
	}

	// destination image
	dimg=get_image_var(dname);
	if (dimg!=NULL)
	{
		if (dimg->bands != 3)
		{
			free(dimg);
			assert(new_image_t!=NULL);
			dimg = new_image_t(simg->w, simg->h, 3);
			libhandle = NULL;
		}
		register_image_undo_var(dname);
	}

	fprintf(stdout, "simg->w=%d\n", simg->w);
	fprintf(stdout, "simg->h=%d\n", simg->h);
	// center of initial front
	if (ci < 1 || ci >= simg->w)
	{
		Tcl_AppendResult(interp,"Initial seed center is out of bounds",NULL);
		return TCL_ERROR;
	}
	if (cj < 1 || cj >= simg->h)
	{
		Tcl_AppendResult(interp,"Initial seed center is out of bounds",NULL);
		return TCL_ERROR;
	}

	// process identifier
	if (procID < 0 || procID > 2)
	{
		Tcl_AppendResult(interp,"procID should be set to either 0 (first call on this image) or 1 (repeat call on this image) or 2 (free up all dynamically allocated memory)",NULL);
		return TCL_ERROR;
	}

	// do the filter 
	if(b < 6 && count > 0)
		b += 1;
	count++;
	assert(DSYM(feature_extract_fltr) != NULL);
	dimg = DSYM(feature_extract_fltr)(simg, cj, ci, r, b, sgn, procID);
	if (procID == 2) count = 0;
	/* FIXME: do something with dimg.
	 * This code is currently not compiled into mvthimage.
	 */
	return TCL_OK;
}
