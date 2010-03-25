/* Pango
 * pangoft2topgm.c: Example program to view a UTF-8 encoding file
 *                  using Pango to render result.
 *
 * Copyright (C) 1999 Red Hat Software
 * Copyright (C) 2001 Sun Microsystems
 *
 * Modified Jan. 10, 2005, Sam Bromley <sbromley@guigne.com>
 * Portions Copyright (C) 2005 Sam Bromley
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#define BUFSIZE 1024
#define MALLOCSIZE 1024
#define DEFAULT_FONT_FAMILY "Sans"
#define DEFAULT_FONT_SIZE 18

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <pango/pango.h>
#include <pango/pangoft2.h>

#include "base/images_types.h"
#include "utils/hsv_utils.h"

#define _MAKE_FONT_NAME(family, size) family " " #size
#define MAKE_FONT_NAME(family, size) _MAKE_FONT_NAME(family, size)

static int opt_dpi = 96;
static char *opt_font =
MAKE_FONT_NAME(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE);
static int opt_margin = 0;
static gboolean opt_auto_dir = TRUE;
static int opt_width = -1;
static int opt_indent = 0;
static PangoFontMap *fontmap=NULL;

PangoFontDescription *
get_font_description(void)
{
	PangoFontDescription *font_description =
		 pango_font_description_from_string(opt_font);

	if ((pango_font_description_get_set_fields(font_description) &
			  PANGO_FONT_MASK_FAMILY) == 0)
		pango_font_description_set_family(font_description,
			 DEFAULT_FONT_FAMILY);

	if ((pango_font_description_get_set_fields(font_description) &
			  PANGO_FONT_MASK_SIZE) == 0)
		pango_font_description_set_size(font_description,
			 DEFAULT_FONT_SIZE * PANGO_SCALE);

	return font_description;
}

static PangoLayout *
make_layout(PangoContext * context, const char *text, double size)
{
	static PangoFontDescription *font_description;
	PangoDirection base_dir;
	PangoLayout *layout;

	layout = pango_layout_new(context);
	pango_layout_set_markup(layout, text, -1);

	pango_layout_set_auto_dir(layout, opt_auto_dir);

	font_description = get_font_description();
	if (size > 0)
		pango_font_description_set_size(font_description,size*PANGO_SCALE);

	if (opt_width > 0)
		pango_layout_set_width(layout,
			 (opt_width * opt_dpi * PANGO_SCALE + 32) / 72);

	if (opt_indent != 0)
		pango_layout_set_indent(layout,
			 (opt_indent * opt_dpi * PANGO_SCALE + 32) / 72);

	base_dir = pango_context_get_base_dir(context);
	pango_layout_set_alignment(layout,
		 base_dir ==
		 PANGO_DIRECTION_LTR ? PANGO_ALIGN_LEFT : PANGO_ALIGN_RIGHT);

	pango_layout_set_font_description(layout, font_description);

	pango_font_description_free(font_description);

	return layout;
}

static void
do_output(PangoContext * context,
	 const char *text, FT_Bitmap * bitmap, int *width, int *height,
	 double ptsize)
{
	PangoLayout *layout;
	PangoRectangle logical_rect;
	int x = opt_margin;
	int y = opt_margin;

	*width = 0;
	*height = 0;

	layout = make_layout(context, text, ptsize);
	pango_layout_get_extents(layout, NULL, &logical_rect);

	*width = MAX(*width, PANGO_PIXELS(logical_rect.width));
	*height += PANGO_PIXELS(logical_rect.height);


	if (bitmap)
		pango_ft2_render_layout(bitmap, layout, x, y);


	g_object_unref(layout);

	*width += 2 * opt_margin;
	*height += 2 * opt_margin;
}

image_t *
text_pango_fltr(const char *message, /* the message to create */
		float colour[4], /* the color to use */
		double ptsize, /* the size of the font */
		unsigned char **mask /* a mask for pasting (not used if NULL) */
		)
{
	float rgb[4];
	PangoContext *context;
	char *text=NULL;
	size_t len=0;
	char *p;
	GError *error = NULL;
	FT_Bitmap bitmap;
	guchar *buf=NULL;
	int width, height;
	image_t *textbuf=NULL;
	int i,j,k;
	static int initialized=0;
	float h,s,v; /* hue, saturation, and value for coloured text */

	if (!initialized)
	{
		g_type_init();
		initialized=1;
	}

	for (k=0;k<4;k++) rgb[k]=colour[k];

	if (g_file_test("./pangorc", G_FILE_TEST_EXISTS))
		putenv("PANGO_RC_FILE=./pangorc");

	/* Get the text
	 */
	text = g_strdup(message);
	assert(text!=NULL);
	len = strlen(text);

	/* Strip trailing whitespace
	 */
	p = text + len;
	while (p > text)
	{
		gunichar ch;

		p = g_utf8_prev_char(p);
		ch = g_utf8_get_char(p);
		if (!g_unichar_isspace(ch))
			break;
		else
			*p = '\0';
	}

	/* Make sure we have valid markup
	 */
	if (!pango_parse_markup(text, -1, 0, NULL, NULL, NULL, &error))
	{
		fprintf(stderr,"Cannot parse input as markup: %s", error->message);
		g_free(text);
		return NULL;
	}

	if (fontmap==NULL) fontmap = pango_ft2_font_map_new();
	assert(fontmap!=NULL);
	pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(fontmap),
		 opt_dpi, opt_dpi);
	context = pango_ft2_font_map_create_context(PANGO_FT2_FONT_MAP(fontmap));
	//g_object_unref(fontmap);
	assert(context!=NULL);
	pango_context_set_language(context, pango_language_from_string("en_US"));
	pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

	/* let pango do one pass just to get size of bitmap */
	do_output(context, text, NULL, &width, &height,ptsize);

	bitmap.width = width;
	bitmap.pitch = (bitmap.width + 3) & ~3;
	bitmap.rows = height;
	buf = bitmap.buffer = g_malloc(bitmap.pitch * bitmap.rows);
	bitmap.num_grays = 256;
	bitmap.pixel_mode = ft_pixel_mode_grays;
	memset(buf, 0x0, bitmap.pitch * bitmap.rows);

	/* now actually draw the bitmap */
	do_output(context, text, &bitmap, &width, &height,ptsize);

	g_object_unref(context);

	/* make image buffer */
	textbuf=(image_t*)calloc(1,sizeof(image_t));
	textbuf->w=width;
	textbuf->h=height;
	textbuf->d=1;
	textbuf->bands=3;
	textbuf->data=(float*)
		calloc(width*height*3,sizeof(float));

	RGB2HSV(rgb[0],rgb[1],rgb[2],&h,&s,&v);
	/* use bitmap to scale colour */
	if (mask!=NULL)
		*mask=(unsigned char*)calloc(width*height,sizeof(unsigned char));
	for (j=0;j<height;j++)
	{
		for (i=0;i<width;i++)
		{
			float frac=buf[j*bitmap.pitch+i]/255.0;
			HSV2RGB(h,s,frac*v,rgb,rgb+1,rgb+2);
			for (k=0;k<3;k++) {
				textbuf->data[3*(textbuf->w*j+i)+k]=rgb[k];
			}
			if (mask!=NULL)
				if (buf[j*bitmap.pitch+i]>=96) (*mask)[j*width+i]=1;
		}
	}
	g_free(buf);
	g_free(text);

	return textbuf;
}
