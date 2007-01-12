/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Rotate an image.
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
#include <popt.h>
#include <mvthPlugin.h>
#include "base/images_context.h"
#include "utils/timestamp.h"

extern int poptPrintHelpMvth(poptContext con, FILE *f, int flags);

extern void rotate_fltr(image_t *wimg,
		const double alpha,
		const double beta,
		const double gamma,
		const double x, const double y, const double z);

MvthReplyCode rotate_cmd(Command *cmd)
{
	char *iname;
	image_t *img; /* must find this */

	double alpha=0.0, beta=0.0,gamma=0.0;
	double x=0.0, y=0.0,z=0.0;

	POPT_START_OPTIONS
		{"alpha",'a',POPT_ARG_DOUBLE,&alpha,0,
			"Rotation about x axis",NULL},
		{"beta",'b',POPT_ARG_DOUBLE,&beta,0,
			"Rotation about y axis",NULL},
		{"gamma",'c',POPT_ARG_DOUBLE,&gamma,0,
			"Rotation about z axis",NULL},
		{"xtrans",'x',POPT_ARG_DOUBLE,&x,0,
			"Translation along x axis",NULL},
		{"ytrans",'y',POPT_ARG_DOUBLE,&y,0,
			"Translation along y axis",NULL},
		{"ztrans",'z',POPT_ARG_DOUBLE,&z,0,
			"Translation along z axis",NULL},
		POPT_END_OPTIONS;
	POPT_START("rotate","[options] <imgvar>");
	POPT_PARSE_OPTIONS;
	if (alpha==0.0 && beta==0.0 && gamma==0.0
			&& x==0.0 && y==0.0 && z==0.0)
	{
		mvthprint(stderr,"Invalid options specified\n");
		return S_HANDLED;
	}
	/* get the last argument */
	POPT_GET_STRING(iname);
	POPT_FINISH;

	/* get image */
	if ((img=get_image_var(iname))==NULL)
	{
		free(iname);
		return S_NOEXIST;
	}

	/* register it with the undo substructure */
	register_image_undo_var(iname);

	/* do the filter */
	rotate_fltr(img,alpha,beta,gamma,x,y,z);
	stamp_image_t(img);
	

	return S_SUCCESS;
}
