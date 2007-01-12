/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Handling routines for memory mapped images.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef MMAP_HEADER_H
#define MMAP_HEADER_H
#include <stdio.h>
#include "images_types.h"

/* external variable that should be defined elsewhere */
extern int setup_mmap_directory(void);
extern int flush_image_t(image_t *m);
extern int fflush_image_t(image_t *m,char *filename);

#endif
