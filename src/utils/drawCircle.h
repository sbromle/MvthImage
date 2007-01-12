#ifndef DRAW_CIRCLE_H
#define DRAW_CIRCLE_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a circle.
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

#include "base/images_context.h"

extern void drawCircle(image_t *img,int cx, int cy, int r, float rgb[4]);

#endif
