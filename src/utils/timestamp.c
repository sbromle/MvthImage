/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Get a timestamp for marking a modified mmap file.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "base/images_types.h"

void stamp_image_t(image_t *img)
{
	//if (img!=NULL) img->timestamp=get_timestamp();
	if (img!=NULL) img->timestamp+=rand(); /* just add random number. Just as good.*/
}

unsigned long get_timestamp(void)
{
	struct timeval tv;
	unsigned long ret;
	gettimeofday(&tv,NULL);
	ret=tv.tv_sec*1000000+tv.tv_usec;
	return ret;
}
