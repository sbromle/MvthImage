/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Resize an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mvthPlugin.h>
#include "base/images_context.h"
#include "base/images_utils.h"
#include "utils/timestamp.h"

static void usage(void)
{
	mvthprint(stderr,"\nresize image syntax:\n");
	mvthprint(stderr,"resize <image #> <fraction>\n\n");
	return;
}

/* dst should be a pointer to a image_t structure without
 * its dst->data allocated.
 */
extern void resize_fltr(image_t *src, image_t *dst, const float fraction);

MvthReplyCode resize_cmd(Command *cmd)
{
	float fraction;
	char *iname;
	Arg *a;
	char *ptr;
	image_t *img;
	image_t newimg={"",0,0,0,0,NULL};

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=3) return S_USAGE;

	a=cmd->arglist->next;
	iname=a->str;
	a=a->next;
	fraction=(float)strtod(a->str,&ptr);

	if (fraction<=0) return S_ARGS;

	/* get image */
	if ((img=get_image_var(iname))==NULL) return S_NOEXIST;

	/* register it with the undo substructure */
	if (fraction==1.0) return S_SUCCESS;

	register_image_undo_var(iname);

	/* do the filter */
	resize_fltr(img,&newimg,fraction);

	resize_image_t(img,newimg.w,newimg.h,newimg.bands);
	memcpy(img->data,newimg.data,newimg.w*newimg.h*newimg.bands*sizeof(float));
	img->w=newimg.w;
	img->h=newimg.h;
	img->bands=newimg.bands;

	stamp_image_t(img);
	

	free(newimg.data);

	mvthprint(stdout,"Resized image \"%s\" by %f\n",iname,fraction);

	return S_SUCCESS;
}
