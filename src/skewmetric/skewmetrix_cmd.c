/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Project image into an skew-symmetric view.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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

extern void skewmetric_fltr(image_t *img,double theta);

MvthReplyCode skewmetric_cmd(Command *cmd)
{
	char *iname;
	image_t *img; /* must find this */

	double theta=45;

	POPT_START_OPTIONS
		{"theta",'t',POPT_ARG_DOUBLE,&theta,0,
			"set angle of skew (degrees)",NULL},
		POPT_END_OPTIONS;
	POPT_START("skewmetric","[options] <imgvar>");
	POPT_PARSE_OPTIONS;
	/* get the image name to modify */;
	POPT_GET_STRING(iname);
	POPT_FINISH;

	/* get image */
	if ((img=get_image_var(iname))==NULL) return S_NOEXIST;

	/* register it with the undo substructure */
	register_image_undo_var(iname);

	/* do the filter */
	skewmetric_fltr(img,theta);
	stamp_image_t(img);
	

	return S_SUCCESS;
}
