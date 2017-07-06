#ifndef PLOTXY_V_H
#define PLOTXY_V_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Plot a graph of xy data within a viewport.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#include "base/images_types.h"

extern int plotxy(float *img, int w, int h, int bands,
		double *x, double *y, size_t len, float *rgb,
		ViewPort_t *viewport);
extern int plotxydots(float *img, int w, int h, int bands,
		double *x, double *y, size_t len, float *rgb,
		ViewPort_t *viewport);
extern int plotfillxy(float *img, int w, int h, int bands,
		double *x, double *y, size_t len, float *rgb,
		ViewPort_t *viewport, double v);
extern int plotxyF(float *img, int w, int h, int bands,
		float *x, float *y, size_t len, float *rgb,
		ViewPort_t *viewport);
extern int plotxydotsF(float *img, int w, int h, int bands,
		float *x, float *y, size_t len, float *rgb,
		ViewPort_t *viewport);
extern int plotfillxyF(float *img, int w, int h, int bands,
		float *x, float *y, size_t len, float *rgb,
		ViewPort_t *viewport, float v);

#endif
