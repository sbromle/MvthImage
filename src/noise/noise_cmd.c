/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Corrupt image with noise.
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
#include "utils/timestamp.h"

static void usage(void)
{
	mvthprint(stderr,"\nCorrupt image with noise:\n");
	mvthprint(stderr,"noise <image #> <sigma>\n\n");
	return;
}

extern void noise_fltr(image_t *img, double sigma);

MvthReplyCode noise_cmd(Command *cmd)
{
	char *iname;
	double sigma;
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
	sigma=strtod(a->str,&ptr);
	if (ptr==a->str || sigma<0)
	{
		mvthprint(stderr,"noise_cmd(): Invalid sigma (must be >0 )\n");
		return S_ARGS;
	}
	
	/* get the image */
	if ((img=get_image_var(iname))==NULL)
	{
		return S_NOEXIST;
	}

	/* register it with the undo substructure */
	register_image_undo_var(iname);

	/* do the filter */
	noise_fltr(img,sigma);

	stamp_image_t(img);
	

	mvthprint(stdout,"Corrupted image \"%s\" with %lf level gaussian noise.\n",iname,sigma);

	return S_SUCCESS;
}
