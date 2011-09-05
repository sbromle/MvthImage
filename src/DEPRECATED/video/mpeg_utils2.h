/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Code to handle mpeg streams using MPEG2 library..
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#ifndef MPEG_UTILS2_H
#define  MPEG_UTILS2_H


extern int initmpeg2(char *filename, int *w, int *h);
extern int getmpeg2frameGray(unsigned char *grey);
extern int getmpeg2frameRGB(unsigned char *rgb);
extern int rewindmpeg2(void);
extern int closempeg2(void);

#endif
