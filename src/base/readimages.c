/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Read in an image from a file.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <IL/il.h>
#include <dlfcn.h>

#include "base/images_types.h"

image_t *readimage(char *filename)
{
	int w,h,bands,bpc;
	image_t *ni=NULL;
	void *handle=NULL;
	ILuint imageid;
	/* library functions that we will look up dynamically */
	ILint (*dlilGetInteger)(ILenum);
	void (*dlilInit)(void);
	void (*dlilShutDown)(void);
	void (*dlilBindImage)(ILuint imageid);
	ILboolean (*dlilGenImages)(ILsizei,ILuint *imageid);
	ILboolean (*dlilLoadImage)(char *filename);
	void (*dlilDeleteImages)(ILsizei,const ILuint *);
	void (*dlilCopyPixels)(
			ILuint Xoff,
			ILuint Yoff,
			ILuint Zoff,
			ILuint Width,
			ILuint Height,
			ILuint Depth,
			ILenum Format,
			ILenum Type,
			void *Data);

	dlerror(); /* clear any old errors in dynamic library loading */
	/* get the library handle */
	handle=dlopen("libIL.so.1",RTLD_LAZY);
	if (handle==NULL)
	{
		fprintf(stderr,"Error opening library `libIL.so.1'\n");
		fprintf(stderr,"%s\n",dlerror());
	}

	/* get all the function handles */
	
	/******* ilGetInteger ***********/
	dlerror();
	dlilGetInteger=dlsym(handle,"ilGetInteger");
	if (dlilGetInteger==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		return NULL;
	}

	/******* ilInit ***********/
	dlerror();
	dlilInit=dlsym(handle,"ilInit");
	if (dlilInit==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		return NULL;
	}

	/******* ilShutDown ***********/
	dlerror();
	dlilShutDown=dlsym(handle,"ilShutDown");
	if (dlilShutDown==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		return NULL;
	}

	/******* ilGenImages ***********/
	dlerror();
	dlilGenImages=dlsym(handle,"ilGenImages");
	if (dlilGenImages==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		return NULL;
	}

	/******* ilBindImage ***********/
	dlerror();
	dlilBindImage=dlsym(handle,"ilBindImage");
	if (dlilBindImage==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		return NULL;
	}

	/******* ilLoadImage ***********/
	dlerror();
	dlilLoadImage=dlsym(handle,"ilLoadImage");
	if (dlilLoadImage==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		return NULL;
	}

	/******* ilDeleteImages ***********/
	dlerror();
	dlilDeleteImages=dlsym(handle,"ilDeleteImages");
	if (dlilDeleteImages==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		return NULL;
	}

	/******* ilCopyPixels ***********/
	dlerror();
	dlilCopyPixels=dlsym(handle,"ilCopyPixels");
	if (dlilCopyPixels==NULL)
	{
		fprintf(stderr,"Error getting symbols.\n");
		fprintf(stderr,"%s\n",dlerror());
		dlclose(handle);
		return NULL;
	}

	if (dlilGetInteger(IL_VERSION_NUM)<IL_VERSION)
	{
		fprintf(stderr,"DevIL library versions differ!\n");
		dlclose(handle);
		return NULL;
	}

	/* initialize the devIL library */
	dlilInit();

  /* generate an image name */
  dlilGenImages(1,&imageid);

  /* bind to this image as current image to do stuff */
  dlilBindImage(imageid);

  /* load an image */
  if (!dlilLoadImage(filename))
  {
		fprintf(stderr,"Error loading image `%s'\n",filename);
		dlilDeleteImages(1,&imageid);
		dlilShutDown();
		dlclose(handle);
		return NULL;
  }

  /* print out info about image */
  w=dlilGetInteger(IL_IMAGE_WIDTH);
  h=dlilGetInteger(IL_IMAGE_HEIGHT);
  bands=dlilGetInteger(IL_IMAGE_BPP);
  bpc=dlilGetInteger(IL_IMAGE_BPC);
	bands/=bpc;
  fprintf(stderr,"Image is %u by %u with %d channels at %u bpc.\n",
			w,h,bands,bpc);

	/* set up the image */
	ni=(image_t*)malloc(sizeof(image_t));
	ni->w=w;
	ni->h=h;
	ni->bands=bands;
	ni->data=(float*)malloc(w*h*bands*sizeof(float));
	unsigned char *tmpdata=(unsigned char *)malloc(w*h*bands*bpc*sizeof(unsigned char));

	unsigned int format=IL_RGB;
	unsigned int type=IL_UNSIGNED_BYTE;
	switch(bands) {
		case 3:
			format=IL_RGB;
			break;
		case 4:
			format=IL_RGBA;
			break;
		default:
			format=IL_LUMINANCE;
			break;
	}
	switch(bpc) {
		case 1:
			type=IL_UNSIGNED_BYTE;
			break;
		case 2:
			type=IL_UNSIGNED_SHORT;
			break;
	}

	dlilCopyPixels(0,0,0,w,h,1,format,type,tmpdata);

	/* convert tmp data to floats */
	unsigned int val=0;
	unsigned int maxval=(1<<bpc*8)-1;
	int i;
	for (i=0;i<w*h*bands;i++) {
		if (bpc==1) val=(unsigned int)tmpdata[i];
		else if (bpc==2) val=(unsigned int)((short*)tmpdata)[i];
		else if (bpc==4) val=((unsigned int*)tmpdata)[i];
		ni->data[i]=(float)val/(float)maxval;
	}

  	/* delete the image */
  	dlilDeleteImages(1,&imageid);

	/* shutdown DevIL and free all memory */
	dlilShutDown();

  	dlclose(handle);

	free(tmpdata);

	return ni;
}
