/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Routines for manipulating images. 
 *
 * Copyright (C) 2003,2004,2005 Samuel P. Bromley <sam@sambromley.com>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "images_types.h"

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
	img->d=1;
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

/* We need a way to create new 3D images */
image_t *new_3d_image_t(int w, int h, int d, int bands) 
{
	image_t *img;

	if (w<=0 || h<=0 || d<=0 || bands<=0 || bands>4)
	{
		fprintf(stderr,"Invalid parameters passed to new_3d_image_t()\n");
		fprintf(stderr,"(w=%d h=%d d=%d bands=%d)\n",w,h,d,bands);
		return NULL;
	}
	img=(image_t*)malloc(sizeof(image_t));
	if (img==NULL) return NULL;
	img->w=w;
	img->h=h;
	img->d=d;
	img->bands=bands;
	img->timestamp=0;
	img->data=(float*)calloc(w*h*d*bands,sizeof(float));
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
	sl=src->w*src->h*src->d*src->bands;
	if (dst->data==NULL)
	{
		dst->data=(float*)malloc(sl*sizeof(float));
		dst->w=src->w;
		dst->h=src->h;
		dst->d=src->d;
		dst->bands=src->bands;
		memcpy(dst->data,src->data,sl*sizeof(float));
		return 0;
	}
	dl=dst->w*dst->h*dst->d*dst->bands;
	if (dl!=sl) dst->data=realloc(dst->data,sl*sizeof(float));
	memcpy(dst->data,src->data,sl*sizeof(float));
	dst->w=src->w;
	dst->h=src->h;
	dst->d=src->d;
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
				mi->w*mi->h*mi->d*mi->bands*sizeof(float));
	}
	return;
}

int resize_image_t(image_t *mi, int w, int h, int bands)
{
	return resize_3d_image_t(mi,w,h,1,bands);
}

int resize_3d_image_t(image_t *mi, int w, int h, int d, int bands)
{
	if (w*h*d*bands<=0) return -1;

	//assert(mi->image->wlock==1); /* for now, crash if lock not held */

	if (w*h*d*bands!=mi->w*mi->h*mi->d*mi->bands)
	{
		/* then we need to resize the image data */
		float *nd=
			(float*)realloc(mi->data,w*h*d*bands*sizeof(float));
		assert(nd!=NULL);
		mi->data=nd;
	}
	mi->w=w;
	mi->h=h;
	mi->d=d;
	mi->bands=bands;
	return 0;
}

int print_image_t(FILE *fp, image_t *img)
{
	if (img!=NULL)
	{
		return fprintf(fp,"[%d,%d,%d,%d] @ %p\n",img->w,img->h,img->d,img->bands,img->data);
	}
	fprintf(fp,"NULL\n");
	return 0;
}

int sprint_image_t(char *buff, image_t *img)
{
	if (img!=NULL)
	{
		return sprintf(buff,"%dx%dx%dx%d",img->w,img->h,img->d,img->bands);
	}
	else
	{
		sprintf(buff,"NULL");
	}
	return 0;
}
