/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Roberts edge detection.
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
#include "utils/timestamp.h"

static void usage(void)
{
	mvthprint(stderr,"\nroberts edge detector syntax:\n");
	mvthprint(stderr,"rob <image #>\n\n");
	return;
}

extern void roberts_fltr(image_t *wimg);

MvthReplyCode roberts_cmd(Command *cmd)
{
	char *iname;
	Arg *a;
	image_t *img; /* must find this */

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=3)
	{
		return S_USAGE;
	}

	a=cmd->arglist->next;
	iname=a->str;
	/* get image */
	if ((img=get_image_var(iname))==NULL)
	{
		return S_NOEXIST;
	}

	/* register it with the undo substructure */
	register_image_undo_var(iname);

	/* do the filter */
	roberts_fltr(img);

	stamp_image_t(img);

	return S_SUCCESS;
}
