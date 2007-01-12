/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert disparity to an RGB color representation.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef GETCOLOR_H
#define GETCOLOR_H

#include "base/stereo_context_t.h"
/* initialized a lookup table for speed */
extern void init_dmap_lut(SContext_t *sc);
/* gets a color for the disparity d, using the
 * current stereo context values.
 */
extern void getDisparityColor(float d, float rgb[4]);

#endif
