/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Compute gradients.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef DERIVATIVES_H
#define DERIVATIVES_H

extern int gradX(float *src, float *dst, int w, int h);
extern int gradY(float *src, float *dst, int w, int h);
extern int gradX32(int *src, int *dst, int w, int h);

extern int gradY32(int *src, int *dst, int w, int h);

extern int gradX16(short *src, short *dst, int w, int h);

extern int gradY16(short *src, short *dst, int w, int h);

#endif
