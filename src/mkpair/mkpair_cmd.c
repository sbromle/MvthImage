/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a square object at a given location in two
 * images, to form a stereo pair.
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
#include "base/images_context.h"
#include "utils/timestamp.h"

extern int verbose;

static void usage(void)
{
	mvthprint(stderr,"\nmkpair syntax:\n");
	mvthprint(stderr,"mkpair <i1> <i2> <disp> <x> <y> <w> <h>\n\n");
	mvthprint(stderr,"Draws an object into a stereo pair.\n\n");
	return;
}

extern void mkpair_fltr(image_t *i1, image_t *i2, 
		int d, int x, int y, int w, int h);

MvthReplyCode mkpair_cmd(Command *cmd)
{
	char *i1,*i2;
	int d;
	int x, y, ow, oh; /* object position and size */
	Arg *a;
	char *ptr;
	image_t *wimgL; /* left WImage  */
	image_t *wimgR; /* right WIMage */

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=8) return S_USAGE;

	a=cmd->arglist->next;
	i1=a->str;

	a=a->next;
	i2=a->str;

	a=a->next;
	d=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || d<0 || d>64)
	{
		mvthprint(stderr,"mkpair(): Invalid disparity. Must be in [0,64]\n");
		return S_ARGS;
	}

	a=a->next;
	x=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str)
	{
		mvthprint(stderr,"mkpair(): Error reading x position.\n");
		return S_ARGS;
	}

	a=a->next;
	y=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str)
	{
		mvthprint(stderr,"mkpair(): Error reading y position.\n");
		return S_ARGS;
	}

	a=a->next;
	ow=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || ow<=0)
	{
		mvthprint(stderr,"mkpair(): Error reading object width.\n");
		return S_ARGS;
	}

	a=a->next;
	oh=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || oh<=0)
	{
		mvthprint(stderr,"mkpair(): Error reading object height.\n");
		return S_ARGS;
	}

	/* get the images */
	wimgL=get_image_var(i1);
	wimgR=get_image_var(i2);

	if (wimgL==NULL || wimgR==NULL)
	{
		mvthprint(stderr,"mkpair_cmd(): NULL IMAGE.\n");
		return S_NOEXIST;
	}

	if (wimgR->bands != 1 || wimgL->bands!=1)
	{
		mvthprint(stderr,"mkpair currently only implemented for grayscale images.\n");
		return S_MISMATCH;
	}
	if (wimgL->w != wimgR->w 
			|| wimgL->h != wimgR->h)
	{
		mvthprint(stderr,"Specified images have a size mismatch.\n");
		return S_MISMATCH;
	}

	/* do the filter */
	mkpair_fltr(wimgL,wimgR,d,x,y,ow,oh);
	stamp_image_t(wimgR);
	
	stamp_image_t(wimgL);
	

	if (verbose)
		mvthprint(stdout,"mkpair on Images \"%s\" and \"%s\".\n", i1,i2);

	return S_SUCCESS;
}
