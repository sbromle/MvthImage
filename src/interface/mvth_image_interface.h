/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Allow access to the images contexts in the mvth-image plugin.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef MVTH_IMAGE_INTERFACE_H
#define MVTH_IMAGE_INTERFACE_H


#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <mvthImage.h>

/* these functions can be used internally and externally */
extern int get_max_images(void);
extern image_t *get_image_var(char *varname);/*get a pointer to an image */
extern int register_image_undo_var(char *varname);
extern int image_undo(void);
extern char *image_undo_var(int *error_id);

extern int flush_image_t(image_t *m);

#endif
