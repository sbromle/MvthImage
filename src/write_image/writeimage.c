/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Write out an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
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
#include <assert.h>
#include <limits.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <dlfcn.h>

#include "base/images_context.h"

int writeimage(float *img,int w,int h,int bands,int bpc,char *filename)
{
	int k;
	void *handle=NULL;
	void *handleILU=NULL;

	ILuint imageid;
	/* library functions that we will look up dynamically */
	ILint (*dlilGetInteger)(ILenum);
	ILboolean (*dliluGetInteger)(ILenum);
	ILvoid (*dlilSetInteger)(ILenum,ILint);
	ILvoid (*dlilInit)(ILvoid);
	ILvoid (*dlilBindImage)(ILuint imageid);
	ILboolean (*dlilOriginFunc)(ILenum Mode);
	ILboolean (*dlilGenImages)(ILsizei,ILuint *imageid);
	ILboolean (*dlilSaveImage)(char *filename);
	ILvoid (*dlilDeleteImages)(ILsizei,const ILuint *);
	ILboolean (*dlilTexImage)(
			ILuint Width,
			ILuint Height,
			ILuint Depth,
			ILubyte Bpp,
			ILenum Format,
			ILenum Type,
			ILvoid *Data);
	/* ILU library functions */
	ILvoid (*dliluInit)(ILvoid);
	ILboolean (*dliluFlipImage)(ILvoid);

	if (bpc!=1 && bpc!=2 && bpc!=4) {
		fprintf(stderr,"writeimage: bpc must be 1, 2, or 4.\n");
		return -1;
	}

	dlerror(); /* clear any old errors in dynamic library loading */
	/* get the library handle */
	handle=dlopen("libIL.so.1",RTLD_LAZY);
	if (handle==NULL)
	{
		fprintf(stderr,"Error opening library `libIL.so.1'\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror(); /* clear any old errors in dynamic library loading */
	/* get the library handle */
	handleILU=dlopen("libILU.so.1",RTLD_LAZY);
	if (handleILU==NULL)
	{
		fprintf(stderr,"Error opening library `libILU.so.1'\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	/* get all the function handles */
	dlerror();
	dlilGetInteger=dlsym(handle,"ilGetInteger");
	if (dlilGetInteger==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dliluGetInteger=dlsym(handleILU,"iluGetInteger");
	if (dliluGetInteger==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dliluFlipImage=dlsym(handleILU,"iluFlipImage");
	if (dliluFlipImage==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dlilSetInteger=dlsym(handle,"ilSetInteger");
	if (dlilSetInteger==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dlilInit=dlsym(handle,"ilInit");
	if (dlilInit==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dliluInit=dlsym(handleILU,"iluInit");
	if (dliluInit==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handleILU);
		dlclose(handle);
		return 0;
	}

	dlerror();
	dlilGenImages=dlsym(handle,"ilGenImages");
	if (dlilGenImages==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dlilBindImage=dlsym(handle,"ilBindImage");
	if (dlilBindImage==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dlilSaveImage=dlsym(handle,"ilSaveImage");
	if (dlilSaveImage==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dlilDeleteImages=dlsym(handle,"ilDeleteImages");
	if (dlilDeleteImages==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dlilTexImage=dlsym(handle,"ilTexImage");
	if (dlilTexImage==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	dlerror();
	dlilOriginFunc=dlsym(handle,"ilOriginFunc");
	if (dlilOriginFunc==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	if (dlilGetInteger(IL_VERSION_NUM)<IL_VERSION
			|| dliluGetInteger(ILU_VERSION_NUM)<ILU_VERSION)
	{
		fprintf(stderr,"DevIL library versions differ!\n");
		dlclose(handle);
		dlclose(handleILU);
		return 0;
	}

	/* initialize the devIL library */
	dlilInit();
	dliluInit();

  /* generate an image name */
  dlilGenImages(1,&imageid);

  /* bind to this image as current image to do stuff */
  dlilBindImage(imageid);

	dlilSetInteger(IL_JPG_QUALITY,80);
	/* create a buffer suitable for DevIL library */
	void *ibuf=NULL;
	if (bpc==1)
		ibuf=malloc(w*h*bands);
	else if (bpc==2)
		ibuf=malloc(w*h*bands*sizeof(short));
	else /* bpc==4 */
		ibuf=malloc(w*h*bands*sizeof(int));

	unsigned int type=IL_UNSIGNED_BYTE;
	for (k=0;k<w*h*bands;k++) {
		int v;
		switch (bpc) {
			case 1:
				type=IL_UNSIGNED_BYTE;
				v=(int)(img[k]*255);
				if (v<0) v=0;
				else if (v>255) v=255;
				((unsigned char*)ibuf)[k]=v;
				break;
			case 2:
				type=IL_UNSIGNED_SHORT;
				v=(int)(img[k]*SHRT_MAX);
				if (v<0) v=0;
				else if (v>SHRT_MAX) v=SHRT_MAX;
				((unsigned short*)ibuf)[k]=v;
				break;
			case 4:
				type=IL_UNSIGNED_INT;
				if (img[k]>1.0) v=INT_MAX;
				else v=(int)(img[k]*INT_MAX);
				if (v<0) v=0;
				((unsigned short*)ibuf)[k]=v;
				break;
		}
	}

	/* create the image buffer from the user buffer */

	if (bands==1)
		dlilTexImage(w,h,1,bands,IL_LUMINANCE,type,(unsigned char*)ibuf);
	else if (bands==3)
		dlilTexImage(w,h,1,bands,IL_RGB,type,(unsigned char*)ibuf);
	else if (bands==4)
		dlilTexImage(w,h,1,bands,IL_RGBA,type,(unsigned char*)ibuf);
	else 
		fprintf(stderr,"HUH? INVALID bands=%d (!=1,3,or 4)\n",bands);

	/* flip the image since DevIL insists on alternate origin */
	dliluFlipImage();

	/* write out the image */
	if (dlilSaveImage(filename)!=1)
	{
		fprintf(stderr,"Error writing image `%s'\n",filename);
	}

  /* delete the image */
  dlilDeleteImages(1,&imageid);

  dlclose(handle);
	free(ibuf);

	return 1;
}
