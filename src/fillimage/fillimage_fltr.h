#ifndef FILL_IMAGE_FLTR_H
#define FILL_IMAGE_FLTR_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Fill an image with a colour.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "base/images_types.h"

extern void fillimage_fltr0(ImageBlock *b, float rgba[4]);
extern void fillimage_fltr(image_t *img,float val[4]);

#endif
