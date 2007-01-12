/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Calculate a disparity map using Area-based stereo vision
 * algorithm based on Faugeras, "Real time correlation-based
 * stere: algorithm, implementations and applications", INRIA
 * Technical report, No. 2013 (1993).
 * (Has complexity O(wh) where w and h, are the width
 * and height of the image. i.e. speed is independent
 * of correlation window size.)
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
#include "base/stereo_context.h"

static void usage(void)
{
	mvthprint(stderr,"\nfaugeras syntax:\n");
	mvthprint(stderr,"faugeras <i1> <i2> <disp range> <window width> <horopter>\n\n");
	mvthprint(stderr,"Does area based stereo on images i1,i2.\n\n");
	return;
}

extern int verbose;

extern void faugeras_stereo_fltr(image_t *i1, image_t *i2, 
		SContext_t *sc);

MvthReplyCode faugeras_stereo_cmd(Command *cmd)
{
	char *i1,*i2;
	int range, window, horopter;
	Arg *a;
	char *ptr;
	image_t *imgL; /* left WImage  */
	image_t *imgR; /* right WIMage */

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=6) return S_USAGE;

	a=cmd->arglist->next;
	i1=a->str;
	a=a->next;
	i2=a->str;
	a=a->next;
	range=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || range<2 || range>64)
	{
		mvthprint(stderr,"astereo(): Invalid range. Must be in [2,64]\n");
		return S_ARGS;
	}

	a=a->next;
	window=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || window<=0 || window>32)
	{
		mvthprint(stderr,"astereo(): Invalid comparison-window size. Must be in [1,32]\n");
		return S_ARGS;
	}

	a=a->next;
	horopter=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str || horopter<-32 || horopter>32)
	{
		mvthprint(stderr,"astereo(): Invalid horopter. Must be in [-32,32]\n");
		return S_ARGS;
	}

	/* get the images */
	imgL=get_image_var(i1);
	imgR=get_image_var(i2);

	if (imgL==NULL || imgR==NULL)
	{
		mvthprint(stderr,"astereo_cmd(): NULL IMAGE.\n");
		return S_NOEXIST;
	}

	if (imgR->bands != 1 || imgL->bands!=1)
	{
		mvthprint(stderr,"Area-based stereo currently only implemented for"
				" grayscale images.\n");
		return S_MISMATCH;
	}

	/* set up the stereo context */
	stereo_context.window=window;
	stereo_context.range=range;
	stereo_context.horopter=horopter;

	/* do the filter */
	faugeras_stereo_fltr(imgL,imgR,&stereo_context);

	if (verbose)
		mvthprint(stdout,"Faugeras' Area Based Stereo on Images \"%s\" and \"%s\".\n",
				i1,i2);

	return S_SUCCESS;
}
