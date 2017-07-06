/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert from one image type to another representation.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef CONVERT_H
#define CONVERT_H

extern int cimg2iimg(unsigned char* src, int *dst, int w, int h,
		int dp, int lw);
extern int iimg2cimg(int* src, unsigned char *dst, int w, int h, int lw);
extern int cimg2simg(unsigned char* src, short int *dst, int w, int h,
		int dp, int lw);
extern int simg2cimg(short int* src, unsigned char *dst, int w, int h, int lw);
extern int cimg2dimg(unsigned char* src, double *dst, int w, int h, int dp, double norm);
extern int dimg2cimg(double *src, unsigned char *dst, int w, int h, int dp, double norm);
extern float fimg2simg(float *src, short int *dst, int w, int h, int dp, int lw);

#endif
