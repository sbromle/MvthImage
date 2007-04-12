/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Maintain image contexts on Tcl side.
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
#include <assert.h>
#include <tcl.h>
#include "images_types.h"
#include "images_utils.h"
#include "mvthimagestate.h"

/* the following structure is created once for each Tcl interpretor*/
typedef struct MvthImageState {
	Tcl_HashTable hash; /* List of blobs by name */
	int uid;            /* used to generate names */
} MvthImageState;

#define MVTHIMAGESTATEKEY "mvthimagestate"

int mvthImageExists(Tcl_Interp *interp, Tcl_Obj *CONST name)
{
	MvthImageState *statePtr=NULL;
	Tcl_HashEntry *entryPtr=NULL;
	if (name==NULL) {
		return 0;
	}
	statePtr=(MvthImageState*)Tcl_GetAssocData(interp,MVTHIMAGESTATEKEY,NULL);

	entryPtr=Tcl_FindHashEntry(&statePtr->hash,Tcl_GetString(name));
	if (entryPtr==NULL) {
		return 0;
	}
	return 1;
}

int getMvthImageFromObj(Tcl_Interp *interp, Tcl_Obj *CONST name,
		MvthImage **iPtrPtr)
{
	MvthImageState *statePtr=NULL;
	Tcl_HashEntry *entryPtr=NULL;
	MvthImage *iPtr=NULL;
	if (name==NULL) {
		Tcl_AppendResult(interp,"name was NULL",NULL);
		return TCL_ERROR;
	}
	if (iPtrPtr==NULL) {
		Tcl_AppendResult(interp,"iPtrPtr was NULL",NULL);
		return TCL_ERROR;
	}
	statePtr=(MvthImageState*)Tcl_GetAssocData(interp,MVTHIMAGESTATEKEY,NULL);

	entryPtr=Tcl_FindHashEntry(&statePtr->hash,Tcl_GetString(name));
	if (entryPtr==NULL) {
		Tcl_AppendResult(interp,"Unknown mvthimage: ", Tcl_GetString(name),NULL);
		return TCL_ERROR;
	}
	iPtr=(MvthImage*)Tcl_GetHashValue(entryPtr);
	*iPtrPtr=iPtr;
	return TCL_OK;
}

/* forward declarations */
int MvthImageCmd(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
int MvthImageCreate(Tcl_Interp *interp, MvthImageState *statePtr,
		Tcl_Obj *wObjPtr, Tcl_Obj *hObjPtr, Tcl_Obj *dObjPtr);
int MvthImageDuplicate(Tcl_Interp *interp, MvthImageState *statePtr,
		MvthImage *iPtr);
int MvthImageOpen(Tcl_Interp *interp, MvthImageState *statePtr,
		Tcl_Obj *fileObjPtr, MvthImage *mimgPtr);
void MvthImageCleanup(ClientData data);
int MvthImageDelete(MvthImage *iPtr, Tcl_HashEntry *entryPtr);
int MvthImageNames(Tcl_Interp *interp, MvthImageState *statePtr);
int MvthImageWHD(Tcl_Interp *interp, MvthImage *iPtr, Tcl_Obj *objPtr, int i);


/* The following routines create, initialize and delete Image_ts */
int MvthImageState_Init(Tcl_Interp *interp) {
	MvthImageState *statePtr;

	/* first check to see if we haven't already registered a
	 * state handler with this interpreter.  */
	if (NULL!=Tcl_GetAssocData(interp,MVTHIMAGESTATEKEY,NULL)) return TCL_OK;

	/* allocate and initialize the hash table. Associate
	 * the MvthImageState with the command by using the ClientData */
	statePtr=(MvthImageState*)ckalloc(sizeof(MvthImageState));
	Tcl_InitHashTable(&statePtr->hash,TCL_STRING_KEYS);
	/* register the statePtr with this interpreter */
	Tcl_SetAssocData(interp,MVTHIMAGESTATEKEY,NULL,(ClientData)statePtr);
	statePtr->uid=0;
	Tcl_CreateObjCommand(interp,"mvthimage", MvthImageCmd,
			(ClientData)statePtr,MvthImageCleanup);
	return TCL_OK;
}

/* this is called when the MvthImage command is destroyed.
 * The hash table is walked, destroying all MvthImages as
 * you go, and then the HashTable itself is freed */
void MvthImageCleanup(ClientData data) {
	MvthImageState *statePtr=(MvthImageState*)data;
	MvthImage *iPtr;
	Tcl_HashEntry *entryPtr;
	Tcl_HashSearch search;

	entryPtr=Tcl_FirstHashEntry(&statePtr->hash,&search);
	while (entryPtr!=NULL) {
		iPtr=Tcl_GetHashValue(entryPtr);
		MvthImageDelete(iPtr,entryPtr);
		/* get the first entry again, not the next one, since
		 * the previous first one is now deleted. */
		entryPtr=Tcl_FirstHashEntry(&statePtr->hash,&search);
	}
	ckfree((char*)statePtr);
}

/* MvthImageCmd --
 * This implements the MvthImage command, which has these subcommands:
 * 	create width height depth
 * 	duplicate name
 * 	open filename ?name?
 * 	names ?pattern?
 * 	width name ?value?
 * 	height name ?value?
 * 	depth name ?value?
 * 	delete name
 *
 * Results:
 *  A standard Tcl command result.
 */
int MvthImageCmd(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]) {
	MvthImageState *statePtr=(MvthImageState *)data;
	MvthImage *iPtr=NULL;
	Tcl_HashEntry *entryPtr=NULL;
	Tcl_Obj *valueObjPtr=NULL;

	/* the subCmd array defines the allowed values for
	 * the subcommand.
	 */
	CONST char *subCmds[] = {
		"create","delete","duplicate","open", "width","height","depth","names",NULL};
	enum MvthImageIx {
		CreateIx, DeleteIx, DuplicateIx,OpenIx, WidthIx, HeightIx, DepthIx, NamesIx,};
	int index;

	if (objc==1 || objc>=6) {
		Tcl_WrongNumArgs(interp,1,objv,"option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp,objv[1],subCmds,"option",0,&index)!=TCL_OK)
		return TCL_ERROR;

	switch (index) {
		case NamesIx:
			if (objc>2) goto err;
			return MvthImageNames(interp,statePtr);
			break;
		case CreateIx:
			if (objc!=5) goto err;
			return MvthImageCreate(interp,statePtr,objv[2],objv[3],objv[4]);
			break;
		case WidthIx:
		case HeightIx:
		case DepthIx:
			if (objc!=3 && objc!=4) goto err;
			if (objc==3) valueObjPtr=NULL;
			else valueObjPtr=objv[3];
			break;
		case DeleteIx:
			if (objc!=3) goto err;
			break;
		case DuplicateIx:
			if (objc!=3) goto err;
			break;
		case OpenIx:
			if (objc!=3 && objc!=4) goto err;
			break;
	}

	/* The rest of the commands take an mvthimage name
	 * or a file name as the third
	 * argument. Hash from the name to the Mvthimage structure */
	switch (index) {
		case WidthIx:
		case HeightIx:
		case DepthIx:
		case DeleteIx:
		case DuplicateIx:
			entryPtr=Tcl_FindHashEntry(&statePtr->hash,Tcl_GetString(objv[2]));
			if (entryPtr==NULL) {
				Tcl_AppendResult(interp,"Unknown mvthimage: ",
						Tcl_GetString(objv[2]),NULL);
				return TCL_ERROR;
			}
			iPtr=(MvthImage*)Tcl_GetHashValue(entryPtr);
			break;
		case OpenIx:
			/* get the filename */
			if (objc==4) { /* get the name of an existing mvthimage */
				entryPtr=Tcl_FindHashEntry(&statePtr->hash,Tcl_GetString(objv[3]));
				if (entryPtr==NULL) {
					Tcl_AppendResult(interp,"Unknown mvthimage: ",
							Tcl_GetString(objv[3]),NULL);
					return TCL_ERROR;
				}
				iPtr=(MvthImage*)Tcl_GetHashValue(entryPtr);
			} else {
				iPtr=NULL;
			}
			return MvthImageOpen(interp,statePtr,objv[2],iPtr);

			break;
	}

	switch (index) {
		case WidthIx:
			return MvthImageWHD(interp,iPtr,valueObjPtr,0);
		case HeightIx:
			return MvthImageWHD(interp,iPtr,valueObjPtr,1);
		case DepthIx:
			return MvthImageWHD(interp,iPtr,valueObjPtr,2);
		case DeleteIx:
			return MvthImageDelete(iPtr,entryPtr);
		case DuplicateIx:
			return MvthImageDuplicate(interp,statePtr,iPtr);
			break;
	}
	return TCL_OK;

err:
	Tcl_WrongNumArgs(interp,1,objv,"option ?arg ...?");
	return TCL_ERROR;
}


int newMvthImage(Tcl_Interp *interp,
	Tcl_Obj *wObjPtr,
	Tcl_Obj *hObjPtr,
	Tcl_Obj *dObjPtr, MvthImage **iPtr_in)
{
	MvthImage *iPtr;
	int w,h,d;

	/* first try to parse the w, h, and depth objects */
	if (Tcl_GetIntFromObj(interp,wObjPtr,&w)!=TCL_OK) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,hObjPtr,&h)!=TCL_OK) return TCL_ERROR;
	if (Tcl_GetIntFromObj(interp,dObjPtr,&d)!=TCL_OK) return TCL_ERROR;

	iPtr=(MvthImage*)ckalloc(sizeof(MvthImage));
	iPtr->widthPtr=wObjPtr; Tcl_IncrRefCount(wObjPtr);
	iPtr->heightPtr=hObjPtr; Tcl_IncrRefCount(hObjPtr);
	iPtr->depthPtr=dObjPtr; Tcl_IncrRefCount(dObjPtr);
	iPtr->img=new_image_t(w,h,d);
	*iPtr_in=iPtr;
	return TCL_OK;
}


/* the following routine actually creates MvthImages */
int MvthImageCreate(Tcl_Interp *interp, MvthImageState *statePtr,
		Tcl_Obj *wObjPtr, Tcl_Obj *hObjPtr, Tcl_Obj *dObjPtr)
{
	Tcl_HashEntry *entryPtr;
	MvthImage *iPtr;
	int new;
	char name[20];

	if (newMvthImage(interp,wObjPtr,hObjPtr,dObjPtr,&iPtr)!=TCL_OK)
		return TCL_ERROR;

	/* generate an MvthImage and put it in the hash table */
	statePtr->uid++;
	sprintf(name,"mvthimage%d",statePtr->uid);
	entryPtr=Tcl_CreateHashEntry(&statePtr->hash,name,&new);
	/* you probably should assert the new==1 to confirm
	 * a new item was added to the hash table */
	Tcl_SetHashValue(entryPtr,(ClientData)iPtr);
	/* copy the name to the interpreter result */
	Tcl_SetStringObj(Tcl_GetObjResult(interp),name,-1);
	return TCL_OK;
}

/* the following routine duplicates MvthImages */
int MvthImageDuplicate(Tcl_Interp *interp, MvthImageState *statePtr,
		MvthImage *iSrcPtr)
{
	Tcl_HashEntry *entryPtr;
	MvthImage *iPtr;
	int new;
	char name[20];

	if (newMvthImage(interp,iSrcPtr->widthPtr,iSrcPtr->heightPtr,iSrcPtr->depthPtr,&iPtr)!=TCL_OK)
		return TCL_ERROR;
	image_t2MvthImage(iSrcPtr->img,iPtr);

	/* put the new MvthImage in the hash table */
	statePtr->uid++;
	sprintf(name,"mvthimage%d",statePtr->uid);
	entryPtr=Tcl_CreateHashEntry(&statePtr->hash,name,&new);
	/* you probably should assert the new==1 to confirm
	 * a new item was added to the hash table */
	Tcl_SetHashValue(entryPtr,(ClientData)iPtr);
	/* copy the name to the interpreter result */
	Tcl_SetStringObj(Tcl_GetObjResult(interp),name,-1);
	return TCL_OK;
}

#include "dynamic_load.h"
/* open a file and either create a new MvthImage, or
 * load the file into an existing one */
int MvthImageOpen(Tcl_Interp *interp, MvthImageState *statePtr,
		Tcl_Obj *fileObjPtr, MvthImage *mimgPtr)
{
	Tcl_HashEntry *entryPtr;
	MvthImage *iPtr;
	int new;
	char name[20];
	image_t *img=NULL;
	char *filename=NULL;

	/* first try to load the file */
	void *libhandle=NULL;
	image_t * (*readimage)(char *filename)=NULL;
	readimage=load_symbol(MVTHIMAGELIB,"readimage",&libhandle);
	assert(readimage!=NULL);
	/* read the image */
	img=readimage(filename=Tcl_GetStringFromObj(fileObjPtr,NULL));
	if (img==NULL || img->data==NULL) {
		if (img!=NULL) free(img);
		Tcl_AppendResult(interp,"Trouble reading image `",filename,"'",NULL);
		return TCL_ERROR;
	}

	if (mimgPtr!=NULL) {
		/* just replace the image with the new one */
		mvthImageReplace(img,mimgPtr);
		return TCL_OK;
	}

	/* otherwise, we need to create a new one */

	/* generate an MvthImage and put it in the hash table */
	statePtr->uid++;
	sprintf(name,"mvthimage%d",statePtr->uid);
	entryPtr=Tcl_CreateHashEntry(&statePtr->hash,name,&new);
	/* you probably should assert the new==1 to confirm
	 * a new item was added to the hash table */
	iPtr=(MvthImage*)ckalloc(sizeof(MvthImage));

	iPtr->widthPtr=Tcl_NewIntObj(img->w);
	iPtr->heightPtr=Tcl_NewIntObj(img->h);
	iPtr->depthPtr=Tcl_NewIntObj(img->bands);

	Tcl_IncrRefCount(iPtr->widthPtr);
	Tcl_IncrRefCount(iPtr->heightPtr);
	Tcl_IncrRefCount(iPtr->depthPtr);

	iPtr->img=img;

	Tcl_SetHashValue(entryPtr,(ClientData)iPtr);
	/* copy the name to the interpreter result */
	Tcl_SetStringObj(Tcl_GetObjResult(interp),name,-1);
	return TCL_OK;
}

/* and delete an MvthImage */
int MvthImageDelete(MvthImage *iPtr, Tcl_HashEntry *entryPtr)
{
	Tcl_DeleteHashEntry(entryPtr);
	if (iPtr->widthPtr!=NULL) {Tcl_DecrRefCount(iPtr->widthPtr);}
	if (iPtr->heightPtr!=NULL) {Tcl_DecrRefCount(iPtr->heightPtr);}
	if (iPtr->depthPtr!=NULL) {Tcl_DecrRefCount(iPtr->depthPtr);}
	if (iPtr->img!=NULL) {free_image_t(iPtr->img);}
	Tcl_Free((char*)iPtr);
	return TCL_OK;
}

int MvthImageNames(Tcl_Interp *interp, MvthImageState *statePtr)
{
	Tcl_HashEntry *entryPtr;
	Tcl_HashSearch search;
	Tcl_Obj *listPtr;
	char *name;
	/* Walk the hash table and build a list of names */
	listPtr=Tcl_NewListObj(0,NULL);
	entryPtr=Tcl_FirstHashEntry(&statePtr->hash,&search);
	while (entryPtr!=NULL) {
		name=Tcl_GetHashKey(&statePtr->hash,entryPtr);
		if (Tcl_ListObjAppendElement(interp,listPtr,
					Tcl_NewStringObj(name,-1))!=TCL_OK) return TCL_ERROR;
		entryPtr=Tcl_NextHashEntry(&search);
	}
	Tcl_SetObjResult(interp,listPtr);
	return TCL_OK;
}

int MvthImageWHD(Tcl_Interp *interp, MvthImage *iPtr, Tcl_Obj *objPtr, int i)
{
	int w,h,d;
	int dim;
	if (objPtr!=NULL) {
		if (Tcl_GetIntFromObj(interp,objPtr,&dim)!=TCL_OK)
			return TCL_ERROR;
		/* what are the current image parameters ? */
		w=iPtr->img->w;
		h=iPtr->img->h;
		d=iPtr->img->bands;
		switch (i) {
			case 0: /* width */
				w=dim;
				if (iPtr->img->w!=w) {
					free_image_t(iPtr->img);
					iPtr->img=new_image_t(w,h,d);
					if (iPtr->widthPtr!=NULL) {
						Tcl_DecrRefCount(iPtr->widthPtr);
					}
					iPtr->widthPtr=objPtr;
					Tcl_IncrRefCount(objPtr);
				}
				break;
			case 1:
				h=dim;
				if (iPtr->img->h!=h) {
					free_image_t(iPtr->img);
					iPtr->img=new_image_t(w,h,d);
					if (iPtr->heightPtr!=NULL)
					{
						Tcl_DecrRefCount(iPtr->heightPtr);
					}
					iPtr->heightPtr=objPtr;
					Tcl_IncrRefCount(objPtr);
				}
				break;
			case 2:
				d=dim;
				if (iPtr->img->bands!=d) {
					free_image_t(iPtr->img);
					iPtr->img=new_image_t(w,h,d);
					if (iPtr->depthPtr!=NULL)
					{
						Tcl_DecrRefCount(iPtr->depthPtr);
					}
					iPtr->depthPtr=objPtr;
					Tcl_IncrRefCount(objPtr);
				}
				break;
		}
	} else {
		switch (i) {
			case 0:
				Tcl_SetObjResult(interp,iPtr->widthPtr);
				break;
			case 1:
				Tcl_SetObjResult(interp,iPtr->heightPtr);
				break;
			case 2:
				Tcl_SetObjResult(interp,iPtr->depthPtr);
				break;
		}
	}
	return TCL_OK;
}

int updateMvthImageDims(MvthImage *mimg, int w, int h, int d)
{
	if (mimg==NULL || mimg->img==NULL || mimg->img->data==NULL)
		return TCL_ERROR;
	Tcl_DecrRefCount(mimg->widthPtr);
	Tcl_DecrRefCount(mimg->heightPtr);
	Tcl_DecrRefCount(mimg->depthPtr);
	mimg->widthPtr=Tcl_NewIntObj(w);
	mimg->heightPtr=Tcl_NewIntObj(h);
	mimg->depthPtr=Tcl_NewIntObj(d);
	Tcl_IncrRefCount(mimg->widthPtr);
	Tcl_IncrRefCount(mimg->heightPtr);
	Tcl_IncrRefCount(mimg->depthPtr);
	return TCL_OK;
}

int mvthImageReplace(image_t *img, MvthImage *mimg)
{
	if (mimg==NULL) return TCL_ERROR;
	if (mimg->img!=NULL) free_image_t(mimg->img);
	mimg->img=img;
	updateMvthImageDims(mimg,img->w,img->h,img->bands);
	return TCL_OK;
}

int image_t2MvthImage(image_t *img, MvthImage *mimg)
{
	image_t *dst=mimg->img;
	if (img==NULL || mimg==NULL || img->data==NULL || mimg->img==NULL) return -1;
	if (img->w!=dst->w || img->h!=dst->h || img->bands!=dst->bands)
	{
		/* free the old one and copy the new one */
		free_image_t(dst);
		mimg->img=NULL;
		dst=new_image_t(img->w,img->h,img->bands);
	}
	/* just do a straight copy */
	memmove(dst->data,img->data,img->w*img->h*img->bands*sizeof(float));
	dst->timestamp=img->timestamp;
	/* update the object */
	if (mimg->img==NULL) mimg->img=dst;
	updateMvthImageDims(mimg,img->w,img->h,img->bands);
	return TCL_OK;
}
