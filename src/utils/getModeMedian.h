/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Get some statistics on a collection of data.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef GETMODEMEDIAN_H
#define GETMODEMEDIAN_H

/* find the min,max, mode and the median of an array of integers */
int getModeMedian(int *data, int dsize,
		int *min, int *max, int *mode, int *median);

#endif
