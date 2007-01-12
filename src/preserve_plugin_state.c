/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Store and retrieve the state of the plugin between reloads.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "base/images_context.h"
#include "base/images_utils.h"

int save_plugin_state(int fd)
{
	/* nothing other than images to save (and images
	 * are handled by mvth core) */
	return 1;
}

int restore_plugin_state(int fd)
{
	/* nothing other than images to restore (and images
	 * are handled by mvth core) */
	return 1;
}
