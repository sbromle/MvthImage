#ifndef TEXT_PANGO_FLTR
#define TEXT_PANGO_FLTR
/* Pango
 * pangoft2toimg.c: Use pango to generate an image_t from pango
 *                  markup text.
 *
 * Copyright (C) 2005 Sam Bromley
 * (Modified Jan. 10, 2005, Sam Bromley <sbromley@guigne.com>)
 *
 * This header file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this code; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "base/images_types.h"

extern image_t *
text_pango_fltr(const char *message, float colour[4], double ptsize,
		unsigned char **mask);

#endif
