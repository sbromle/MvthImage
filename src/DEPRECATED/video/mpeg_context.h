/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Context for mpeg streams.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef MPEG_CONTEXT_H
#define MPEG_CONTEXT_H

typedef struct
{
	int w,h,bands;
	int field;
} MPEG_Context;

#endif
