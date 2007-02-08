/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a line in an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef DRAW_LINE_H
#define DRAW_LINE_H

/* standard size of square */

#define PSIZE 3
/* simple square drawing routine */

extern void drawLine(float *img, int w, int h, int bands,
		int x, int y, int dx, int dy,
		float *rgb);
/* Draw a multi-segment line */
extern void drawPolyLine(float *img, int w, int h, int bands,
		int x[], int y[], int n,
		float *rgb);
/* Plot data points as single pixels */
extern void drawDots(float *img, int w, int h, int bands,
		int x[], int y[], int n,
		float *rgb);
#endif
