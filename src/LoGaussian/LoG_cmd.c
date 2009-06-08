/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Laplacian of Gaussian filter.
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

/* Laplacian of Gaussian command (LoG) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mvthPlugin.h>
#include "base/images_types.h"
#include "utils/timestamp.h"

extern void LoG_fltr(image_t *,float);

static void usage(void)
{
	mvthprint(stderr,"\nLoG (Laplacian of Gaussian) syntax:\n");
	mvthprint(stderr,"log <image #> <sigma>\n\n");
	return;
}


MvthReplyCode LoG_cmd(Command *cmd)
{
	float sigma;
	char *iname;
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
		mvthprint(stderr,"%s\n",SERVER_USAGE);
		return S_USAGE;
	}
	a=cmd->arglist->next;
	iname=a->str;

	a=a->next;
	sigma=(float)strtod(a->str,&ptr);
	if (sigma<0 || ptr==a->str)
	{
		mvthprint(stderr,"LoG(): Sigma must be >0\n");
		return S_ARGS;
	}

	/* get image */
	if ((img=get_image_var(iname))==NULL)
	{
		return S_NOEXIST;
	}

	/* register it with the undo substructure */
	register_image_undo_var(iname);

	/* do the filter */
	LoG_fltr(img,sigma);
	stamp_image_t(img);
	

	mvthprint(stdout,"Laplacian of Gaussian on Image \"%s\" with Sigma=%f\n",
			iname,sigma);

	return S_SUCCESS;
}
