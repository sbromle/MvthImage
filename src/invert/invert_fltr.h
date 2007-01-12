#ifndef MVTHIMAGE_INVERT_FLTR_H
#define MVTHIMAGE_INVERT_FLTR_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Invert an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "base/images_types.h"

extern void invert_fltr0(ImageBlock* b);
extern void invert_fltr(image_t *wimg);

#endif
