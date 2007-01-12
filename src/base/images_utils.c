/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Routines for manipulating images. 
 *
 * Copyright (C) 2003,2004,2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "images_context.h"

/* We need a way to create new images */
image_t *new_image_t(int w, int h, int bands) 
{
	image_t *img;

	if (w<=0 || h<=0 || bands<=0 || bands>4)
	{
		fprintf(stderr,"Invalid parameters passed to new_image_t()\n");
		fprintf(stderr,"(w=%d h=%d bands=%d)\n",w,h,bands);
		return NULL;
	}
	img=(image_t*)malloc(sizeof(image_t));
	if (img==NULL) return NULL;
	img->w=w;
	img->h=h;
	img->bands=bands;
	img->timestamp=0;
	img->data=(float*)calloc(w*h*bands,sizeof(float));
	if (img->data==NULL)
	{
		free(img);
		return NULL;
	}
	return img;
}

void free_image_t(image_t *img)
{
	if (img==NULL) return;
	else if (img->data!=NULL) free(img->data);
	free(img);
	return;
}

int copy_image_t(image_t *src, image_t *dst)
{
	int sl,dl;
	if (src==NULL || dst==NULL) return -1;
	else if (src->data==NULL) return -1;
	sl=src->w*src->h*src->bands;
	if (dst->data==NULL)
	{
		dst->data=(float*)malloc(sl*sizeof(float));
		dst->w=src->w;
		dst->h=src->h;
		dst->bands=src->bands;
		memcpy(dst->data,src->data,sl*sizeof(float));
		return 0;
	}
	dl=dst->w*dst->h*dst->bands;
	if (dl!=sl) dst->data=realloc(dst->data,sl*sizeof(float));
	memcpy(dst->data,src->data,sl*sizeof(float));
	dst->w=src->w;
	dst->h=src->h;
	dst->bands=src->bands;
	dst->timestamp=src->timestamp;
	return 0;
}

void zero_image_t(image_t *mi)
{
	if (mi!=NULL && mi->data!=NULL)
	{
		//assert(mi->image->wlock==1); /* for now, crash if lock not held */
		memset(mi->data,0,
				mi->w*mi->h*mi->bands*sizeof(float));
	}
	return;
}

int resize_image_t(image_t *mi, int w, int h, int bands)
{
	if (w*h*bands<=0) return -1;

	//assert(mi->image->wlock==1); /* for now, crash if lock not held */

	if (w*h*bands!=mi->w*mi->h*mi->bands)
	{
		/* then we need to resize the image data */
		float *nd=
			(float*)realloc(mi->data,w*h*bands*sizeof(float));
		assert(nd!=NULL);
		mi->data=nd;
	}
	mi->w=w;
	mi->h=h;
	mi->bands=bands;
	return 0;
}

int print_image_t(FILE *fp, image_t *img)
{
	if (img!=NULL)
	{
		return fprintf(fp,"[%d,%d,%d] @ %p\n",img->w,img->h,img->bands,img->data);
	}
	fprintf(fp,"NULL\n");
	return 0;
}

int sprint_image_t(char *buff, image_t *img)
{
	if (img!=NULL)
	{
		return sprintf(buff,"%dx%dx%d",img->w,img->h,img->bands);
	}
	else
	{
		sprintf(buff,"NULL");
	}
	return 0;
}
