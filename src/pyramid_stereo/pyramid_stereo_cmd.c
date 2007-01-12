/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Pyramid based stereo.
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
#include "base/stereo_context.h"

static void usage(void)
{
	mvthprint(stderr,"\npyramid stereo syntax:\n");
	mvthprint(stderr,"pyr <i1> <i2> <nlevels> <disp> <corr>\n\n");
	mvthprint(stderr,"Does area based stereo on i1,i2 using image pyramid of depth <nlevels>.\n");
	return;
}

extern void pyramid_stereo_fltr(image_t *i1, image_t *i2,
		int nlevels, SContext_t *sc);

MvthReplyCode pyramid_stereo_cmd(Command *cmd)
{
	char *i1,*i2;
	int range, window, nlev;
	char *ptr;
	Arg *a;
	image_t *img1; /* left WImage  */
	image_t *img2; /* right WIMage */

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=6)
	{
		return S_USAGE;
	}

	a=cmd->arglist->next;
	i1=a->str;
	a=a->next;
	i2=a->str;
	a=a->next;
	nlev=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || nlev<0 || nlev>5)
	{
		mvthprint(stderr,"pyramid_stereo(): Invalid nlev. Must be in [0,5]\n");
		return S_ARGS;
	}
	a=a->next;
	range=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || range<2 || range>48)
	{
		mvthprint(stderr,"pyramid_stereo(): Invalid range. Must be in [2,48]\n");
		return S_ARGS;
	}

	a=a->next;
	window=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || window<=0 || window>32)
	{
		mvthprint(stderr,"pyramid_stereo(): Invalid comparison-window size. Must be in [1,32]\n");
		return S_ARGS;
	}

	/* get images  */
	img1=get_image_var(i1);
	img2=get_image_var(i2);

	if (img1==NULL || img2==NULL)
	{
		return S_NOEXIST;
	}

	if (img1->w!=img2->w
			|| img1->h!=img2->h)
	{
		return S_MISMATCH;
	}

	stereo_context.range=range;
	stereo_context.window=window;
	/* do the filter */
	pyramid_stereo_fltr(img1, img2, nlev, &stereo_context);

	return S_SUCCESS;
}
