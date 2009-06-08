/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Moravec interest operator filter.
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
