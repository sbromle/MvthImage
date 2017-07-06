/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * MNG image stream routines.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef MNG_UTILS_H
#define MNG_UTILS_H

#include <stdio.h>

extern int init_mng(const char *filename,
		unsigned int *w, unsigned int *h, unsigned int *bands);
extern int grab_mng_frame(char *pixels);
extern int rewind_mng(void);
extern int close_mng(void);
extern int end_of_mng(void);
extern int getMNGframesize(int *w, int *h, int *bands);

#endif
