/* 
** This file is part of MVTH (Machine Vision Test Harness).
**
** Copyright (c) 2004 Samuel P. Bromley <sam@sambromley.com>
**
** This file is part of MVTH - the Machine Vision Test Harness.
**
** MVTH is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License Version 3,
** as published by the Free Software Foundation.
**
** MVTH is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** (see the file named "COPYING"), and a copy of the GNU Lesser General
** Public License (see the file named "COPYING.LESSER") along with MVTH.
** If not, see <http://www.gnu.org/licenses/>.
**
*/ 

/* we will wlock and rlock variables in the
 * image_t structure as locking semaphores for writing and reading,
 * respectively.
 */
/* will use both variables to avoid race conditions */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "../base/images_types.h"

/* spin lock for write. returns when we have a lock */
void lock_for_write(image_t *img)
{
	/* locking now handled by flush_mmap_image(). */
	return;
}

void lock_for_read(image_t *img)
{
	/* no need for running mvth core to lock for read. */
	/* let external applications do the locking for read when they read. */
	return;
}

void unlock_write(image_t *img)
{
	return;
}

void unlock_read(image_t *img)
{
	return;
}
