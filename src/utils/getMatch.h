/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Find a match using area-based correlation.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef GETMATCH_H
#define GETMATCH_H

extern float getWindowMatch(float *tplateRows,
		                        float *srchRows,
														int nrows, int rowstride,
														int w, int range);
#endif
