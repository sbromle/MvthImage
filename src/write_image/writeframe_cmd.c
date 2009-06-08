/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Write out an image frame.
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
#include "writeimage.h"

static int framenum=0;

static void usage(void)
{
	mvthprint(stderr,"\nwriteframe syntax:\n");
	mvthprint(stderr,"wf <image #> <basename>\n\n");
	mvthprint(stderr,"Write a jpeg of frame to filename\n");
	mvthprint(stderr,"of format basename%%04d.pnm where %%04d\n");
	mvthprint(stderr,"is replaced with the current frame number.\n");
	mvthprint(stderr,"Frame number is incremented automatically.\n");
	mvthprint(stderr,"(Reset frame number by passing in -1 as iname)\n\n");
	return;
}


MvthReplyCode writeframe_cmd(Command *cmd)
{
	char *iname;
	Arg *a;
	char filename[2048];
	char *basename;
	image_t *img;

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
	
	/* get the image number */
	iname=a->str;

	if (strcmp(iname,"-1")==0)
	{
		framenum=0;
		return S_SUCCESS;
	}

	/* get the basename of the image */
	a=a->next;
	basename=a->str;
	if (basename==NULL)
	{
		mvthprint(stderr,"writeframe_cmd(): Error setting basename.\n");
		return S_ARGS;
	}

	if ((img=get_image_var(iname))==NULL)
	{
		return S_NOEXIST;
	}

	/* constructing file name */
	snprintf(filename,sizeof(filename),"%s%04d.ppm",basename,framenum);

	writeimage(img->data,
			img->w,img->h,img->bands,filename);

	framenum++;

	mvthprint(stdout,"Wrote image \"%s\" to file `%s',\n",iname, filename);

	return S_SUCCESS;
}
