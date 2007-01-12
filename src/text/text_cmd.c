/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a single-line string of text to a location in an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <tclArgv.h>
#include <assert.h>
#include "dynamic_load.h"
#include "base/mvthimagestate.h"
#include "utils/timestamp.h"

int text_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	int xoff=0,yoff=0;
	double red=1.0,green=1.0,blue=1.0;
	double ptsize=14.0;
	float color[4];
	unsigned char *mask=NULL;
	char *mystring=NULL;
	MvthImage *mimg=NULL;
	image_t *img=NULL;
	image_t *text=NULL;
	void *libhandle=NULL;
	void (*paste_with_mask_fltr)(image_t *src, image_t *dst,int xoff, int yoff, unsigned char *mask)=NULL;
	image_t * (*text_pango_fltr)(const char *message, float colour[4], double ptsize,unsigned char **mask)=NULL;

	Tcl_ArgvInfo argTable[] = {
		{"-xoff",TCL_ARGV_INT,NULL,&xoff,
			"set x offset"},
		{"-yoff",TCL_ARGV_INT,NULL,&yoff,
			"set y offset"},
		{"-size",TCL_ARGV_FLOAT,NULL,&ptsize,
			"set font size (pts)"},
		{"-red",TCL_ARGV_FLOAT,NULL,&red,
			"set red component of font colour"},
		{"-green",TCL_ARGV_FLOAT,NULL,&green,
			"set green component of font colour"},
		{"-blue",TCL_ARGV_FLOAT,NULL,&blue,
			"set blue component of font colour"},
		TCL_ARGV_AUTO_HELP,
		TCL_ARGV_TABLE_END
	};

	Tcl_Obj **remObjv=NULL;

	if (Tcl_ParseArgsObjv(interp,&objc,objv,&remObjv,argTable)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	/* make sure we still have an image specified */
	if (objc!=3) {
		Tcl_WrongNumArgs(interp,1,objv,"[options] ?imgvar? ?text?");
		fprintf(stderr,"objc=%d\n",objc);
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,remObjv[1],&mimg)!=TCL_OK)
	{
		if (remObjv!=NULL) free(remObjv);
		return TCL_ERROR;
	}
	img=mimg->img;
	mystring=Tcl_GetStringFromObj(remObjv[2],NULL);

	if (red<0.0) red=0.0;
	else if (red>1.0) red=1.0;
	if (green<0.0) green=0.0;
	else if (green>1.0) green=1.0;
	if (blue<0.0) blue=0.0;
	else if (blue>1.0) blue=1.0;
	color[0]=(float)red;
	color[1]=(float)green;
	color[2]=(float)blue;
	color[3]=0.0;

	if (ptsize==0.0)
	{
		Tcl_AppendResult(interp,"Font size must be non-zero.\n",NULL);
		return TCL_ERROR;
	}

	text_pango_fltr=load_symbol(MVTHIMAGELIB,"text_pango_fltr",&libhandle);
	assert(text_pango_fltr!=NULL);
	paste_with_mask_fltr=load_symbol(MVTHIMAGELIB,"paste_with_mask_fltr",&libhandle);
	assert(paste_with_mask_fltr!=NULL);
	text=text_pango_fltr(mystring,color,ptsize,&mask);
	if (text==NULL || mask==NULL)
	{
		Tcl_AppendResult(interp,"Text sprint was NULL!\n",NULL);
		return TCL_ERROR;
	}

	//register_image_undo_var(iname);

	paste_with_mask_fltr(text,img,xoff,yoff,mask);
	stamp_image_t(img);
	
	free(text->data);
	free(text);
	free(mask);

	release_handle(&libhandle);
	return TCL_OK;
}
