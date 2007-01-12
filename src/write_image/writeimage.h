/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Write out an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef WRITEIMAGE_H
#define WRITEIMAGE_H

int writeimage(float *img,int w,int h,int bands,int bpc,char *filename);

#endif
