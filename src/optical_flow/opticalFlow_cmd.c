/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Compute optical flow between two images.
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
 * (see the file named "COPYING), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mvthPlugin.h>
#include "base/images_types.h"
#include "base/images_utils.h"
#include "utils/timestamp.h"

static void usage(void)
{
	mvthprint(stderr,"\noptical syntax:\n");
	mvthprint(stderr,"optical <i1> [<i2> [<i3>]] <corr size> <search range>\n\n");
	mvthprint(stderr,"or\n");
	mvthprint(stderr,"optical <i1> <range> /* outputs flow field schematic only*/\n");
	mvthprint(stderr,"Does optical from (i1) to (i2).\n");
	mvthprint(stderr,"With optional output to (i3).\n\n");
	return;
}

extern void opticalFlow_fltr(image_t *,image_t *,image_t *,int,int,int);

MvthReplyCode opticalFlow_cmd(Command *cmd)
{
	char *iname[3]={NULL,NULL,NULL};
	int w[3],h[3],bands[3];
	int range;
	int corrsize=0;
	int i;
	int it; /* total images specified on command line */
	Arg *a;
	char *ptr;
	image_t *img[3]={NULL,NULL,NULL};

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=3 && cmd->narg!=4 && cmd->narg!=6)
	{
		return S_USAGE;
	}

	/* get the image number */
	a=cmd->arglist->next;

	if (cmd->narg==3) it=1;
	else if (cmd->narg==4) it=2;
	else it=3;

	for (i=0;i<it;i++)
	{
		iname[i]=a->str;
		a=a->next;
	}

	if (it!=1)
	{
		/* get the correlation window */
		corrsize=(int)strtol(a->str,&ptr,10);
		if (ptr==a->str || corrsize<3 || corrsize>32)
		{
			mvthprint(stderr,"opticalFlow(): Invalid corrsize. Must be in [3,32]\n");
			return S_ARGS;
		}
		a=a->next;
	}
	
	/* get the search range */
	range=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || range<1 || range>64)
	{
		mvthprint(stderr,"opticalFlow(): Invalid range. Must be in [1,64]\n");
		return S_ARGS;
	}

	/* if user only specified 3 arguments, then they are asking
	 * for a flow schematic image only */
	if (it==1)
	{
		mvthprint(stderr,"Flow only requested.\n");
		/* get images */
		if ((img[0]=get_image_var(iname[0]))==NULL)
		{
			return S_NOEXIST;
		}
		else if (img[0]->bands!=3)
		{
			mvthprint(stderr,"Image must be RGB!\n");
			return S_IFORM;
		}

		opticalFlow_fltr(NULL, NULL, img[0], 0, range, 1); /* second arg can be NULL */

		stamp_image_t(img[0]);
		

		return S_SUCCESS;
	}

	/* otherwise, we should set everything up */

	/* get images and nodes */
	for (i=0;i<it;i++)
	{
		if ((img[i]=get_image_var(iname[i]))==NULL) return -1;
	
		w[i]=img[i]->w;
		h[i]=img[i]->h;
		bands[i]=img[i]->bands;
	}

	if (w[0]!=w[1] || h[0]!=h[1] || bands[0]!=bands[1])
	{
		mvthprint(stderr,"Image size or depth mismatch. Ignoring request.\n");
		return S_MISMATCH;
	}

	/* Then we need to make sure the img there is big enough */
	/* test to see if its the correct size */
	if (img[2]!=NULL && (img[2]->w!=w[0]
			|| img[2]->h!=h[0]
			|| img[2]->bands!=3))
	{
		/* then must resize it */
		resize_image_t(img[2],w[0],h[0],3);
	}

	/* ok, so we are set up, ready to go */
	/* hand everything off to the filter */

	opticalFlow_fltr(img[0], img[1], img[2],
			corrsize, range, 0); /* second arg can be NULL */

	if (img[2]!=NULL)
	{
		stamp_image_t(img[2]);
		
	}

	mvthprint(stdout,"Optical Flow on \"%s\"->\"%s\"\n",iname[0],iname[1]);
	if (img[2]!=NULL)
	{
		mvthprint(stdout,"Output to \"%s\"\n",iname[2]);
	}

	return S_SUCCESS;
}
