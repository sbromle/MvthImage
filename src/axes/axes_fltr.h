#ifndef MVTH_AXES_FLTR_H
#define MVTH_AXES_FLTR_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a simple pair of axes. Will add further sophistication later.
 *
 * Copyright (C) 2004 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include "base/images_types.h"

/* axes are drawn *at* the boundaries of the viewport,
 * thus, if you want the text to be visable, the viewport
 * should be smaller than the image size.
 */
extern int axes_fltr(image_t *img,ViewPort_t *viewport, float rgb[3]);

#endif
