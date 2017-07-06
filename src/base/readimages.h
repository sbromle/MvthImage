/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Read in an image from a file.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef READIMAGES_H
#define READIMAGES_H
#include "base/images_types.h"

image_t * readimage(char *filename);

#endif
