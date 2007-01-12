/*
 * This file is part of MVTH Image - the Machine Vision Test Harness
 * Image plugin.
 *
 * Compute gradients.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "derivatives.h"
#include "xyfilters.h"

int gradX(float *src, float *dst,
		int w, int h)
{
	float mask[3]={-1,0,1};
	do_x_filter(src,dst,w,h,mask, 3, 1.0);
	return 1;
}

int gradX32(int *src, int *dst,
		int w, int h)
{
	int mask[3]={-1,0,1};
	do_x_filter32(src,dst,w,h,mask, 3, 1.0);
	return 1;
}

int gradY(float *src, float *dst,
		int w, int h)
{
	float mask[3]={-1,0,1};
	do_y_filter(src,dst,w,h,mask, 3, 1);
	return 1;
}

int gradY32(int *src, int *dst,
		int w, int h)
{
	int mask[3]={-1,0,1};
	do_y_filter32(src,dst,w,h,mask, 3, 1);
	return 1;
}

int gradX16(short *src, short *dst,
		int w, int h)
{
	short mask[3]={-1,0,1};
	do_x_filter16(src,dst,w,h,mask, 3, 1);
	return 1;
}

int gradY16(short *src, short *dst,
		int w, int h)
{
	short mask[3]={-1,0,1};
	do_y_filter16(src,dst,w,h,mask, 3, 1);
	return 1;
}
