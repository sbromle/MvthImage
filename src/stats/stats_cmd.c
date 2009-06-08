/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Calculate and report stats for an image.
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
