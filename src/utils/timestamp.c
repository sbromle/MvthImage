/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Get a timestamp for marking a modified mmap file.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * MVTH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License Version 3,
 * as published by the Free Software Foundation.
 *
 * MVTH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * (see the file named "COPYING"), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 *
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
