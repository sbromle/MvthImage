/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Maintain image contexts.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef MVTH_IMAGE_STATE_H
#define MVTH_IMAGE_STATE_H


#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <tcl.h>
#include "images_types.h"

typedef struct MvthImage {
	Tcl_Obj *widthPtr; /* original string value for width */
	Tcl_Obj *heightPtr; /* original string value for height */
	Tcl_Obj *depthPtr; /* original string value for bitdepth */
	image_t *img; /* a low-level  mvth image structure */
} MvthImage;

extern int getMvthImageFromObj(Tcl_Interp *interp, Tcl_Obj *CONST name,
		MvthImage **iPtrPtr);
extern int MvthImageState_Init(Tcl_Interp *interp);
extern int mvthImageExists(Tcl_Interp *interp, Tcl_Obj *CONST name);
extern int mvthImageReplace(image_t *img, MvthImage *mimg);
extern int image_t2MvthImage(image_t *img, MvthImage *mimg);
extern int updateMvthImageDims(MvthImage *mimg, int w, int h, int d);


#endif
