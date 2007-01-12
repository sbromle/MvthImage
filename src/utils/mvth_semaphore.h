/* 
** This file is part of MVTH (Machine Vision Test Harness).
**
** Copyright (c) 2004 Samuel P. Bromley <sam@sambromley.com>
** 
** MVTH is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** MVTH is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with MVTH; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/ 

#ifndef M_SEMAPHORE_H
#define M_SEMAPHORE_H

#include "base/images_types.h"
/* lock an image for reading or writing */

extern void lock_for_write(image_t *img);
extern void lock_for_read(image_t *img);
extern void unlock_read(image_t *img);
extern void unlock_write(image_t *img);

#endif
