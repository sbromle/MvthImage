/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Maintain image contexts.
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
#include <generic_contexts.h>
#include "images_context.h"
#include "images_utils.h"
#include "../utils/timestamp.h"

#define IMAGE_CONTEXT_NAME "image"
static GenericContext *_gcimage=NULL;

/* if used by our plugin interface library (see
 * src/interfaces/mvth_image_interface.c), EXTERNAL_INTERFACE
 * is defined. This allows us to prevent code duplication.
 */
#ifndef EXTERNAL_INTERFACE
/* The mvthimage plugin uses this to set up a context with mvth core */
int init_image_context(void)
{
	generic_context_new(IMAGE_CONTEXT_NAME);
	_gcimage=generic_context_get_by_name(IMAGE_CONTEXT_NAME);
	return 0;
}
#else
/* Plugins external to mvthimage use this to connect to the mvthimage
 * plugin already registered with mvthcore. */
static int attach_to_image_context(void)
{
	if (_gcimage!=NULL) return 0;
	_gcimage=generic_context_get_by_name(IMAGE_CONTEXT_NAME);
	if (_gcimage==NULL) return -1;
	return 0;
}
#endif

#ifndef EXTERNAL_INTERFACE
/* the following 3 functions are never used outside of mvthimage */
char **get_image_vars(int *numvar)
{
	return generic_context_get_vars(_gcimage,numvar);
}
#endif

/* get a pointer to an image */
image_t *get_image_var(char *varname)
{
	ATTACH_TO_CONTEXT(NULL)
	return (image_t*)generic_context_get_var(_gcimage,varname);
}

#ifndef EXTERNAL_INTERFACE
/* external modules need not register or deregister images */

/* add an image_t to our array */
int register_image_var(image_t *ni, char *varname)
{
	image_t *prev=NULL;
	ATTACH_TO_CONTEXT(-1)
	if (ni==NULL) {
		fprintf(stderr,"register_image got NULL image_t pointer!\n");
		return -2;
	}
	snprintf(ni->name,sizeof(ni->name),"%s",varname);
	/* get any image that used be be referenced by varname */
	prev=(image_t*)generic_context_steal_var(_gcimage,varname);
	if (prev!=NULL)
	{
		if (prev->data!=NULL) free(prev->data);
		free(prev);
	}
	generic_context_set_var(_gcimage,varname,(void*)ni);
	//if (verbose) fprintf(stderr,"Registered image with name \"%s\"\n",varname);
	return 0;
}

/* deregisters a image named varname. */
int deregister_image_var(char *varname)
{
	image_t *old;
	old=(image_t*)generic_context_steal_var(_gcimage,varname);
	if (old!=NULL)
	{
		if (old->data!=NULL) free(old->data);
		free(old);
	}
	return 0;
}

#endif

int register_image_undo_var(char *varname)
{
	image_t *img;
	image_t *imgcopy;
	ATTACH_TO_CONTEXT(-1)
	/* get the image currently called varname */
	img=(image_t*)generic_context_get_var(_gcimage,varname);
	if (img==NULL) return -1;
	/* otherwise, make a copy */
	imgcopy=(image_t*)malloc(sizeof(image_t));
	imgcopy->w=img->w;
	imgcopy->h=img->h;
	imgcopy->bands=img->bands;
	imgcopy->timestamp=img->timestamp;
	imgcopy->data=(float*)calloc(img->w*img->h*img->bands,sizeof(float));
	memcpy(imgcopy->data,img->data,img->w*img->h*img->bands*sizeof(float));
	snprintf(imgcopy->name,sizeof(imgcopy->name),"%s",img->name);
	/* register with the undo system */
	img=(image_t*)generic_context_register_undo_var(_gcimage,imgcopy,varname);
	/* free the old image that used to be registered */
	if (img!=NULL)
	{
		if (img->data!=NULL) free(img->data);
		free(img);
	}
	return 0;
}


char *image_undo_var(int *error_id)
{
	ATTACH_TO_CONTEXT(NULL)
	char *result=generic_context_undo_var(_gcimage,error_id);
	if (result!=NULL)
	{
		image_t *mi=get_image_var(result);
		if (mi!=NULL) {
			fprintf(stderr,"Stamped image %s\n",result);
			stamp_image_t(mi);
			flush_image_t(mi);
		}
	}
	return result;
}
