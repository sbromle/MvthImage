#ifndef STEREO_CONTEXT_T_H
#define STEREO_CONTEXT_T_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Stereo context data struct type definition.
 *
 * Copyright (C) 2003,2004,2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

/* a structure holding the parameters of the last
 * request to do stereo matching.
 */

typedef struct SContext_s {
	int window;
	int range;
	int horopter;
	int w, h;
	float *dmap;
} SContext_t;

#endif
