/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Remove radial lens distortion.
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
