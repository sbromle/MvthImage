/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Moravec interest operator filter.
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
#include "utils/timestamp.h"

static void usage(void)
{
	mvthprint(stderr,"\nMoravec interest operator:\n");
	mvthprint(stderr,"moravec <image #> <window>\n\n");
	return;
}

extern void moravec_fltr(image_t *img, int winsize);

MvthReplyCode moravec_cmd(Command *cmd)
{
	char *iname;
	int size;
	Arg *a;
	char *ptr;
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
	a=a->next;
	size=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || size<3)
	{
		mvthprint(stderr,"moravec_cmd(): Invalid size (must have 3<=size)\n");
		return S_ARGS;
	}
	
	/* get the image */
	if ((img=get_image_var(iname))==NULL)
	{
		return S_NOEXIST;
	}

	if (img->bands!=1)
	{
		mvthprint(stderr,"Moravec filter defined only for grayscale.\n");
		return S_IFORM;
	}

	/* register it with the undo substructure */
	register_image_undo_var(iname);

	/* do the filter */
	moravec_fltr(img,size);

	stamp_image_t(img);
	

	mvthprint(stdout,"Moravec interest operator on Image \"%s\"\n",iname);

	return S_SUCCESS;
}
