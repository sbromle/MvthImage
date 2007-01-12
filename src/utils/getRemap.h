/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Get a remapping table for image transforms.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef REMAP_H
#define REMAP_H

#include "base/images_types.h"

extern double **getMapping(int w, int h, R3Dmatrix_t rm);
extern double **getMapping3d(int w, int h, Tmatrix tm);
extern double **getIsometricMap(int w, int h,double theta);
extern double **getIsometricMap2(double w, double h,
                                 int W, int H,
                                 double theta);
extern double **getRadialMap(int theta,int rlen,int W,int H,int ycenter);
extern double **getUnitMapping(int w, int h, R3Dmatrix_t rm);
/* images must have same number of color channels (bands) */
extern void remapImage(float *src, int sw, int sh, 
		float *dst, int dw, int dh, int bands,
		double **table);

#endif
