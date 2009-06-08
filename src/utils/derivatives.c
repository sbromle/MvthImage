/*
 * This file is part of MVTH Image - the Machine Vision Test Harness
 * Image plugin.
 *
 * Compute gradients.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * MVTH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License Version 3,
 * as published by the Free Software Foundation.
 *
 * MVTH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * (see the file named "COPYING"), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 *
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
