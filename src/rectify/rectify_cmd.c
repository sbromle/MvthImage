/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Rectify an image.
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
#include "base/images_context.h"
#include "utils/timestamp.h"
#include "utils/timestamp.h"

static void usage(void)
{
	mvthprint(stderr,"\n(rectify image) syntax:\n");
	mvthprint(stderr,"rectify <image #> <arg 0> ... <arg 8>\n\n");
	return;
}

extern void rectify_fltr(image_t *wimg, double fargs[9]);

MvthReplyCode rectify_cmd(Command *cmd)
{
	double fargs[9]; /* an array holding the elements of the rectification matrix*/
	char *iname;
	int i;
	Arg *a;
	char *ptr;
	image_t *img;

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=11)
	{
		return S_USAGE;
	}

	/* get the image number */
	a=cmd->arglist->next;
	iname=a->str;

	/* get the rest of the arguments */
	for (i=0;i<9;i++)
	{
		a=a->next;
		fargs[i]=strtod(a->str,&ptr);
		if (ptr==a->str)
		{
			mvthprint(stderr,"Error reading arg %d\n",i);
			return S_ARGS;
		}
	}

	/* get image */
	if ((img=get_image_var(iname))==NULL)
	{
		return S_NOEXIST;
	}

	/* register it with the undo substructure */
	register_image_undo_var(iname);

	/* do the filter */
	rectify_fltr(img,fargs);
	stamp_image_t(img);
	

	mvthprint(stdout,"Rectified image \"%s\".\n", iname);

	return S_SUCCESS;
}
