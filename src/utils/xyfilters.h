/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Generic filter routines.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef XYFILTERS_H
#define XYFILTERS_H

extern int do_x_filter(float *src, float *dst,
		int w, int h, float *mask, int mw, float norm);
extern int do_y_filter(float *src, float *dst,
		int w, int h, float *mask, int mw, float norm);
extern int do_x_filter16(short *src, short *dst,
		int w, int h, short *mask, int mw, int norm);

extern int do_x_filter32(int *src, int *dst,
		int w, int h, int *mask, int mw, int norm);

extern int do_y_filter16(short *src, short *dst,
		int w, int h, short *mask, int mw,  int norm);

extern int do_y_filter32(int *src, int *dst,
		int w, int h, int *mask, int mw,  int norm);

extern void do_2D_filterF(float *src,
		float *dst,
		int w, int h, float **fmask, int mw, float fnorm);
#endif
