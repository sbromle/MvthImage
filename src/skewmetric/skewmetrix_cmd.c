/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Project image into an skew-symmetric view.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
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
#include <popt.h>
#include <mvthPlugin.h>
#include "base/images_types.h"
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
