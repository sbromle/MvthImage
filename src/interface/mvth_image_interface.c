/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Allow access to image contexts within the mvthImage plugin.
 *
 * Copyright (C) 2003,2004,2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <generic_contexts.h>
#include "utils/timestamp.h"
#include "base/images_types.h"
#include "base/images_utils.h"
#include "base/mmap_handler.h"

#define EXTERNAL_INTERFACE
#include "../base/images_context.c"

