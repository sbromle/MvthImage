/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a disparity map.
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
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mvthPlugin.h>
#include "base/images_types.h"
#include "base/images_utils.h"
#include "base/stereo_context.h"
#include "utils/timestamp.h"

extern int verbose;

static void usage(void)
{
	mvthprint(stderr,"\ndmap syntax:\n");
	mvthprint(stderr,"dmap <image #> [scale val (>0)]\n\n");
	return;
}

MvthReplyCode dmap_draw_fltr(image_t *img, SContext_t *sc, float scale);

int dmap_draw_cmd(Command *cmd)
{
	char *iname;
	Arg *a;
	char *ptr;
	image_t *img=NULL;
	int w,h,bands;
	int nvalid;
	float scale=-1.0;

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=2 && cmd->narg!=3)
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

	if (cmd->narg==3)
	{
		a=a->next;
		/* get the source image number */
		scale=(float)strtod(a->str,&ptr);
		if (ptr==a->str || (scale<=0.0))
		{
			mvthprint(stderr,"dmap_draw_cmd(): scale must be >0.\n");
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

	nvalid=dmap_draw_fltr(img,&stereo_context, scale);
	stamp_image_t(img);
	
		
	if (verbose)
	{
		mvthprint(stderr,"Drew dmap to image \"%s\"\n",iname);
		mvthprint(stdout,"%d valid points in dmap\n",nvalid);
	}

	return S_SUCCESS;
}
