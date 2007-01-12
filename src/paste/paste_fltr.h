#ifndef PASTE_FILTER_H
#define PASTE_FILTER_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Paste one image on top of another.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#include "base/images_types.h"

extern void paste_fltr(image_t *src, image_t *dst,int xoff, int yoff, float t, float dt);
extern void paste_with_mask_fltr(image_t *src, image_t *dst,int xoff, int yoff, unsigned char *mask);

#endif
