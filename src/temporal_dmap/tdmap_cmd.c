/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a disparity map.
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
#include "base/images_types.h"
#include "base/images_utils.h"
#include "base/stereo_context.h"
#include "utils/timestamp.h"

/* copy existing dmap to a temporally buffered dmap */
static void usage(void)
{
	mvthprint(stderr,"\ntdmap syntax:\n");
	mvthprint(stderr,"tdmap <image #> <age> [scale val (>0)]\n\n");
	return;
}

extern SContext_t tdmap_fltr(SContext_t *sc, int age);
extern int dmap_draw_fltr(image_t *img, SContext_t *sc, float scale);
extern int linfill_dmap_fltr(SContext_t *sc);

MvthReplyCode tdmap_cmd(Command *cmd)
{
	char *iname;
	int age;
	Arg *a;
	char *ptr;
	image_t *img=NULL;
	int w,h,bands;
	int nvalid;
	float scale=-1.0;
	SContext_t sc;

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=3 && cmd->narg!=4)
	{
		return S_USAGE;
	}

	/* test to see if a disparity map exists */
	if (stereo_context.dmap == NULL
			|| stereo_context.w<=0
			|| stereo_context.h<=0)
	{
		mvthprint(stderr,"Stereo context not yet valid.\n");
		return S_ERROR;
	}

	a=cmd->arglist->next;
	/* get the source image number */
	iname=a->str;

	a=a->next;
	/* get the source image number */
	age=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || (age<0))
	{
		mvthprint(stderr,"tdmap_draw_cmd(): age must be >=0.\n");
		return S_ARGS;
	}

	if (cmd->narg==4)
	{
		a=a->next;
		/* get the source image number */
		scale=(float)strtod(a->str,&ptr);
		if (ptr==a->str || (scale<=0.0))
		{
			mvthprint(stderr,"tdmap_draw_cmd(): scale must be >0.\n");
			return S_ARGS;
		}
	}

	w=stereo_context.w;
	h=stereo_context.h;
	bands=-3; /* ask for bands=3 unless wimg already a grayscale */

	/* get the image */
	img=get_image_var(iname);
	if (img==NULL)
	{
		return S_NOEXIST;
	}

	/* register with undo */
	register_image_undo_var(iname);

	if (img->w!=w || img->h!=h)
	{
		resize_image_t(img,w,h,img->bands);
	}

	/* store the new info */
	sc=tdmap_fltr(&stereo_context,age);

//#define LINFILL_DMAP
#ifdef LINFILL_DMAP
	linfill_dmap_fltr(&sc);
#endif

	nvalid=dmap_draw_fltr(img,&sc, scale);
	stamp_image_t(img);
	

		
	mvthprint(stderr,"Drew dmap to image \"%s\"\n",iname);

	mvthprint(stdout,"%d valid points in dmap\n",nvalid);

	return S_SUCCESS;
}
