/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Code to handle mpeg streams using MPEG2 library..
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
#include <string.h>
#include <inttypes.h>

#include "mpeg2dec/mpeg2.h"
#include "mpeg2dec/mpeg2convert.h"

int rewindmpeg2(void);
int closempeg2(void);
static int _initmpeg2(FILE *mpgfile,int *w, int *h);

static struct fbuf_s {
    uint8_t * rgb[3];
    int used;
} fbuf[3];

static struct fbuf_s * get_fbuf (void)
{
    int i;

    for (i = 0; i < 3; i++)
	if (!fbuf[i].used) {
	    fbuf[i].used = 1;
	    return fbuf + i;
	}
    fprintf (stderr, "Could not find a free fbuf.\n");
    exit (1);
}

static mpeg2dec_t *decoder=NULL;
static const mpeg2_info_t * info;
static FILE *global_mpegfile=NULL;

int
initmpeg2(char *filename, int *w, int *h)
{
	FILE *mpgfile;
	mpgfile = fopen (filename, "rb");
	if (!mpgfile) {
	    fprintf (stderr, "Could not open file \"%s\".\n", filename);
	   	return 0;
	}
	return _initmpeg2(mpgfile,w,h);
}

#define BUFFER_SIZE 4096
static int _initmpeg2(FILE *mpgfile,int *w, int *h)
{
    static uint8_t buffer[BUFFER_SIZE];
    mpeg2_state_t state;
    struct fbuf_s * current_fbuf;
    size_t size;
    int pixels;
		int i;

	global_mpegfile=mpgfile;

		if (decoder!=NULL) {
 	mpeg2_close (decoder);
	decoder=NULL;
		}
    decoder = mpeg2_init ();
    if (decoder == NULL) {
	fprintf (stderr, "Could not allocate a decoder object.\n");
	exit (1);
    }
    info = mpeg2_info (decoder);

    size = (size_t)-1;
		do {
	state = mpeg2_parse (decoder);
	if (state==STATE_BUFFER) {
		//fprintf(stderr,"Got STATE_BUFFER\n");
	    size = fread (buffer, 1, BUFFER_SIZE, global_mpegfile);
	    mpeg2_buffer (decoder, buffer, buffer + size);
	}
	else if (state==STATE_SEQUENCE) {
		//fprintf(stderr,"Got STATE_SEQUENCE\n");
	    mpeg2_convert (decoder, mpeg2convert_rgb24, NULL);
	    mpeg2_custom_fbuf (decoder, 1);
			if (w!=NULL) *w=info->sequence->width;
			if (h!=NULL) *h=info->sequence->height;
	    pixels = info->sequence->width * info->sequence->height;
	    for (i = 0; i < 3; i++) {
		fbuf[i].rgb[0] = (uint8_t *) malloc (3 * pixels);
		fbuf[i].rgb[1] = fbuf[i].rgb[2] = NULL;
		if (!fbuf[i].rgb[0]) {
		    fprintf (stderr, "Could not allocate an output buffer.\n");
		    exit (1);
		}
		fbuf[i].used = 0;
	    }
	    for (i = 0; i < 2; i++) {
		current_fbuf = get_fbuf ();
		mpeg2_set_buf (decoder, current_fbuf->rgb, current_fbuf);
	    }
			break;
	}
	else if (state==STATE_PICTURE
			|| state==STATE_SLICE
			|| state==STATE_END
			|| state==STATE_INVALID_END) {
		//if (state==STATE_SLICE) fprintf(stderr,"Got STATE_PICTURE\n");
		//if (state==STATE_SLICE) fprintf(stderr,"Got STATE_SLICE\n");
		//if (state==STATE_END) fprintf(stderr,"Got STATE_END\n");
		//if (state==STATE_INVALID_END) fprintf(stderr,"Got STATE_INVALID_END\n");
		  fprintf(stderr,"GOT unexpected state during initialization.\n");
		  return 0;
	}
    } while (size);

		return 1;
}

#define BUFFER_SIZE 4096


int _getmpeg2frame(unsigned char *rgb,int gray)
{
		int i;
    static uint8_t buffer[BUFFER_SIZE];
    mpeg2_state_t state;
    size_t size;
    int pixels;
    struct fbuf_s * current_fbuf;
		int done=0;

    size = (size_t)-1;
    do {
	state = mpeg2_parse (decoder);
	if (state==STATE_BUFFER) {
		//fprintf(stderr,"Got STATE_BUFFER\n");
	    size = fread (buffer, 1, BUFFER_SIZE, global_mpegfile);
	    mpeg2_buffer (decoder, buffer, buffer + size);
	}
	else if (state==STATE_SEQUENCE) {
		//fprintf(stderr,"Got STATE_SEQUENCE\n");
	    mpeg2_convert (decoder, mpeg2convert_rgb24, NULL);
	    mpeg2_custom_fbuf (decoder, 1);
	    pixels = info->sequence->width * info->sequence->height;
	    for (i = 0; i < 3; i++) {
		fbuf[i].rgb[0] = (uint8_t *) malloc (3 * pixels);
		fbuf[i].rgb[1] = fbuf[i].rgb[2] = NULL;
		if (!fbuf[i].rgb[0]) {
		    fprintf (stderr, "Could not allocate an output buffer.\n");
		    exit (1);
		}
		fbuf[i].used = 0;
	    }
	    for (i = 0; i < 2; i++) {
		current_fbuf = get_fbuf ();
		mpeg2_set_buf (decoder, current_fbuf->rgb, current_fbuf);
	    }
	}
	else if (state==STATE_PICTURE) {
		//fprintf(stderr,"Got STATE_PICTURE\n");
	    current_fbuf = get_fbuf ();
	    mpeg2_set_buf (decoder, current_fbuf->rgb, current_fbuf);
	}
	else if (state==STATE_SLICE
			|| state==STATE_END
			|| state==STATE_INVALID_END) {
		//if (state==STATE_SLICE) fprintf(stderr,"Got STATE_SLICE\n");
		//if (state==STATE_END) fprintf(stderr,"Got STATE_END\n");
		//if (state==STATE_INVALID_END) fprintf(stderr,"Got STATE_INVALID_END\n");
	    if (info->display_fbuf)
			{
				int jj;
				int pixels;
				uint8_t *base;
				base=info->display_fbuf->buf[0];
				/* we have a complete image ready */
				if (gray)
				{
					pixels=info->sequence->width*info->sequence->height;
					for (jj=0;jj<pixels;jj++)
					{
						rgb[jj]=(base[0]+base[1]+base[3])/3;
						base+=3;
					}

				}
				else
				{
					pixels=3*info->sequence->width*info->sequence->height;
					/* we have a complete image ready */
					memcpy(rgb,base,pixels*sizeof(unsigned char));
	
				}
				done=1;
			}
	    if (info->discard_fbuf)
                ((struct fbuf_s *)info->discard_fbuf->id)->used = 0;
	    if (state != STATE_SLICE)
		for (i = 0; i < 3; i++)
		    free (fbuf[i].rgb[0]);
			if (state==STATE_END)
		{
			//fprintf(stderr,"Got STATE_END\n");
			rewindmpeg2();
		}
		if (state==STATE_INVALID_END)
		{
			//fprintf(stderr,"Got STATE_INVALID_END\n");
			rewindmpeg2();
		}

			/* we got a single frame */
			if (done)
			{
				if (size!=0) return 1;
				return 0;
			}
	}
    } while (size);
		return 0;
}

int getmpeg2frameGray(unsigned char *rgb)
{
	return _getmpeg2frame(rgb,1);
}

int getmpeg2frameRGB(unsigned char *rgb)
{
	return _getmpeg2frame(rgb,0);
}

int
closempeg2(void)
{
	if (global_mpegfile==NULL) return 1;
	/* close the mpeg decoder */
	if (decoder!=NULL) mpeg2_close(decoder);
	decoder=NULL;
	fclose(global_mpegfile);
	global_mpegfile=NULL;
	return 1;
}

int
rewindmpeg2(void)
{
	if (global_mpegfile==NULL) return 1;
	/* close the mpeg decoder */
	if (decoder!=NULL) mpeg2_close(decoder);
	decoder=NULL;
	/* rewind the old mpgfile if opened */
	if (global_mpegfile!=NULL) rewind(global_mpegfile);
	/* reinitialize the mpeg decoder */
  return _initmpeg2(global_mpegfile,NULL,NULL);
}
