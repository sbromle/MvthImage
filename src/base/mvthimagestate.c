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
	Tcl_HashTable hash; /* List of images by name */
	int uid;            /* used to auto-generate names */
} MvthImageState;

#define MVTHIMAGESTATEKEY "mvthimagestate"

int mvthImageExists0(Tcl_Interp *interp,
		MvthImageState *statePtr,
		char *name)
{
	Tcl_HashEntry *entryPtr=NULL;
	if (name==NULL) return 0;
	entryPtr=Tcl_FindHashEntry(&statePtr->hash,name);
	if (entryPtr==NULL) return 0;
	return 1;
}

int mvthImageExists(Tcl_Interp *interp, Tcl_Obj *CONST name)
{
	MvthImageState *statePtr=NULL;
	if (name==NULL) return 0;
	statePtr=(MvthImageState*)Tcl_GetAssocData(interp,MVTHIMAGESTATEKEY,NULL);

	return mvthImageExists0(interp,statePtr,Tcl_GetString(name));
}

int mvthImageExistsTcl(Tcl_Interp *interp,
		MvthImageState *statePtr,
		Tcl_Obj *CONST name)
{
	if (mvthImageExists0(interp,statePtr,Tcl_GetString(name)))
	{
		Tcl_SetObjResult(interp,Tcl_NewIntObj(1));
		return TCL_OK;
	}
	Tcl_SetObjResult(interp,Tcl_NewIntObj(0));
	return TCL_OK;
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
int MvthImageCreate(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]);
int MvthImageDuplicate(ClientData data, Tcl_Interp *interp, int objc,
		Tcl_Obj *CONST objv[]);\
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
	Tcl_CreateObjCommand(interp,"mi", MvthImageCmd,
			(ClientData)statePtr,MvthImageCleanup);
	Tcl_VarEval(interp,
			"interp alias {} mvthimage {} mi;",
			"interp alias {} copyimage {} mi copy;",
			NULL);
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
 * 	copy srcname dstname
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
		"create","delete","duplicate","copy","exists","open", "width","height","depth","names","size",NULL};
	enum MvthImageIx {
		CreateIx, DeleteIx, DuplicateIx,CopyIx,ExistsIx,OpenIx, WidthIx, HeightIx, DepthIx, NamesIx,SizeIx};
	int index;

	if (objc==1 || objc>=6) {
		Tcl_WrongNumArgs(interp,1,objv,"option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp,objv[1],subCmds,"option",0,&index)!=TCL_OK)
		return TCL_ERROR;

	switch (index) {
		case ExistsIx:
			if (objc!=3) goto err;
			return mvthImageExistsTcl(interp,statePtr,objv[2]);
			break;
		case NamesIx:
			if (objc>2) goto err;
			return MvthImageNames(interp,statePtr);
			break;
		case CreateIx:
			return MvthImageCreate(statePtr,interp,objc-1,objv+1);
			break;
		case WidthIx:
		case HeightIx:
		case DepthIx:
		case SizeIx:
			if (objc!=3 && objc!=4) goto err;
			if (objc==3) valueObjPtr=NULL;
			else valueObjPtr=objv[3];
			break;
		case DeleteIx:
			if (objc!=3) goto err;
			break;
		case DuplicateIx:
			return MvthImageDuplicate((ClientData)statePtr,interp,objc-1,objv+1);
			break;
		case CopyIx:
			return MvthImageCopy((ClientData)statePtr,interp,objc-1,objv+1);
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
		case SizeIx:
		case DeleteIx:
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
		case SizeIx:
			return MvthImageWHD(interp,iPtr,valueObjPtr,3);
		case DeleteIx:
			return MvthImageDelete(iPtr,entryPtr);
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

int MvthImageCopy(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	image_t *simg;
	image_t *dimg=NULL;
	MvthImage *smimg=NULL;
	MvthImage *dmimg=NULL;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"srcname dstname");
		return TCL_ERROR;
	}

	if (getMvthImageFromObj(interp,objv[1],&smimg)!=TCL_OK) return TCL_ERROR;
	simg=smimg->img;
	if (getMvthImageFromObj(interp,objv[2],&dmimg)!=TCL_OK) return TCL_ERROR;

	if (smimg==dmimg) {
		Tcl_AppendResult(interp,"Source and Destination are the same!",NULL);
		return TCL_ERROR;
	}

	/* make a copy */
	dimg=new_image_t(simg->w,simg->h,simg->bands);
	assert(dimg!=NULL);
	memcpy(dimg->name,simg->name,sizeof(dimg->name));
	copy_image_t(simg,dimg);
	//register_image_var(dimg,dstname);
	//stamp_image_t(dimg);
	mvthImageReplace(dimg,dmimg);

	Tcl_SetObjResult(interp,objv[2]);

	return TCL_OK;
}


/* the following routine actually creates MvthImages */
int MvthImageCreate(ClientData data, Tcl_Interp *interp, 
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImageState *statePtr=(MvthImageState *)data;
	Tcl_HashEntry *entryPtr;
	MvthImage *iPtr;
	int new;
	int len;
	char *name_ptr=NULL;
	char name[20];

	if (objc>3) {
		Tcl_WrongNumArgs(interp,0,NULL,"mi create {w h d} ?name?");
		return TCL_ERROR;
	}

	Tcl_Obj **whd=NULL;
	if (Tcl_ListObjGetElements(interp,objv[1],&len,&whd)!=TCL_OK)
		return TCL_ERROR;
	if (len!=3) {
		Tcl_AppendResult(interp,"list must contain 3 elements, {w h d}\n",NULL);
		return TCL_ERROR;
	}

	/* did the user provide a name for this image? */
	if (objc==3) {
		/* does an image with this name already exist ? */
		name_ptr=Tcl_GetStringFromObj(objv[2],&len);
		/* then we need to get the string */
		if (len==0 || len>20) {
			Tcl_AppendResult(interp,"String length must be >0 and <20 characters.\n",NULL);
			return TCL_ERROR;
		}
		if (mvthImageExists0(interp,statePtr,name_ptr)) {
			Tcl_AppendResult(interp,"Image named ",name_ptr," already exists!\n",NULL);
			return TCL_ERROR;
		}
	} else {
		/* generate an MvthImage and put it in the hash table */
		statePtr->uid++;
		sprintf(name,"mi#%03d",statePtr->uid);
		while (mvthImageExists0(interp,statePtr,name)) {
			statePtr->uid++;
			sprintf(name,"mi#%03d",statePtr->uid);
		}
		name_ptr=name;
	}

	/* ok, we can now safely make the image and register it */
	if (newMvthImage(interp,whd[0],whd[1],whd[2],&iPtr)!=TCL_OK)
		return TCL_ERROR;
	entryPtr=Tcl_CreateHashEntry(&statePtr->hash,name_ptr,&new);
	/* you probably should assert the new==1 to confirm
	 * a new item was added to the hash table */
	Tcl_SetHashValue(entryPtr,(ClientData)iPtr);
	/* copy the name to the interpreter result */
	Tcl_SetStringObj(Tcl_GetObjResult(interp),name_ptr,-1);
	return TCL_OK;
}

/* the following routine duplicates MvthImages */
int MvthImageDuplicate(ClientData data, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	MvthImageState *statePtr=(MvthImageState*)data;
	MvthImage *iSrcPtr;

	Tcl_HashEntry *entryPtr;
	MvthImage *iPtr;
	int new;
	char name[20];
	char *src_name=NULL;
	char *dst_name=NULL;


	if (objc>3) {
		Tcl_WrongNumArgs(interp,0,NULL,"mi duplicate src ?dst?");
		return TCL_ERROR;
	}

	src_name=Tcl_GetString(objv[1]);
	entryPtr=Tcl_FindHashEntry(&statePtr->hash,src_name);
	if (entryPtr==NULL) {
		Tcl_AppendResult(interp,"Unknown mvthimage: ",src_name,NULL);
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
		if (mvthImageExists0(interp,statePtr,dst_name)) {
			/* then we really should copy the image */
			return MvthImageCopy(NULL,interp,3,objv);
		} 
	} else {
		/* generate an MvthImage and put it in the hash table */
		statePtr->uid++;
		sprintf(name,"mi#%03d",statePtr->uid);
		while (mvthImageExists0(interp,statePtr,name)) {
			statePtr->uid++;
			sprintf(name,"mi#%03d",statePtr->uid);
		}
		dst_name=name;
	}
	/* if we get here, dst_name is set and we must create a new image */

	iSrcPtr=(MvthImage*)Tcl_GetHashValue(entryPtr);
	if (newMvthImage(interp,iSrcPtr->widthPtr,iSrcPtr->heightPtr,iSrcPtr->depthPtr,&iPtr)!=TCL_OK)
		return TCL_ERROR;
	image_t2MvthImage(iSrcPtr->img,iPtr);

	/* put the new MvthImage in the hash table */
	entryPtr=Tcl_CreateHashEntry(&statePtr->hash,dst_name,&new);
	/* you probably should assert the new==1 to confirm
	 * a new item was added to the hash table */
	Tcl_SetHashValue(entryPtr,(ClientData)iPtr);
	/* copy the name to the interpreter result */
	Tcl_SetStringObj(Tcl_GetObjResult(interp),dst_name,-1);
	return TCL_OK;
}

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
	int dim[3];
	Tcl_Obj *ptr[3]={NULL,NULL,NULL};

	/* what are the current image parameters ? */
	dim[0]=w=iPtr->img->w;
	dim[1]=h=iPtr->img->h;
	dim[2]=d=iPtr->img->bands;
	int len=0;
	int j;

	if (i<0 || i>3) return TCL_ERROR;

	if (objPtr!=NULL) {
		switch (i) {
			case 0:
			case 1:
			case 2:
				/* then objPtr contains only a single value */
				if (Tcl_GetIntFromObj(interp,objPtr,&dim[i])!=TCL_OK)
					return TCL_ERROR;
				ptr[i]=objPtr;
				break;
			case 3:
				/* otherwise, it should be a list of 3 values */
				if (Tcl_ListObjLength(interp,objPtr,&len)!=TCL_OK) return TCL_ERROR;
				if (len!=3) {
					Tcl_AppendResult(interp,"List must contain 3 elements.\n",NULL);
					return TCL_ERROR;
				}
				for (j=0;j<3;j++) {
					if (Tcl_ListObjIndex(interp,objPtr,j,&ptr[j])!=TCL_OK) return TCL_ERROR;
					if (Tcl_GetIntFromObj(interp,ptr[j],&dim[j])!=TCL_OK) return TCL_ERROR;
				}
				break;
			default:
				break;
		}

		if (dim[0]!=w || dim[1]!=h || dim[2]!=d) {
			/* then some dimension has changed */
			free_image_t(iPtr->img);
			iPtr->img=new_image_t(dim[0],dim[1],dim[2]);
		}
		if (dim[0]!=w) {
			if (iPtr->widthPtr!=NULL) {
				Tcl_DecrRefCount(iPtr->widthPtr);
			}
			iPtr->widthPtr=ptr[0];
			Tcl_IncrRefCount(ptr[0]);
		}
		if (dim[1]!=h) {
			if (iPtr->heightPtr!=NULL) {
				Tcl_DecrRefCount(iPtr->heightPtr);
			}
			iPtr->heightPtr=ptr[1];
			Tcl_IncrRefCount(ptr[1]);
		}
		if (dim[2]!=d) {
			if (iPtr->depthPtr!=NULL) {
				Tcl_DecrRefCount(iPtr->depthPtr);
			}
			iPtr->depthPtr=ptr[2];
			Tcl_IncrRefCount(ptr[2]);
		}
	}
	else  /* if objPtr is NULL, we are just requesting information */
	{
		Tcl_Obj *result=NULL;
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
			case 3:
				result=Tcl_NewListObj(0,NULL);
				if (Tcl_ListObjAppendElement(interp,result,iPtr->widthPtr)!=TCL_OK)
					return TCL_ERROR;
				if (Tcl_ListObjAppendElement(interp,result,iPtr->heightPtr)!=TCL_OK)
					return TCL_ERROR;
				if (Tcl_ListObjAppendElement(interp,result,iPtr->depthPtr)!=TCL_OK)
					return TCL_ERROR;
				Tcl_SetObjResult(interp,result);
				break;
			default:
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
