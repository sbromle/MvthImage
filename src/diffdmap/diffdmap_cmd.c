/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Quantify differences between two disparity maps.
 * Places where disparity map #1 = 255 are ignored.
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

static void usage(void)
{
	mvthprint(stderr,"\ndiffdmap syntax:\n");
	mvthprint(stderr,"diffdmap <i1> <i2>\n\n");
	mvthprint(stderr,"Quantifies differences between two dmap images.\n\n");
	return;
}

extern float * diffdmap_fltr(image_t *testimg, image_t *truth, unsigned char dthresh);

MvthReplyCode diffdmap_cmd(Command *cmd)
{
	char *i1,*i2;
	Arg *a;
	image_t *img1; /* left WImage  */
	image_t *img2; /* right WIMage */
	float *results;

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=3) return S_USAGE;

	a=cmd->arglist->next;
	i1=a->str;
	a=a->next;
	i2=a->str;

	/* get the images */
	img1=get_image_var(i1);
	img2=get_image_var(i2);

	if (img1==NULL || img2==NULL)
	{
		mvthprint(stderr,"diffdmap_cmd(): NULL IMAGE.\n");
		return S_NOEXIST;
	}

	if (img2->bands != 1 || img1->bands!=1)
	{
		mvthprint(stderr,"Dmap differences only calculated for grayscale dmaps.\n");
		return S_MISMATCH;
	}

	/* do the filter */
	results=diffdmap_fltr(img1,img2, (unsigned char)0);
	if (results!=NULL) free(results);

	return S_SUCCESS;
}
