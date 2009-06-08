/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Warp one image to another using radial coordinate transformation.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sbromley@guigne.com>
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
#include <popt.h>
#include <mvthPlugin.h>
#include "base/images_types.h"
#include "utils/timestamp.h"
#include "rwarp_fltr.h"

MvthReplyCode rwarp_cmd(Command *cmd)
{
	char *srcname,*dstname;
	int ycenter=0;
	image_t *srcimg=NULL;
	image_t *dstimg=NULL;

	POPT_START_OPTIONS
		{"ycenter",'y',POPT_ARG_INT,&ycenter,0,
			"height above image to place origin",NULL},
		POPT_END_OPTIONS;
	POPT_START("rwarp","<src name> <dst name>");
	POPT_PARSE_OPTIONS;
	POPT_GET_STRING(srcname);
	POPT_GET_STRING(dstname);
	POPT_FINISH;

	if (ycenter<0)
	{
		mvthprint(stderr,"Must specify a positive height above image.\n");
		return S_ARGS;
	}

	/* try to get the images */
	srcimg=get_image_var(srcname);
	if (srcimg==NULL)
	{
		free(srcname);
		free(dstname);
		return S_NOEXIST;
	}
	dstimg=get_image_var(dstname);
	if (dstimg==NULL)
	{
		free(srcname);
		free(dstname);
		return S_NOEXIST;
	}

	register_image_undo_var(dstname);
	rwarp_fltr(srcimg,dstimg,ycenter);
	stamp_image_t(dstimg);
	

	free(srcname);
	free(dstname);
	return S_SUCCESS;
}
