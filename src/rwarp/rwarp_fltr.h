#ifndef RWARP_FLTR_H
#define RWARP_FLTR_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Warp one image to another using radial coordinate transformation.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include "base/images_types.h"

extern int rwarp_fltr(image_t *src, image_t *dst, int ycenter);

#endif
