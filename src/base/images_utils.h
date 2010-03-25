#ifndef IMAGES_UTILS_H
#define IMAGES_UTILS_H

/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Copyright (C) 2003,2004,2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include "images_types.h"


/* get a new image_t structure */
extern image_t *new_image_t(int w, int h, int bands);
extern image_t *new_3d_image_t(int w, int h, int d, int bands);
/* free an image_t structure */
extern void free_image_t(image_t *img);
/* copy one */
extern int copy_image_t(image_t *src, image_t *dst);
/* blank an image */
extern void zero_image_t(image_t *img);
/* resize an image */
extern int resize_image_t(image_t *img, int w, int h, int bands);
extern int resize_3d_image_t(image_t *img, int w, int h, int d, int bands);
/* print one */
extern int print_image_t(FILE *fp, image_t *img);
extern int sprint_image_t(char *buff, image_t *img);
/* functions to read and write an image */
extern image_t * readimage(char *filename);
extern int writeimage(float *img,int w,int h,int bands,char *filename);

#endif
