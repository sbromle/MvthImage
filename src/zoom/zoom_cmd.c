/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Zoom a region of an image.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mvthPlugin.h>
#include "base/images_types.h"
#include "utils/timestamp.h"

extern void paste_fltr(image_t *src, image_t *dst,int xoff, int yoff, float rgb[6], float alpha);

/* for now just make it simple */
extern image_t * zoom_fltr(image_t *img,
		int x0, int y0, int x1, int y1,
		int width, int height);

extern int verbose;

MvthReplyCode zoom_cmd(Command *cmd)
{
	char *iname;
	int x0,y0,x1,y1;
	image_t *img=NULL;
	float rgb[6]={0,0,0,0,0,0};

	/* build the options table */
	POPT_START_OPTIONS
		{"x0",'x',POPT_ARG_INT,&x0,0,
			"set left x coordinate",NULL},
		{"x1",'X',POPT_ARG_INT,&x1,0,
			"set right x coordinate",NULL},
		{"y0",'y',POPT_ARG_INT,&y0,0,
			"set top y coordinate",NULL},
		{"y1",'Y',POPT_ARG_INT,&y1,0,
			"set bottom y coordinate",NULL},
		POPT_END_OPTIONS;
	POPT_START("zoon","[options] <image name>");
	POPT_PARSE_OPTIONS;
	/* try to get the last argument */
	POPT_GET_STRING(iname);
	POPT_FINISH;

	/* get the ping */
	img=get_image_var(iname);
	if (img==NULL)
	{
		mvthprint(stderr,"No such image called \"%s\"\n",iname);
		free(iname);
		return S_NOEXIST;
	}

	image_t *newimage=zoom_fltr(img,x0,y0,x1,y1,img->w,img->h);
	/* register with the undo substructure */
	register_image_undo_var(iname);
	paste_fltr(newimage,img,0,0,rgb,1.0);
	stamp_image_t(img);
	
	free(newimage->data);
	free(newimage);

	mvthprint(stderr,"Zoomed area of image %d.\n",iname);
	free(iname);
	return S_HANDLED;
}
