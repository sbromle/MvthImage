#ifndef HSV_UTILS_H
#define HSV_UTILS_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert between RGB and HSV colour spaces.
 *
 * Copyright (C) 2006 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

extern void RGB2HSV(float r, float g, float b,
		float *h, float *s, float *v);
extern void HSV2RGB(float h, float s, float v,
		float *r, float *g, float *b);

#endif
