/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Calculate and report stats for an image.
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
#include <string.h>
#include <mvthPlugin.h>
#include "base/images_types.h"

static void usage(void)
{
	mvthprint(stderr,"\nstats syntax:\n");
	mvthprint(stderr,"stats <image #> [filename]\n\n");
	mvthprint(stderr," -- Print out some statistical info about image.\n\n");
	return;
}

extern int stats_fltr(image_t *img, char *filename);

MvthReplyCode stats_cmd(Command *cmd)
{
	char *iname;
	char *filename=NULL;
	Arg *a;
	image_t *img; /* must find this */

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=2 && cmd->narg!=3)
	{
		return S_USAGE;
	}

	a=cmd->arglist->next;
	iname=a->str;

	if (cmd->narg==3)
	{
		a=a->next;
		filename=a->str;
	}
	
	/* get image */
	if ((img=get_image_var(iname))==NULL)
	{
		return S_NOEXIST;
	}

	/* do the filter */
	if (stats_fltr(img,filename)!=0)
	{
		return S_ERROR;
	}

	return S_SUCCESS;
}
