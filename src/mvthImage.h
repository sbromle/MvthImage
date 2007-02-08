#ifndef MVTH_IMAGE_H
#define MVTH_IMAGE_H

/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Copyright (C) 2003,2004,2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <mvthimage/images_types.h>

/* create a new image_t structure */
extern image_t *new_image_t(int w, int h, int bands);
/* free an image_t structure */
extern void free_image_t(image_t *img);
/* copy one */
extern int copy_image_t(image_t *src, image_t *dst);
/* blank an image */
extern void zero_image_t(image_t *img);
/* resize an image */
extern int resize_image_t(image_t *img, int w, int h, int bands);
/* print one */
extern int print_image_t(FILE *fp, image_t *img);
extern int sprint_image_t(char *buff, image_t *img);
/* functions to read and write an image */
extern image_t * readimage(char *filename);
extern int writeimage(float *img,int w,int h,int bands,char *filename);

extern double **getRadialMap(int theta,int rlen,int W,int H,int ycenter);
extern double **getUnitMapping(int w, int h, R3Dmatrix_t rm);
extern void remapImage(float *src, int sw, int sh, 
		float *dst, int dw, int dh, int dbands, double **table);

extern void stamp_image_t(image_t *img);

extern int getJetRGB(double v, double vmin, double vmax, float *rgb);
/* plot an image based on arrays of doubles */
#include <mvthimage/plot_imagescale.h>
/* draw a line */
extern void drawLine(float *img, int w, int h, int bands,
		int x, int y, int dx, int dy,
		float *rgb);
extern void drawCircle(image_t *img,int cx, int cy, int r, float *rgb);
/* Draw a multi-segment line */
extern void drawPolyLine(float *img, int w, int h, int bands,
		int x[], int y[], int n,
		float *rgb);
/* plot a multisegment line within a viewport */
extern int plotxy(float *img, int w, int h, int bands,
		double x[], double y[], size_t len, float *rgb,
		ViewPort_t *viewport);
extern int plotxydots(float *img, int w, int h, int bands,
		double x[], double y[], size_t len, float *rgb,
		ViewPort_t *viewport);
int plotpoly(image_t *img, ViewPort_t *viewport,
		int order, double *coefs, float *rgb);
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

/* should add the rest of the provide filters here */
/******* SOME FILTERS and their helper functions ************/
extern int axes_fltr(image_t *img,ViewPort_t *viewport, float *rgb);
/* paste one image on top of another */
extern void paste_fltr(image_t *src, image_t *dst,int xoff, int yoff, int t);
/* zoom a region of an image */
extern image_t *zoom_fltr(image_t *src, int x0, int y0, int x1, int y1, int w, int h);
/* project an image into an isometric view */
extern void isometric_fltr(image_t *img,double theta);
extern double **getMapping(int w, int h, R3Dmatrix_t rm);
extern double **getMapping3d(int w, int h, Tmatrix tm);
extern double **getIsometricMap(int w, int h,double theta);
extern double **getIsometricMap2(double w, double h,
                                 int W, int H,
                                 double theta);

#endif
