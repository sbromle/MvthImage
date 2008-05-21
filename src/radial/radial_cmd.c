/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Remove radial lens distortion.
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
	mvthprint(stderr,"\nradial syntax:\n");
	mvthprint(stderr,"radial <image #> <cx> <cy> <kappa1> <kappa2>\n\n");
	return;
}

extern void radial_fltr(image_t *img, double *params);

MvthReplyCode radial_cmd(Command *cmd)
{
	char *iname;
	int i;
	Arg *a;
	char *ptr;
	image_t *img; /* must find this */
	double params[4]; /* will hold cx,cy,k1,k2 */

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
	iname=a->str;

	for (i=0;i<4;i++)
	{
		a=a->next;
		params[i]=(float)strtod(a->str,&ptr);
		if (ptr==a->str)
		{
			mvthprint(stderr,"radial_cmd(): Error reading param %d\n",i);
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
	radial_fltr(img,params);

	stamp_image_t(img);
	

	return S_SUCCESS;
}
