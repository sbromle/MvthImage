/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Maintain image contexts on Tcl side.
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
#include <assert.h>
#include <tcl.h>
#include "dynamic_symbols_mvth.h"
#include "images_types.h"
#include "variable_state.h"
#include "mvthimagestate.h"


#define MVTHIMAGESTATEKEY "mvthimagestate"

int getMvthImageFromObj(Tcl_Interp *interp, Tcl_Obj *CONST name,
		image_t **iPtrPtr)
{
	image_t *iPtr=NULL;
	return getVarFromObjKey(MVTHIMAGESTATEKEY,interp,name,(void**)iPtrPtr);
}

/* forward declarations */
int MvthImageCmd(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
int MvthImageCreate(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
int MvthImageDuplicate(ClientData data, Tcl_Interp *interp, int objc,
		Tcl_Obj *CONST objv[]);
int MvthImageCopy(ClientData data, Tcl_Interp *interp, int objc,
		Tcl_Obj *CONST objv[]);
int MvthImageOpen(ClientData data, Tcl_Interp *interp, int objc,
		Tcl_Obj *CONST objv[]);
void MvthImageDelete(void *ptr);
int MvthImageWHDB(Tcl_Interp *interp, image_t *iPtr, Tcl_Obj *objPtr, int i);
int MvthImageScale(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);


/* The following routines create, initialize and delete Image_ts */
int MvthImageState_Init(Tcl_Interp *interp) {
	InitializeStateManager(interp,MVTHIMAGESTATEKEY,"mi",MvthImageCmd,MvthImageDelete);
	Tcl_VarEval(interp,
			"interp alias {} mvthimage {} mi;",
			"interp alias {} copyimage {} mi copy;",
			NULL);
	return TCL_OK;
}

/* MvthImageCmd --
 * This implements the MvthImage command, which has these subcommands:
 * 	create width height depth
 * 	duplicate name
 * 	copy srcname dstname
 * 	open filename ?name?
 * 	width name ?value?
 * 	height name ?value?
 * 	depth name ?value?
 *
 * Results:
 *  A standard Tcl command result.
 */
int MvthImageCmd(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]) {
	StateManager_t statePtr=(StateManager_t)data;
	image_t *iPtr=NULL;
	Tcl_Obj *valueObjPtr=NULL;

	/* the subCmd array defines the allowed values for
	 * the subcommand.
	 */
	CONST char *subCmds[] = {
		"create","duplicate","copy","open", "width","height","depth","bands","size","scale",NULL};
	enum MvthImageIx {
		CreateIx, DuplicateIx,CopyIx,OpenIx, WidthIx, HeightIx, DepthIx, BandsIx,SizeIx,ScaleIx};
	int index;

	if (objc==1 || objc>=6) {
		Tcl_WrongNumArgs(interp,1,objv,"option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp,objv[1],subCmds,"option",0,&index)!=TCL_OK)
		return TCL_ERROR;

	/* reset the result so that we don't have the error from StateManagerCmd */
	Tcl_ResetResult(interp);

	switch (index) {
		case CreateIx:
			return MvthImageCreate(data,interp,objc,objv);
			break;
		case WidthIx:
		case HeightIx:
		case DepthIx:
		case BandsIx:
		case SizeIx:
			if (objc!=3 && objc!=4) goto err;
			if (objc==3) valueObjPtr=NULL;
			else valueObjPtr=objv[3];
			break;
		case DuplicateIx:
			return MvthImageDuplicate((ClientData)statePtr,interp,objc-1,objv+1);
			break;
		case CopyIx:
			return MvthImageCopy((ClientData)statePtr,interp,objc-1,objv+1);
			break;
		case OpenIx:
			return MvthImageOpen((ClientData)statePtr,interp,objc-1,objv+1);
			break;
		case ScaleIx:
			return MvthImageScale((ClientData)statePtr,interp,objc-1,objv+1);
			break;
	}

	/* The rest of the commands take an mvthimage name
	 * or a file name as the third
	 * argument. Hash from the name to the Mvthimage structure */
	switch (index) {
		case WidthIx:
		case HeightIx:
		case DepthIx:
		case BandsIx:
		case SizeIx:
			if (getVarFromObj((ClientData)statePtr,interp,objv[2],(void**)&iPtr)!=TCL_OK)
				return TCL_ERROR;
			break;
	}

	switch (index) {
		case WidthIx:
			return MvthImageWHDB(interp,iPtr,valueObjPtr,0);
		case HeightIx:
			return MvthImageWHDB(interp,iPtr,valueObjPtr,1);
		case DepthIx:
			return MvthImageWHDB(interp,iPtr,valueObjPtr,2);
		case BandsIx:
			return MvthImageWHDB(interp,iPtr,valueObjPtr,3);
		case SizeIx:
			return MvthImageWHDB(interp,iPtr,valueObjPtr,4);
	}
	return TCL_OK;

err:
	Tcl_WrongNumArgs(interp,1,objv,"option ?arg ...?");
	return TCL_ERROR;
}


int newMvthImage(Tcl_Interp *interp,
		int w, int h, int d, int b,
		image_t **iPtr_in)
{
	image_t *iPtr;
	iPtr=DSYM(new_3d_image_t)(w,h,d,b);
	*iPtr_in=iPtr;
	return TCL_OK;
}

int MvthImageCopy(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *simg;
	image_t *dimg=NULL;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"srcname dstname");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&simg)!=TCL_OK) return TCL_ERROR;
	if (getMvthImageFromObj(interp,objv[2],&dimg)!=TCL_OK) return TCL_ERROR;

	if (simg==dimg) {
		Tcl_AppendResult(interp,"Source and Destination are the same!",NULL);
		return TCL_ERROR;
	}

	/* make a copy */
	dimg=DSYM(new_image_t)(simg->w,simg->h,simg->bands);
	assert(dimg!=NULL);
	memcpy(dimg->name,simg->name,sizeof(dimg->name));
	DSYM(copy_image_t)(simg,dimg);

	Tcl_SetObjResult(interp,objv[2]);

	return TCL_OK;
}

int MvthImageScale(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *simg;
	image_t *dimg=NULL;
	double factor=1.0;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"srcname fraction");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&simg)!=TCL_OK) return TCL_ERROR;

	if (simg->d!=1) {
		Tcl_AppendResult(interp,"Scaling only supports 2d images.\n",NULL);
		return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp,objv[2],&factor)!=TCL_OK) return TCL_ERROR;

	if (factor<=0) {
		Tcl_AppendResult(interp,"Scale factor must be >0\n",NULL);
		return TCL_ERROR;
	}

	/* Try to load the resize function from the library */
	if (DSYM(resize_image)==NULL) {
		Tcl_AppendResult(interp,"Could not dynamically load `resize_image()' from ",
				MVTHIMAGELIB,"\n",NULL);
		return TCL_ERROR;
	}
	/* make an image to store the result. */
	int nw,nh;
	nw=(int)(simg->w*factor+0.5);
	nh=(int)(simg->h*factor+0.5);
	dimg=DSYM(new_image_t)(nw,nh,simg->bands);
	assert(dimg!=NULL);
	/* do the resizing */
	DSYM(resize_image)(simg->data,simg->w,simg->h,simg->bands,dimg->data,factor);
	DSYM(stamp_image_t)(dimg);
	//stamp_image_t(dimg);
	mvthImageReplace(dimg,simg);
	free(dimg);

	/* return the new dimensions of the image */
	Tcl_Obj *dlist=Tcl_NewListObj(0,NULL);
	if (Tcl_ListObjAppendElement(interp,dlist,Tcl_NewIntObj(simg->w))!=TCL_OK) return TCL_ERROR;
	if (Tcl_ListObjAppendElement(interp,dlist,Tcl_NewIntObj(simg->h))!=TCL_OK) return TCL_ERROR;
	if (Tcl_ListObjAppendElement(interp,dlist,Tcl_NewIntObj(simg->d))!=TCL_OK) return TCL_ERROR;
	if (Tcl_ListObjAppendElement(interp,dlist,Tcl_NewIntObj(simg->bands))!=TCL_OK) return TCL_ERROR;
	Tcl_SetObjResult(interp,dlist);

	return TCL_OK;
}

/* the following routine actually creates MvthImages */
int MvthImageCreate(ClientData data, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	StateManager_t statePtr=(StateManager_t)data;
	image_t *iPtr;
	int new;
	int len;
	char *name_ptr=NULL;
	char name[1024];

	if (objc>4 || objc<3) {
		Tcl_WrongNumArgs(interp,0,NULL,"mi create {w h d} ?name?");
		return TCL_ERROR;
	}

	Tcl_Obj **whd=NULL;
	if (Tcl_ListObjGetElements(interp,objv[2],&len,&whd)!=TCL_OK)
		return TCL_ERROR;
	if (len!=3) {
		Tcl_AppendResult(interp,"list must contain 3 elements, {w h bands}\n",NULL);
		return TCL_ERROR;
	}
	int w,h,bands;
	if (Tcl_GetIntFromObj(interp,whd[0],&w)!=TCL_OK) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,whd[1],&h)!=TCL_OK) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,whd[2],&bands)!=TCL_OK) return TCL_ERROR;

	/* did the user provide a name for this image? */
	if (objc==4) {
		/* does an image with this name already exist ? */
		name_ptr=Tcl_GetStringFromObj(objv[3],&len);
		/* then we need to get the string */
		if (len==0 || len>20) {
			Tcl_AppendResult(interp,"String length must be >0 and <20 characters.\n",NULL);
			return TCL_ERROR;
		}
		if (varExists0(statePtr,name_ptr)) {
			Tcl_AppendResult(interp,"Image named ",name_ptr," already exists!\n",NULL);
			return TCL_ERROR;
		}
	} else {
		if (varUniqName(interp,statePtr,name)!=TCL_OK) {
			return TCL_ERROR;
		}
		name_ptr=name;
	}

	/* ok, we can now safely make the image and register it */
	iPtr=DSYM(new_3d_image_t)(w,h,1,bands);
	registerVar(interp,statePtr,(ClientData)iPtr,name_ptr,REG_VAR_DELETE_OLD);
	Tcl_AppendResult(interp,name_ptr,NULL);
	return TCL_OK;
}

/* the following routine duplicates MvthImages */
int MvthImageDuplicate(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	StateManager_t statePtr=(StateManager_t)data;
	image_t *iSrcPtr;

	image_t *iPtr;
	int new;
	char name[20];
	char *src_name=NULL;
	char *dst_name=NULL;


	if (objc<2 || objc>3) {
		Tcl_WrongNumArgs(interp,0,NULL,"mi duplicate src ?dst?");
		return TCL_ERROR;
	}

	src_name=Tcl_GetString(objv[1]);
	if (getVarFromObj(data,interp,objv[1],(void**)&iSrcPtr)!=TCL_OK) {
		return TCL_ERROR;
	}

	if (objc==3) {
		int len;
		/* then user is requesting that we copy to a specific name */
		dst_name=Tcl_GetStringFromObj(objv[2],&len);
		/* then we need to get the string */
		if (len==0 || len>20) {
			Tcl_AppendResult(interp,"String length must be >0 and <20 characters.\n",NULL);
			return TCL_ERROR;
		}
		if (varExists0(statePtr,dst_name)) {
			/* then we really should copy the image */
			return MvthImageCopy(NULL,interp,3,objv);
		} 
	} else {
		/* generate an MvthImage and put it in the hash table */
		varUniqName(interp,statePtr,name);
		dst_name=name;
	}
	/* if we get here, dst_name is set and we must create a new image */

	if (newMvthImage(interp,iSrcPtr->w,iSrcPtr->h,iSrcPtr->d,iSrcPtr->bands, &iPtr)!=TCL_OK)
		return TCL_ERROR;

	/* put the new MvthImage in the hash table */
	registerVar(interp,statePtr,(ClientData)iSrcPtr,dst_name,REG_VAR_DELETE_OLD);
	/* copy the name to the interpreter result */
	Tcl_SetStringObj(Tcl_GetObjResult(interp),dst_name,-1);
	return TCL_OK;
}

/* open a file and either create a new MvthImage, or
 * load the file into an existing one */
int MvthImageOpen(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *iPtr=NULL;
	int new;
	char name[20];
	image_t *img=NULL;
	char *filename=NULL;
	char *iname=NULL;

	if (objc<2 || objc>3) {
		/* note that we are preprocessed by the "mi" command so that
		 * we only see "open filename ?varname?" as arguments.
		 */
		Tcl_WrongNumArgs(interp,0,NULL,"mi open filename ?varname?");
		return TCL_ERROR;
	}

	if (objc==3) { /* get the user supplied name of an mvthimage */
		getVarFromObj(data,interp,objv[2],(void**)&iPtr);
	} 

	/* get the filename */
	filename=Tcl_GetString(objv[1]);

	/* try to read the image */
	img=DSYM(readimage)(filename);
	if (img==NULL || img->data==NULL) {
		if (img!=NULL) free(img);
		Tcl_AppendResult(interp,"Trouble reading image `",filename,"'",NULL);
		return TCL_ERROR;
	}

	if (iPtr==NULL) {
		/* need to try to create a new one */
		if (iname==NULL) {
			varUniqName(interp,(StateManager_t)data,name);
			iname=name;
		}
		/* you probably should assert the new==1 to confirm
	 	* a new item was added to the hash table */
		iPtr=(image_t*)ckalloc(sizeof(image_t));
		registerVar(interp,(StateManager_t)data,(ClientData)iPtr,iname,REG_VAR_DELETE_OLD);
	} else {
		mvthImageReplace(img,iPtr);
		free(img);
	}

	/* copy the name to the interpreter result */
	Tcl_SetStringObj(Tcl_GetObjResult(interp),iname,-1);
	return TCL_OK;
}

/* and delete an MvthImage */
void MvthImageDelete(void *ptr)
{
	image_t *iPtr=(image_t *)ptr;
	if (iPtr!=NULL) {
		if (iPtr->data!=NULL) free(iPtr->data);
		free((char*)iPtr);
	}
	return;
}

int MvthImageWHDB(Tcl_Interp *interp, image_t *iPtr, Tcl_Obj *objPtr, int i)
{
	int w,h,d,bands;
	int dim[4];
	Tcl_Obj *ptr[3]={NULL,NULL,NULL};

	/* what are the current image parameters ? */
	dim[0]=w=iPtr->w;
	dim[1]=h=iPtr->h;
	dim[2]=d=iPtr->d;
	dim[3]=bands=iPtr->bands;
	int len=0;
	int j;

	if (i<0 || i>4) return TCL_ERROR;

	int newdim[3];
	newdim[0]=dim[0];
	newdim[1]=dim[1];
	newdim[2]=dim[3]; /* Yes, 3, since newdim only stores w,h,bands */

	if (objPtr!=NULL) {
		switch (i) {
			case 0:
			case 1:
			case 2:
			case 3:
				/* then objPtr contains only a single value */
				if (Tcl_GetIntFromObj(interp,objPtr,&newdim[i])!=TCL_OK)
					return TCL_ERROR;
				ptr[i]=objPtr;
				break;
			case 4:
				/* otherwise, it should be a list of 3 values */
				/* can only resize 2D images */
				if (dim[2]!=1) {
					Tcl_AppendResult(interp,"Can only resize 2D images.\n",NULL);
					return TCL_ERROR;
				}
				if (Tcl_ListObjLength(interp,objPtr,&len)!=TCL_OK) return TCL_ERROR;
				if (len!=3) {
					Tcl_AppendResult(interp,"List must contain 3 elements.\n",NULL);
					return TCL_ERROR;
				}
				for (j=0;j<3;j++) {
					if (Tcl_ListObjIndex(interp,objPtr,j,&ptr[j])!=TCL_OK) return TCL_ERROR;
					if (Tcl_GetIntFromObj(interp,ptr[j],&newdim[j])!=TCL_OK) return TCL_ERROR;
				}
				break;
			default:
				break;
		}

		/* note that we have re-used dim[2] here for bands */
		if (newdim[0]!=w || newdim[1]!=h || newdim[2]!=bands) {
			/* then some dimension has changed */
			DSYM(resize_image_t)(iPtr,newdim[0],newdim[3],newdim[2]);
		}
	}
	else  /* if objPtr is NULL, we are just requesting information */
	{
		Tcl_Obj *result=NULL;
		switch (i) {
			case 0:
				Tcl_SetObjResult(interp,Tcl_NewIntObj(iPtr->w));
				break;
			case 1:
				Tcl_SetObjResult(interp,Tcl_NewIntObj(iPtr->h));
				break;
			case 2:
				Tcl_SetObjResult(interp,Tcl_NewIntObj(iPtr->d));
				break;
			case 3:
				Tcl_SetObjResult(interp,Tcl_NewIntObj(iPtr->bands));
				break;
			case 4:
				result=Tcl_NewListObj(0,NULL);
				if (Tcl_ListObjAppendElement(interp,result,
						Tcl_NewIntObj(iPtr->w))!=TCL_OK)
					return TCL_ERROR;
				if (Tcl_ListObjAppendElement(interp,result,
						Tcl_NewIntObj(iPtr->h))!=TCL_OK)
					return TCL_ERROR;
				if (Tcl_ListObjAppendElement(interp,result,
						Tcl_NewIntObj(iPtr->d))!=TCL_OK)
					return TCL_ERROR;
				if (Tcl_ListObjAppendElement(interp,result,
						Tcl_NewIntObj(iPtr->bands))!=TCL_OK)
					return TCL_ERROR;
				Tcl_SetObjResult(interp,result);
				break;
			default:
				break;
		}
	}
	return TCL_OK;
}

int mvthImageReplace(image_t *simg, image_t *dimg)
{
	if (dimg==simg) return TCL_OK;
	if (simg==NULL || dimg==NULL
			|| dimg->data==NULL || simg->data==NULL) return TCL_ERROR;
	free(dimg->data);
	*dimg=*simg;
	return TCL_OK;
}
