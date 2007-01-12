/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert a green channel of an rgb image to grayscale.
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

extern void green2gray_fltr(image_t *img);

static void usage(void)
{
	mvthprint(stderr,"\ngreen2gray syntax:\n");
	mvthprint(stderr,"g2g <image #>\n\n");
	return;
}

MvthReplyCode green2gray_cmd(Command *cmd)
{
	char *iname;
	Arg *a;
	image_t *img; /* must find this */

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=2)
	{
		return S_USAGE;
	}

	a=cmd->arglist->next;
	iname=a->str;
	/* get image and node*/
	if ((img=get_image_var(iname))==NULL) return S_NOEXIST;

	/* register with the undo substructure */
	register_image_undo_var(iname);

	green2gray_fltr(img);

	stamp_image_t(img);
	

	mvthprint(stdout,"Converted green channel of image \"%s\" to grayscale.\n",iname);

	return S_SUCCESS;
}
