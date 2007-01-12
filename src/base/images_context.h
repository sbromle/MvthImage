/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Maintain image contexts.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef IMAGE_CONTEXT_H
#define IMAGE_CONTEXT_H


#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "base/images_types.h"
#include "base/mmap_handler.h"

/* "EXTERNAL_INTERFACE" is defined when building the external plugin interface*/
#ifndef EXTERNAL_INTERFACE
/* these functions are for internal use by mvthimage only */
extern int init_image_context(void);
extern char **get_image_vars(int *nvars);
/* add an ASI ping to our array */
extern int register_image_var(image_t *ni, char *varname);
/* deregisters an ping called varname. */
extern int deregister_image_var(char *varname);
#  define ATTACH_TO_CONTEXT(_ret)
#else 
#  define ATTACH_TO_CONTEXT(_ret) \
	if (attach_to_image_context()!=0) return (_ret);
#endif

/* these functions can be used internally and externally */
extern image_t *get_image_var(char *varname);/*get a pointer to an image */
extern int register_image_undo_var(char *varname);
extern char *image_undo_var(int *error_id);

#endif
