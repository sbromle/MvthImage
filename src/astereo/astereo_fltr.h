#ifndef MVTH_ASTEREO_FLTR_H
#define MVTH_ASTEREO_FLTR_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Calculate a disparity map using Area-based stereo vision.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "base/images_types.h"
#include "base/stereo_context_t.h"

extern void astereo_fltr(image_t *img1, image_t *img2, SContext_t *sc);

#endif
