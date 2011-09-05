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

extern int getMvthImageFromObj(Tcl_Interp *interp, Tcl_Obj *CONST name,
		image_t **iPtrPtr);
extern int MvthImageState_Init(Tcl_Interp *interp);
extern int mvthImageExists(Tcl_Interp *interp, Tcl_Obj *CONST name);
extern int mvthImageReplace(image_t *img, image_t *mimg);

#endif
