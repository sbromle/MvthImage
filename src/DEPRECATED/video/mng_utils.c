/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * MNG image stream routines.
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
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmng.h>

static FILE *mngfile=NULL; /* file handle to the current mng file */
static unsigned char *backbuffer=NULL; /* a place to load the image in the bg */
static int imageready=0; /* flag showing if backbuffer is completely drawn*/
static int bwidth=0; 
static int bheight=0;
static mng_handle themnghandle=MNG_NULL;
static char *g_filename=NULL;

/* callback function used by mng library to allocate memory */
static mng_ptr my_memalloc(mng_size_t iLen)
{
	return (mng_ptr)calloc(1,iLen);
}

/* callback function used by libmng to free its memory */
static void my_memfree(mng_ptr pPtr, mng_size_t iLen)
{
	free(pPtr);
}

/* callback for libmng to read more data */
static mng_bool my_read(mng_handle hHandle,
		mng_ptr pBuf,
		mng_uint32 iBuflen,
		mng_uint32p pRead)
{
	*pRead=fread(pBuf,1,iBuflen,mngfile);
	return MNG_TRUE;
}

static mng_bool my_openstream(mng_handle hHandle)
{
	char *filename;
	/* get the filename */
	filename=g_filename;
	mngfile=fopen(filename,"rb");
	if (mngfile==NULL) 
	{
		fprintf(stderr,"mng: Error opening file `%s'\n",filename);
		return MNG_FALSE;
	}
	return MNG_TRUE;
}

mng_bool my_closestream(mng_handle hHandle)
{
	if (mngfile!=NULL) fclose(mngfile);
	if (!backbuffer)
	{
		free(backbuffer);
		backbuffer=NULL;
	}
	return MNG_TRUE;
}

mng_bool my_processheader(mng_handle hHandle,
		mng_uint32 iWidth, mng_uint32 iHeight)
{
	/* set our "canvas" to 24-bit RGB */
	mng_set_canvasstyle(hHandle,MNG_CANVAS_RGB8);
	/* allocate the memory for the backbuffer */
	backbuffer=
		(unsigned char *)realloc(backbuffer,3*iWidth*iHeight*sizeof(unsigned char));
	bwidth=iWidth;
	bheight=iHeight;
	return MNG_TRUE;
}


/* callback to return a pointer to a line within backbuffer */
mng_ptr my_getcanvasline(mng_handle hHandle, mng_uint32 iLineNr)
{
	return &backbuffer[3*iLineNr*bwidth];
}

/* callback to indicate that a complete frame has been drawn */
mng_bool my_refresh(mng_handle hHandle,
		mng_uint32 iX,
		mng_uint32 iY,
		mng_uint32 iWidth,
		mng_uint32 iHeight)
{
	imageready=1;
	fprintf(stderr,"mng: refreshed.\n");
	return MNG_TRUE;
}

#include <sys/timeb.h>
/* callback to return a number of "ticks" for animation purposes */
mng_uint32 my_gettickcount(mng_handle hHandle)
{
	mng_uint32 milliseconds;
	struct timeb tb;
	ftime(&tb);
	milliseconds=tb.time*1000+tb.millitm;
	return milliseconds;
}

mng_bool my_settimer(mng_handle hHandle, mng_uint32 msecs)
{
	return MNG_TRUE;
}

static int setup_mng_callbacks(mng_handle handle);

int init_mng(const char *filename,
		unsigned int *w, unsigned int *h, unsigned int *bands)
{
	mng_retcode ret;

	if (filename==NULL) return -1;

	if (g_filename!=NULL) { free(g_filename); g_filename=NULL;}
	g_filename=strdup(filename);

	if (themnghandle==MNG_NULL)
	{
		themnghandle=mng_initialize(NULL,my_memalloc,my_memfree,NULL);
		if (themnghandle==MNG_NULL)
		{
			fprintf(stderr,"Error initializing mng library\n");
			return -1;
		}
		if (backbuffer!=NULL) free(backbuffer);
		bwidth=0;
		bheight=0;
		ret=setup_mng_callbacks(themnghandle);
		if (ret!=0) return ret;
	}
	else
	{
		/* clear any old streams */
		mng_display_freeze(themnghandle);
		mng_reset(themnghandle);
		mng_closestream(themnghandle);
	}

	/* attempt to read the mng stream */
	ret=mng_read(themnghandle);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error reading from mng file (%d).\n",ret);
		return -1;
	}

	if  (backbuffer==NULL)
	{
		fprintf(stderr,"mng: Error! backbuffer==NULL after read!\n");
		return -1;
	}

	*w=bwidth;
	*h=bheight;
	*bands=3;

	/* start the display */
	ret=mng_display(themnghandle);
	mng_display_freeze(themnghandle);

	return 0;
}

static int setup_mng_callbacks(mng_handle handle)
{
	mng_bool ret;
	/* setup the callback for openning the data stream */
	ret=mng_setcb_openstream(handle,my_openstream);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting openstream callback.\n");
		return -1;
	}

	/* setup the callback for closing the data stream */
	ret=mng_setcb_closestream(handle,my_closestream);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting closestream callback.\n");
		return -1;
	}

	/* setup the callback for reading data */
	ret=mng_setcb_readdata(handle,my_read);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting readdata callback.\n");
		return -1;
	}

	/* setup the callback for after the header is processed */
	ret=mng_setcb_processheader(handle,my_processheader);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting processheader callback.\n");
		return -1;
	}

	/* set up callback for storing data to backbuffer */
	ret=mng_setcb_getcanvasline(handle,my_getcanvasline);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting getcanvasline callback.\n");
		return -1;
	}

	ret=mng_set_bgcolor(handle,0,0,0);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting background color to black.\n");
		return -1;
	}

	ret=mng_setcb_refresh(handle,my_refresh);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting background color to black.\n");
		return -1;
	}

	ret=mng_setcb_gettickcount(handle,my_gettickcount);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting gettickcount callback.\n");
		return -1;
	}

	ret=mng_setcb_settimer(handle,my_settimer);
	if (ret!=MNG_NOERROR)
	{
		fprintf(stderr,"mng: Error setting settimer callback.\n");
		return -1;
	}

	return 0;
}

int grab_mng_frame(char *pixels)
{
	mng_retcode ret;

	while (!imageready)
	{
		ret=mng_display_resume(themnghandle);
	}
	/* copy the backbuffer over to pixels */
	memcpy(pixels,backbuffer,3*bwidth*bheight*sizeof(char));
	imageready=0;
	mng_display_freeze(themnghandle); /* don't overwrite it until we are told*/
	return 0;
}

int rewind_mng(void)
{
	mng_retcode ret;
	ret=mng_display_reset(themnghandle);
	ret=mng_display(themnghandle);
	return 0;
}

int close_mng(void)
{
	if (g_filename!=NULL)
	{
		free(g_filename);
		g_filename=NULL;
	}
	mng_display_freeze(themnghandle);
	mng_cleanup(&themnghandle);
	mng_closestream(themnghandle);
	return 0;
}

int end_of_mng(void)
{
	return 0;
}

int getMNGframesize(int *w, int *h, int *bands)
{
	if (backbuffer!=NULL)
	{
		*w=bwidth;
		*h=bheight;
		*bands=3;
	}
	else{
		*w=*h=*bands=0;
	}
	return 0;
}
