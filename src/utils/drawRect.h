/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw/fill a rectangle in an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef DRAW_RECT_H
#define DRAW_RECT_H

#include "base/images_types.h"

/* standard size of square */

#define PSIZE 3
/* simple square drawing routine */
extern void drawPDot(float *img, int w, int h, int bands,
		int x, int y, int s, /* position and size of square */
		float *rgb); /* rgb colour */

extern void drawPDotZbuffer(float *img, int w, int h, int bands,
		int x, int y, int s,
		float *rgb, float *zbuffer, float depth);

extern void drawRect(float *img, int w, int h, int bands,
		int x1, int y1, int x2, int y2,
		float *rgb);

extern void fillRect(float *img, int w, int h, int bands,
		Rect_t rect,
		float *rgb);
#endif
