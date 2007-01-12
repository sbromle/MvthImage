#ifndef V_PLOTPOLY_H
#define V_PLOTPOLY_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Plot a polynomial to an images within a viewport.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "base/images_types.h"

extern
int plotpoly(image_t *img, ViewPort_t *viewport,
		int order, double *coefs, float rgb[4]);

#endif
